#key="../path_to_the_key/key.pem"
python build.py cbor ./eat.txt ./eat.cbor
#../scripts/check_iat -k $key  ./eat.cbor -p
../scripts/check_iat ./eat.cbor -p
