#include <iostream>
#include <sstream>
#include <iomanip>
#include "encrypt_aes.h"

using namespace std;

void help_menu()
{
    std::cout << "Usage: ./encrypt_decrypt_AES [options]\n\n";
    std::cout << "Options:\n";
    std::cout << "  -h, --help                    Display this help menu\n";
    std::cout << "  -e, --encrypt <bits>          Encrypt file with specified key size (128, 192, or 256)\n";
    std::cout << "  -d, --decrypt <key>           Decrypt file using provided key (hexadecimal format)\n";
    std::cout << "  -iv <initialization_vector>   Initialization vector in hex format (required with -d)\n";
    std::cout << "  -p, --path <filepath>         Path to the file to encrypt/decrypt\n";
    std::cout << "  -o, --output <directory>      Output directory (optional, defaults to input file directory)\n";
    std::cout << "  -v, --verbose                 Enable verbose output\n\n";
    std::cout << "Examples:\n";
    std::cout << "  Encrypt a file:\n";
    std::cout << "    ./encrypt_decrypt_AES -e 256 -p myfile.txt\n\n";
    std::cout << "  Decrypt a file:\n";
    std::cout << "    ./encrypt_decrypt_AES -d <key_hex> -iv <iv_hex> -p encrypted_file.bin\n\n";
    std::cout << "  Decrypt with custom output:\n";
    std::cout << "    ./encrypt_decrypt_AES -d <key_hex> -iv <iv_hex> -p encrypted_file.bin -o /output/dir\n";
}

// Helper function to convert hex string to byte array
bool hex_to_bytes(const std::string& hex, unsigned char* bytes, size_t max_length, size_t& actual_length)
{
    if (hex.length() % 2 != 0)
    {
        std::cerr << "Error: Hex string must have even number of characters.\n";
        return false;
    }

    actual_length = hex.length() / 2;
    if (actual_length > max_length)
    {
        std::cerr << "Error: Hex string too long (max " << max_length << " bytes).\n";
        return false;
    }

    for (size_t i = 0; i < actual_length; i++)
    {
        std::string byte_string = hex.substr(i * 2, 2);
        try
        {
            bytes[i] = static_cast<unsigned char>(std::stoi(byte_string, nullptr, 16));
        }
        catch (const std::exception& e)
        {
            std::cerr << "Error: Invalid hex character in string.\n";
            return false;
        }
    }
    return true;
}

int main(int argc, char **argv)
{
    bool verbose_flag = false;
    bool encrypt_flag = false;
    bool decrypt_flag = false;
    bool path_flag = false;
    bool iv_flag = false;
    int encryption_level = 0;
    std::string path2file;
    std::string key_hex;
    std::string iv_hex;
    std::string dest_path;

    AES::Encryptor encryptor;

    for (int i = 1; i < argc; i++)
    {
        std::string arg = argv[i];
        if (arg == "-h" || arg == "--help")
        {
            help_menu();
            return 0;
        }
        else if (arg == "-e" || arg == "--encrypt")
        {
            if (i + 1 >= argc)
            {
                std::cerr << "Error: Encryption level value not provided.\n";
                help_menu();
                return 1;
            }

            if (!isdigit(argv[i + 1][0]))
            {
                std::cerr << "Error: Invalid encryption level value.\n";
                help_menu();
                return 1;
            }
            encryption_level = std::stoi(argv[i + 1]);
            if (encryption_level != 128 && encryption_level != 192 && encryption_level != 256)
            {
                std::cout << "Invalid encryption level. Please provide 128, 192, or 256\n";
                help_menu();
                return 1;
            }
            encrypt_flag = true;
            i++; // Skip the next argument since it is the encryption level value
        }
        else if (arg == "-d" || arg == "--decrypt")
        {
            if (i + 1 >= argc)
            {
                std::cerr << "Error: Decryption key not provided.\n";
                help_menu();
                return 1;
            }
            key_hex = argv[i + 1];
            decrypt_flag = true;
            i++; // Skip the next argument since it is the key value
        }
        else if (arg == "-iv")
        {
            if (i + 1 >= argc)
            {
                std::cerr << "Error: Initialization vector not provided.\n";
                help_menu();
                return 1;
            }
            iv_hex = argv[i + 1];
            iv_flag = true;
            i++; // Skip the next argument since it is the IV value
        }
        else if (arg == "-p" || arg == "--path")
        {
            path_flag = true;
            if (i + 1 < argc)
            {
                path2file = argv[i + 1];
                i++; // Skip the next argument since it is the path value
            }
            else
            {
                std::cout << "Error: Path argument requires a value.\n";
                return 1;
            }
        }
        else if (arg == "-o" || arg == "--output")
        {
            if (i + 1 < argc)
            {
                dest_path = argv[i + 1];
                i++; // Skip the next argument since it is the output path value
            }
            else
            {
                std::cout << "Error: Output path argument requires a value.\n";
                return 1;
            }
        }
    }

    // Handle encryption
    if (encrypt_flag)
    {
        if (!path_flag)
        {
            std::cerr << "Error: Path to file is required for encryption.\n";
            help_menu();
            return 1;
        }
        encryptor.generate_save_key(encryption_level);
        encryptor.print_key();
        encryptor.encrypt_file(path2file, dest_path);
    }
    // Handle decryption
    else if (decrypt_flag)
    {
        if (!path_flag)
        {
            std::cerr << "Error: Path to encrypted file is required for decryption.\n";
            help_menu();
            return 1;
        }
        if (!iv_flag)
        {
            std::cerr << "Error: Initialization vector (-iv) is required for decryption.\n";
            help_menu();
            return 1;
        }

        // Convert hex strings to byte arrays
        unsigned char key[32];
        unsigned char iv[AES_BLOCK_SIZE];
        size_t key_length;
        size_t iv_length;

        if (!hex_to_bytes(key_hex, key, 32, key_length))
        {
            std::cerr << "Error: Failed to parse key hex string.\n";
            return 1;
        }

        if (!hex_to_bytes(iv_hex, iv, AES_BLOCK_SIZE, iv_length))
        {
            std::cerr << "Error: Failed to parse IV hex string.\n";
            return 1;
        }

        if (iv_length != AES_BLOCK_SIZE)
        {
            std::cerr << "Error: IV must be exactly " << AES_BLOCK_SIZE << " bytes ("
                      << (AES_BLOCK_SIZE * 2) << " hex characters).\n";
            return 1;
        }

        // Validate key length
        if (key_length != 16 && key_length != 24 && key_length != 32)
        {
            std::cerr << "Error: Key must be 16, 24, or 32 bytes (32, 48, or 64 hex characters) "
                      << "for AES-128, AES-192, or AES-256 respectively.\n";
            return 1;
        }

        std::cout << "Decrypting file: " << path2file << std::endl;
        std::cout << "Using " << (key_length * 8) << "-bit key" << std::endl;

        encryptor.decrypt_file(path2file, key, key_length, iv, dest_path);

        std::cout << "Decryption completed successfully." << std::endl;
    }
    else
    {
        std::cerr << "Error: Please specify either -e (encrypt) or -d (decrypt).\n";
        help_menu();
        return 1;
    }

    return 0;
}