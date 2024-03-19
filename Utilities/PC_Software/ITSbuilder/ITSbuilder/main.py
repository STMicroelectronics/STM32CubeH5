#
# ****************************************************************************
# @file    main.py
# @author  MCD Application Team
# @brief   Main application file.
# ****************************************************************************
# @attention
#
# Copyright (c) 2023 STMicroelectronics.
# All rights reserved.
#
# This software is licensed under terms that can be found in the LICENSE file
# in the root directory of this software component.
# If no LICENSE file comes with this software, it is provided AS-IS.
#
# ****************************************************************************


import sys
import click
from ITSbuilder import blob
from struct import pack
from array import array

SOURCE_VERSION = "1.1.2"
APPLICATIONNAME = "ITSbuilder"
STDINFO='You can use stdin for source (i.e. -), use - if you want to use stdout for output. More information in the README file of ' + APPLICATIONNAME

#to accept hexadecimal
class BasedIntParamType(click.ParamType):
    name = "integer"

    def convert(self, value, param, ctx):
        if isinstance(value, int):
            return value

        try:
            if value[:2].lower() == "0x":
                return int(value[2:], 16)
            return int(value, 10)
        except ValueError:
            self.fail(f"{value!r} is not a valid integer", param, ctx)

BASED_INT = BasedIntParamType()


def menuBlob()->blob.ITS_blob:
    nbkey:int=0
    nbits:int=0
    nbobj=int(input("How many object do you want ? :"))
    blb=blob.ITS_blob()
    flg=blob.PSA_flag()
    if nbobj!=0:
        print(str(nbobj)+' objects')
        while nbobj!=nbkey+nbits :
            nbits=int(input("How many data ?"))
            nbkey=int(input("How many keys ?"))
        while nbkey+nbits!=0:
            if nbkey!=0 and nbits!=0:
                choix=input("Which kind of object do want you to add? (data=0/key=1)")
            elif nbkey==0:
                print("Enter ITS data:")
                choix='0'
            else:
                print("Enter Key data:")
                choix='1'
            if choix=='0':
                owner=int(input("Number of the owner ? (decimal)"))
                uuid=int(input("Number of the uuid ? (decimal)"))
                
                strflg=flg.get_flags()
                cont=True
                flag=[]
                while cont:
                    flag.append(input("What is the flag ? (%s)" % strflg ))
                    if input("Another ? (YES/NO)").upper()!="YES":
                        cont=False
                filename=input("Name of the datafile ? (raw)" )
                try:
                    f=open(filename,"rb")
                    data=f.read()
                    its=blob.ITS_data_area()
                    its.init(owner, uuid,data, flag)
                    blb.addITS(its)
                    nbits-=1
                except Exception as e:
                    raise blob.BlobException("read ITS file failure, retry :"+str(e))
                finally:
                    f.close()
            elif choix=='1':
                kl=blob.ITS_keyLoader()
                owner=int(input("Number of the owner ? (decimal)"))
                kid=int(input("Number of the key id ? (decimal)"))
                ktype=input("Which kind of key ?")
                algo=input("Which kind of algo ?")
                digest=input("Which kind of digest ?")
                bits=int(input("How many bits ? (decimal)"))
                cont=True
                usage=[]
                while cont==True:
                    usage.append(input("Which usage ?"))
                    again=input("Another usage Yes/No ?")
                    if(again.upper()!="YES" and again.upper()!="Y"):
                        cont=False
                fileformat=input("Which kind of file format ?")
                filename=input("Key file name ?")
                if fileformat.upper()=="PEM":
                    pemoption=input("Key name ?")
                else:
                    default_asymmetric:int=blob.PSA_key.default_asymmetric(ktype.upper(), usage)
                    if default_asymmetric==1: pemoption="PUBLIC KEY"
                    elif default_asymmetric==2: pemoption="PRIVATE KEY"
                    else: pemoption=""
                try:
                    f=open(filename,"rb")
                    key=blob.ITS_key_area()
                    data=kl.read(f, fileformat.upper(),pemoption.upper())
                    key.init(owner, kid,1, ktype.upper(),digest,algo, bits,usage, pemoption.upper())
                    key.add_data(data)
                    blb.addkey(key)
                    nbkey-=1
                except Exception as e:
                    raise blob.BlobException("read key file failure, retry"+str(e))
                finally:
                    f.close()
    return blb


