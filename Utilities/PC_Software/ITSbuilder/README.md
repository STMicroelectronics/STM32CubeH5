# ITSbuilder

ITSbuilder is a tool for Secure Manager projects. It is used to build the initial Internal Trusted Storage (ITS) structure following the PSA standard, called 'blob'.
The user build a blob file, incrementally, by adding ITS data and keys, element by element. This blob will be encrypted and stored in the Factory ITS area (FITS) during the secure manager installation.

@see PSA Cryptography 1.0 and mbltls 2.28 API for more details

## Project contents
* Installer
* Usage of the executable
* Executable generation

## How to use the tool
### Installation
There are two modes to run this tool.
* .exe file.
* python project. To install it, run :
  ```bash
  python setup.py install
  ```

>**Note**:
> python required version >= 3.10


### Executable
Using the Windows exe :
* dist/ITSbuilder/ITSbuilder.exe


## Usage modes
This tool has multiple features:
1. Build a blob file
	1. Build a blob file via a menu
	2. Build a blob file via command
		1. Build a new header
		2. Add a data
		3. Add a key
2. Give information on supported key
3. Check if a key identifiant is already in the blob
4. Display the blob content in a format that can be easily interpreted by the user

### Options
To have a general help, run:
```
> python ITSbuilder.py --help
```

Here's the available functions : 
* createblob : Create a new empty file (blob)
* adddata2blob : Add data to a blob 
* addkey2blob : Add key to a blob
* checkblob : Check if owner or key ID is already used
* infoblob : Show the blob content & info

To obtain more information on each function and option available, use the following command:
```
> python ITSbuilder.py {function} --help
```

### Examples

#### 1. Keys & data creation

```
> openssl rand -base64 32 > aes256.base64
> openssl rand 16 > aes128.raw
> openssl genrsa -out private.rsa2048.pem 2048
> openssl rsa -in private.rsa2048.pem -outform DER -out private.rsa2048.der
> openssl ecparam -genkey -name secp256r1 -out private.secp256r1.pem
> openssl ecparam -genkey -name brainpoolP192r1 -out private.k1.brainpoolP192r1.pem
> echo my_data > its.bin
```

#### 2. Empty blob creation

* -v integer	(--version) 	Version of the header blob : required option
```
> python ITSBuilder.py createblob -v 1 blob.bin
```

#### 3. Add data

* -p string		(--protect)		Protection mode : required option
* -i int		(--id)			UIID of the data in the ITS area : required option (hexadecimal or decimal representation)
* -o int		(--owner)		Owner id : required option (positive, decimal or hexadecimal representation)
* -a user.bin	(--add)			Data to provision in the ITS area : required option

```
> python ITSbuilder.py adddata2blob blob.bin blob.bin --owner=0xFFFFFFFF --id=1 --protect=WRITE_ONCE --add=its.bin
```

#### 4. Add AES keys

* -t string		(--keytype)		Key type symmetric or asymmetric : required option
* -a string		(--algo)		Algorithm for symmetric or derivation Key : required option
* -b number		(--bits)		Number of bit : required option
* -i int		(--idkey)		Id of the key in the ITS area : required option (hexadecimal or decimal representation)
* -f string		(--format)		Format of the key file : required option
* -k file.key	(--key)			Key to provision in the ITS area : required option
* -u string		(--usage)		Usage of the key : required option
```
> python ITSbuilder.py addkey2blob blob.bin blob.bin --owner=0xFFFFFFFF --id=0x4 --keytype=AES --algo=CBC --addKey=aes128.raw -u EXPORT -u ENCRYPT -u DECRYPT --bits=128 --format=Raw --persistence=PERSISTENT
```

* -p int or str	(--persistance)	Lifetime of the key, default value is PERSISTENT : hexadecimal or decimal representation, max 32bits
* -l int		(--location)	Combined with persistence : allow to construct a full lifetime value
```
> python ITSbuilder.py addkey2blob blob.bin blob.bin --owner=0xFFFFFFFF --id=5 --keytype=AES --algo=CTR --addKey=aes256.base64 -u EXPORT --bits=256 --format=base64 --persistence=0x12 --location=0x2f
```

#### 5. Display the content on the console with - option
```
> python ITSBuilder.py infoblob fileout.bin -
```

#### 6. Add data with text format

* -t string		(--text)		Text to provision in the ITS area : required option if not use -a
```
> python ITSBuilder.py adddata2blob fileout.bin fileout.bin -o 0xFFFFFFFF -i 1 -t 'my data' --protect=WRITE_ONCE
```

#### 7. Add RSA Key

