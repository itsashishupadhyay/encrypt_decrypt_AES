# AES File Encryption and Decryption Tool

A high-performance C++ command-line utility for AES (Advanced Encryption Standard) file encryption and decryption. This tool provides secure file encryption using OpenSSL with support for 128-bit, 192-bit, and 256-bit AES-CBC encryption modes.

## Features

- **Multiple AES Key Sizes**: Support for AES-128, AES-192, and AES-256 bit encryption
- **CBC Mode**: Uses Cipher Block Chaining (CBC) mode for enhanced security
- **Automatic Key Generation**: Generates cryptographically secure random keys and initialization vectors (IV)
- **Key File Export**: Saves encryption keys to a text file for secure storage and sharing
- **File Encryption/Decryption**: Encrypt and decrypt files of any type and size
- **Chunk Processing**: Efficiently handles large files through buffered processing
- **Command-Line Interface**: Easy-to-use CLI with flexible options

## Requirements

### System Requirements
- C++17 compatible compiler (GCC 7+, Clang 5+, or MSVC 2017+)
- CMake 3.10 or higher
- OpenSSL development libraries

### Dependencies
- **OpenSSL**: Provides AES encryption/decryption functionality
  - Linux (Ubuntu/Debian): `libssl-dev`
  - Linux (Fedora/RHEL): `openssl-devel`
  - macOS: `openssl` (via Homebrew)
  - Windows: Download from [OpenSSL website](https://www.openssl.org/)

## Installation

### Install Dependencies

**Ubuntu/Debian:**
```bash
sudo apt-get update
sudo apt-get install build-essential cmake libssl-dev
```

**Fedora/RHEL/CentOS:**
```bash
sudo dnf install gcc-c++ cmake openssl-devel
```

**macOS (using Homebrew):**
```bash
brew install cmake openssl
```

**Windows:**
- Install [Visual Studio](https://visualstudio.microsoft.com/) with C++ support
- Install [CMake](https://cmake.org/download/)
- Install [OpenSSL for Windows](https://slproweb.com/products/Win32OpenSSL.html)

### Build from Source

1. **Clone or download the repository:**
```bash
cd /path/to/encrypt_decrypt_AES
```

2. **Build the project using CMake:**
```bash
cmake -B build -S .
cmake --build build
```

Alternatively, you can use Ninja for faster builds:
```bash
cmake -B build -S . -G Ninja
cmake --build build
```

3. **The executable will be created in the build directory:**
```bash
./build/encrypt_decrypt_AES
```

## Usage

### Basic Syntax
```bash
./encrypt_decrypt_AES [options]
```

### Command-Line Options

| Option | Description |
|--------|-------------|
| `-h, --help` | Display help menu with usage information |
| `-e, --encrypt <bits>` | Encrypt a file with specified key size (128, 192, or 256 bits) |
| `-d, --decrypt <key>` | Decrypt a file using provided key in hexadecimal format |
| `-iv <initialization_vector>` | Initialization vector in hexadecimal format (required with `-d`) |
| `-p, --path <filepath>` | Specify the path to the file to encrypt/decrypt |
| `-o, --output <directory>` | Specify output directory (optional, defaults to input file directory) |
| `-v, --verbose` | Enable verbose output for detailed processing information |

### Examples

#### 1. Encrypt a file with 256-bit AES:
```bash
./encrypt_decrypt_AES -e 256 -p /path/to/your/file.txt
```

**Output:**
- Encrypted file: `file_encrypted.txt.bin`
- Key file: `aes_key256.txt` (contains the encryption key and IV)
- Decrypted file: `file_encrypted_decrypted.txt.bin` (verification)

#### 2. Encrypt with 128-bit AES:
```bash
./encrypt_decrypt_AES -e 128 -p document.pdf
```

#### 3. Encrypt with 192-bit AES:
```bash
./encrypt_decrypt_AES -e 192 -p image.jpg
```

#### 4. Decrypt a file:
```bash
./encrypt_decrypt_AES -d aeee95f1d12e20c27ff4f4788ec52147f91a833d946415b37c6eb687053bd00a \
                      -iv 06ec7db13e5c7d8a443c44c8516c308c \
                      -p file_encrypted.txt.bin
```

**Note:** The key and IV are found in the `aes_key<bits>.txt` file generated during encryption.

**Output:**
- Decrypted file: `file_encrypted.txt_decrypted.bin` (in same directory as encrypted file)

#### 5. Decrypt with custom output directory:
```bash
./encrypt_decrypt_AES -d aeee95f1d12e20c27ff4f4788ec52147f91a833d946415b37c6eb687053bd00a \
                      -iv 06ec7db13e5c7d8a443c44c8516c308c \
                      -p file_encrypted.txt.bin \
                      -o /path/to/output
```

#### 6. Display help:
```bash
./encrypt_decrypt_AES -h
```

## How It Works

### Encryption Process

1. **Key Generation**:
   - Generates a cryptographically secure random AES key (16, 24, or 32 bytes for 128, 192, or 256-bit encryption)
   - Generates a random 16-byte initialization vector (IV)
   - Saves both key and IV to a text file (`aes_key<bits>.txt`)

2. **File Encryption**:
   - Reads the input file in 4KB chunks for memory efficiency
   - Encrypts data using AES-CBC mode with the generated key and IV
   - Writes encrypted data to output file with `_encrypted` suffix
   - Original filename extension is preserved with `.bin` added

3. **Automatic Verification**:
   - After encryption, automatically decrypts the file to verify integrity
   - Creates a decrypted file with `_decrypted` suffix for verification

### Decryption Process

1. **Key and IV Input**:
   - User provides the encryption key and IV from the saved key file (`aes_key<bits>.txt`)
   - Both must be in hexadecimal format
   - Key can be 32, 48, or 64 hex characters (16, 24, or 32 bytes for AES-128/192/256)
   - IV must be exactly 32 hex characters (16 bytes)

2. **File Decryption**:
   - Reads the encrypted file
   - Decrypts data using AES-CBC mode with the provided key and IV
   - Writes decrypted data to output file with `_decrypted` suffix
   - Output is placed in the same directory as the input file (or custom directory if `-o` is used)

3. **Validation**:
   - Validates key and IV format before attempting decryption
   - Ensures key length matches AES requirements
   - Verifies IV is exactly 16 bytes

### Key File Format

The generated key file (e.g., `aes_key256.txt`) contains:
```
AES Key (256 bits):
<hexadecimal key data>
IV AES_BLOCK_SIZE (16):
<hexadecimal IV data>
```

**IMPORTANT**: Store this key file securely. You will need it to decrypt your files later.

## Security Considerations

- **Key Storage**: Keep your key files (`aes_key*.txt`) in a secure location. Anyone with access to the key can decrypt your files.
- **AES-CBC Mode**: This implementation uses CBC (Cipher Block Chaining) mode, which provides good security for file encryption.
- **Random Number Generation**: Uses OpenSSL's `RAND_bytes()` for cryptographically secure random key generation.
- **Key Size**: For maximum security, use 256-bit encryption. 128-bit and 192-bit are also secure but offer less protection against brute-force attacks.

## Technical Details

- **Language**: C++17
- **Build System**: CMake 3.10+
- **Encryption Library**: OpenSSL (EVP API)
- **Encryption Algorithm**: AES (Advanced Encryption Standard)
- **Cipher Mode**: CBC (Cipher Block Chaining)
- **Block Size**: 128 bits (16 bytes)
- **Key Sizes**: 128, 192, or 256 bits
- **Buffer Size**: 4096 bytes for file I/O

## Project Structure

```
encrypt_decrypt_AES/
├── CMakeLists.txt              # Main CMake configuration
├── main.cpp                    # Main application entry point
├── README.md                   # This file
└── library/
    ├── CMakeLists.txt          # Library CMake configuration
    ├── include/
    │   └── encrypt_aes.h       # Header file with class definitions
    └── source/
        └── encrypt_aes.cpp     # Implementation of encryption/decryption
```

## Troubleshooting

### OpenSSL Not Found or CMake Configuration Errors

If CMake cannot find OpenSSL or fails with errors like `Target "encryption_decryption_library" links to: OpenSSL::SSL but the target was not found`, you may need to specify the OpenSSL paths explicitly.

**Quick Solution (macOS with Homebrew):**

For Intel Macs:
```bash
cmake -B build -S . \
  -DOPENSSL_ROOT_DIR=/usr/local/opt/openssl@3 \
  -DOPENSSL_INCLUDE_DIR=/usr/local/opt/openssl@3/include \
  -DOPENSSL_SSL_LIBRARY=/usr/local/opt/openssl@3/lib/libssl.dylib \
  -DOPENSSL_CRYPTO_LIBRARY=/usr/local/opt/openssl@3/lib/libcrypto.dylib
```

For Apple Silicon Macs (M1/M2/M3):
```bash
cmake -B build -S . \
  -DOPENSSL_ROOT_DIR=/opt/homebrew/opt/openssl@3 \
  -DOPENSSL_INCLUDE_DIR=/opt/homebrew/opt/openssl@3/include \
  -DOPENSSL_SSL_LIBRARY=/opt/homebrew/opt/openssl@3/lib/libssl.dylib \
  -DOPENSSL_CRYPTO_LIBRARY=/opt/homebrew/opt/openssl@3/lib/libcrypto.dylib
```

Then build:
```bash
cmake --build build
```

**Alternative: Navigate to build directory:**
```bash
cd build
cmake \
  -DOPENSSL_ROOT_DIR=/opt/homebrew/opt/openssl@3 \
  -DOPENSSL_INCLUDE_DIR=/opt/homebrew/opt/openssl@3/include \
  -DOPENSSL_SSL_LIBRARY=/opt/homebrew/opt/openssl@3/lib/libssl.dylib \
  -DOPENSSL_CRYPTO_LIBRARY=/opt/homebrew/opt/openssl@3/lib/libcrypto.dylib \
  ..
make
```

**Linux:**
```bash
cmake -B build -S . -DOPENSSL_ROOT_DIR=/usr
```

### Common Issues

**1. Stale CMake Cache:**
If you're still getting errors after specifying paths, clean the build directory:
```bash
rm -rf build
mkdir build
# Then run cmake with the OpenSSL paths as shown above
```

**2. Multiple OpenSSL Versions:**
If you have multiple OpenSSL versions installed, CMake might find stale references. Use the explicit path method shown above to force CMake to use the correct version.

**3. macOS SDK Warning:**
You might see a warning about `CMAKE_OSX_SYSROOT` - this can be safely ignored and doesn't affect the build.

### Build Errors
- Ensure you have a C++17 compatible compiler
- Verify OpenSSL development headers are installed:
  - macOS: `brew list openssl@3`
  - Ubuntu/Debian: `dpkg -l | grep libssl-dev`
  - Fedora/RHEL: `rpm -q openssl-devel`
- Check that CMake version is 3.10 or higher: `cmake --version`
- On macOS, ensure OpenSSL is installed: `brew install openssl@3`

## Keywords

AES encryption, AES decryption, file encryption, file decryption, OpenSSL encryption, AES-128, AES-192, AES-256, AES-CBC, secure file encryption, cryptography tool, command-line encryption, C++ encryption, encrypt files, decrypt files, data security, file security, encryption utility

## License

This project is provided as-is for educational and practical use.

## Contributing

Contributions, issues, and feature requests are welcome.

## Author

**Ashish Upadhyay**
Email: [itsashishupadhyay@gmail.com](mailto:itsashishupadhyay@gmail.com)

Created as a secure file encryption utility using industry-standard AES encryption.