@click.argument('filein', type=click.File('rb'))
@click.option('-i', '--id', 'objectid', type=BasedIntParamType(), required=True, help='Id of the key')
@click.option('-o', '--owner', default=1, show_default=True, type=BasedIntParamType(), required=False, help='Owner of the key')
@click.command(help='[usage] %s check blob.source.bin'%STDINFO, short_help='Check if the key is in a blob.')
def checkBlob(filein, owner:int, objectid:int)->int:
    try:
        b=blob.ITS_blob()
        data=filein.read()
        b.readfile(data)
        if b.is_in(owner,objectid):
            click.echo('True')
        else:
            click.echo('False')
        sys.exit(0)
    except Exception as e:
        print("Can't read source, check failed, "+str(e), file=sys.stderr)
        sys.exit(1)


@click.argument('dump', type=click.File('w', encoding='utf8'))
@click.argument('filein', type=click.File('rb'))
@click.command(help='[usage] %s infoblob blob.source.bin info.txt.'%STDINFO, short_help='Description of a blob file.')
def infoBlob(filein, dump)->int:
    try:
        b=blob.ITS_blob()
        data=filein.read()
        b.readfile(data)
        try:
            dump.write(APPLICATIONNAME+" v:"+SOURCE_VERSION+"\n")
            dump.write(str(b))
            sys.exit(0)
        except Exception as e:
            print("Can's save info, infoBlob failed, "+str(e), file=sys.stderr)
            sys.exit(2)
    except Exception as e:
        print("Can't read source, infoBlob failed, "+str(e), file=sys.stderr)
        sys.exit(1)

@click.option('-d', '--dump',  type=click.File('w', encoding='utf8'), help='Location for the text description of the read blob file')
@click.option('-v', '--version', type=int, help="Header version")
@click.argument('fileout', type=click.File('wb'), required=True)
@click.option('-m', '--menu', is_flag=True, help="Interactive menu to create a blob")
@click.option('-c', '--command', is_flag=True, help="To create a blob from a command file")
@click.command(help='[usage] %s createblob blob.source.bin : Build a header of a Blob file.' +STDINFO, short_help='Create a blob file.')
def createBlob(fileout, version, menu, command, dump)->int:
    ret:int=0
    if (version and menu) or (version and command) or (command and menu) or (not command and not menu and version is None):
        print("Incompatible option, you must use one of -v, -m or -c option. try '%s createblob --help' for help" % APPLICATIONNAME, file=sys.stderr)
        ret=1
    else:
        try:
            if version:
                b=blob.ITS_blob(version)
                b.writefile(fileout)
                ret=0
            elif command:
                click.echo("Not implemented")
                ret=2
            elif menu:
                b=menuBlob()
                b.writefile(fileout)
                ret=0
            if dump:
                if b:
                    dump.write(str(b))
                else:
                    print("Unable to build blob", file=sys.stderr)
        except Exception as e:
            print("Can't write file, createBlob failed :"+str(e), file=sys.stderr)
            ret=1
    sys.exit(ret)

@click.argument('fileout', type=click.File('wb'), required=False)
@click.argument('filein', type=click.File('rb'), required=False)
@click.option('-d', '--dump',  type=click.File('w', encoding='utf8'), required=False, help='Location for the text description of the read blob file')
@click.option('-i', '--id', type=BasedIntParamType(), required=True, help='Id number, ids have to be unique')
@click.option('-o', '--owner', type=BasedIntParamType(),default=1, show_default=True, help='Owner number, owners have to be unique')
@click.option('-p', '--protect', type=click.Choice(['NONE', 'WRITE_ONCE', 'NO_CONFIDENTIALITY', "NO_REPLAY_PROTECTION"], case_sensitive=False), multiple=True, show_default=True, default=["NONE"], required=False, \
help='With WRITE_ONCE, the data will not be able to be modified or deleted, with NO_CONFIDENTIALITY, the data is public and therefore does not require confidentiality, with NO_REPLAY_PROTECTION the storage of data is faster but it permits an attacker with physical access to revert to an earlier version of the data')
@click.option('-a', '--add','itsfile', type=click.File('rb'), help='Location of a data binary file. Choose -a or -f option')
@click.option('-t', '--text','textdata', type=str, help='Text data to add to the blob. Choose -a or -f option')
@click.command(help='[usage] %s adddata2blob blob.filein.bin blob.fileout.bin  : Add a data to a blob file.'%STDINFO, short_help='Add a data in a blob file.')
def addData2Blob(fileout, filein, itsfile, textdata:str, protect, id, owner, dump)->int:
    try:
        b=blob.ITS_blob()
        its=blob.ITS_data_area()
        #delete ' char at the beginning and the end
        if itsfile and textdata:
            print("Only one option, add or text is allowed", file=sys.stderr)
        elif not itsfile and not textdata:
            print("One option add or text is required", file=sys.stderr)
        elif itsfile:
            data=itsfile.read()
            its.init(owner, id, data, protect)
        else:
            encoded=textdata.encode('utf-8')
            data = bytearray(encoded)
            its.init(owner, id, data, protect)
        b.readfile(filein.read())
        b.addITS(its)
        b.writefile(fileout)
        if dump:
            dump.write(str(b))
        sys.exit(0)
    except Exception as e:
        print("Can't add data :"+str(e), file=sys.stderr)
        sys.exit(1)

