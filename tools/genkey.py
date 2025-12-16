#!/usr/bin/env python3
"""
Generate ECDSA P-256 keypair for STM32 Secure Boot Challenge

Creates:
    keys/private_key.pem - Private key (KEEP SECRET)
    keys/public_key.pem  - Public key 
    
Also prints the public key as a C array for embedding in bootloader.
"""

import os
import sys
from pathlib import Path

try:
    from cryptography.hazmat.primitives.asymmetric import ec
    from cryptography.hazmat.primitives import serialization
    from cryptography.hazmat.backends import default_backend
except ImportError:
    print("Error: cryptography library not installed")
    print("Install with: pip install cryptography")
    sys.exit(1)


def main():
    # Create keys directory
    keys_dir = Path(__file__).parent.parent / "keys"
    keys_dir.mkdir(exist_ok=True)
    
    priv_path = keys_dir / "private_key.pem"
    pub_path = keys_dir / "public_key.pem"
    
    # Check if keys already exist
    if priv_path.exists():
        print(f"Warning: {priv_path} already exists!")
        response = input("Overwrite? [y/N] ")
        if response.lower() != 'y':
            print("Aborted.")
            sys.exit(0)
    
    # Generate keypair
    print("Generating ECDSA P-256 keypair...")
    private_key = ec.generate_private_key(ec.SECP256R1(), default_backend())
    public_key = private_key.public_key()
    
    # Save private key
    priv_pem = private_key.private_bytes(
        encoding=serialization.Encoding.PEM,
        format=serialization.PrivateFormat.PKCS8,
        encryption_algorithm=serialization.NoEncryption()
    )
    priv_path.write_bytes(priv_pem)
    print(f"Private key saved to: {priv_path}")
    
    # Save public key
    pub_pem = public_key.public_bytes(
        encoding=serialization.Encoding.PEM,
        format=serialization.PublicFormat.SubjectPublicKeyInfo
    )
    pub_path.write_bytes(pub_pem)
    print(f"Public key saved to: {pub_path}")
    
    # Get raw public key bytes (64 bytes: x || y)
    pub_numbers = public_key.public_numbers()
    x_bytes = pub_numbers.x.to_bytes(32, byteorder='big')
    y_bytes = pub_numbers.y.to_bytes(32, byteorder='big')
    raw_pubkey = x_bytes + y_bytes
    
    # Print as C array
    print("\n" + "=" * 60)
    print("Copy this into src/crypto.c (public_key array):")
    print("=" * 60)
    print("static const u8 public_key[64] = {")
    print("    /* X coordinate (32 bytes) */")
    for i in range(0, 32, 8):
        line = "    " + ", ".join(f"0x{b:02x}" for b in x_bytes[i:i+8]) + ","
        print(line)
    print("    /* Y coordinate (32 bytes) */")
    for i in range(0, 32, 8):
        line = "    " + ", ".join(f"0x{b:02x}" for b in y_bytes[i:i+8])
        if i < 24:
            line += ","
        print(line)
    print("};")
    print("=" * 60)
    
    # Also save raw pubkey for reference
    raw_path = keys_dir / "public_key.bin"
    raw_path.write_bytes(raw_pubkey)
    print(f"\nRaw public key (64 bytes) saved to: {raw_path}")


if __name__ == '__main__':
    main()

