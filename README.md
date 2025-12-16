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

## Understanding AES Encryption

### What is AES? (In a Nutshell)

**AES (Advanced Encryption Standard)** is a symmetric block cipher established by NIST in 2001 as the U.S. government standard for encrypting sensitive data. It's a **symmetric algorithm**, meaning the same key is used for both encryption and decryption. AES operates on fixed-size blocks of 128 bits (16 bytes) and supports key sizes of 128, 192, or 256 bits.

AES is widely regarded as one of the most secure encryption algorithms available today and is used globally by governments, banks, and security-conscious organizations to protect classified and sensitive information.

### Mathematical Foundation

AES is based on the **Rijndael cipher**, designed by Belgian cryptographers Vincent Rijmen and Joan Daemen. The algorithm performs multiple rounds of transformations on the data:

- **AES-128**: 10 rounds
- **AES-192**: 12 rounds
- **AES-256**: 14 rounds

Each round (except the last) consists of four mathematical operations performed on a 4×4 matrix of bytes called the **state**:

1. **SubBytes** (S-box substitution):
   - Non-linear byte substitution using a substitution table (S-box)
   - Each byte is replaced with another according to a lookup table
   - Provides confusion by obscuring the relationship between plaintext and ciphertext
   - Based on the multiplicative inverse in the Galois Field GF(2⁸)

2. **ShiftRows**:
   - Cyclically shifts the rows of the state matrix
   - Row 0: no shift, Row 1: 1 byte left, Row 2: 2 bytes left, Row 3: 3 bytes left
   - Provides diffusion by mixing bytes across columns

3. **MixColumns**:
   - Linear mixing operation on the columns of the state
   - Each column is treated as a polynomial and multiplied with a fixed polynomial modulo x⁴ + 1
   - Operates in Galois Field GF(2⁸) for maximum diffusion
   - Ensures that changes in one byte affect multiple bytes in the output

4. **AddRoundKey**:
   - XORs the state with a round key derived from the main encryption key
   - Round keys are generated using the **Key Expansion** algorithm
   - Provides the actual "keying" of the encryption

The final round omits the MixColumns step. This mathematical structure ensures that:
- **Confusion**: Complex relationship between key and ciphertext (via SubBytes and AddRoundKey)
- **Diffusion**: Spreading plaintext bits across ciphertext (via ShiftRows and MixColumns)

**Key Schedule**: AES uses a key expansion algorithm that takes the original key and generates a separate round key for each round using recursive transformations and the Rijndael S-box.

### CBC Mode: Cipher Block Chaining

This tool uses **CBC (Cipher Block Chaining)** mode, one of the most widely used block cipher modes of operation.

#### How CBC Works

1. The plaintext is divided into blocks (128-bit blocks for AES)
2. Each plaintext block is XORed with the previous ciphertext block before encryption
3. The first block is XORed with an **Initialization Vector (IV)** - a random 128-bit value
4. The output of each encryption becomes the input for the next block

```
C₁ = Eₖ(P₁ ⊕ IV)
C₂ = Eₖ(P₂ ⊕ C₁)
C₃ = Eₖ(P₃ ⊕ C₂)
...
```

Where:
- `Cᵢ` = Ciphertext block i
- `Pᵢ` = Plaintext block i
- `Eₖ` = AES encryption with key K
- `⊕` = XOR operation

#### Strengths of CBC Mode

✅ **Diffusion Across Blocks**: Each ciphertext block depends on all previous plaintext blocks, creating an avalanche effect

✅ **IV Randomization**: Using a unique random IV for each encryption ensures that identical plaintexts produce different ciphertexts

✅ **Pattern Hiding**: Unlike ECB mode, CBC effectively hides patterns in the plaintext data

✅ **Industry Standard**: Well-studied, widely implemented, and trusted by security standards (TLS 1.0-1.2, IPsec)

✅ **Parallel Decryption**: While encryption must be sequential, decryption can be parallelized

#### Weaknesses and Vulnerabilities of CBC Mode

⚠️ **Padding Oracle Attacks**:
- If an attacker can determine whether padding is valid, they can decrypt data byte-by-byte
- Famous examples: POODLE attack (SSLv3), Lucky Thirteen (TLS)
- **Impact**: Complete plaintext recovery without knowing the key

⚠️ **IV Predictability**:
- If the IV is predictable or reused, the first block's encryption is vulnerable
- Chosen plaintext attacks become possible
- **Impact**: Can reveal information about the plaintext

⚠️ **Bit-Flipping Attacks**:
- Modifying ciphertext bits in block `n` causes predictable corruption in block `n` and unpredictable changes in block `n+1`
- An attacker with write access can manipulate specific plaintext bits
- **Impact**: Integrity violations, potential authentication bypass

⚠️ **No Built-in Authentication**:
- CBC provides confidentiality but NOT integrity or authenticity
- Ciphertext can be modified without detection
- **Impact**: Vulnerable to tampering

