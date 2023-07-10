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

SOURCE_VERSION = "1.0.0"
APPLICATIONNAME = "ITSbuilder"
STDINFO='You can use stdin forsource (i.e. -), use - if you want to use stdout for output. More information in the README file of ' + APPLICATIONNAME

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
            nbits=int(input("How many data area ?"))
            nbkey=int(input("How many key area ?"))
        while nbkey+nbits!=0:
            if nbkey!=0 and nbits!=0:
                choix=input("which kind of object want you to add? (data=0/key=1)")
            elif nbkey==0:
                print("Enter ITS data:")
                choix='0'
            else:
                print("Enter Key data:")
                choix='1'
            if choix=='0':
                owner=int(input("Numero of the owner ? (decimal)"))
                uuid=int(input("Numero of the uuid ? (decimal)"))
                
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
                owner=int(input("Numero of the owner ? (decimal)"))
                kid=int(input("Numero of the key id ? (decimal)"))
                ktype=input("Which kind of key ?")
                algo=input("Which kind of algo ?")
                digest=input("Which kind of digest ?")
                bits=int(input("How many bits ? (decimal)"))
                cont=True
                usage=[]
                while cont==True:
                    usage.append(input("witch usage ?"))
                    again=input("Another  usage Yes/No ?")
                    if(again.upper()!="YES" and again.upper()!="Y"):
                        cont=False
                fileformat=input("Which kind of file format ?")
                filename=input("Name of key file ?")
                if fileformat.upper()=="PEM":
                    pemoption=input("Name of key ?")
                else:
                    default_asymmetric:int=blob.PSA_key().default_asymmetric(ktype.upper(), usage)
                    if default_asymmetric==1: pemoption="PUBLIC KEY"
                    elif default_asymmetric==2: pemoption="PRIVATE KEY"
                    else: pemoption=""
                try:
                    f=open(filename,"rb")
                    key=blob.ITS_key_area()
                    data=kl.read(f, fileformat.upper(),pemoption.upper())
                    key.init(owner, kid,1, ktype.upper(),digest,algo, bits,usage,data, pemoption.upper())
                    blb.addkey(key)
                    nbkey-=1
                except Exception as e:
                    raise blob.BlobException("read key file failure, retry"+str(e))
                finally:
                    f.close()
    return blb


@click.argument('filein', type=click.File('rb'))
@click.option('-i', '--id', 'objectid', type=BasedIntParamType(), required=True, help='id of the key')
@click.option('-o', '--owner', default=1, show_default=True, type=BasedIntParamType(), required=False, help='owner of the key')
@click.command(help='[usage] %s check blob.source.bin ' + STDINFO, short_help='check if key is in blob.')
def checkBlob(filein, owner:int, objectid:int):
    try:
        b=blob.ITS_blob()
        data=filein.read()
        b.readfile(data)
        if b.is_in(owner,objectid):
            click.echo('True')
        else:
            click.echo('False')
        return 0
    except Exception as e:
        print("Can't read source, check failed, "+str(e), file=sys.stderr)
        return 1


@click.argument('dump', type=click.File('w', encoding='utf8'))
@click.argument('filein', type=click.File('rb'))
@click.command(help='[usage] %s infoblob blob.source.bin info.txt' + STDINFO, short_help='Description of a blob file.')
def infoBlob(filein, dump)->int:
    try:
        b=blob.ITS_blob()
        data=filein.read()
        b.readfile(data)
        try:
            dump.write(APPLICATIONNAME+" v:"+SOURCE_VERSION+"\n")
            dump.write(str(b))
            return 0
        except Exception as e:
            print("Can's save info, InfoBlob failed, "+str(e), file=sys.stderr)
            return 2
    except Exception as e:
        print("Can't read source, InfoBlob failed, "+str(e), file=sys.stderr)
        return 1

@click.option('-d', '--dump',  type=click.File('w', encoding='utf8'), help='Location for the text description of the read Blob file')
@click.option('-v', '--version', type=int, help="To create empty blob")
@click.argument('fileout', type=click.File('wb'), required=True)
@click.option('-m', '--menu', is_flag=True, help="Interactive menu to create blob")
@click.option('-c', '--command', is_flag=True, help="To create blob from an command file")
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
    return ret

