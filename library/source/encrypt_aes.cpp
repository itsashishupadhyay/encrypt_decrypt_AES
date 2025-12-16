#include "encrypt_aes.h"
#include <string.h>
#include <fstream>
#include <openssl/aes.h>
#include <openssl/err.h>

namespace AES
{

    void Encryptor::generate_save_key(int user_key_size)
    {

        if (user_key_size != 128 && user_key_size != 192 && user_key_size != 256)
        {
            throw std::invalid_argument("Invalid key size. Choose from 128, 192, or 256.");
        }

        key_data.length = user_key_size / 8;
        // Generate a random AES key
        if (RAND_bytes(key_data.key, user_key_size / 8) != 1)
        {
            throw std::runtime_error("Failed to generate random key.");
        }

        // Generate a random IV
        if (RAND_bytes(key_data.iv, AES_BLOCK_SIZE) != 1)
        {
            throw std::runtime_error("Failed to generate IV.");
        }

        // Save the key to a text file
        std::string key_file_name = "aes_key";
        key_file_name += std::to_string(user_key_size);
        key_file_name += ".txt";
        std::ofstream keyFile(key_file_name, std::ios::binary);
        if (!keyFile.is_open())
        {
            throw std::runtime_error("Failed to open key file.");
        }

        keyFile << "AES Key (" << user_key_size << " bits):" << std::endl;
        for (int i = 0; i < user_key_size / 8; i++)
        {
            keyFile << std::hex << std::setw(2) << std::setfill('0') << (int)key_data.key[i];
        }
        keyFile << std::endl;

        keyFile << "IV AES_BLOCK_SIZE (" << AES_BLOCK_SIZE << "):" << std::endl;
        for (int i = 0; i < AES_BLOCK_SIZE; i++)
        {
            keyFile << std::hex << std::setw(2) << std::setfill('0') << (int)key_data.iv[i];
        }
        keyFile << std::endl;

        keyFile.close();

        std::cout << "Key saved to " << key_file_name << std::endl;
        return;
    }

    void Encryptor::encrypt_file(const std::string &path2file, const std::string &path2destination)
    {
        // Open input file
        std::ifstream inFile(path2file, std::ios::binary);
        if (!inFile.is_open())
        {
            throw std::runtime_error("Error opening input file.");
        }

        // Get file size
        inFile.seekg(0, std::ios::end);
        size_t fileSize = inFile.tellg();
        inFile.seekg(0, std::ios::beg);

        // Print file details
        std::cout << "File name: " << path2file << std::endl;
        std::cout << "File size: " << fileSize << " bytes" << std::endl;

        // Set destination path if not provided
        std::string destPath = path2destination;
        if (destPath.empty())
        {
            size_t lastSlash = path2file.find_last_of("/\\");
            destPath = (lastSlash == std::string::npos) ? "." : path2file.substr(0, lastSlash);
        }

        // Create output file name
        size_t dotPos = path2file.find_last_of(".");
        std::string filename;
        if (dotPos != std::string::npos)
        {
            filename = path2file.substr(path2file.find_last_of("/\\") + 1, dotPos - path2file.find_last_of("/\\") - 1);
        }
        else
        {
            filename = path2file.substr(path2file.find_last_of("/\\") + 1);
        }
        std::string extension = path2file.substr(dotPos);
        std::string outputFile = destPath + "/" + filename + "_encrypted" + extension + ".bin";

        // Open output file
        std::ofstream outFile(outputFile, std::ios::binary);
        if (!outFile.is_open())
        {
            throw std::runtime_error("Error opening output file.");
        }

        // Set up AES encryption
        std::shared_ptr<EVP_CIPHER_CTX> ctx(EVP_CIPHER_CTX_new(), EVP_CIPHER_CTX_free);
        if (!ctx)
        {
            throw std::runtime_error("Error creating cipher context.");
        }

        if (EVP_EncryptInit_ex(ctx.get(),
                               (this->key_data.length == 32) ? EVP_aes_256_cbc() : (this->key_data.length == 16) ? EVP_aes_128_cbc()
                                                                                                                 : EVP_aes_192_cbc(),
                               NULL, this->key_data.key, this->key_data.iv) != 1)
        {
            throw std::runtime_error("Error initializing encryption.");
        }

        const size_t bufferSize = 4096;
        std::unique_ptr<unsigned char[]> buffer(new unsigned char[bufferSize]);
        std::unique_ptr<unsigned char[]> encryptedBuffer(new unsigned char[bufferSize + AES_BLOCK_SIZE]);
        int encryptedLength = 0;
        int finalLength = 0;

        while (true)
        {
            inFile.read((char *)buffer.get(), bufferSize);
            std::streamsize bytesRead = inFile.gcount();
            if (bytesRead <= 0)
                break;

            if (EVP_EncryptUpdate(ctx.get(), encryptedBuffer.get(), &encryptedLength, buffer.get(), bytesRead) != 1)
            {
                throw std::runtime_error("Error encrypting file.");
            }

            outFile.write((char *)encryptedBuffer.get(), encryptedLength);
            if (!outFile)
            {
                throw std::runtime_error("Error writing to output file.");
            }
        }

        if (EVP_EncryptFinal_ex(ctx.get(), encryptedBuffer.get(), &finalLength) != 1)
        {
            throw std::runtime_error("Error finalizing encryption.");
        }

        outFile.write((char *)encryptedBuffer.get(), finalLength);
        if (!outFile)
        {
            throw std::runtime_error("Error writing to output file.");
        }

        // Cleanup
        inFile.close();
        outFile.close();
    }

