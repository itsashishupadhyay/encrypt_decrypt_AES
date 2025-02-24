#include "encrypt_aes.h"
#include <string.h>

namespace AES{

void Encryptor::generate_save_key(int user_key_size) {
    
    if (user_key_size != 128 && user_key_size != 192 && user_key_size != 256) {
        throw std::invalid_argument("Invalid key size. Choose from 128, 192, or 256.");
    }

    
    key_data.length = user_key_size / 8;

    // Generate a random AES key
    if (RAND_bytes(key_data.key, user_key_size / 8) != 1) {
        throw std::runtime_error("Failed to generate random key.");
    }

    // Save the key to a text file
    std::string key_file_name = "aes_key";
    key_file_name += std::to_string(user_key_size);
    key_file_name += ".txt";
    std::ofstream keyFile(key_file_name, std::ios::binary);
    if (!keyFile.is_open()) {
        throw std::runtime_error("Failed to open key file.");
    }

    keyFile << "AES Key (" << user_key_size << " bits):" << std::endl;
    for (int i = 0; i < user_key_size / 8; i++) {
        keyFile << std::hex << (int) key_data.key[i];
    }
    keyFile << std::endl;

    keyFile.close();

    return;

}

void Encryptor:: print_key(){
    printf("AES Key (%zu bits)\n", key_data.length * 8);
    for (int i = 0; i < key_data.length; i++) {
        std::cout << std::hex << (int) key_data.key[i];
    }
    printf("\n");
}

} //name space AES