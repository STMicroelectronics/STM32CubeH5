# ****************************************************************************
# @file    blob.py
# @author  MCD Application Team
# @brief   Objects for Blob handling, business layer
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
from typing import Tuple
import base64

MAGIC_NUMBER = 0x10101010
MAXPADDING = 16
DATA_TYPE = { "ITS":0x2, "KEY":0x1, "PS":0x4 }
BIT32=2**32-1 #unsigned 32bits


class ITS_ParamList:
    def keyDicList(self,tab)->str:
        s:str=""
        for x in tab:
            if not s:
                s=x
            else:
                s=s+", "+x
        return s
    
    def _valueDicList(self,tab)->str:
        s:str=""
        for x in tab.values():
            if not s:
                s=hex(x)
            else:
                s=s+", "+hex(x)
        return
#
# class to control key usage value
#
class PSA_usage():
    __USAGE = { 'EXPORT':1, 'COPY':2, 'ENCRYPT':0x100,  'DECRYPT':0x200, 'SIGN':0x400, 'VERIFY':0x800, 'SIGN_HASH':0x1000, 'VERIFY_HASH':0x2000, 'DERIVE':0x4000, 'VERIFY_DERIVATION':0x8000 }
    def read(self, value:int)->None:
        self.__usage=value
        if value>=0x10000:
            raise BlobException("usage (%s) not known"% hex(value))

    def bytearray(self, byteorder:str='little')->bytearray:
        try:
            return self.__usage.to_bytes(length=4,byteorder=byteorder, signed=False)
        except:
            raise BlobException("usage not set")

    def get_usages(self)->str:
        lst=ITS_ParamList()
        return lst.keyDicList(self.__USAGE)
        
    def set(self, usageslist:list[str])->None:
        self.__usage=0
        if type(usageslist)==list or type(usageslist)==tuple:
            for usage in usageslist:
                u=usage.upper()
                if u not in self.__USAGE.keys():
                    raise BlobException('usage '+usage+' not known, '+self.get_usages()+' allowed')
                self.__usage|=self.__USAGE[u.strip()]
        if self.__usage==0:
            raise BlobException('need an usage at least')
    
    def set_usage_public_key(self)->None:
        self.__usage|=self.__USAGE["EXPORT"]
    
    def is_set(self, usage:str)->bool:
        try:
            if self.__USAGE[usage]&self.__usage:
                return True
            else:
                return False
        except:
            raise BlobException("usage not known")
    
    def is_key_agreement(self)->bool:
        if self.__USAGE['DERIVE']&self.__usage or self.__USAGE['VERIFY_DERIVATION'] &self.__usage:
            return True
        else:
            return False
    
    def is_sign(self)->bool:
        if self.__USAGE['SIGN']&self.__usage or self.__USAGE['VERIFY']&self.__usage :
            return True
        else:
            return False
            
    def is_hash(self)->bool:
        if self.__USAGE['SIGN_HASH'] &self.__usage or self.__USAGE['VERIFY_HASH'] &self.__usage:
            return True
        else:
            return False
    
    def is_crypt(self)->bool:
        if self.__USAGE['ENCRYPT']&self.__usage or self.__USAGE['DECRYPT']&self.__usage:
            return True
        else:
            return False
    
    def is_allowed(self, allowedusages:list[str])->bool:
        for k,v in self.__USAGE.items():
            if (v & self.__usage) and k not in allowedusages:
                return False
        return True# allowed if all element of __usage is in the list

    def __str__(self)->str:
        res:str=""
        if hasattr(self, '_PSA_usage__usage'):
            for k,v in self.__USAGE.items():
                if self.__usage & v:
                    if res:
                        res=res+', '+k
                    else:
                        res=k
        return res

#
# class to control data Flag value
#
class PSA_flag():
    __FLAG = { "NONE":0, "WRITE_ONCE" : 1 , "NO_CONFIDENTIALITY":2, "NO_REPLAY_PROTECTION":4 }
    def read(self, value:int)->None:
        if value>7:
            raise BlobException("flag value (%s) not known"% hex(value))
        else:
            self.__flag=value

    def bytearray(self, byteorder:str='little')->bytearray:
        try:
            return self.__flag.to_bytes(length=4,byteorder=byteorder, signed=False)
        except:
            raise BlobException("flag not set")

    def get_flags(self)->str:
        lst=ITS_ParamList()
        return lst.keyDicList(self.__FLAG)

    def set(self, flaglist:list[str])->None:
        noneFlag=False
        if type(flaglist)==list or type(flaglist)==tuple:
            self.__flag=0
            for flag in flaglist:
                f=flag.upper()
                if f in self.__FLAG.keys():
                    fs=f.strip()
                    if fs=="NONE":
                       noneFlag=True
                    else:
                        self.__flag|=self.__FLAG[fs]
                else:
                    raise BlobException('flag '+flag+' not known, '+self.get_flags()+' allowed')
            if noneFlag and self.__flag!=0:
                raise BlobException("none or something, but not both")
        else:
            raise BlobException("multiple option error")
  
    def __str__(self)->str:
        res:str=""
        if hasattr(self, '_PSA_flag__flag'):
            if self.__flag==0:
                res='NONE'
            else:
                for k,v in self.__FLAG.items():
                    if v!=0 and self.__flag & v:
                        if res:
                            res=res+', '+k
                        else:
                            res=k
        return res

