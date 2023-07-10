# ITSbuilder

ITSbuilder is a tool for Immutable Root of Trust (IROT) projects, useful to build the Internal trusted storage structure ITS, following the Professionnal service automation (PSA) standard.
The user build a blob file, incrementally, by adding ITS data and keys, element by element. This blob, will be encrypted and stored in the Feature ITS area (FITS)

IROT allow to crypt and encrypt, to sign and verify data and to control connections via the Derive/Key agreement mechanism. IROT requires key to proceed :
*The symmetric encryption/decryption sequence is : source + password => crypt + password => source
The key and the file have to be send to receiver. The same key is use for encryption and decryption.
*The asymmetric encryption/decryption sequence is : source + password + private key => crypt + public key + password => source
The public key and the file have to be send to the receiver, and the receiver can decrypt
*The symmetric sign/verify sequence is:  source + password => sign, source + password + sign => True/False
The key and the file have to be send to receiver. The same key is use for sign and for verify that the source is not altered.
*The asymmetric sign/verify sequence is: source + private key (+password) => sign, public key + source (+password), sign => True/False
The public key and the file have to be send to receiver, and the receiver can verify that the file is not altered

*The Key agreement and derivation/Verif_derivation sequence is more complex :
Alice give her public key to bob.
Bob give his public key to Alice.
Alice use Bob's public key with her private key to generate a shared secret.
Bob use Alice's public key with his private to generate the same secret as Alice's.
This secret can be use as symmetric key, for example, to crypt the communication. The derivation process allow to generate as secret as you need.

@see PSA Cryptography API 1.0 for more details
@see examples.bat

## Project contents
* installer
* usage of the executable
* executable creation

## How to use the tools
### Installer
The are two modes ways to execute this tool.
* .exe file 
* .py project With this project, it is possible to modify the sources. In this mode is HIGH recommended to run the setup.py script in order to get all the library dependencies installed

  ```bash
  python setup.py install
  ```
### executable
Using the Windows exe :
* dist/ITSbuilder/ITSbuilder.exe  (one dir configuration)
* dist/ITSbuilder.exe  (onefile configuration)

Note: The application is py files 


## usage modes
There are many way to use the tool
1. give information on supported key
2. check if a key identifiant is already in the blob
3. display a human comprehensive Blob content
4. build a blob file
 1. build a blob file via a menu
 2. build a blob file via command
   1. build a new header
   2. add a data area
   3. add a key area

