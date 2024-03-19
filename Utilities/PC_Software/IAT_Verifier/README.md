# IAT Verifier installation (python 3 only)

To install the utility, follow these steps:

1. Uninstall `pycose` by running `pip uninstall pycose`.
2. Run `python setup.py install`.

Note: Uninstalling `pycose` is necessary to force the installation of version 0.1.2.

# Set Key Path (Optional)

To set the key path in `./st_tools/checkiat.sh`, follow these steps:
1. Locate the key used to check the token signature.
2. Set the key path in the `checkiat.sh` file.

# Check Initial Attestation Token

To check the initial attestation token, follow these steps:
2. Comment or uncomment the line to decrypt the token signature with the key in `./st_tools/checkiat.sh`.
3. Copy the token generated in `eat.txt` file in `./st_tools` folder.
4. Execute `./checkiat.sh`.

An example of the token is shown below:

```json
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
```