#
# class to control key type value
#
class PSA_key():
    __ASYMMETRIC={"PUBLIC":0x4001, "PAIR":0x7001, "PRIVATE":0x7001}
    __KEY_TYPE = {'NONE':0, 'RAW':0x1001, 'HMAC':0x1100, 'AES':0x2400, 'DES':0x2301, 'SM4':0x2405, 'CHACHA20':0x2004, 'ARC4':0x2002} #'CAMELLIA': 0x2403
    __CURVE_ECDSA={'SECP_K1':0x17, 'SECP_R1':0x12, 'SECP_R2':0x1b, 'SECT_R1':0x22, 'SECT_R2':0x2b, 'SECT_K1':0x27}
    __CURVE_ECDH={'MONTGOMERY':0x41, 'BRAINPOOL':0x30, 'FRP':0x33}
    __GROUP={'RFC7919':0x3}
    __error_keytype="key not set, {}"
    __RSA={'RSA_OAEP':0, 'RSA_PSS':1, 'RSA_PKCS1V15':2}
    
    def default_asymmetric(self, kname:str, kusage:list[str])->int:
        #0 no value needed
        #1 public or private
        #2 private at least
        #-1 public or private or nothing
        error_form="usages not available for {}"
        if kname is None or kusage is None:
            raise BlobException("null data")
        else:
            keyname=kname.upper()
            asymmetric=0
            if keyname=='RSA_PKCS1V15':
                for keyusage in kusage:
                    keymusage=keyusage.upper()
                    if keymusage in ["SIGN", "SIGN_HASH", "DECRYPT"]:
                        asymmetric=2
                    elif keymusage in ["VERIFY", "VERIFY_HASH", "ENCRYPT", "EXPORT", "COPY"]:
                        asymmetric=max(asymmetric,1)
                if asymmetric==0:
                    raise BlobException(error_form.format(kname))
            elif keyname in self.__CURVE_ECDSA.keys() or keyname=="RSA_PSS":
                for keyusage in kusage:
                    keymusage=keyusage.upper()
                    if keymusage=="SIGN":
                        asymmetric=2
                    elif keymusage in ["EXPORT", "COPY", "VERIFY"]:
                        asymmetric=max(asymmetric,1)
                if asymmetric==0:
                    raise BlobException(error_form.format(kname))
            elif keyname=="RAW":
                asymmetric= -1
            elif keyname=="DH" or keyname in self.__CURVE_ECDH.keys():
                for keyusage in kusage:
                    keymusage=keyusage.upper()
                    if keymusage in ["DERIVE","VERIFY_DERIVATION"]:
                        asymmetric=2
                    elif keymusage in ["EXPORT", "COPY"]:
                        asymmetric=max(asymmetric,1)
                if asymmetric==0:
                    raise BlobException(error_form.format(kname))
            elif keyname in self.__RSA.keys():
                for keyusage in kusage:
                    keymusage=keyusage.upper()
                    if keymusage in ["ENCRYPT", "EXPORT", "COPY"]:
                        asymmetric=max(asymmetric,1)
                    elif keymusage=="DECRYPT":
                        asymmetric=2
                if asymmetric==0:
                    raise BlobException(error_form.format(kname))
            elif keyname not in self.__KEY_TYPE.keys():
                raise BlobException("key %s is unknown" % kname)
        return asymmetric

    def bytearray(self, byteorder:str='little') ->bytearray:
        try:
            return self.__ktype.to_bytes(length=2,byteorder=byteorder, signed=False)
        except:
            raise BlobException("Key type not set")
    
    def _is_containt_asymmetric(self, s:str)->int:
        if s:
            for k,v in self.__ASYMMETRIC.items():
                if k in s:
                    return v
        lst=ITS_ParamList()
        raise BlobException('asymmetric key should be '+lst.keyDicList(self.__ASYMMETRIC))

    def set(self, kname:str, asymmetric:str="")->None:
        if kname is None:
            raise BlobException("key type can't be null")
        else:
            keyname=kname.upper()
            if keyname in self.__CURVE_ECDSA.keys():
                k=self._is_containt_asymmetric(asymmetric.upper())
                if k==self.__ASYMMETRIC['PAIR']:
                    self.__set_ecc_key_pair(keyname, self.__CURVE_ECDSA)
                elif k==self.__ASYMMETRIC['PUBLIC']:
                    self.__set_ecc_public_key(keyname, self.__CURVE_ECDSA)
                else :
                    raise BlobException("unknown kind of key, %s" % asymmetric)
            elif keyname in self.__CURVE_ECDH.keys():
                k=self._is_containt_asymmetric(asymmetric.upper())
                if k==self.__ASYMMETRIC['PAIR']:
                    self.__set_ecc_key_pair(keyname, self.__CURVE_ECDH)
                elif k==self.__ASYMMETRIC['PUBLIC']:
                    self.__set_ecc_public_key(keyname, self.__CURVE_ECDH)
                else :
                    raise BlobException("unknown kind of key, %s" % asymmetric)
            elif keyname in self.__RSA:
                k=self._is_containt_asymmetric(asymmetric.upper())
                self.__ktype=k
            elif keyname=="DH":
                k=self._is_containt_asymmetric(asymmetric.upper())
                if k==self.__ASYMMETRIC['PAIR']:
                    self.set_dh_key_pair('RFC7919')
                elif k==self.__ASYMMETRIC['PUBLIC']:
                    self.set_dh_public_key('RFC7919')
            elif keyname in self.__KEY_TYPE.keys():
                self.__ktype=self.__KEY_TYPE[keyname]
            else:
                raise BlobException("key %s given is unknown" % keyname)

    def read(self, key:int):
        found=False
        if(key>BIT32):
            raise BlobException("key is a 16 bis value, %s given" % hex(key))
        self.__ktype=key
        if self.is_asymmetric():
            found=True
        else:
            for v in self.__KEY_TYPE.values():
                if v==key:
                    found=True
                    break
        if not found:
            raise BlobException("Type of Key (%s), not known" % hex(key))
    
    def key_pair_of_public_key(self)->bool:#PSA_KEY_TYPE_KEY_PAIR_OF_PUBLIC_KEY
        try:
            return self.__ktype & 0x3000
        except Exception as e:
            raise BlobException(self.__error_keytype.format(str(e)))

    def public_of_key_pair(self)->int: #PSA_KEY_TYPE_PUBLIC_KEY_OF_KEY_PAIR
        try:
            return self.__ktype & ~0x3000
        except Exception as e:
            raise BlobException(self.__error_keytype.format(str(e)))


    #Diffie–Hellman key exchange
    def dh_get_family(self)->int:#PSA_KEY_TYPE_DH_GET_FAMILY
        try:
            return self.__ktype & 0x00ff
        except Exception as e:
            raise BlobException(self.__error_keytype.format(str(e)))


    def set_dh_key_pair(self,group:str)->int:#PSA_KEY_TYPE_DH_KEY_PAIR
        try:
            self.__ktype= 0x7200 | self.__GROUP[group]
        except Exception as e:
            raise BlobException(self.__error_keytype.format(str(e)))
            
    def set_dh_public_key(self,group:str)->int:#PSA_KEY_TYPE_DH_PUBLIC_KEY
        try:
            self.__ktype= 0x4200 | self.__GROUP[group]
        except Exception as e:
            raise BlobException(self.__error_keytype.format(str(e)))
    #elliptic curve
    def ecc_get_family(self)->int:#PSA_KEY_TYPE_ECC_GET_FAMILY
        try:
            return self.__ktype & 0x00ff
        except Exception as e:
            raise BlobException(self.__error_keytype.format(str(e)))
    #customisation of PSA define for ecdsa and ecdh:
    def __set_ecc_key_pair(self,curve:str, curves:dict)->int:#PSA_KEY_TYPE_ECC_KEY_PAIR
        try:
            self.__ktype=0x7100 | curves[curve]
        except Exception as e:
            raise BlobException(self.__error_keytype.format(str(e)))
    def __set_ecc_public_key(self, curve:str, curves:dict)->int:#PSA_KEY_TYPE_ECC_PUBLIC_KEY
        try:
            self.__ktype=0x4100 | curves[curve]
        except Exception as e:
            raise BlobException(self.__error_keytype.format(str(e)))
            
    def is_ecdh(self)->bool:
        for x in self.__CURVE_ECDH.values():
            if self.__ktype & 0x00ff==x:
                return True
        return False
    def is_ecdsa(self)->bool:
        for x in self.__CURVE_ECDSA.values():
            if self.__ktype & 0x00ff==x:
                return True
        return False

    #is fct
    def is_raw_data(self)->bool:
        try:
            if self.__ktype == self.__KEY_TYPE['RAW'] or self.__ktype == self.__KEY_TYPE['NONE']:
                return True
            else:
                return False
        except Exception as e:
            raise BlobException(self.__error_keytype.format(str(e)))
    def is_asymmetric(self)->bool:#PSA_KEY_TYPE_IS_ASYMMETRIC
        try:
            if (self.__ktype & 0x4000) == 0x4000:
                return True
            else:
                return False
        except Exception as e:
            raise BlobException(self.__error_keytype.format(str(e)))
            
    def is_symmetric(self)->bool:
            if self.__ktype in [self.__KEY_TYPE['AES'], self.__KEY_TYPE['DES'], self.__KEY_TYPE['SM4'], self.__KEY_TYPE['CHACHA20'], self.__KEY_TYPE['ARC4'] ]:
                return True
            else:
                return False

    def is_dh(self)->bool:#PSA_KEY_TYPE_IS_DH
        try:
            if (self.public_of_key_pair() & 0xff00) == 0x4200:
                return True
            else:
                return False
        except Exception as e:
            raise BlobException(self.__error_keytype.format(str(e)))
    def is_dh_key_pair(self)->bool:#PSA_KEY_TYPE_IS_DH_KEY_PAIR
        try:
            if (self.__ktype & 0xff00) == 0x7200:
                return True
            else:
                return False
        except Exception as e:
            raise BlobException(self.__error_keytype.format(str(e)))
    def is_dh_public_key(self)->bool:#PSA_KEY_TYPE_IS_DH_PUBLIC_KEY
        try:
            if (self.__ktype & 0xff00) == 0x4200:
                return True
            else:
                return False
        except:
            raise BlobException("Key type not set")
    def is_ecc(self)->bool:#PSA_KEY_TYPE_IS_ECC
        try:
            if (self.public_of_key_pair() & 0xff00) == 0x4100:
                return True
            else:
                return False
        except Exception as e:
            raise BlobException(self.__error_keytype.format(str(e)))
    def is_ecc_name(self,keyname:str):
        return keyname in self.__CURVE
    def is_ecc_key_pair(self)->bool:#PSA_KEY_TYPE_IS_ECC_KEY_PAIR
        try:
            if (self.__ktype & 0xff00) == 0x7100:
                return True
            else:
                return False
        except Exception as e:
            raise BlobException(self.__error_keytype.format(str(e)))
    def is_ecc_public_key(self)->bool:#PSA_KEY_TYPE_IS_ECC_PUBLIC_KEY
        try:
            if (self.__ktype & 0xff00) == 0x4100:
                return True
            else:
                return False
        except:
            raise BlobException("Key type not set")
    def is_key_pair(self)->bool:#PSA_KEY_TYPE_IS_KEY_PAIR
        try:
            if (self.__ktype & 0x7000) == 0x7000:
                return True
            else:
                return False
        except Exception as e:
            raise BlobException(self.__error_keytype.format(str(e)))
    def is_public_key(self)->bool:#PSA_KEY_TYPE_IS_PUBLIC_KEY
        try:
            if (self.__ktype & 0x7000) == 0x4000:
                return True
            else:
                return False
        except Exception as e:
            raise BlobException(self.__error_keytype.format(str(e)))
    def is_rsa(self)->bool:#PSA_KEY_TYPE_IS_RSA
        try:
            if self.public_of_key_pair() == 0x4001:
                return True
            else:
                return False
        except Exception as e:
            raise BlobException(self.__error_keytype.format(str(e)))
    def is_unstructured(self)->bool:#PSA_KEY_TYPE_IS_UNSTRUCTURED
        try:
            if (self.__ktype & 0x7000) == 0x1000 or (self.__ktype & 0x7000) == 0x2000:
                return True
            else:
                return False
        except Exception as e:
            raise BlobException(self.__error_keytype.format(str(e)))
    def is_hmac(self)->bool:
        try:
            if self.__ktype==0x1100:
                return True
            else:
                return False
        except Exception as e:
            raise BlobException(self.__error_keytype.format(str(e)))
        

    def get_Curve(self):
        curve = {**self.__CURVE_ECDSA, **self.__CURVE_ECDH }
        return curve.keys()
    def get_DerivationCurve(self):
        return self.__CURVE_ECDH.keys()
    def get_GroupNames(self):
        return self.__GROUP.keys()
    def get_RSA(self):
        return self.__RSA.keys()
    def get_Names(self)->str:
        lst=ITS_ParamList()
        obj= {**self.__KEY_TYPE, **self.__RSA, **self.__CURVE_ECDSA, **self.__CURVE_ECDH}
        return lst.keyDicList(obj)

    def __str__(self)->str:
        if hasattr(self, '_PSA_key__ktype'):
            if self.is_dh():
                return 'DH'
            elif self.is_ecc():
                curve:int=self.ecc_get_family()
                curves = {**self.__CURVE_ECDSA, **self.__CURVE_ECDH}
                for k,v in curves.items():
                    if v==curve:
                        return k
                raise BlobException('Unknown curve key %d' %hex(self.__ktype))
            elif self.is_rsa():
                return 'RSA'
            elif self.is_asymmetric():
                raise BlobException('Unknown asymmetric key %d' %hex(self.__ktype))
            else:
                for k,v in self.__KEY_TYPE.items():
                    if v==self.__ktype:
                        return k
                raise BlobException('Unknown key %d' %hex(self.__ktype))
        else:
            return ""