### Options
The command "py ITSbuilder.py --help" give a short help. Others options are availables. 
* -v			(--version) header version, version of the blob. The default value is 1. 
* -t			give the list of type of keys.
* -s key_type	give the list of digest for a specifique type of key
* -b key_type	give the list of available bits for a specifique type of key
* -d key_type	give the list of derivation algorithm for ECDH or DH.
### Options for checkBlob command, to check if a key id is already in blob
py ITSbuilder.py checkblob filein
*-i				(--id) id of the key
*-b				(--owned) owner of the key
#### Options for infoBlob command, to see the blob content.
py ITSbuilder.py infoblob filein fileout
* -h			(--help) help command
#### Options for createBlob command to create a new empty file
py ITSbuilder.py createblob fileout
* -h			(--help) help command
* -v			(--version) header version
* -m fileout.bin(--menu) fileout  build the file via an interface. crush fileout.
* -d info.log	(--dump) produce a file info
#### Options for addData2blob command, to add bytes to its area
py ITSbuilder.py adddata2blob filein fileout
* -h           	(--help) help command
* -p string		(--protect) add one or fewer protection modes : WRITE_ONCE, NO_CONFIDENTIALITY, NO_REPLAY_PROTECTION or NONE. At least, one should be chosen. NONE have to be alone.
* -i int		(--id) uuid of its. Decimal or hexadecimal value (0x....)
* -o int		(--owner) owner id, can't be 0L. 32 bits positive decimal or hexadecimal value
* -a user.bin	(--add) data file to include in the its area. Required, but use -a or -t option.
* -t string		(--text) text to include in the its area. Required, but use -a or -t option.
* -d info.log	(--dump) produce a file info
#### options for addKey2Blob command, to add key options for command line
py ITSbuilder.py addkey2blob filein fileout asymmetric
* -h			(--help) help command
* -t string		(--keytype) key type symmetric or asymmetric (aes: CBC CCM CFB ECB CTR CTR OFB GCM /rsa: SHA256 ).  @see InfoBuilder -t for more option
* -a string		(--algo) algorithm for symmetric or derivation Key.  Type "InfoBuilder -a key_type" to have the list. Derivation algorithms as HKDF,TLS12_PRF, PSK_TO_MS. Raw is use for pure KeyAgreement, can't be derived further. 
* -b number		(--bits) number of bit, required.
* -i int		(--idkey) idkey. Decimal or hexadecimal value (0x....)
* -f string		(--format) format of the key file (Base64, hex, Raw, or PEM for asymmetric key).
* -k file.key	(--key) add key file
* -u string		(--usage) define all usages yu want among EXPORT COPY ENCRYPT DECRYPT SIGN VERIFY SIGN_HASH VERIFY_HASH DERIVE VERIFY_DERIVATION. A asymmetric public key is automatically set exportable. @see the table above to choose your usages.
* -o int		(--owner) owner id, can't be 0x0, 32 bits positive decimal or hexadecimal value
* -p int or str	(--persistance) key lifetime, default PERSISTENT, VOLATILE, and READONLY, otherwise any decimal or hexadecimal integer, max 32bits
* -l int		(--localisation) with persistence, allow to construct a full lifetime value
* -d info.log	(--dump) produce a file info
* -D			(--deterministic) only for ECDSA keys
* -s string		(--digest) digest for asymmetric key and Hmac
* -m string		(--asymmetric) asymmetric argument is mendatory for asymmetric key only. Useful parameter to identify the type of key, but also to find the key value in the PEM file, see the following key file section to have more information
				If no options is given, the usage drive the kind of Key (PUBLIC/PRIVATE) used. A derivation key will be assumed private, and if the file format is PEM, ITSbuilder will search for a PRIVATE KEY.
				if the usage is incompatible, an error occur, see Parity and usages section below. Keyword PRIVATE and PAIR have an identical treatment.


The owner ID for the non secure world is 0xFFFFFFFF. For the secure world, the owner ID is the secure module ASID of the data's owner

### Examples:

ITSBuilder infoblob filein.bin info.log
ITSBuilder createblob -v 50 fileout.bin
ITSBuilder adddata2blob filein.bin fileout.bin -i 124 -o 5124 -p WRITE_ONCE -p NO_REPLAY_PROTECTION -a source.bin -d fileinfo.log
ITSBuilder addkey2blob filein.bin fileout.bin -i 125 -o 5124 -k keyfile.hex -f Hex -t AES -b 128 -a CBC -u ENCRYPT -p 1212

ITSBuilder createblob -v 120 fileout.bin
ITSBuilder adddata2blob fileout.bin fileout.bin -p PUBLIC -o 5124 -i 12124545 -t 'Toto fait du velo'
ITSBuilder addkey2blob fileout.bin  fileout.bin 'PRIVATE KEY' -o 5124 -i 11212 -k key.perm -f PEM -t RSA_OAEP -b 128 -u ENCRYPT -p PERSISTENT -l 0x54
ITSBuilder infoblob fileout.bin info.log


### Notes:
So you can have a template fileout.bin for your product model, and then, add specific keys and data to customize each product.
Notes :
* for filein and file out, - is stdin and stdout, respectively. The errors are available in stdout.
* If you need to uses more than one option, just cumulate its (@see click api)


## dump by infoblob or --dump options
	version of Blob file
	number of data areas in memory
	number of key areas in memory
	version
	for each elements :
		owner id
		data or keys characteristic ( id, encryption length / key type, key value, usage)
		size of area

