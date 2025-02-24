#include <iostream>
#include "encrypt_aes.h"

using namespace std;

void help_menu()
{
    std::cout << ("Usage: ./main [options] \n");
    std::cout << ("Options: \n");
    std::cout << ("  -h, --help, Display this help menu \n");
    std::cout << ("  -e, --encrypt, provide encryption level 128-bit, 192-bit, or 256-bit\n");
    std::cout << ("  -d, --decrypt, provide decryption key\n");
    std::cout << ("  -p, --path        Specify the path to the file\n");
    std::cout << ("  -v, --verbose     verbose output\n");
}

int main(int argc, char **argv)
{
    bool verbose_flag = false;
    bool encrypt_flag = false;
    bool path_flag = false;
    int encryption_level = 0;
    std::string path2file;

    AES::Encryptor encryptor;

    for (int i = 1; i < argc; i++)
    {
        std::string arg = argv[i];
        if (arg == "-h" || arg == "--help")
        {
            help_menu();
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
    }
    if (encrypt_flag)
    {
        encryptor.generate_save_key(encryption_level);
        encryptor.print_key();
        encryptor.encrypt_file(path2file=path2file);
    }
}