#
# class to control value of algo
#
class PSA_algo():
    __ALGO= {'NONE':0x0, "Poly1305":0x05100500 , "CBC": 0x04404000, "CCM":0x05500100, 'GCM':0x05500200, 'CFB':0x04c01100, 'ECB':0x04404400, 'CTR':0x04c01000, \
    'CBC_PKCS7':0x04404100, 'CBC_MAC':0x03c00100, 'CMAC':0x03c00200, 'XTS':0x0440ff00, 'OFB':0x04c01200, 'PKCS1V15_SIGN_RAW':0x06000200, 'PKCS1V15_CRYPT':0x07000200, \
    'STREAM_CIPHER':0x04800100}
    __DIGEST={'NONE':0x0, 'MD2':0x02000001, 'MD4':0x02000002, 'MD5':0x02000003, "RIPEMD160":0x02000004, 'SHA1':0x02000005, "SHA224":0x02000008, 'SHA256':0x02000009, \
    'SHA384':0x0200000a, 'SHA512':0x0200000b, 'SHA512_224':0x0200000c, 'SHA512_256':0x0200000d, 'SHA3_224':0x02000010, 'SHA3_256':0x02000011, 'SHA3_384':0x02000012, 'SHA3_512':0x02000013, 'SM3':0x02000014}
    __KEYAGREEMENT={'DH':0x09010000, 'ECDH':0x09020000}
    __KDF={"DH":{'FFDH':0x09010000}, 'ECDH':{'HKDF':0x08000100,"TLS12_PRF":0x08000200, "PSK_TO_MS":0x08000300, 'RAW':0}}
    __DIGEST_ANY=0x020000ff
    __ECDSA_ANY=0x06000600
    __COMPATIBLE={'AES':['CBC','CCM','GCM','CFB','ECB','CTR','OFB','XTS', 'CMAC'], 'DES':['CBC','ECB'], 'CHACHA20':['Poly1305','STREAM_CIPHER'], \
            'ARC4':['STREAM_CIPHER'], 'SM4':['CBC', 'CFB','ECB','CTR','OFB', 'XTS'], 'RSA':['PKCS1V15_CRYPT', 'PKCS1V15_SIGN_RAW'] } #'CAMELLIA':['CAMELIA','XTS']
    __error_algo="algo/digest not set, {}"


    def get_Names(self, keytype:str)->str:
        s=""
        kt=keytype.upper()
        if kt in self.__COMPATIBLE.keys() and kt!='RSA':
            for x in self.__COMPATIBLE[keytype]:
                if s=="":
                    s=x
                else:
                    s=s+", "+x
        elif kt in self.__KEYAGREEMENT.keys():
            for x in self.__KDF[kt].keys():
                if s=="":
                    s=x
                else:
                    s=s+", "+x
        elif kt in PSA_key().get_DerivationCurve():
            for x in self.__KDF["ECDH"].keys():
                if s=="":
                    s=x
                else:
                    s=s+", "+x
        return s

    def get_digestNames(self)->str:
        lst=ITS_ParamList()
        d:dict=self.__DIGEST
        return lst.keyDicList(d)
        

    def is_derivation(self, arg:str)->bool:
        for elem in self.__KDF.keys():
            if arg in self.__KDF[elem].keys():
                return True
        return False

    def bytearray(self, byteorder='little')->bytearray:
        try:
            return self.__algo.to_bytes(4,byteorder)
        except:
            raise BlobException("algo/digest not set")
        
   
    def set(self, algo:str, key_type:PSA_key, option:str="")->None:
        ualgo=algo.upper()
        if key_type.is_raw_data():
            try:
                self.__algo=int(algo)
            except:
                self.__algo=0
        elif not (key_type.is_asymmetric() or  key_type.is_hmac()) or option=="PKCS1V15":
            self.__algo=self.__ALGO[ualgo]
            if ualgo not in self.__COMPATIBLE[str(key_type)]:
                raise BlobException('%s and %s not compatible'% (algo, str(key_type)))
        elif ualgo in self.__DIGEST.keys():
            if key_type.is_rsa():
                if option=="OAEP":
                    self.set_rsa_oaep(ualgo)
                elif option=="SIGN":
                    self.set_rsa_pkcs1v15_sign(ualgo)
                elif option=="PSS":
                    self.set_rsa_pss(ualgo)
            elif key_type.is_hmac():
                self.set_hmac(ualgo)
            elif key_type.is_ecdsa():
                if "ECDSA"==option:
                    self.set_ecdsa(ualgo)
                elif "ECDSAD"==option:
                    self.set_deterministic_ecdsa(ualgo)
            elif key_type.is_ecdh():
                if option in self.__KDF['ECDH'].keys():
                    self.set_key_agreement("ECDH", ualgo, option)
            elif key_type.is_dh():
                self.set_key_agreement("DH", ualgo, "FFDH")
            else : raise BlobException("algorithm undetermined for %s" %str(key_type))
        elif key_type.is_ecdsa():
            if "HASH"==option:
                self.__algo=self.__ECDSA_ANY=0x06000600
            elif algo:
                raise BlobException("can't set %s and %s "% (algo, str(key_type)))
        elif key_type.is_ecdh():
            self.set_key_agreement("ECDH", "NONE", "RAW")
        else:
            raise BlobException("can't set %s and %s "% (algo, str(key_type)))

    def read(self, algo:int):
        found=False
        self.__algo=algo
        for v in self.__ALGO.values():
            if v==algo:
                found=True
                break
        if not found:
            halgo=self.get_hash()
            for v in self.__DIGEST.values():
                if v==halgo:
                    found=True
                    break
        if not found:
            if self.is_ecdh() or self.is_ffdh():
                found=True
        if not found:
            raise BlobException('unkwon algo '+hex(algo))

    def is_aead(self)->bool: #PSA_ALG_IS_AEAD
        try:
            return ((self.__algo & 0x7f000000) == 0x05000000)
        except Exception as e:
            raise BlobException(self.__error_algo.format(str(e)))
    def is_aead_on_block_cipher(self)->bool:#PSA_ALG_IS_AEAD_ON_BLOCK_CIPHER
        try:
            return ((self.__algo & 0x7f400000) == 0x05400000)
        except Exception as e:
            raise BlobException(self.__error_algo.format(str(e)))
    def is_asymmetric_encryption(self)->bool:#PSA_ALG_IS_ASYMMETRIC_ENCRYPTION
        try:
            return ((self.__algo & 0x7f000000) == 0x07000000)
        except Exception as e:
            raise BlobException(self.__error_algo.format(str(e)))
    def is_block_cipher_mac(self)->bool:#PSA_ALG_IS_BLOCK_CIPHER_MAC
        try:
            return (self.__algo & 0x7fc00000) == 0x03c00000
        except Exception as e:
            raise BlobException(self.__error_algo.format(str(e)))
    def is_cipher(self)->bool: #PSA_ALG_IS_BLOCK_CIPHER_MAC
        try:
            return (self.__algo & 0x7f000000) == 0x03000000
        except Exception as e:
            raise BlobException("algo not set")
    def is_deterministic_ecdsa(self)->bool:#PSA_ALG_IS_DETERMINISTIC_ECDSA
        try:
            return ((self.__algo & ~0x000000ff) == 0x06000700)
        except Exception as e:
            raise BlobException(self.__error_algo.format(str(e)))
    def is_ecdh(self)->bool:#PSA_ALG_IS_ECDH
        try:
            return (self.__algo & 0x7fff0000) == 0x09020000
        except Exception as e:
            raise BlobException(self.__error_algo.format(str(e)))
    def is_ecdsa(self)->bool:#PSA_ALG_IS_ECDSA
        try:
            return ((self.__algo & ~0x000001ff) == 0x06000600)
        except Exception as e:
            raise BlobException(self.__error_algo.format(str(e)))
    def is_ffdh(self)->bool:#PSA_ALG_IS_FFDH
        try:
            return ((self.__algo & 0x7fff0000) == 0x09010000)
        except Exception as e:
            raise BlobException(self.__error_algo.format(str(e)))
    def is_hash(self)->bool:#PSA_ALG_IS_HASH
        try:
            if (self.__algo & 0x7f000000) == 0x02000000:
                return True
            else:
                return False
        except Exception as e:
            raise BlobException(self.__error_algo.format(str(e)))
    def is_hash_and_sign(self)->bool:#PSA_ALG_IS_HASH_AND_SIGN
        try:
            return (self.is_rsa_pss() or self.is_rsa_pkcs1v15_sign() or self.is_ecdsa())
        except Exception as e:
            raise BlobException(self.__error_algo.format(str(e)))

    def is_hmac(self)->bool:#PSA_ALG_IS_HMAC
        try:
            return ((self.__algo & 0x7fc0ff00) == 0x03800000)
        except Exception as e:
            raise BlobException(self.__error_algo.format(str(e)))
    def is_key_agreement(self)->bool:#PSA_ALG_IS_KEY_AGREEMENT
        try:
            return ((self.__algo & 0x7f000000) == 0x09000000)
        except Exception as e:
            raise BlobException(self.__error_algo.format(str(e)))
    def is_key_derivation(self)->bool:#PSA_ALG_IS_KEY_DERIVATION
        try:
            return ((self.__algo & 0x7f000000) == 0x08000000)
        except Exception as e:
            raise BlobException(self.__error_algo.format(str(e)))
    def is_mac(self)->bool:#PSA_ALG_IS_MAC
        try:
            return ((self.__algo & 0x7f000000) == 0x03000000)
        except Exception as e:
            raise BlobException(self.__error_algo.format(str(e)))
    def is_randomized_ecdsa(self)->bool:#PSA_ALG_IS_RANDOMIZED_ECDSA
        try:
            return ((self.__algo & ~0x000000ff) == 0x06000600)
        except Exception as e:
            raise BlobException(self.__error_algo.format(str(e)))
    def is_raw_key_agreement(self)->bool:#PSA_ALG_IS_RAW_KEY_AGREEMENT
        try:
            return ((self.__algo & 0x7f00ffff) == 0x09000000)
        except Exception as e:
            raise BlobException(self.__error_algo.format(str(e)))
    def is_rsa_oaep(self)->bool:#PSA_ALG_IS_RSA_OAEP
        try:
            return ((self.__algo & ~0x000000ff) == 0x07000300)
        except Exception as e:
            raise BlobException(self.__error_algo.format(str(e)))
    def is_rsa_pkcs1v15_sign(self)->bool:#PSA_ALG_IS_RSA_PKCS1V15_SIGN
        try:
            return ((self.__algo & ~0x000000ff) == 0x06000200)
        except Exception as et:
            raise BlobException(self.__error_algo.format(str(e)))
    def is_rsa_pss(self)->bool:#PSA_ALG_IS_RSA_PSS
        try:
            return ((self.__algo & ~0x000000ff) == 0x06000300)
        except Exception as e:
            raise BlobException(self.__error_algo.format(str(e)))
    def is_sign(self)->bool:#PSA_ALG_IS_SIGN
        try:
            return ((self.__algo & 0x7f000000) == 0x06000000)
        except Exception as e:
            raise BlobException(self.__error_algo.format(str(e)))
    def is_sign_hash(self)->bool:#PSA_ALG_IS_SIGN_HASH
        try:
            return self.is_sign(self.__algo)
        except Exception as e:
            raise BlobException(self.__error_algo.format(str(e)))
    def is_sign_message(self)->bool:#PSA_ALG_IS_SIGN_MESSAGE
        try:
            return self.is_sign() and self.__algo != self.__ECDSA_ANY and self.__algo!= self.__ALGO['PKCS1V15_SIGN_RAW']
        except Exception as e:
            raise BlobException(self.__error_algo.format(str(e)))
    def is_ECDSA_any(self)->bool:
        try:
            return self.__algo == self.__ECDSA_ANY
        except Exception as e:
            raise BlobException(self.__error_algo.format(str(e)))
    def is_stream_cipher(self)->bool:#PSA_ALG_IS_STREAM_CIPHER
        try:
            return ((self.__algo & 0x7f800000) == 0x04800000)
        except Exception as e:
            raise BlobException(self.__error_algo)
    def is_wildcard(self)->bool:#PSA_ALG_IS_WILDCARD
        try:
            return (self.__algo & 0x000000ff)==0
        except Exception as e:
            raise BlobException(self.__error_algo.format(str(e)))

    def get_hash(self)->int:#PSA_ALG_GET_HASH
        try:
            if (self.__algo & 0x000000ff) == 0:
                return self.__DIGEST['NONE']
            else:
                return 0x02000000 | (self.__algo & 0x000000ff)
        except Exception as e:
            raise BlobException(self.__error_algo.format(str(e)))

    def aead_with_default_length_tag(aead_alg:int)->int:#PSA_ALG_AEAD_WITH_DEFAULT_LENGTH_TAG
        try:
            if (aead_alg & ~0x003f0000) == 0x05400100:
                return self.ALGO['CCM']
            elif (aead_alg & ~0x003f0000) == 0x05400200:
                return self.ALGO['GCM']
            elif (aead_alg & ~0x003f0000) == 0x05000500:
                return self.ALGO['Poly1305']
            else:
                return self.ALGO['NONE']
        except Exception as e:
            raise BlobException(self.__error_algo.format(str(e)))
    def aead_with_shortened_tag(self, aead_alg:int, tag_length:int)->int:#PSA_ALG_AEAD_WITH_SHORTENED_TAG
        try:
            return (aead_alg & ~0x003f0000) | ((tag_length & 0x3f) << 16)
        except Exception as e:
            raise BlobException(self.__error_algo.format(str(e)))
        
    def set_deterministic_ecdsa(self, hash_alg:str)->int:#PSA_ALG_DETERMINISTIC_ECDSA
        try:
            self.__algo=0x06000700 | (self.__DIGEST[hash_alg] & 0x000000ff)
        except Exception as e:
            raise BlobException(self.__error_algo.format(str(e)))
    
    def set_ecdsa(self, hash_alg:str)->int:#PSA_ALG_ECDSA
        try:
            self.__algo=0x06000600 | (self.__DIGEST[hash_alg] & 0x000000ff)
        except Exception as e:
            raise BlobException(self.__error_algo.format(str(e)))
        
    def set_full_length_mac(self, mac_alg:int)->int:#PSA_ALG_FULL_LENGTH_MAC
        try:
            self.__algo= mac_alg & ~0x003f0000
        except Exception as e:
            raise BlobException(self.__error_algo.format(str(e)))
    def key_agreement_getbase(self)->int:#PSA_ALG_KEY_AGREEMENT_GET_BASE
        try:
            return self.__algo & 0xffff0000
        except Exception as e:
            raise BlobException(self.__error_algo.format(str(e)))
    def key_agreement_get_kdf(self)->int:#PSA_ALG_KEY_AGREEMENT_GET_KDF
        try:
            return self.__algo & 0xfe00ff00
        except Exception as e:
            raise BlobException(self.__error_algo.format(str(e)))
    def set_rsa_oaep(self, hash_alg:str)->int:#PSA_ALG_RSA_OAEP
        try:
            if hash_alg in self.__DIGEST.keys():
                self.__algo=0x07000300 | (self.__DIGEST[hash_alg] & 0x000000ff)
            else:
                raise BlobException("Unknown Hash algo, "+self.__error_algo.format(str(e)))
        except Exception as e:
            raise BlobException(self.__error_algo.format(str(e)))
            
    def set_key_agreement(self, keytype:str, digest:str, kdf_alg:str):#PSA_ALG_KEY_AGREEMENT"
        try:
            ka_alg_value=self.__KEYAGREEMENT[keytype]
            digest=self.__DIGEST[digest] & 0x000000ff
            kdf_alg_value=self.__KDF[keytype][kdf_alg]
            self.__algo=ka_alg_value|kdf_alg_value|digest
        except Exception as e:
            raise BlobException(self.__error_algo.format(str(e)))

    def set_rsa_pkcs1v15_sign(self, hash_alg:str)->int:#PSA_ALG_RSA_PKCS1V15_SIGN
        try:
            self.__algo=0x06000200 | (self.__DIGEST[hash_alg] & 0x000000ff)
        except Exception as e:
            raise BlobException(self.__error_algo)
    def set_rsa_pss(self, hash_alg:str)->int:#PSA_ALG_RSA_PSS
        try:
            self.__algo=0x06000300 | (self.__DIGEST[hash_alg] & 0x000000ff)
        except Exception as e:
            raise BlobException(self.__error_algo.format(str(e)))

    def set_truncated_mac(self,mac_alg:int, mac_length:int)->int:#PSA_ALG_TRUNCATED_MAC
        try:
            self.__algo=  (mac_alg & ~0x003f0000) | ((mac_length & 0x3f) << 16)
        except Exception as e:
            raise BlobException(self.__error_algo.format(str(e)))
    def set_hmac(self, hash_alg:str)->int:#PSA_ALG_HMAC
        try:
            self.__algo= 0x03800000 | (self.__DIGEST[hash_alg] & 0x000000ff)
        except Exception as e:
            raise BlobException(self.__error_algo.format(str(e)))
    def get_raw(self)->int:
        return self.__algo
    def __str__(self)->str:
        if hasattr(self, '_PSA_algo__algo'):
            found=""
            for k,v in self.__ALGO.items():
                if v==self.__algo:
                    found=k
                    break
            if not found:
                hkdfalgo=""
                ahash=self.get_hash()
                if (ahash==0):
                    if self.is_ECDSA_any():
                        found="ECDSA_any"
                    else:
                        found="NONE"
                else:
                    for k,v in self.__DIGEST.items():
                        if v==ahash:
                            found=k
                            break
                if self.is_ecdh():
                    hkdf=self.key_agreement_get_kdf()
                    for k,v in self.__KDF['ECDH'].items():
                         if v==hkdf:
                            found=k+" "+found
                            break
                error='Unknown Hash '+hex(self.__algo)
            if not found:
                raise BlobException(error)
            else:
                return found
        else:
            return ""