    void Encryptor::decrypt_file(const std::string &path2encryptedfile,
                                 const unsigned char *key,
                                 size_t key_length,
                                 const unsigned char *iv,
                                 const std::string &path2destination)
    {
        // Open encrypted file
        std::ifstream inFile(path2encryptedfile, std::ios::binary);
        if (!inFile.is_open())
        {
            throw std::runtime_error("Error opening encrypted file.");
        }

        // Get file size
        inFile.seekg(0, std::ios::end);
        size_t fileSize = inFile.tellg();
        inFile.seekg(0, std::ios::beg);

        // Set destination path if not provided
        std::string destPath = path2destination;
        if (destPath.empty())
        {
            size_t lastSlash = path2encryptedfile.find_last_of("/\\");
            destPath = (lastSlash == std::string::npos) ? "." : path2encryptedfile.substr(0, lastSlash);
        }

        // Create output file name
        size_t dotPos = path2encryptedfile.find_last_of(".");
        std::string filename;
        if (dotPos != std::string::npos)
        {
            filename = path2encryptedfile.substr(path2encryptedfile.find_last_of("/\\") + 1, dotPos - path2encryptedfile.find_last_of("/\\") - 1);
        }
        else
        {
            filename = path2encryptedfile.substr(path2encryptedfile.find_last_of("/\\") + 1);
        }
        std::string extension = path2encryptedfile.substr(dotPos);
        std::string outputFile = destPath + "/" + filename + "_decrypted" + extension;

        // Open output file
        std::ofstream outFile(outputFile, std::ios::binary);
        if (!outFile.is_open())
        {
            throw std::runtime_error("Error opening output file.");
        }

        // Set up AES decryption
        EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
        if (!ctx)
        {
            throw std::runtime_error("Error creating cipher context.");
        }

        if (EVP_DecryptInit_ex(ctx,
                               (key_length == 32) ? EVP_aes_256_cbc() : (key_length == 16) ? EVP_aes_128_cbc()
                                                                                           : EVP_aes_192_cbc(),
                               NULL, key, iv) != 1)
        {
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("Error initializing decryption.");
        }

        // Read encrypted file
        unsigned char *encryptedBuffer = new unsigned char[fileSize];
        inFile.read((char *)encryptedBuffer, fileSize);
        if (!inFile)
        {
            delete[] encryptedBuffer;
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("Error reading encrypted file.");
        }

        // Decrypt file
        unsigned char *decryptedBuffer = new unsigned char[fileSize];
        int decryptedLength = 0;
        if (EVP_DecryptUpdate(ctx, decryptedBuffer, &decryptedLength, encryptedBuffer, fileSize) != 1)
        {
            delete[] encryptedBuffer;
            delete[] decryptedBuffer;
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("Error decrypting file.");
        }

        // Finalize decryption
        int finalLength = 0;
        if (EVP_DecryptFinal_ex(ctx, decryptedBuffer + decryptedLength, &finalLength) != 1)
        {
            delete[] encryptedBuffer;
            delete[] decryptedBuffer;
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("Error finalizing decryption.");
        }
        decryptedLength += finalLength;

        // Write decrypted data to output file
        outFile.write((char *)decryptedBuffer, decryptedLength);
        if (!outFile)
        {
            delete[] encryptedBuffer;
            delete[] decryptedBuffer;
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("Error writing to output file.");
        }

        // Clean up
        delete[] encryptedBuffer;
        delete[] decryptedBuffer;
        EVP_CIPHER_CTX_free(ctx);
        inFile.close();
        outFile.close();
    }

    void Encryptor::print_key()
    {
        printf("AES Key (%zu bits)\n", key_data.length * 8);
        for (int i = 0; i < key_data.length; i++)
        {
            std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)key_data.key[i];
        }
        printf("\n");
        printf("IV AES_BLOCK_SIZE (%d bytes)\n", AES_BLOCK_SIZE);
        for (int i = 0; i < AES_BLOCK_SIZE; i++)
        {
            std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)key_data.iv[i];
        }
        printf("\n");
    }

} // name space AES