#ifndef ENCRYPT_AES_H
#define ENCRYPT_AES_H

#include <string>
#include <iostream>
#include <fstream>
#include <openssl/aes.h>
#include <openssl/err.h>
#include <openssl/rand.h>

#ifdef __cplusplus
extern "C"
{
#endif

    namespace AES
    {

        class Encryptor
        {
        public:
            void generate_save_key(int user_key_size);
            // Get key pointer
            unsigned char *get_key() { return key_data.key; }
            // Get key pointer
            unsigned char *get_iv() { return key_data.iv; }
            // Get key length
            size_t getKeyLength() { return key_data.length; }
            int get_iv_length() { return AES_BLOCK_SIZE; }

            void encrypt_file(const std::string &path2file, const std::string &path2destination = std::string());
            void decrypt_file(const std::string &path2encryptedfile,
                      const unsigned char *key,
                      size_t key_length,
                      const unsigned char *iv,
                      const std::string &path2destination);
            void print_key();

        private:
            struct key_data
            {
                unsigned char key[32];            // Max 256-bit key
                unsigned char iv[AES_BLOCK_SIZE]; // Initialization vector
                size_t length;                    // Key length in bytes
            } key_data;
        };

    } // namespace AES

#ifdef __cplusplus
}
#endif

#endif // ENCRYPT_AES_H