* -m string		(--asymmetric)	Type of the key. For PEM format : value searched in the key file after the "BEGIN". Default value : PRIVATE KEY
* -s string		(--digest)		Digest type : required option for some algorithms
```
> python ITSbuilder.py addkey2blob fileout.bin fileout.bin -m "PRIVATE KEY" --owner=0xFFFFFFFF --id=4 --keytype=RSA_OAEP --digest=SHA256 --addKey=private.rsa2048.der --usage=EXPORT --bits=2048 --format=DER --dump=info.log
```

#### 8. Add ECDSA Key
```
> python ITSbuilder.py addkey2blob fileout.bin fileout.bin -m "EC PRIVATE KEY" --owner=0xFFFFFFFF --id=5 --keytype=SECP_R1 --addKey=private.secp256r1.pem --usage=SIGN --usage=VERIFY --bits=256 --format=PEM
```

#### 9. Add ECDH Key
```
> python ITSbuilder.py addkey2blob fileout.bin fileout.bin -m "EC PRIVATE KEY" --owner=0xFFFFFFFF --id=6 --keytype=BRAINPOOL --addKey=private.k1.brainpoolP192r1.pem --usage=DERIVE --bits=192 --format=PEM --algo=HKDF
```

#### 10. Blob dump by 'infoblob' or '--dump' options

* Version of blob file
* Number of data areas in memory
* Number of key areas in memory
* For each elements :
		** Owner id
		** Data or Keys characteristics
		** Data length

#### Output example:
```
ITSbuilder v:1.1.2
	version:	0x0001
		data:	1
		keys:	1
	DATA Id:	0x0000000C
		owner:	0x0001
		flag:	ONCE
		data length :	11
		value :	0x124a5ef56f47b4cc131251
	AES key (256b)
		Id:	0x0005
		owner:	0x0001
		persistence:	PERSISTENT, location: 0x0012
		usage(s):	EXPORT, ENCRYPT
		argument(s):	CBC
		data length:	48
		value:	0x53616c7465645f5fc2fab4843ca53cedf685207977cc43b8d65b348c9de446cbb1c999ca569721ab09e35d4fcc4dc557
```

#### 11. Script example

A script example is available in `Firmware/Project/STM32H573I-DK/ROT_Provisioning/SM/its_blob.bat`

#### Notes:

* It is possible to have a template fileout.bin for a product model, and then, add specific keys and data to customize each product.
* For filein and fileout, - is stdin and stdout, respectively. The errors are available in stdout.
* The owner ID for the non secure world is 0xFFFFFFFF.
* IDs have to be unique for each owner.
* The argument value is the algorithm, the digest or both.


## ITSbuilder capabilities

### Limitation ###

* The max size of a key or a data is 2016 octets.
* For RSA key, PEM format is not available, use DER format.
* Except for ECDSA/ECDH the value of key is not checked. If the data is not as expected, the secure manager will not provision the key in ITS area.


### Available Keys and usages
---
| Key | Key Type | Digest | Algorithms | bits | EXPORT  <br>COPY | ENCRYPT  <br>DECRYPT | SIGN  <br>VERIFY | SIGN_HASH  <br>VERIFY_HASH (1) | DERIVE KEY AGREEMENT  <br>VERIFY_DERIVATION |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| Symmetric need algo only | AES |     | CBC,CCM,GCM,CFB,ECB,CTR,OFB,CMAC | 128,192,256 | x   | x   |     |     |     |
|     | AES |     | XTS | 256,384,512 | x   | x   |     |     |     |
|     | DES |     | CBC,ECB | 64, 128, 192 | x   | x   |     |     |     |
|     | CHACHA20 |     | Poly1305, STREAM_CIPHER | 256 | x   | x   |     |     |     |
|     | ARC4 |     | STREAM_CIPHER | 256 | x   | x   |     |     |     |
| ECDH family need algo & hash | BRAINPOOL R1 | HASH_algo(3) | DERIVE_algo(4) | 160,192,224,256, 320,384,512 | x   |     |     |     | x   |
|     | MONTGOMERY(2) | HASH_algo(3) | DERIVE_algo(4) | 255,448 | x   |     |     |     | x   |
|     | FRP | HASH_algo(3) | DERIVE_algo(4) | 256 | x   |     |     |     | x   |
| ECDSA Family | SECP_K1 | HASH_algo(3) or None |     | 192,225,256 | x   |     |     | x   | ECDSA_ANY if No Hash |
|     | SECP_R1 | HASH_algo(3) or None |     | 192,224,256,384,521 | x   |     |     | x   | ECDSA_ANY if No Hash |
|     | SECP_R2 | HASH_algo(3) or None |     | 160 | x   |     |     | x   | ECDSA_ANY if No Hash |
|     | SECT_K1 | HASH_algo(3) or None |     | 163,233,239,283,409,571 | x   |     |     | x   | ECDSA_ANY if No Hash |
|     | SECT_R1 | HASH_algo(3) or None |     | 163,233,283,409,571 | x   |     |     | x   | ECDSA_ANY if No Hash |
|     | SECT_R2 | HASH_algo(3) or None |     | 168 | x   |     |     | x   | ECDSA_ANY if No Hash |
| HMAC family | HMAC | HASH_algo(3) |     | bits %8 !=0 | x   |     | x   |     |     |
| RSA family | RSA_OAEP | HASH_algo(3) or None |     | 2048, 3072 | x   | x   |     |     |     |
|     | RSA_PKCS1V15 | HASH_algo(3) or None |     | 2048, 3072 | x   | PKCS1V15_CRYPT if No Hash | x   | PKCS1V15\_SIGN\_RAW if No Hash |     |
|     | RSA_PSS | HASH_algo(3) or None |     | 2048, 3072 | x   |     | x   | if No Hash |     |
| RAW | RAW | an int (with digest) |     | int(5) | x   | x   | x   | x   | x   |

