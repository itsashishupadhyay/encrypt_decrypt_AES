#include <iostream>
#include "encrypt_aes.h"

using namespace std;


void help_menu() {
    std::cout << ("Usage: ./main [options] \n");
    std::cout << ("Options: \n");
    std::cout << ("  -h, --help, Display this help menu \n");
    std::cout << ("  -e, --encrypt, provide encryption level 128-bit, 192-bit, or 256-bit\n");
    std::cout << ("  -d, --decrypt, provide decryption key\n");
    std::cout <<("  -p, --path        Specify the path to the file\n");
    std::cout <<("  -v, --verbose     verbose output\n");
  

}

int main(int argc, char **argv)
{
    bool verbose = false;
    bool encrypt = false;
    int encryption_level = 0;
    std::string path;
    
     AES::Encryptor encryptor;

     AES::Encryptor encryptor_128;

    encryptor.generate_save_key(256);
    encryptor_128.generate_save_key(128);


    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-h" || arg == "--help") {
            help_menu();
        } else if (arg == "-e" || arg == "--encrypt") {
            encrypt = true;
            encryption_level = std::stoi(argv[i+1]);
            if (encryption_level != 128 && encryption_level != 192 && encryption_level != 256) {
                std::cout << "Invalid encryption level. Please provide 128, 192, or 256\n";
                help_menu();
                return 1;
            }
        } 

    }

encryptor.print_key();
        encryptor_128.print_key();

}