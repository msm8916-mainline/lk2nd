#!/usr/bin/env python3
# SPDX-License-Identifier: MIT
# Source: https://github.com/bkerler/android_universal/blob/6a6c0b7dd6dc4b063410c6e1893aaf9e6e82e128/root/scripts/bootsignature.py
# Dependencies: pyasn1 pyasn1_modules pycryptodome
# Formatted and cleaned up

from __future__ import print_function
import Crypto.PublicKey.RSA
from pyasn1_modules import rfc2459, pem
from pyasn1.codec.der import encoder, decoder
from Crypto.Signature import PKCS1_v1_5
from Crypto.Hash import SHA256
import argparse
import os
import shutil
import struct
import sys
from binascii import hexlify, unhexlify
from pyasn1.codec.der.decoder import decode as der_decoder
from pyasn1.codec.der.encoder import encode as der_encoder
from pyasn1.type import char, namedtype, univ
from pyasn1_modules import rfc2459, rfc4055


class AuthenticatedAttributes(univ.Sequence):
    componentType = namedtype.NamedTypes(
        namedtype.NamedType('target', char.PrintableString()),
        namedtype.NamedType('length', univ.Integer())
    )


class BootSignature(univ.Sequence):
    """
    BootSignature ::= SEQUENCE {
        formatVersion ::= INTEGER
        certificate ::= Certificate
        algorithmIdentifier ::= SEQUENCE {
            algorithm OBJECT IDENTIFIER,
            parameters ANY DEFINED BY algorithm OPTIONAL
        }
        authenticatedAttributes ::= SEQUENCE {
            target CHARACTER STRING,
            length INTEGER
        }
        signature ::= OCTET STRING
    }
    """

    _FORMAT_VERSION = 1

    componentType = namedtype.NamedTypes(
        namedtype.NamedType('formatVersion', univ.Integer()),
        namedtype.NamedType('certificate', rfc2459.Certificate()),
        namedtype.NamedType('algorithmIdentifier',
                            rfc2459.AlgorithmIdentifier()),
        namedtype.NamedType('authenticatedAttributes',
                            AuthenticatedAttributes()),
        namedtype.NamedType('signature', univ.OctetString())
    )

    @classmethod
    def create(cls, target, length):
        boot_signature = cls()
        boot_signature['formatVersion'] = cls._FORMAT_VERSION
        boot_signature['authenticatedAttributes']['target'] = target
        boot_signature['authenticatedAttributes']['length'] = length
        return boot_signature