#
# class to control number of bits of a key
#
class PSA_bits():
    __AES_LENGTH = [ 128, 192, 256 ]
    __XT_LENGTH = [ 256, 384,512 ]
    __DES_LENGTH = [ 64, 128, 192 ]
    __RSA_LENGTH = [ 2048, 3072 ]
    #__CAMELLIA_LENGTH = [ 128, 192, 256 ]
    #__CAMELLIA_XTLENGTH = [ 256, 384, 512 ]
    
    __ECC_LENGTH={"SECP_K1":[192,225,256], 'SECP_R1':[192,224,256,384,521], 'SECP_R2':[160], 'SECT_K1':[163,233,239,283,409,571], 'SECT_R1':[163,233,283,409,571], \
        'SECT_R2':[163], 'BRAINPOOL':[160,192,224,256,320,384,512], 'MONTGOMERY':[255,448], 'FRP':[256]}
    __FFDH_LENGTH=[2048, 3072, 4096, 6144, 8192]
    __LENGTH={'AES':{'CBC':__AES_LENGTH,'CCM':__AES_LENGTH, 'GCM':__AES_LENGTH,'CFB':__AES_LENGTH,'ECB':__AES_LENGTH,'CTR':__AES_LENGTH,'OFB':__AES_LENGTH, 'XTS':__XT_LENGTH}, \
    'DES':{'CBC':__AES_LENGTH,'ECB':__AES_LENGTH}, 'SM4':{'CBC':[128],'CFB':[128],'ECB':[128],'CTR':[128],'OFB':[128], 'XTS':[128]}, \
    'CHACHA20' :{'Poly1305':[256],'STREAM_CIPHER':[256]},'ARC4':{ 'STREAM_CIPHER':[256]}, 'SM4':{'XTS':[128]} } #'CAMELLIA':{'CAMELLIA':__CAMELLIA_LENGTH,'XTS':__CAMELLIA_XTLENGTH}

    def get_Names(self, keytype:str)->str:
        s=""
        ukeytype=keytype.upper()
        if ukeytype in self.__LENGTH.keys():
            for k,v in self.__LENGTH[ukeytype].items():
                if s=="":
                    s=k+":"+str(v)+"\n"
                else:
                    s=s+k+":"+str(v)+"\n"
        elif ukeytype in ['RSA_OAEP', 'RSA_PSS', 'RSA_PKCS1V15']:
            s=str(self.__RSA_LENGTH)
        elif ukeytype in self.__ECC_LENGTH.keys():
            s=str(self.__ECC_LENGTH[ukeytype])
        return s

    def set(self, key:PSA_key,bits:int, algo:str="")->None:
        self.value=bits
        if key.is_rsa():
            if bits not in self.__RSA_LENGTH:
                raise BlobException("Incorrect bits")
        elif key.is_ecc():
            if bits not in self.__ECC_LENGTH[str(key)]:
                raise BlobException("Incorrect bits")
        elif key.is_dh():
            if bits not in self.__FFDH_LENGTH:
                raise BlobException("Incorrect bits")
        elif key.is_hmac():
            if bits %8 ==0:
                raise BlobException("Incorrect bits, modulo 8")
        else:
            keyname=str(key)
            if keyname=='ARC':
                if not (bits>=40 and bits<=2048 and bits%8==0):
                    raise BlobException("Incorrect bits")
            elif keyname!="RAW" and bits not in self.__LENGTH[keyname][algo.upper()]:
                raise BlobException("Incorrect bits")
    
    def bytearray(self, byteorder='little') ->bytearray:
        return self.value.to_bytes(2,byteorder)
    