@click.option('-f', '--format','fileformat', required=True, type=click.Choice(['Raw','hex','Base64','PEM', 'DER'], case_sensitive=False), help="Specifies the format of the key file. Symmetric key and HMAC are limited to HEX, RAW (binary), and Base64, while asymmetric key accepts PEM, DER (ANS.1), Base64, RAW (binary). For asymmetric key, it requires the option '--asymmetric'.")
@click.option('-d', '--dump', required=False, type=click.File('w', encoding='utf8'), help='Location for the text description of the read blob file.')
@click.option('-i', '--id', "id", required=True, type=BasedIntParamType(), help='Id number, ids have to be unique.')
@click.option('-o', '--owner', default=1, show_default=True, type=BasedIntParamType(), help='Owner number, owners have to be unique.')
@click.option('-s', '--digest', type=str, required=False, default="NONE", help="Digest for HMAC and asymmetric key, see %s -s option." % APPLICATIONNAME)
@click.option('-b', '--bits', required=True, type=int, help="Number of bits, see %s -b option." % APPLICATIONNAME)
@click.option('-u', '--usage', type=str, required=True, multiple=True, help='Usage : EXPORT, COPY, ENCRYPT, DECRYPT, SIGN, VERIFY, SIGN_HASH, VERIFY_HASH, DERIVE, VERIFY_DERIVATION. For allowed usages, read README file.')
@click.option('-p','--persistence', default="PERSISTENT", show_default=True, help='Decimal Integer or Hexadecimal integer or PERSISTENT/VOLATILE/READONLY. Persistence is combined with location to make lifetime parameter.', required=False)
@click.option('-t','--keytype', required=True, type=str, help="Key type, see %s -b option." % APPLICATIONNAME)
@click.option('-k', '--addKey', 'addfile', type=click.File('rb'), help='Location of a key file.', required=True)
@click.option('-l', '--location', default=0, type=BasedIntParamType(), required=False, help='Location is combined with persistence to make lifetime parameter.')
@click.option('-D', '--deterministic', is_flag=True, default=False, required=False, help="Only for ECDSA.")
@click.option('-a', '--algo', type=str, required=False, help="Specifies the algorithm for symmetric or derivation key. For AES, the following modes are available : CBC, CCM, CFB, ECB, CTR, CTR, OFB and GCM. For DES : CBC and ECB. See %s -b AES to have more details. Derivation algorithms include HKDF, TLS12_PRF, PSK_TO_MS. Raw is used for pure KeyAgreement and cannot be derived further. This is a mandatory option for symmetric, EC keys, and HMAC." % APPLICATIONNAME)
@click.option('-m', '--asymmetric', type=str, required=False, help="This parameter allow to identify the kind of label to get in PEM file and/or kind of asymmetric key (PUBLIC/PAIR=PRIVATE). Check the key name in your PEM file. If not set then it depends on usages, for asymmetric key only.")
@click.argument('fileout', type=click.File('wb'), required=False)
@click.argument('filein', type=click.File('rb'), required=False)
@click.command(help='[usage] %s addkey2blob blob.filein.bin blob.fileout.bin : Add a key in a blob file.'%STDINFO, short_help='Add a key in a blob file.')
def addKey2Blob(fileout, filein, owner : int, id : int, keytype:str, bits :int, usage, addfile, fileformat : str, dump, persistence:str, location:int, asymmetric:str, digest: str,deterministic:bool,algo:str)->int:
    try:
        b=blob.ITS_blob()
        b.readfile(filein.read())
        k=blob.ITS_key_area()
        persistence=persistence.upper()
        if persistence=="PERSISTENT":
            lifetime=1
        elif persistence=="VOLATILE":
            lifetime=0
        elif persistence=="READONLY":
            lifetime=0xFF
        else:
            if persistence[:2]== "0X":
                ptype=16
                persistence=persistence[2:]
            else:
                ptype=0
            for char in persistence:
                if char in 'ABCDEF':
                    ptype=16
                else:
                    if char.isdigit():
                        if ptype==0:
                            ptype=10
                    else:
                        print("Persistence value is not available (%s)" %char, file=sys.stderr)
                        sys.exit(1)
            if ptype==0:
                print("Persistence value is not available", file=sys.stderr)
                sys.exit(1)
            lifetime=int(persistence, ptype)
        if location:
            lifetime=location<<8| (lifetime & 0xFF)
        uktype=keytype.upper()
        kl=blob.ITS_keyLoader()
        default_asymmetric:int=blob.PSA_key.default_asymmetric(uktype, usage)
        
        if fileformat:
            fileType=fileformat.upper()
        if default_asymmetric==0:
            if asymmetric is not None:
                print("Do not set asymmetric argument (%s) for symmetric keys" % asymmetric, file=sys.stderr)
                sys.exit(1)
            if fileformat=="PEM" or fileformat=="DER":
                print("PEM or DER not allowed for symmetric key", file=sys.stderr)
                sys.exit(1)
            else:
                data=kl.read(addfile, fileType)
        else:
            if blob.PSA_key.is_rsa_name(uktype) and fileType=="PEM":
                fileType="PEM_RSA" #force upload PEM file with transformation for MBEDTLS compatibility
            if default_asymmetric==-1: #RAW KEY
                if asymmetric is None:
                    try:
                        data=kl.read(addfile, fileType,"PRIVATE KEY")
                        asymmetric="PRIVATE KEY"
                    except:
                        data=kl.read(addfile, fileType,"PUBLIC KEY")
                        asymmetric="PUBLIC KEY"
                    click.echo("upload "+asymmetric)
                else:
                    data=kl.read(addfile, fileType,asymmetric.upper())
            elif default_asymmetric==1:
                if asymmetric is None:
                    try:
                        data=kl.read(addfile, fileType,"PRIVATE KEY")
                        asymmetric="PRIVATE KEY"
                    except:
                        addfile.seek(0)
                        data=kl.read(addfile, fileType,"PUBLIC KEY")
                        asymmetric="PUBLIC KEY"
                    click.echo("upload "+asymmetric)
                else:
                    asymmetric=asymmetric.upper()
                    data=kl.read(addfile, fileType,asymmetric)
                    if "PRIVATE" not in asymmetric and "PAIR" not in asymmetric and "PUBLIC" not in asymmetric:
                        click.echo("Not a private or public key as expected (%s)"%asymmetric)
            elif default_asymmetric==2:
                if asymmetric is None:
                    data=kl.read(addfile, fileType,"PRIVATE KEY")
                    asymmetric="PRIVATE"
                else:
                    asymmetric=asymmetric.upper()
                    data=kl.read(addfile, fileType,asymmetric)
                    if "PRIVATE" not in asymmetric and "PAIR" not in asymmetric:
                        print("Can't add key, not a private key as expected", file=sys.stderr)
                        sys.exit(1)
        k.init(owner, id, lifetime, uktype, digest, algo, bits, usage, asymmetric, deterministic)
        k.add_data(data,kl.raw)
        b.addkey(k)
        b.writefile(fileout)
        if dump:
            dump.write(str(b))
        sys.exit(0)
    except Exception as e:
        print("Can't add key, "+str(e), file=sys.stderr)
        sys.exit(1)

