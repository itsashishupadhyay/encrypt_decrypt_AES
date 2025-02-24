#ifndef ENCRYPT_AES_H
#define ENCRYPT_AES_H

#include <string>
#include <iostream>
#include <fstream>
#include <openssl/aes.h>
#include <openssl/err.h>
#include <openssl/rand.h>

#ifdef __cplusplus
extern "C" {
#endif

namespace AES {

class Encryptor {
public:
    void generate_save_key(int user_key_size);
    // Get key pointer
    unsigned char* getKey() { return key_data.key; }
    // Get key length
    size_t getKeyLength() { return key_data.length; }


    std::string encrypt_file(const std::string &plainText);
    void print_key();
    

private:
    struct key_data {
        unsigned char key[32]; // 256-bit key
        size_t length; // Key length in bytes
    } key_data;

};

} // namespace AES

#ifdef __cplusplus
}
#endif

#endif // ENCRYPT_AES_H