#
# class to load a file
#
class ITS_keyLoader():
    #as decripbe in <openssl/pem.h>
    Pem_Descriptor=["CERTIFICATE PAIR", "ANY PRIVATE KEY", "PUBLIC KEY", "RSA PRIVATE KEY", "RSA PUBLIC KEY", "DSA PRIVATE KEY", "DSA PUBLIC KEY", "ENCRYPTED PRIVATE KEY", "PRIVATE KEY", "ECDSA PUBLIC KEY", "EC PRIVATE KEY"]
        # "PARAMETERS" "EC PARAMETERS", "DH PARAMETERS", "X9.42 DH PARAMETERS", "SSL SESSION PARAMETERS", "DSA PARAMETERS", "NEW CERTIFICATE REQUEST", "CERTIFICATE REQUEST", "X509 CRL", "TRUSTED CERTIFICATE", "X509 CERTIFICATE", "CERTIFICATE"
        #"PKCS7", "PKCS #7 SIGNED DATA", "CMS"
    def isPemType(self, keytype:str)->bool:
        Pem_KEYTPE=['RSA', 'EC', 'DSA', 'ECDSA', 'ENCRYPTED']
        found=False
        for t in Pem_KEYTPE:
            if keytype[:len(t)]==t:
                found=True
        return found

    def __getPem(self,descriptor:str, pemfile:str)->str:
        d:str=descriptor.upper()
        if d not in self.Pem_Descriptor:
            lst=ITS_ParamList()
            raise BlobException("Pem format unknown, choose : "+lst.keyDicList(self.Pem_Descriptor))
        else:
            pemstring=pemfile.replace('\n','')
            bstr="-----BEGIN "+d+"-----"
            idxb:int = pemstring.index(bstr)#launch VALUE Error if not present
            idxe:int = pemstring.index("-----END "+d+"-----")
            return pemstring[idxb+len(bstr):idxe]

    def getFileType(self)->str:
        return {'RAW':0,'HEX':1,'BASE64':2, 'PEM':3}
    
    def read(self, file, file_type:str, option:str='', byteorder:str='little')->bytearray:
        data=bytearray()
        try:
            lst=list(self.getFileType())
            filetype=file_type.upper()
            if filetype==lst[0]:#raw
                data=file.read()
            elif filetype==lst[1]:#hex generated with openssl
                indice:int=0
                hexadata=file.read()
                indice=0
                #openssl produce file with a "= " delimiter
                for i in range(len(hexadata)-1):
                    if str(hexadata[i])=="=" and str(hexadata[i+1])==" ":
                        indice=i+1
                        break
                #no delimiter. try all
                while indice<len(hexadata)-1:
                    octet=hexadata[indice:indice+2]
                    if octet[0]!=13 and octet[1]!=10:
                        value=int(octet, 16)
                        data.append(value)
                        indice+=2
                    else:
                        indice+=1
            elif filetype==lst[2]:#base64
                data64:str=file.read().decode()
                data=bytearray(base64.b64decode(data64))
            elif filetype==lst[3]:#PEM
                data=file.read()
                data64=self.__getPem(option, data.decode())
                if data64:
                    data=bytearray(base64.b64decode(data64))
            else:
                raise BlobException("don't know file format "+filetype)
            if len(data)==0:
                raise BlobException("empty file error")
        except Exception as e:
            raise BlobException("can't read key file, "+str(e))
        
        return data