output example:
ITSbuilder v:1.0.0\
&emsp;version:&emsp;0x0001\
&emsp;&emsp;data:&emsp;1\
&emsp;&emsp;keys:&emsp;1\
&emsp;DATA Id:&emsp;0x0000000C\
&emsp;&emsp;owner:&emsp;0x0001\
&emsp;&emsp;flag:&emsp;ONCE\
&emsp;&emsp;data length :&emsp;11\
&emsp;&emsp;value :	0x124a5ef56f47b4cc131251\
&emsp;AES key (256b)\
&emsp;&emsp;Id:&emsp;0x0005\
&emsp;&emsp;owner:&emsp;0x0001\
&emsp;&emsp;persistence:&emsp;PERSISTENT, location: 0x0012\
&emsp;&emsp;usage(s):&emsp;EXPORT, ENCRYPT\
&emsp;&emsp;argument(s):&emsp;CBC\
&emsp;&emsp;data length:&emsp;48\
&emsp;&emsp;value:&emsp;0x53616c7465645f5fc2fab4843ca53cedf685207977cc43b8d65b348c9de446cbb1c999ca569721ab09e35d4fcc4dc557

Note : id have to be uniq for each owner. The argument value is the algo, the digest or both. The version argument is the version of the file.

## ITSbuilder capabilities
### Available Keys and usages
---
| Key|Key Type|Digest|Algorithms|bits|EXPORT COPY|ENCRYPT DECRYPT|SIGN VERIFY|SIGN_HASH VERIFY_HASH (1)|DERIVE KEY AGREEMENT VERIFY_DERIVATION|
|--|--|--|--|--|--|--|--|--|--|--|
|Symmetric need algo only|AES||CBC,CCM,GCM,CFB,ECB,CTR,OFB,CMAC|128,192,256|x|x||||			
||AES||XTS|256,384,512|x|x||||
||DES||CBC,ECB|64, 128, 192|x|x||||
||CHACHA20||Poly1305, STREAM_CIPHER|256|x|x||||
||ARC4||STREAM_CIPHER|256|x|x|
|||||||||||
|ECDH family need algo & hash|BRAINPOOL|HASH_algo(3)| DERIVE_algo(4)|160,192,224,256, 320,384,512|x||||x|
||MONTGOMERY(2)|HASH_algo(3)|DERIVE_algo(4)|255,448|x||||x|
||FRP|HASH_algo(3)|DERIVE_algo(4)|256|x||||x|
|||||||||||
|ECDSA Family|SECP_K1|HASH_algo(3) or None||192,225,256|x|||x|ECDSA_ANY if No Hash||
|| SECP_R1|HASH_algo(3) or None||192,224,256,384,521|x|||x|ECDSA_ANY if No Hash||
||SECP_R2|HASH_algo(3) or None||160|x|||x|ECDSA_ANY if No Hash||
||SECT_K1|HASH_algo(3) or None||163,233,239,283,409,571|x|||x|ECDSA_ANY if No Hash||
||SECT_R1|HASH_algo(3) or None||163,233,283,409,571|x|||x|ECDSA_ANY if No Hash||
|| SECT_R2|HASH_algo(3) or None||168x|||x|ECDSA_ANY if No Hash||
|||||||||||
|HMAC family|HMAC|HASH_algo(3)||bits %8 !=0|x||x|||
|||||||||||
|RSA family|RSA_OAEP|HASH_algo(3) or None||2048, 3072|x|x||||
||RSA_PKCS1V15|HASH_algo(3) or None||2048, 3072|x|PKCS1V15_CRYPT if No Hash|x|PKCS1V15_SIGN_RAW if No Hash|||
||RSA_PSS|HASH_algo(3) or None||2048, 3072|x|||x|if no Hash||
|||||||||||
|RAW|RAW|an int (with digest)||int(5)|x|x|x|x|x|