@click.argument('fileout', type=click.File('wb'), required=False)
@click.argument('filein', type=click.File('rb'), required=False)
@click.option('-d', '--dump',  type=click.File('w', encoding='utf8'), required=False, help='Location for the text description of the read Blob file')
@click.option('-i', '--id', type=BasedIntParamType(), required=True, help='Id number, owner and id have to be uniq. owner is positive only')
@click.option('-o', '--owner', type=BasedIntParamType(),default=1, show_default=True, help='Owner number, owner and id have to be uniq')
@click.option('-p', '--protect', type=click.Choice(['NONE', 'WRITE_ONCE', 'NO_CONFIDENTIALITY', "NO_REPLAY_PROTECTION"], case_sensitive=False), multiple=True, show_default=True, default=["NONE"], required=False, \
help='With WRITE_ONCE, the data will not be able to be modified or deleted, with NO_CONFIDENTIALITY, the data is public and therefore does not require confidentiality, with NO_REPLAY_PROTECTION the storage of data is faster but it permits an attacker with physical access to revert to an earlier version of the data')
@click.option('-a', '--add','itsfile', type=click.File('rb'), help='Location of a data binary file. Choose -a or -f option')
@click.option('-t', '--text','textdata', type=str, help='Text data to add to the blob. Choose -a or -f option')
@click.command(help='[usage] %s adddata2blob blob.filein.bin blob.fileout.bin  : Add an data area to a blob file. '+STDINFO, short_help='Add a data area in a Blob file.')
def addData2Blob(fileout, filein, itsfile, textdata:str, protect, id, owner, dump)->int:
    try:
        b=blob.ITS_blob()
        its=blob.ITS_data_area()
        #delete ' char at the beginning and the end
        if itsfile and textdata:
            print("Only one option, add or text are is allowed", file=sys.stderr)
        elif not itsfile and not textdata:
            print("One option add or text are is required", file=sys.stderr)
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
        return 0
    except Exception as e:
        print("Can't add data :"+str(e), file=sys.stderr)
        return 1