class BlobException(Exception):
    def __init__(self, message):
        self.message = message
        super().__init__(message)

    def __str__(self):
        return self.message

# Node class
class ITS_data_area():
    # Function to initialise the ITS object
    _form_owner="\t\towner:\t0x{:08X}\n"
    _form_flag="\t\tflag:\t{}\n"
    _form_id="\n\tDATA\tId:\t0x{:016X}\n"
    _form_data="\t\tvalue length :\t{}\n\t\tdata :\t0x{}"
    
    #def __init__(self):
        
    #initialisation from a file
    def read(self, data : bytearray, byteorder) ->int:
        indice: int=0
        self.uuid=int.from_bytes(data[indice:indice+8], byteorder=byteorder, signed=False)
        if(self.uuid==0): raise("UUID can't be null")
        indice+=8
        self.owner=int.from_bytes(data[indice:indice+4], byteorder=byteorder, signed=False)
        if self.owner==0: raise("Owner can't be null")
        indice+=4
        self.flag=PSA_flag()
        self.flag.read(int.from_bytes(data[indice:indice+4], byteorder=byteorder, signed=False))
        indice+=4
        lendata=int.from_bytes(data[indice:indice+4], byteorder=byteorder, signed=False)
        if(lendata==0): raise BlobException("length can't be null")
        indice+=4
        self.data=data[indice:indice+lendata]
        return len(data)-indice-lendata
        
    #initialisation from interactive users
    def init(self, owner : int, objectid : int, data : bytearray, flags:list[str] ) ->None:
        if type(owner)!=int or type(objectid)!=int or not (type(data)==bytearray or type(data)==bytes) or not (type(flags)==list or type(flags)==tuple):
            raise BlobException("multiple option error")
        if owner is not None and owner<=BIT32 and owner>0:
            self.owner=owner
        else:
            raise BlobException("Owner out of range (1...0xFFFF")
        #uuid 64 bits
        if(objectid is not None and objectid>0):
            self.uuid=objectid
        else:
            raise BlobException("UUID cout of range (1...0xFFFF FFFF")
        found=False
        if data is not None:
            lendata=len(data)
            if (lendata!=0 and lendata<=BIT32):
                self.data=data
                found=True
        if not found:
            raise BlobException("Data out of range [1 ..#FFFF FFFF")
        found=False
        self.flag=PSA_flag()
        self.flag.set(flags)

        
    def uniq(self, its : 'ITS_data_area') -> bool:
        if(its.uuid is not None and its.owner is not None and self.uuid==its.uuid and self.owner==its.owner):
            return False
        else:
            return True
        
    #extract printable information. Print do not produce error, just error log
    def __str__(self) ->str:
        try:
            i=len(self.data)
            if(i!=0):
                str_data=self.data.hex()
                return self._form_id.format(self.uuid)+ \
                       self._form_owner.format(self.owner)+ \
                       self._form_flag.format(self.flag)+ \
                       self._form_data.format(i, str_data)
            else:
                sys.stderr.write("No data, please init the structure")
        except Exception as e:
            sys.stderr.write("Internal error\n"+str(e))
        return ""
    
    #transfome the data to a ITS area
    def bytearray(self, byteorder='little')->bytearray:
        ba=bytearray()
        try:
            ba.extend(self.uuid.to_bytes(length=8, byteorder=byteorder, signed=False))
            ba.extend(self.owner.to_bytes(length=4, byteorder=byteorder, signed=False))
            ba.extend(self.flag.bytearray())
            lendata=len(self.data)
            ba.extend(lendata.to_bytes(length=4, byteorder=byteorder, signed=False))
            ba.extend(self.data)
        except Exception as e:
            raise BlobException("Can't build the Data area\n"+str(e))
        return ba
    
    def type(self) ->int:
        return DATA_TYPE['ITS']

