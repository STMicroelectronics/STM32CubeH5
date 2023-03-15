-1-Python Installation : (use python 3)
in directory ./tools/iat_verifier
pip uninstall pycose
python setup.py install

NB: uninstall pycose is required to force the installation of 0.1.2 pycose which
is required for iatverifier

-2-(optional) Set key path in ./tools/iat/verifier/st_tools/checkiat.sh :
- key used to check the token signature.


-3-Check initial attestation token

The script checkiat.sh is used to check initial attestion response.

- comment/uncomment the line to decrypt the token signature with the key in /tools/iat/verifier/st_tools/chekiat.sh :
  ../scripts/check_iat -k $key  ./eat.cbor -p

- recopy token generated in eat.txt file in ./tools/iat_verifier/st_tools folder

- execute ./checkiat.sh

an example of token :

cbor
Signature OK
Token format OK
Token:
{
    "CHALLENGE": "b'00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000'",
    "BOOT_SEED": "b'DE5761733287E4A0F427E731930578934A6183490F11F1205896E1AFD27A4404'",
    "INSTANCE_ID": "b'01FA58755F658627CE5460F29B75296713248CAE7AD9E2984B90280EFCBCB50248'",
    "IMPLEMENTATION_ID": "b'E397AA8BFB40C8A188643D39AF142AFF1FCC2EA08F5B8957C3D3CC93C4E4E8E4'",
    "CLIENT_ID": -1,
    "SECURITY_LIFECYCLE": "SL_SECURED",
    "SW_COMPONENTS": [
        {
            "SW_COMPONENT_TYPE": "SPE",
            "SW_COMPONENT_VERSION": "1.0.0",
            "SIGNER_ID": "b'05B081D953FA2DA76219365256A21A457B52001E5CA8296D907B13519C9540D3'",
            "MEASUREMENT_DESCRIPTION": "SHA256",
            "MEASUREMENT_VALUE": "b'8268C4F12D16E24D0AD9BA09B1090216B2186E1DC91B54D7F80F44324532F910'"
        },
        {
            "SW_COMPONENT_TYPE": "NSPE",
            "SW_COMPONENT_VERSION": "1.0.0",
            "SIGNER_ID": "b'3F6C9B372408CC68A8C2CD4E289135766DF5782DE44D24DC4BDF7E691C2A11FD'",
            "MEASUREMENT_DESCRIPTION": "SHA256",
            "MEASUREMENT_VALUE": "b'2599215D6961E1728D8C9058A5A78ADE24345CFA62F229A2FC88945458F44602'"
        }
    ],
    "ORIGINATOR": "www.trustedfirmware.org",
    "PROFILE_ID": "psa-tfm-profile-1.md",
    "HARDWARE_ID": "04080202000000"
}

