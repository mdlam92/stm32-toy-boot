#!/usr/bin/env python3
"""
Sign Application Image for STM32 Secure Boot Challenge

Uses ECDSA P-256 with SHA-256 to sign application images.

Usage:
    python3 sign_image.py <input.bin> <output.bin> --key <private_key.pem>

The output file will have:
    - 256-byte header (app_header_t)
    - Original binary contents

The signature covers the application binary (NOT the header).
"""

import argparse
import struct
import sys
from pathlib import Path

try:
    from cryptography.hazmat.primitives import hashes
    from cryptography.hazmat.primitives.asymmetric import ec
    from cryptography.hazmat.primitives.asymmetric.utils import decode_dss_signature
    from cryptography.hazmat.primitives import serialization
    from cryptography.hazmat.backends import default_backend
except ImportError:
    print("Error: cryptography library not installed")
    print("Install with: pip install cryptography")
    sys.exit(1)

# Must match bootloader.h
APP_MAGIC = 0xDEADBEEF
APP_IMAGE_ADDR = 0x08004100
HEADER_SIZE = 256
SIGNATURE_SIZE = 64


def parse_version(version_str: str) -> int:
    """Parse semantic version string to packed u32."""
    parts = version_str.split('.')
    if len(parts) != 3:
        raise ValueError(f"Invalid version format: {version_str}")
    
    major, minor, patch = map(int, parts)
    if not (0 <= major <= 255 and 0 <= minor <= 255 and 0 <= patch <= 65535):
        raise ValueError(f"Version components out of range: {version_str}")
    
    # Pack as: major.minor.patch -> 0xMMmmPPPP
    return (major << 24) | (minor << 16) | patch


def load_private_key(key_path: Path) -> ec.EllipticCurvePrivateKey:
    """Load ECDSA private key from PEM file."""
    key_data = key_path.read_bytes()
    private_key = serialization.load_pem_private_key(
        key_data, password=None, backend=default_backend()
    )
    
    if not isinstance(private_key, ec.EllipticCurvePrivateKey):
        raise ValueError("Key is not an EC private key")
    
    if not isinstance(private_key.curve, ec.SECP256R1):
        raise ValueError(f"Key must be P-256, got {private_key.curve.name}")
    
    return private_key


def sign_data(private_key: ec.EllipticCurvePrivateKey, data: bytes) -> bytes:
    """
    Sign data with ECDSA P-256 / SHA-256.
    
    Returns 64-byte signature (r || s, each 32 bytes, big-endian).
    """
    # Sign with SHA-256
    der_signature = private_key.sign(data, ec.ECDSA(hashes.SHA256()))
    
    # Decode DER signature to get r and s
    r, s = decode_dss_signature(der_signature)
    
    # Convert to fixed-size big-endian bytes
    r_bytes = r.to_bytes(32, byteorder='big')
    s_bytes = s.to_bytes(32, byteorder='big')
    
    return r_bytes + s_bytes


def create_header(app_data: bytes, version: int, signature: bytes) -> bytes:
    """Create the 256-byte application header."""
    
    app_size = len(app_data)
    
    # Entry point is the reset handler (second word in vector table)
    # For Cortex-M, first word is SP, second is reset vector
    if len(app_data) >= 8:
        entry_point = struct.unpack('<I', app_data[4:8])[0]
    else:
        entry_point = APP_IMAGE_ADDR
    
    # Build header struct:
    # u32 magic
    # u32 version  
    # u32 app_size
    # u32 entry_point
    # u32 load_addr
    # u8  signature[64]
    # u8  reserved[172]
    
    header = struct.pack('<IIIII',
        APP_MAGIC,
        version,
        app_size,
        entry_point,
        APP_IMAGE_ADDR
    )
    
    header += signature
    header += b'\x00' * 172  # Reserved/padding
    
    assert len(header) == HEADER_SIZE, f"Header size mismatch: {len(header)}"
    return header


def main():
    parser = argparse.ArgumentParser(
        description='Sign an application image for the STM32 secure boot challenge'
    )
    parser.add_argument('input', help='Input binary file')
    parser.add_argument('output', help='Output signed image file')
    parser.add_argument('--key', '-k', required=True,
                        help='Private key PEM file')
    parser.add_argument('--version', '-v', default='1.0.0',
                        help='Application version (default: 1.0.0)')
    
    args = parser.parse_args()
    
    # Read input binary
    input_path = Path(args.input)
    if not input_path.exists():
        print(f"Error: Input file not found: {args.input}", file=sys.stderr)
        sys.exit(1)
    
    app_data = input_path.read_bytes()
    
    if len(app_data) == 0:
        print("Error: Input file is empty", file=sys.stderr)
        sys.exit(1)
    
    # Load private key
    key_path = Path(args.key)
    if not key_path.exists():
        print(f"Error: Key file not found: {args.key}", file=sys.stderr)
        print("Run 'make genkey' to generate a keypair.", file=sys.stderr)
        sys.exit(1)
    
    try:
        private_key = load_private_key(key_path)
    except Exception as e:
        print(f"Error loading key: {e}", file=sys.stderr)
        sys.exit(1)
    
    # Parse version
    try:
        version = parse_version(args.version)
    except ValueError as e:
        print(f"Error: {e}", file=sys.stderr)
        sys.exit(1)
    
    # Sign the application data
    print(f"Signing {len(app_data)} bytes...")
    signature = sign_data(private_key, app_data)
    
    # Create header
    header = create_header(app_data, version, signature)
    
    # Write output
    output_path = Path(args.output)
    output_path.write_bytes(header + app_data)
    
    # Print info
    print(f"Signed image created: {args.output}")
    print(f"  Header size:  {HEADER_SIZE} bytes")
    print(f"  App size:     {len(app_data)} bytes")
    print(f"  Total size:   {len(header) + len(app_data)} bytes")
    print(f"  Version:      {args.version} (0x{version:08X})")
    print(f"  Entry point:  0x{struct.unpack('<I', app_data[4:8])[0]:08X}")
    print(f"  Signature:    {signature[:8].hex()}...{signature[-8:].hex()}")


if __name__ == '__main__':
    main()