class ITS_key_area():
    _form_name="\n\t{} key ({}b)"
    _form_id="\n\t\tId:\t0x{:08X}"
    _form_owner="\n\t\towner:\t0x{:08X}"
    _form_lifetime=["\n\t\tpersistence:\t0x{:02X}, location:\t0x{:012X}", "\n\t\tpersistence:\t{}, location:\t0x{:012X}"]
    
    _form_algo="\n\t\targument(s):\t{}"
    _form_usage="\n\t\tusage(s):\t{}"
    _form_value="\n\t\tdata length:\t{}\n\t\tvalue:\t0x{}"
    
    #initialisation from file
    def read(self, data : bytearray, byteorder : str)->int:
        indice:int=0
        self.key_id=int.from_bytes(data[indice:indice+4], byteorder=byteorder, signed=False)
        if self.key_id==0: raise BlobException("Key id can't be null")
        indice+=4
        self.owner=int.from_bytes(data[indice:indice+4], byteorder=byteorder, signed=False)
        if self.owner==0:raise BlobException("Owner can't be null")
        indice+=4
        self.lifetime=int.from_bytes(data[indice:indice+4], byteorder=byteorder, signed=False)
        if self.lifetime==0: raise BlobException("Lifetime can't be null")
        indice+=4
        try:
            self.key_type=PSA_key()
            self.key_type.read(int.from_bytes(data[indice:indice+2], byteorder=byteorder, signed=False))
            indice+=2
            bits=int.from_bytes(data[indice:indice+2], byteorder=byteorder, signed=False)
            indice+=2
            self.usage=PSA_usage()
            usage=int.from_bytes(data[indice:indice+4], byteorder=byteorder, signed=False)
            indice+=4
            self.usage.read(usage)
            self.algo1=PSA_algo()
            self.algo1.read(int.from_bytes(data[indice:indice+4], byteorder=byteorder, signed=False))
            indice+=4
            self.algo2=PSA_algo()
            self.algo2.read(int.from_bytes(data[indice:indice+4], byteorder=byteorder, signed=False))
            indice+=4
            self.bits=PSA_bits()#to set after key & algo
            self.bits.set(self.key_type, bits, str(self.algo1))
        except Exception as e:
            raise BlobException("unable to set variable, "+str(e))
        lendata=int.from_bytes(data[indice:indice+4], byteorder=byteorder, signed=False)
        if lendata==0: raise BlobException("Length can't be null")
        indice+=4
        self.value=data[indice:indice+lendata]
        return indice+lendata


    #initialisation the key from interactive user
    def init(self, owner : int, objectid : int, lifetime : int, type_key:str, digest:str, algo : str, bits:int,  usages:list[str] , value:bytearray, asymmetric:str="", deterministic:bool=False)->None:
        err_algo_form:str="no algorithm is needed for {}"
        if type(owner)!=int or type(objectid)!=int or type(lifetime)!=int or type(type_key)!=str or type(bits)!=int or not (type(digest)==str or digest is None) or not (type(algo)==str or algo is None)\
            or not (type(value)==bytearray or type(value)==bytes) or type(asymmetric)!=str or type(deterministic)!=bool or not (type(usages)==list or type(usages)==tuple):
            raise BlobException("multiple option error")
        if(lifetime is None or lifetime>BIT32 or lifetime<0):
            raise BlobException("lifetime out of range [0..0xFFFF]")
        else:
            self.lifetime=lifetime
        if(objectid is None or objectid>BIT32 or objectid<=0):
            raise BlobException("key id out of range [1..0xFFFF]")
        else:
            self.key_id = objectid
        if(owner is None or owner>BIT32 or owner<=0) :
            raise BlobException("key owner out of range [1..0xFFFF]")
        else:
            self.owner=owner
        self.__add_data(value)
        try:
            option=""
            if type_key=="DH":
                raise BlobException("DH not implemented")
            self.key_type=PSA_key()
            self.algo1=PSA_algo()
            self.algo2=PSA_algo()
            self.usage=PSA_usage()
            self.bits=PSA_bits()
            self.usage.set(usages)
            self.key_type.set(type_key, asymmetric)
            if digest=="NONE" or digest is None or not digest:
                if (self.key_type.is_rsa() and type_key!="RSA_PKCS1V15") or self.key_type.is_hmac():
                    raise BlobException("key type %s needs digest " % type_key)
                else:
                    digest="NONE"
            elif self.key_type.is_symmetric():
                raise BlobException("no digest for symmetric key")
            else:
                digest=digest.upper()
            if not algo or algo is None:
                if self.key_type.is_ecdh() or self.key_type.is_symmetric():
                    raise BlobException(str(self.key_type)+" need algorithm")
            elif not (self.key_type.is_ecdh() or self.key_type.is_symmetric()):
                raise BlobException(err_algo_form.format(self.key_type))
            else:
                algo=algo.upper()
            if self.key_type.is_ecdsa():
                if self.usage.is_sign():
                    if deterministic:
                        option="ECDSAD"
                    else:
                        option="ECDSA"
                elif self.usage.is_hash():
                    option="HASH"
                else:
                    raise BlobException("usage not allowed for ECDSA "+type_key)
            elif self.key_type.is_ecdh():
                if self.algo1.is_derivation(algo):
                    option=algo
                else:
                    raise BlobException("derivation algorithms not allowed for ECDH "+derivation)
            elif self.key_type.is_rsa():
                if type_key=="RSA_OAEP":
                    option="OAEP"
                elif type_key=="RSA_PKCS1V15" and self.usage.is_sign() and digest !="NONE":
                    option="SIGN"
                elif type_key=="RSA_PKCS1V15" and self.usage.is_crypt():
                    if digest!="NONE": raise BlobException("no argument allowed for encrypt/decrypt %s, %s given" %(type_key, algo))
                    digest="PKCS1V15_CRYPT"
                    option="PKCS1V15"
                elif type_key=="RSA_PKCS1V15" and self.usage.is_hash():
                    digest="PKCS1V15_SIGN_RAW"
                    option="PKCS1V15"
                elif type_key=="RSA_PSS"and self.usage.is_sign():
                    option="PSS"
            if self.key_type.is_symmetric():
                algokeyargument=algo
            else:
                algokeyargument=digest
            if deterministic and option!="ECDSAD":
                raise BlobException("deterministic option for ECDSA only")
            if self.key_type.is_asymmetric() and self.key_type.is_public_key():
                self.usage.set_usage_public_key()
            self.algo1.set(algokeyargument, self.key_type,option)
            self.algo2.set(algokeyargument, self.key_type, option)
            self.bits.set(self.key_type, bits, algo) #algo use onli for symmetric
            
        except Exception as e:
            raise BlobException("unable to set variable, "+str(e))
        self.__check_key_usage()

    def __add_data(self, value : bytearray)->None:
        lendata=len(value)
        if value is None or lendata==0 :
            raise BlobException("key data can't be null (length:%s)"%lendata)
        elif lendata>BIT32:
            raise BlobException("key data to long (length:%s)"%lendata)
        else:
            self.value=value

    def __check_key_usage(self)->None:
        _form="usage(s) not allowed for {}"
        key_type_name=str(self.key_type)
        if self.key_type.is_asymmetric():
            algo:str=str(self.algo1)
            if self.key_type.is_rsa():
                if self.algo1.is_rsa_pss() and algo!='NONE':
                    if not self.usage.is_allowed(['EXPORT', 'COPY', 'SIGN', 'VERIFY']):
                        raise BlobException(_form.format(key_type_name))
                elif self.algo1.is_rsa_pss() and algo=='NONE':
                    if not self.usage.is_allowed(['EXPORT', 'COPY', 'SIGN_HASH', 'VERIFY_HASH']):
                        raise BlobException(_form.format(key_type_name))
                elif self.algo1.is_rsa_oaep():
                    if not self.usage.is_allowed(['EXPORT', 'COPY', 'DECRYPT', 'ENCRYPT']):
                        raise BlobException(_form.format(key_type_name))
                elif algo=="PKCS1V15_CRYPT":
                    self.usage.is_allowed(['EXPORT', 'COPY', 'DECRYPT', 'ENCRYPT'])
                elif algo=="PKCS1V15_SIGN_RAW":
                    self.usage.is_allowed(['EXPORT', 'COPY', 'SIGN_HASH', 'VERIFY_HASH'])
                elif self.algo1.is_sign_message():
                    if not self.usage.is_allowed(['EXPORT', 'COPY', 'SIGN', 'VERIFY']):
                        raise BlobException(_form.format(key_type_name))
                else:
                    raise BlobException(_form.format(key_type_name))
            elif self.key_type.is_ecc():
                if self.algo1.is_ecdh():
                    if not self.usage.is_allowed(['EXPORT', 'COPY', 'DERIVE','VERIFY_DERIVATION']):
                        raise BlobException(_form.format(key_type_name))
                elif self.algo1.is_ecdsa():
                    if not self.usage.is_allowed(['EXPORT', 'COPY', 'SIGN', 'VERIFY', 'SIGN_HASH', 'VERIFY_HASH']):
                        raise BlobException(_form.format(key_type_name))
                else:
                    raise BlobException("incoherent data ECC "+key_type_name)
            elif self.key_type_name.is_dh():
                if not self.usage.is_allowed(['EXPORT', 'COPY', 'DERIVE' ,'VERIFY_DERIVATION']):
                    raise BlobException(_form.format(key_type_name))
        elif self.key_type.is_symmetric():
            if not self.usage.is_allowed(['EXPORT', 'COPY', 'DECRYPT', 'ENCRYPT']):
                raise BlobException(_form.format(key_type_name))

        elif self.key_type.is_hmac():
            if not self.usage.is_allowed(['EXPORT', 'COPY', 'SIGN', 'VERIFY']):
                raise BlobException(_form.format(key_type_name))
            if not self.usage.is_sign():
                raise BlobException(_form.format(key_type_name))
        

    #extract printable information. Print do not produce error, just error log
    def __str__(self)->str:
        try:
            persistence=self.lifetime&0xff
            if persistence==0:
                str_livetime=self._form_lifetime[1].format("VOLATILE", self.lifetime>>8)
            elif  persistence==1:
                str_livetime=self._form_lifetime[1].format("PERSISTENT", self.lifetime>>8)
            elif persistence==0xff:
                str_livetime=self._form_lifetime[1].format("READONLY", self.lifetime>>8)
            else:
                str_livetime=self._form_lifetime[0].format(self.lifetime&0xff, self.lifetime>>8)
            if len(self.value):
                if self.key_type.is_asymmetric():
                    if self.key_type.is_rsa():
                        if self.algo1.is_rsa_oaep():
                            skt="RSA_OAEP"
                        elif self.algo1.is_rsa_pss():
                            skt="RSA_PSS"
                        else:
                            skt="RSA_PKCS1V15"
                    elif self.algo1.is_deterministic_ecdsa():
                        skt="Deterministic "+str(self.key_type)+" EDCSA"
                    elif self.algo1.is_ecdsa():
                        skt=str(self.key_type)+" EDCSA"
                    elif self.key_type.is_dh():
                        skt="FFDH"
                    elif self.algo1.is_ecdh():
                        skt=str(self.key_type)+" ECDH"
                    else:
                        skt=str(self.key_type)
                    if self.key_type.is_public_key():
                        skt=skt+" PUBLIC"
                    else:
                        skt=skt+" KEY PAIR"
                    algo=str(self.algo1)
                elif self.key_type.is_raw_data():
                    algo=str(self.algo1.get_raw())
                    skt="RAW"
                else:
                    skt=str(self.key_type)
                    algo=str(self.algo1)
                return self._form_name.format(skt, self.bits.value)+ \
                       self._form_id.format(self.key_id)+ \
                       self._form_owner.format(self.owner)+ \
                       str_livetime +\
                       self._form_usage.format(self.usage)+ \
                       self._form_algo.format(algo)+ \
                       self._form_value.format(len(self.value), self.value.hex())
            else:
                sys.stderr.write('Empty key, error')
        except Exception as e:
            sys.stderr.write("Internal error\n"+str(e))
        return ""

    def type(self):
        return DATA_TYPE['KEY']
    
    #true if this is the same key identity
    def uniq(self, key :'ITS_key_area') -> bool:
        if(self.key_id is not None and self.owner is not None and self.key_id==key.key_id and self.owner==key.owner):
            return False
        else:
            return True
    #transfome the data to a key area
    def bytearray(self,byteorder='little')->bytearray:  #data for key, 32 bits
        ba=bytearray()
        ba.extend(self.key_id.to_bytes(length=4, byteorder=byteorder, signed=False))
        ba.extend(self.owner.to_bytes(length=4, byteorder=byteorder, signed=False))
        ba.extend(self.lifetime.to_bytes(length=4, byteorder=byteorder, signed=False))
        ba.extend(self.key_type.bytearray(byteorder))
        #bits
        bits : int =self.bits.value
        ba.extend(self.bits.bytearray(byteorder))
        #usage
        ba.extend(self.usage.bytearray(byteorder))
        #algo
        ba.extend(self.algo1.bytearray(byteorder))
        ba.extend(self.algo2.bytearray(byteorder))
        #len and data
        lendata=len(self.value)
        ba.extend(lendata.to_bytes(length=4, byteorder=byteorder, signed=False)) 
        ba.extend(self.value)
        return ba