@click.group(invoke_without_command=True)
@click.option('-v', '--version', is_flag=True, help="Version of %s tool." % APPLICATIONNAME)
@click.option('-t', '--keytype', is_flag=True, help="List of available types of keys.")
@click.option('-a', '--algo', type=str, help="List of available algorithm for DH, ECDH and symmetric keys.")
@click.option('-s', '--digest', is_flag=True, help="List of available digest for HMAC and asymmetric keys.")
@click.option('-b', '--bits', type=str, help="List of available bits for a key type.")
def ITSbuilder(keytype, algo, digest, bits, version)->int:
    """Application to build an ITS blob for PSA Cryptography API 1.01. First create a blob, then add keys or data. --help for more information"""
    if(version):
        click.echo(SOURCE_VERSION)
        sys.exit(0)
    if keytype:
        click.echo(blob.PSA_key.get_Names())
        sys.exit(0)
    if digest:
        click.echo(blob.PSA_algo.get_digestNames())
        sys.exit(0)
    if algo:
        click.echo(blob.PSA_algo.get_Names(algo))
        sys.exit(0)
    if bits:
        click.echo(blob.PSA_bits.get_Names(bits))
        sys.exit(0)
    pass

ITSbuilder.add_command(createBlob)
ITSbuilder.add_command(infoBlob)
ITSbuilder.add_command(addData2Blob)
ITSbuilder.add_command(addKey2Blob)
ITSbuilder.add_command(checkBlob)