@click.option('-f', '--format','fileformat', required=True, type=click.Choice(['Raw','Hex','Base64','PEM'], case_sensitive=False), help="file format, fill pemoption for PEM file")
@click.option('-d', '--dump', required=False, type=click.File('w', encoding='utf8'), help='Location for the text description of the read Blob file')
@click.option('-i', '--id', "id", required=True, type=BasedIntParamType(), help='Id number, owner and id have to be uniq')
@click.option('-o', '--owner', default=1, show_default=True, type=BasedIntParamType(), help='Owner number, owner and id have to be uniq')
@click.option('-s', '--digest', type=str, required=False, default="NONE", help="digest for HMAC and asymmetric key, see %s -s option" % APPLICATIONNAME)
@click.option('-b', '--bits', required=True, type=int, help="number of bits, see %s -b option" % APPLICATIONNAME)
@click.option('-u', '--usage', type=str, required=True, multiple=True, help='usages : EXPORT, COPY, ENCRYPT, DECRYPT, SIGN, VERIFY, SIGN_HASH, VERIFY_HASH, DERIVE, VERIFY_DERIVATION. Read README file')
@click.option('-p','--persistence', default="PERSISTENT", show_default=True, help='integer or PERSISTENT/VOLATILE/READONLY. Persistence is combined with localisation to make lifetime parameter.', required=False)
@click.option('-t','--keytype', required=True, type=str, help="Key type, see %s -b option" % APPLICATIONNAME)
@click.option('-k', '--addKey', 'addfile', type=click.File('rb'), help='Location of a key file', required=True)
@click.option('-l', '--location', default=0, type=BasedIntParamType(), required=False, help='localisation is combined with persistence to make lifetime parameter.')
@click.option('-D', '--deterministic', is_flag=True, default=False, required=False, help="only for ECDSA")
@click.option('-a', '--algo', type=str, required=False, help="ECDH or symmetric algorithm, see %s -a option" % APPLICATIONNAME)
@click.option('-m', '--asymmetric', type=str, required=False, help="This parameter allow to identify kind of label to get in PEM file and/or kind of asymmetric key (PUBLIC/PAIR=PRIVATE). Have a look of the key name in your PEM file. If not set then depends of usages, for asymmetric key only.")
@click.argument('fileout', type=click.File('wb'), required=False)
@click.argument('filein', type=click.File('rb'), required=False)
@click.command(help='[usage] %s addkey2blob blob.filein.bin blob.fileout.bin : Add a key area in a Blob file.'+STDINFO, short_help='Add a key area in a Blob file.')
def addKey2Blob(fileout, filein, owner : int, id : int, keytype:str, bits :int, usage, addfile, fileformat : str, dump, persistence:str, location:int, asymmetric:str, digest: str,deterministic:bool,algo:str):
    try:
        b=blob.ITS_blob()
        b.readfile(filein.read())
        k=blob.ITS_key_area()
        
        if isinstance(persistence, int):
            lifetime=persistence
        elif isinstance(persistence, str):
            if persistence.upper()=="PERSISTENT":
                lifetime=1
            elif persistence.upper()=="VOLATILE":
                lifetime=0
            elif persistence.upper()=="READONLY":
                lifetime=0xFF
            elif persistence[:2].lower() == "0x":
                lifetime=int(persistence[2:], 16)
        else:
            print("persistence type match error", file=sys.stderr)
            return 1
        if location:
            lifetime=location<<8| (lifetime & 0xFF)
        
        uktype=keytype.upper()
        kl=blob.ITS_keyLoader()
        default_asymmetric:int=blob.PSA_key().default_asymmetric(uktype, usage)
        if fileformat and fileformat.upper()=="PEM":
            if default_asymmetric==0:
                print(keytype+" not compatible with PEM file", file=sys.stderr)
                return 1
            else:
                if default_asymmetric==-1 :
                    if asymmetric is None:
                        try:
                            data=kl.read(addfile, 'PEM',"PRIVATE KEY")
                            asymmetric="PRIVATE KEY"
                        except:
                            data=kl.read(addfile, 'PEM',"PUBLIC KEY")
                            asymmetric="PUBLIC  KEY"
                    else:
                        asymmetric=asymmetric.upper()
                        data=kl.read(addfile, 'PEM',asymmetric)
                        
                elif default_asymmetric==1:
                    if asymmetric is None:
                        try:
                            data=kl.read(addfile, 'PEM',"PRIVATE KEY")
                            asymmetric="PRIVATE  KEY"
                        except:
                            data=kl.read(addfile, 'PEM',"PUBLIC KEY")
                            asymmetric="PUBLIC  KEY"
                    else:
                        asymmetric=asymmetric.upper()
                        data=kl.read(addfile, 'PEM',asymmetric)
                        if "PRIVATE" not in asymmetric and "PAIR" not in asymmetric and "PUBLIC" not in asymmetric:
                            click.echo("Not a private ou public key as expected")
                elif default_asymmetric==2:
                    if asymmetric is None:
                        data=kl.read(addfile, 'PEM',"PRIVATE KEY")
                        asymmetric="PRIVATE"
                    else:
                        asymmetric=asymmetric.upper()
                        data=kl.read(addfile, 'PEM',asymmetric)
                        if "PRIVATE" not in asymmetric and "PAIR" not in asymmetric:
                            click.echo("Not a private key as expected")
        else:
            if default_asymmetric==0 and not asymmetric is None:
                print("Do not set asymmetric argument (%s) for symmetric keys" % asymmetric, file=sys.stderr)
                return 1
            elif (default_asymmetric==2 or default_asymmetric==1) and asymmetric=="":
                asymmetric="PRIVATE"
            data=kl.read(addfile, fileformat.upper(),asymmetric)
        k.init(owner, id, lifetime, uktype, digest, algo, bits, usage, data, asymmetric, deterministic)
        b.addkey(k)
        b.writefile(fileout)
        if dump:
            dump.write(str(b))
        return 0
    except Exception as e:
        print("Can't add key, "+str(e), file=sys.stderr)
        return 1

@click.group(invoke_without_command=True)
@click.option('-v', '--version', is_flag=True, help="version of %s tool." % APPLICATIONNAME)
@click.option('-t', '--keytype', is_flag=True, help="lpy infoist of available types of keys.")
@click.option('-a', '--algo', type=str, help="list of available algorithm for DH, ECDH and symmetric keys.")
@click.option('-s', '--digest', is_flag=True, help="list of available digest for HMAC and asymmetric keys.")
@click.option('-b', '--bits', type=str, help="list of available bits for a key type.")
def ITSbuilder(keytype, algo, digest, bits, version)->int:
    """Application to build ITS blob for PSA Cryptography API 1.01. First create a blob, then add keys or data. --help for more help"""
    if(version):
        click.echo(SOURCE_VERSION)
    if keytype:
        click.echo(blob.PSA_key().get_Names())
    if digest:
        click.echo(blob.PSA_algo().get_digestNames())
    if algo:
        click.echo(blob.PSA_algo().get_Names(algo))
    if bits:
        click.echo(blob.PSA_bits().get_Names(bits))
    pass
    return 0

ITSbuilder.add_command(createBlob)
ITSbuilder.add_command(infoBlob)
ITSbuilder.add_command(addData2Blob)
ITSbuilder.add_command(addKey2Blob)
ITSbuilder.add_command(checkBlob)

