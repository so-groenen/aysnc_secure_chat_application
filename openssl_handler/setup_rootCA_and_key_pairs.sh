#! /bin/bash

function init_project
{
    project=$1
    rm -r    $project
    mkdir -p $project
    cd 	     $project
}
function make_private_key
{
	echo "Making private key:"
	openssl genrsa -aes256 -out $1.key 4096
    echo ""
}
function make_certificate_from_key
{
	echo "Making certificate key:"
	openssl req -x509 -new -nodes -key $1.key -sha256 -days 1826 -out $1.crt
    echo ""
}
function make_root_CA
{
	make_private_key $1
	make_certificate_from_key $1
}
function make_private_key_with_csr 
{
    echo "Making private key and certificate sign request to hold public key"
    openssl req -new -nodes -out $1.csr -newkey rsa:4096 -keyout $1.key
}
function sign_requested_csr_using_CA
{
    requester=$1
    CAuthority=$2
    echo "signing $requester's certificate signing request using $CAuthority" 
    openssl x509 -req -in $requester.csr -CA  $CAuthority.crt -CAkey $CAuthority.key -CAcreateserial -out $requester.crt -days 730 -sha256 
}
function make_v3
{
    cat > $MYCERT.v3.ext #<< #EOF
    authorityKeyIdentifier=keyid,issuer
    basicConstraints=CA:FALSE
    keyUsage = digitalSignature, nonRepudiation, keyEncipherment, dataEncipherment
    EOF
}
####################################


echo "Setting up rootCA, then entity [for ex: server] private/public keys"
echo ""
read -p "PROJECT NAME: " project

rm -r    $project
mkdir -p $project
cd 	     $project

####################################
#########       MAIN        ########
####################################
echo ""

# Make CA Authority
read -p "Enter CA Authority (rootCA): " rootCA
make_root_CA "$rootCA"
echo ""
echo "Finished making rootCA!"
echo ""
echo "Making public/private key with signing request!"
echo ""

# Make client/Server

read -p "Enter name for requester entity (for ex: client/server): " requester
make_private_key_with_csr "$requester" 
echo ""
sign_requested_csr_using_CA "$requester" "$rootCA"

# Verify
echo ""
echo "verifying if signed success:"
openssl verify -CAfile "$rootCA.crt" "$requester.crt"
echo ""

# and finish!!
echo "Finished"
echo ""