class ITS_blob():
    _form_version="\tversion:\t0x{:04X}"
    _form_quantity="\n\tdata area:\t{}\n\tkeys area:\t{}" #\n\tps:\t{}
    
    def __init__(self, version=1):
        if(version==0 and version>=BIT32):
            raise BlobException("incorrect version number")
        else:
            self.version=version
        self._elements = [] # no area

    #count area type
    def count(self, datatype : str)->int:
        cpt:int=0
        if datatype in DATA_TYPE.keys():
            for elem in self._elements:
                if elem.type() == DATA_TYPE[datatype]:
                    cpt+=1
        return cpt
    
    #expend with an ITS area
    def addITS(self, its : ITS_data_area)->None:
        #check if id is already known
        for elem in self._elements:
            if elem.type() == DATA_TYPE['ITS']:
                if not elem.uniq(its): raise BlobException("id already used")
        self._elements.append(its)
    
    #expend with a key area
    def addkey(self, key :ITS_key_area)->None:
        for elem in self._elements:
            if elem.type() == DATA_TYPE['KEY']:
                if not elem.uniq(key): raise BlobException("id already used")
        self._elements.append(key)
    
    #user information. Print do not produce error, just error log
    def __str__(self)->str:
        ret="" #in exception case, should be initialised for the return
        try:
            if(self.version is not None):
                strversion=self._form_version.format(self.version)
            else:
                strversion=self._form_version.format('incorrect')
            ret : str = strversion+self._form_quantity.format(self.count('ITS'), self.count('KEY')) #, self.count('PS')
            for elem in self._elements:
                ret=ret+str(elem)
        except Exception as e:
            sys.stderr.write("Internal error\n"+str(e))
        return ret
    
    def _clean(self)->None:
        self.version=1
        self._elements.clear()

    def is_in(self, owner:int, objectid:int)->bool:
        for elem in self._elements:
            if elem.type() == DATA_TYPE['ITS']:
                if elem.uuid==objectid and elem.owner==owner:
                    return True
            elif elem.type() == DATA_TYPE['KEY']:
                if elem.key_id==objectid and elem.owner==owner:
                    return True
        return False

    #write the blob in a file        
    def writefile(self, file, byteorder='little')->int:
        ba = bytearray()
        #build header
        try:
            ba.extend(MAGIC_NUMBER.to_bytes(length=4, byteorder=byteorder, signed=False))
            ba.extend(self.version.to_bytes(length=4, byteorder=byteorder, signed=False))
            nbi: int=self.count("ITS")
            nbk: int=self.count("KEY")
            nbp: int=self.count("PS")
            ba.extend(nbi.to_bytes(length=2, byteorder=byteorder, signed=False))
            ba.extend(nbp.to_bytes(length=2, byteorder=byteorder, signed=False))
            ba.extend(nbk.to_bytes(length=2, byteorder=byteorder, signed=False))
            ba.extend((nbi+nbk+nbp).to_bytes(length=2, byteorder=byteorder, signed=False))
            padding=0
            for elem in self._elements: #build in ba all elements
                elemtype=elem.type()
                #build area header
                dataarray=elem.bytearray()
                datalen=len(dataarray)
                ba.extend(elemtype.to_bytes(length=4, byteorder=byteorder, signed=False)) #build element type
                ba.extend(datalen.to_bytes(length=4, byteorder=byteorder, signed=False)) #build element length
                ba.extend(dataarray)
                padding=0
                while len(ba) % MAXPADDING!=0:
                   ba.append(0)
                   padding+=1
            file.write(ba)
            return padding
        except Exception as e:
            raise BlobException("Internal error\n"+str(e))        
        try:
            file.write(ba)
        except Exception as e:
            raise BlobException('writefile error\n'+str(e))
    
    #read the blob from a file 
    def readfile(self, ba : bytearray, byteorder='little')->None:
        self._clean()
        if len(ba)==0:
            raise BlobException("File empty")
        try:
            zone="header"
            indice : int =0
            magic = int.from_bytes(ba[indice:indice+4], byteorder, signed=False)
            if (magic != MAGIC_NUMBER):
                raise BlobException("Incorrect magic number")
            indice+=4
            self.version = int.from_bytes(ba[indice:indice+4], byteorder, signed=False)
            if(self.version==0):
                raiseBlobException("Incorrect version"+ba.hex())
            indice+=4
            nb=[0, 0, 0, 0]
            for j in range(0,4):
                nb[j]=int.from_bytes(ba[indice:indice+2], byteorder, signed=False)
                indice+=2
            if(nb[3]!=(nb[0]+nb[1]+nb[2])): raise BlobException("Incoherent numbers of elements")
            while(indice<len(ba)):
                areatype = int.from_bytes(ba[indice:indice+4], byteorder, signed=False)
                found=False
                zone=""
                for k,v in DATA_TYPE.items():
                    if v==areatype:
                        if k=="ITS":
                            elem=ITS_data_area()
                            nb[0]-=1
                            zone="DATA"
                        elif k=='KEY':
                            elem=ITS_key_area()
                            nb[2]-=1
                            zone="KEY"
                        found=True
                        indice+=4
                        break
                if not found: raise BlobException("Data type error, unknown area (found %s)" % hex(areatype))
                arealength=int.from_bytes(ba[indice:indice+4], byteorder, signed=False)
                indice+=4
                if arealength==0: raise BlobException("Area size null")
                finarea=arealength+indice
                elem.read(ba[indice:finarea+4], byteorder) #+4 because of uint8_t * length
                padding= MAXPADDING - finarea%MAXPADDING
                indice=finarea+padding
                if zone=="DATA":
                    self.addITS(elem)
                elif zone=="KEY": 
                    self.addkey(elem)
            if nb[0]!=0 or nb[1]!=0:
                raise BlobException("Incoherent objects numbers")
        except Exception as e:
            raise BlobException("Incoherent %s data, %s (%d)" % (zone,str(e), indice))
 