(2) MONTGOMERY:	Curve25519, Curve448 respectively
(3) HASH_algo :	MD2,MD4,MD5, RIPEMD160, SHA1, SHA224,SHA256, SHA384, SHA512, SHA512_224, SHA512_256, SHA3_224, SHA3_256, SHA3_384, SHA3_512, SM3
(4) DERIVE_algo:	HKDF, TLS12_PRF, PSK_TO_MS, RAW
(5) int: integer, 32 bits

### Parity and usages

* A public key can be exported from a private key.
* The `DECRYPT`, `SIGN`, `SIGN_HASH` operations require a private key, but `ENCRYPT`, `VERIFY`, `VERIFY_HASH` accept both private and public key.
* `DERIVATION`/`key_agreement` and `VERIFY_DERIVATION` require a private key.


### Key files ###

Base64, raw, hexadecimal and PEM (only for asymmetric algorithm) formats are supported. All those formats are generated by openssl.
* `openssl enc` command use `-base64` option to produce a Base64 file.
* `openssl dgst` command use `-hex` option to produce a hex file.
* `openssl genpkey` command use `-outform PEM` to produce PEM file.

#### Asymmetric keys

The `--asymmetric` option is determined by the content of the key file. ITSbuilder will search the '--asymmetric' option value inside the key file to process the data.

For this private ECDH PEM file, the `--asymmetric` option of ITSBuilder is `PRIVATE KEY`
```
-----BEGIN PRIVATE KEY-----
MIGIAgEAMBQGByqGSM49AgEGCSskAwMCCAEBCARtMGsCAQEEIGmthsOo075LYdvt
zst/JjtG5S1Q3e3aJklTXo1qag/noUQDQgAEbHFnjIKA7eK/EMeMa6gLadaTuPfA
LGpVRXqBaLSliwciXQ5TaUtsnVy5WmeK3dYwbOprgRwupc5jCkldPT9UGA==
-----END PRIVATE KEY-----
```

For this private ECDSA PEM file, the `--asymmetric` option of ITSBuilder is `ENCRYPTED PRIVATE KEY`
```
-----BEGIN ENCRYPTED PRIVATE KEY-----
MIHsMFcGCSqGSIb3DQEFDTBKMCkGCSqGSIb3DQEFDDAcBAgrC3Kd8QumTAICCAAw
DAYIKoZIhvcNAgkFADAdBglghkgBZQMEASoEELU0TOSXBMfPYa1IU5IbKMkEgZDh
E4zbej2fpgybnMtzfkiC9RtI8Rq6Aerzts6WvczhLeYSNj/nfHogK53c+RUQXQD6
RM5KCQ7JxzEhvAcEOSTo2LlnH4+tVFXeG4b4jPXsaBWpATZCLZdg3TcRb3T/VvjQ
cz+CC5+AHBB0WLRbMGb7kJD40EeWzrsJvkKpdiMDC15Y69hYykazhjicKrkrxgk=
-----END ENCRYPTED PRIVATE KEY-----
```

# Raw key type

Raw is for unplanned key type, the digest value is use to set the PSA crypto algorithm value.

## Executable creation

pyinstaller must be installed :
```
pip3 install pyinstaller
```


In the root folder, execute the command :

```
pyinstaller ITSbuilder/main.py --onefile --name ITSbuilder
```

The executable file will be found in the dist folder.

For more information, see the pyinstaller documentation.