⚠️ **Sequential Encryption**:
- Cannot parallelize encryption (each block depends on the previous)
- **Impact**: Slower performance on modern multi-core processors

#### Mitigations and Best Practices

🛡️ **Use Authenticated Encryption (AEAD)**:
- **Recommended**: AES-GCM (Galois/Counter Mode), AES-CCM, or ChaCha20-Poly1305
- Provides both confidentiality AND authenticity
- Detects any tampering with the ciphertext
- **Best Practice**: For new implementations, prefer AEAD modes

🛡️ **Implement HMAC for Authentication**:
- Use "Encrypt-then-MAC" approach: `ciphertext + HMAC(key, ciphertext)`
- Verify HMAC before attempting decryption (prevents padding oracle attacks)
- Use separate keys for encryption and MAC

🛡️ **Secure IV Generation**:
- **Always generate cryptographically random IVs** using a CSPRNG (e.g., OpenSSL's `RAND_bytes()`)
- Never reuse IVs with the same key
- IV doesn't need to be secret but must be unpredictable
- This tool uses OpenSSL's secure random generator for IV creation ✓

🛡️ **Constant-Time Padding Validation**:
- Implement padding checks in constant time to prevent timing attacks
- Don't leak information about padding validity

🛡️ **Regular Key Rotation**:
- Limit the amount of data encrypted with a single key
- Rotate keys periodically (e.g., after encrypting 2³² blocks ≈ 64 GB with AES-128)

🛡️ **Transport Security**:
- When transmitting encrypted data, use TLS 1.3 (which deprecates CBC mode)
- Never transmit keys over insecure channels

### Further Reading

For deeper understanding of AES and cryptographic modes:

- **NIST FIPS 197** - Official AES Specification: [https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.197-upd1.pdf](https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.197-upd1.pdf)
- **NIST SP 800-38A** - Block Cipher Modes of Operation: [https://nvlpubs.nist.gov/nistpubs/Legacy/SP/nistspecialpublication800-38a.pdf](https://nvlpubs.nist.gov/nistpubs/Legacy/SP/nistspecialpublication800-38a.pdf)
- **The Design of Rijndael** (Original AES Paper): [https://link.springer.com/book/10.1007/978-3-662-04722-4](https://link.springer.com/book/10.1007/978-3-662-04722-4)
- **Serious Cryptography** by Jean-Philippe Aumasson - Chapter 4 (Block Ciphers): [https://nostarch.com/seriouscrypto](https://nostarch.com/seriouscrypto)
- **Cryptography Engineering** by Ferguson, Schneier, and Kohno: [https://www.schneier.com/books/cryptography-engineering/](https://www.schneier.com/books/cryptography-engineering/)
- **Applied Cryptography** by Bruce Schneier: [https://www.schneier.com/books/applied-cryptography/](https://www.schneier.com/books/applied-cryptography/)
- **Understanding Cryptography** by Christof Paar (with video lectures): [https://www.crypto-textbook.com/](https://www.crypto-textbook.com/)

**Online Resources:**
- **AES Visualization**: [https://www.cryptool.org/en/cto/aes-animation](https://www.cryptool.org/en/cto/aes-animation)
- **Block Cipher Modes Explained**: [https://en.wikipedia.org/wiki/Block_cipher_mode_of_operation](https://en.wikipedia.org/wiki/Block_cipher_mode_of_operation)
- **OpenSSL EVP Documentation**: [https://www.openssl.org/docs/man3.0/man7/evp.html](https://www.openssl.org/docs/man3.0/man7/evp.html)

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

3. **Verification**:
   - You must manually decrypt the file using the `-d` option to verify the encryption was successful
   - Use the key and IV from the generated key file to decrypt

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

### **CRITICAL: Key File Security** ⚠️

**The security of your encrypted files depends entirely on the security of your key files (`aes_key*.txt`). This is YOUR RESPONSIBILITY.**

- **Anyone with access to the key file can decrypt your files** - treat these files with extreme caution
- **DO NOT leave key files in the working directory** - they should be moved immediately after generation
- **Recommended actions**:
  - Move key files to a secured location with restricted permissions (e.g., `chmod 600 aes_key*.txt`)
  - Store keys in a password manager or encrypted vault
  - Add key files to your system keychain (macOS Keychain, Windows Credential Manager, Linux Secret Service)
  - Consider encrypting the key files themselves with a master password
  - **Add `*.key` and `aes_key*.txt` to your `.gitignore`** to prevent accidental commits
- **NEVER**:
  - Commit key files to version control
  - Send key files over unencrypted channels (email, messaging apps)
  - Store keys in cloud storage without additional encryption
  - Leave keys on shared or public systems

**If you lose the key file, your encrypted data is irrecoverable. If someone else obtains your key file, your encrypted data is compromised.**

### Additional Security Considerations

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