def __get_signable_image_size(f):
    magic = f.read(8)
    if magic != b'ANDROID!':
        raise ValueError('Invalid image header: missing magic')

    header = struct.Struct('<'    # little endian
                           + 'i'  # kernel_size
                           + 'i'  # kernel_addr
                           + 'i'  # ramdisk_size
                           + 'i'  # ramdisk_addr
                           + 'i'  # second_size
                           + 'q'  # second_addr + tags_addr
                           + 'i'  # page_size
                           + 'i'  # dt_size
                           )

    header_raw = f.read(header.size)
    header_struct = header.unpack_from(header_raw)

    kernel_size = header_struct[0]
    ramdsk_size = header_struct[2]
    second_size = header_struct[4]
    page_size = header_struct[6]
    dt_size= header_struct[7]

    # include the page aligned image header
    length = page_size \
             + ((kernel_size + page_size - 1) // page_size) * page_size \
             + ((ramdsk_size + page_size - 1) // page_size) * page_size \
             + ((second_size + page_size - 1) // page_size) * page_size \
             + ((dt_size + page_size - 1) // page_size) * page_size

    length = ((length + page_size - 1) // page_size) * page_size

    if length <= 0:
        raise ValueError('Invalid image header: invalid length')

    return length


def get_signable_image_size(image_path):
    with open(image_path, 'rb') as f:
        return __get_signable_image_size(f)


def get_image_hash(image_path, extra_data=None, chunk_size=1*1024*1024):
    digest = SHA256.new()
    with open(image_path, 'rb') as f:
        for block in iter(lambda: f.read(chunk_size), b''):
            digest.update(block)

    if extra_data is not None:
        digest.update(extra_data)

    return digest

def verify(image_path):
    image_length = os.path.getsize(image_path)
    signable_size = get_signable_image_size(image_path)
    if signable_size == image_length:
        print('NOTE: missing signature. Aborting.')
        exit(0)
    hash=SHA256.new()
    with open(image_path,'rb') as rf:
        rf.seek(signable_size)
        signature=rf.read()
        rf.seek(0)
        toread=signable_size
        while (toread):
            size=1*1024*1024
            if toread<size:
                size=toread
            data=rf.read(size)
            hash.update(data)
            toread-=size
    der_items=der_decoder(signature,asn1Spec=BootSignature())
    authenticated_attributes = der_items[0]['authenticatedAttributes']
    encoded_authenticated_attributes = der_encoder(authenticated_attributes)
    hash.update(encoded_authenticated_attributes)
    digest=hash.digest()

    publickey=der_items[0]["certificate"]["tbsCertificate"]["subjectPublicKeyInfo"]["subjectPublicKey"]
    publickey=int(str(publickey), 2).to_bytes((len(str(publickey)) + 7) // 8, 'big')
    signature = bytes(der_items[0]["signature"])

    key=Crypto.PublicKey.RSA.importKey(publickey)

    def extract_hash(pub_key, data):
        hashlen = 32  # SHA256
        encrypted = int(hexlify(data), 16)
        decrypted = hex(pow(encrypted, pub_key.e, pub_key.n))[2:]
        #print(decrypted)
        if len(decrypted) % 2 != 0:
            decrypted = "0" + decrypted
        decrypted = unhexlify(decrypted)
        hash = decrypted[-hashlen:]
        if (decrypted[-0x21:-0x20] != b'\x20') or (len(hash) != hashlen):
            raise Exception('Signature error')
        return hash

    result=extract_hash(key,signature)
    if result==digest:
        print("Signature verification ok.")
    else:
        print("Signature verification failed.")
    return result

def sign(target, image_path, key_path, cert_path):
    image_length = os.path.getsize(image_path)
    signable_size = get_signable_image_size(image_path)

    if signable_size < image_length:
        print('NOTE: truncating file', image_path, 'from', image_length,
              'to', signable_size, 'bytes', file=sys.stderr)
        with open(image_path, 'rb+') as f:
            f.truncate(signable_size)
    elif signable_size > image_length:
        raise ValueError('Invalid image: too short, expected {} bytes'.format(signable_size))

    boot_signature = BootSignature.create(target, signable_size)

    with open(cert_path,'r') as rf:
        # cert2 = x509.load_pem_x509_certificate(pem_data, default_backend())
        data = pem.readPemFromFile(rf)
        key, rest = decoder.decode(data, asn1Spec=rfc2459.Certificate())
        boot_signature['certificate'] = key
        authenticated_attributes = boot_signature['authenticatedAttributes']
        encoded_authenticated_attributes = der_encoder(authenticated_attributes)

        digest = get_image_hash(image_path,
                                extra_data=encoded_authenticated_attributes)

        #key = RSA.load_key(key_path)
        data=open(key_path, 'rb').read()
        key = Crypto.PublicKey.RSA.importKey(data)
        #signature = key.sign(digest, algo='sha256')
        signer = PKCS1_v1_5.new(key)
        signature = signer.sign(digest)

        boot_signature['signature'] = signature
        boot_signature['algorithmIdentifier']['algorithm'] = rfc4055.sha256WithRSAEncryption

        encoded_boot_signature = der_encoder(boot_signature)

        with open(image_path, 'ab') as f:
            f.write(encoded_boot_signature)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Sign android boot image.')
    parser.add_argument('-t', '--target',
                        help='target name, typically /boot')
    parser.add_argument('-k', '--key',
                        help='path to a private key (PEM)')
    parser.add_argument('-c', '--cert',
                        help='path to the matching public key certificate')
    parser.add_argument('-i', '--input', required=True,
                        help='path to boot image to sign')
    parser.add_argument('-o', '--output',
                        help='where to output the signed boot image')
    parser.add_argument('-s', '--sign', action="store_true",
                        help='sign a boot image')
    parser.add_argument('-v', '--verify', action="store_true",
                        help='verify a signed boot image')
    args = parser.parse_args()

    if args.sign:
        if args.output==None or args.key==None or args.cert==None or args.target==None:
            print("Usage: ./bootsignature.py -s -i [imagetosign] -o [outfilename] -c [certfile] -k [keyfile] -t [target]")
            print("Example: ./bootsignature.py -s -i boot.img -o boot.signed -c verity.x509.pem -k verity.pem -t /boot")
            exit(0)
        shutil.copy(args.input, args.output)
        sign(target=args.target, image_path=args.output,
             key_path=args.key, cert_path=args.cert)
        print("Done signing.")
    if args.verify:
        verify(image_path=args.input)
