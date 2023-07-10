#Getting the CubeProgammer_cli path
source ../env.sh

#Keys files
# Absolute path to this script
SCRIPT=$(readlink -f $0)
projectdir=`dirname $SCRIPT`
cd $projectdir/keys
keys_pem_dir='pwd'
cd $projectdir
cd $projectdir/../../Applications/ROT/OEMiROT_Boot/Src
keys_c_dir='pwd'
cd $projectdir
cd $projectdir/../../../../Middlewares/Third_Party/mcuboot
mcuboot_dir='pwd'
cd $projectdir
keys_c="$keys_c_dir\keys.c"

cnt=0

error_key()
{
    echo "$command_key : failed"
    echo "Script failure"
    $SHELL
    return 1
}

rsa_key_error()
{
    cnt=$cnt+1
    if [ "$cnt" -ge "2" ]; then 
        error_key
    else 
        keygen 
    fi
}
keygen()
{
    #ecc 256 auth key
    key_ecc="$keys_pem_dir\OEMiRoT_Authentication.pem"
    command_key="$python$imgtool keygen -k $key_ecc -t ecdsa-p256"
    $command_key
    if [ "$?" -ne "0" ]; then error_key; fi

    command_key="$python$imgtool getpub -k $ey_ecc$  > $keys_c"
    $command_key
    if [ "$?" -ne "0" ]; then error_key; fi

    # priv key to encode images
    key_ecc_enc_priv="$keys_pem_dir\OEMiRoT_Encryption.pem"
    key_ecc_enc_pub="$keys_pem_dir\OEMiRoT_Encryption_Pub.pem"
    command_key="$python$imgtool -k $key_ecc_enc_priv -t ecdsa-p256 -e $key_ecc_enc_pub"
    $command_key
    if [ "$?" -ne "0" ]; then error_key; fi

    command_key="$python$imgtool getpriv  --minimal -k $key_ecc_enc_priv  >> $keys_c"
    $command_key
    if [ "$?" -ne "0" ]; then error_key; fi
    echo "Script success!"
    $SHELL;
    exit 0
}

imgtool="$mcuboot_dir/scripts/dist/imgtool/imgtool.exe"
uname | grep -i -e windows -e mingw
if [ $? == 0 ] && [ -e "$imgtool" ]; then
    echo "Keygen with windows executable"
else
    echo "Keygen with python script"
    imgtool="$mcuboot_dir\scripts\imgtool.py"
    python="python "
fi

keygen