(2) MONTGOMERY:	Curve25519, Curve448 respectively
(3) HASH_algo :	MD2,MD4,MD5, RIPEMD160, SHA1, SHA224,SHA256, SHA384, SHA512, SHA512_224, SHA512_256, SHA3_224, SHA3_256, SHA3_384, SHA3_512, SM3
(4) DERIVE_algo:	HKDF, TLS12_PRF, PSK_TO_MS, RAW
(5) int: integer, 32 bits

### Parity and usages
A public key can be export from a private key.
The DECRYPT, SIGN, SIGN_HASH operations require a private key, but ENCRYP, VERIFY, VERIFY_HASH accept both, private and public key.
DERIVATION/key_agreement and VERIFY_DERIVATION require a private key


### Key files ###
Base64, row, or Hexadecimal encodage, and PEM for asymmetric key are available formats. All this format are generated by openssl command. openssl produce Raw format but :
"openssl enc" command use "-base64" option to produce a Base64 file. 
"openss dgst" command use "-hex" option to produce a Hex file.
"openss genpkey" command use "-outform PEM" to produce PEM file.

#### Asymmetric keys
DER is the method of encoding the data that makes up the certificate. CMS is a finary file described in PKCS#7 (with often a .p7 extension). The PEM is the CER base 64 encoded key bounded by a begin/end. PEM stands for Privacy Enhanced Mail; mail cannot contain un-encoded binary values such as DER directly.
PEM may also encode / protect other kinds of data that is related to certificates such as public / private keys, certificate requests, etc.
The .cer or .crt extension is just stands for certificate. You need to take a look at the content (e.g. using the file utility on posix systems) to see what is within the file to be 100% sure.

For this private ECDH PEM file, the --asymmetric option of ITSBuilder is "PRIVATE KEY"
-----BEGIN PRIVATE KEY-----
MIGIAgEAMBQGByqGSM49AgEGCSskAwMCCAEBCARtMGsCAQEEIGmthsOo075LYdvt
zst/JjtG5S1Q3e3aJklTXo1qag/noUQDQgAEbHFnjIKA7eK/EMeMa6gLadaTuPfA
LGpVRXqBaLSliwciXQ5TaUtsnVy5WmeK3dYwbOprgRwupc5jCkldPT9UGA==
-----END PRIVATE KEY-----

For this private ECDSA PEM file, the --asymmetric option of ITSBuilder is "ENCRYPTED PRIVATE KEY"
-----BEGIN ENCRYPTED PRIVATE KEY-----
MIHsMFcGCSqGSIb3DQEFDTBKMCkGCSqGSIb3DQEFDDAcBAgrC3Kd8QumTAICCAAw
DAYIKoZIhvcNAgkFADAdBglghkgBZQMEASoEELU0TOSXBMfPYa1IU5IbKMkEgZDh
E4zbej2fpgybnMtzfkiC9RtI8Rq6Aerzts6WvczhLeYSNj/nfHogK53c+RUQXQD6
RM5KCQ7JxzEhvAcEOSTo2LlnH4+tVFXeG4b4jPXsaBWpATZCLZdg3TcRb3T/VvjQ
cz+CC5+AHBB0WLRbMGb7kJD40EeWzrsJvkKpdiMDC15Y69hYykazhjicKrkrxgk=
-----END ENCRYPTED PRIVATE KEY-----

For Base64, RAW, Hex file format, the --asymmetric option of ITSBuilder requires only PRIVATE/PAIR or PUBLIC.

# RAW keytype
RAW is for unplanned key type, the digest value is use to set the PSA crypto algo value.

## Executable creation

The pyinstaller must be installed :

if python 3.11 (or greater) is recognized by the operating system
```
pip3 install pyinstaller
```
otherwise

```
python3 -m pip install pyinstaller
```

In the root folder, execute the command :

```
pyinstaller ITSbuilder.py --onedir --name ITSbuilder
```

The executable file will be founded in the dist folder.

For more information,  see the pyinstaller documentation