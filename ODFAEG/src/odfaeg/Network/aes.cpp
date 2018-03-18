#include "../../../include/odfaeg/Network/aes.h"
namespace odfaeg {
    namespace network {

        using namespace std;
        unsigned char* AES_ENC::ossl_encrypt(const unsigned char* data, unsigned int dataSize, unsigned int& newSize) {
            int len;
            int ciphertext_len;
            unsigned char *encData = new unsigned char[dataSize + AES_BLOCK_SIZE];
            if (!EVP_EncryptInit_ex(e_ctx, EVP_aes_256_ecb(), nullptr, ossl_key, iv))
                return nullptr;
            if (!EVP_EncryptUpdate(e_ctx, encData, &len, data, dataSize))
                return nullptr;
            ciphertext_len = len;
            if(!EVP_EncryptFinal_ex(e_ctx, encData+len, &len)) {
                return nullptr;
            }
            newSize = ciphertext_len + len;
            return encData;
        }
        unsigned char* AES_ENC::ossl_decrypt(const unsigned char* encData, unsigned int dataSize, unsigned int& newSize) {
            int len;
            int plaintext_len;
            unsigned char *data = new unsigned char[dataSize];
            if (!EVP_DecryptInit_ex(d_ctx, EVP_aes_256_ecb(), nullptr, ossl_key, iv))
                return nullptr;
            if (!EVP_DecryptUpdate(d_ctx, data, &len, encData, dataSize))
                return nullptr;
            plaintext_len = len;
            if (!EVP_DecryptFinal_ex(d_ctx, data+len, &len)) {
                char* err = (char*) malloc(130);
                ERR_load_crypto_strings();
                ERR_error_string(ERR_get_error(), err);
                fprintf(stderr, "Error encrypting message: %s\n", err);
                free(err);
            }
            newSize = plaintext_len + len;
            return data;
        }
        void AES_ENC::ossl_setKey(char* sKey) {
           delete ossl_key;
           ossl_key = new unsigned char[strlen(sKey)];
           for (unsigned int i = 0; i < strlen(sKey); i++) {
                ossl_key[i] = (unsigned char) sKey[i];
           }
        }
        void AES_ENC::ossl_setIv(char* sIv) {
           delete iv;
           iv = new unsigned char[strlen(sIv)];
           for (unsigned int i = 0; i < strlen(sIv); i++) {
                iv[i] = (unsigned char) sIv[i];
           }
        }
        char* AES_ENC::ossl_getKey() {
           return (char*)(ossl_key);
        }
        char* AES_ENC::ossl_getIv() {
           return (char*)(iv);
        }
        void AES_ENC::generateKey() {
            for (unsigned int i = 0; i < 256; i++) {
                std::string hashChars = "\n\r\t ";
                std::string key;
                std::map<std::string, unsigned char>::iterator it;
                do {
                    for (unsigned int j = 0; j < BLOCK_SIZE; j++) {
                        int r = math::Math::random(0, 4);
                        key += hashChars.at(r);
                    }
                    it = hashTable.find(key);
                } while (it != hashTable.end());
                hashTable.insert(make_pair(key, i));
                invHashTable.insert(make_pair(i, key));
            }
        }
        void AES_ENC::updateKeys (std::string keys) {
            hashTable.clear();
            invHashTable.clear();
            for (unsigned int i = 0; i < 256; i++) {
                std::string key = keys.substr(i * BLOCK_SIZE, i * BLOCK_SIZE + BLOCK_SIZE - 1);
                hashTable.insert(make_pair(key, i));
                invHashTable.insert(make_pair(i, key));
            }
        }
        std::string AES_ENC::getKeys() {
            std::string keys;
            std::map<std::string, unsigned char>::iterator it;
            for (it = hashTable.begin(); it != hashTable.end(); it++) {
                keys += it->first;
            }
            return keys;
        }
        /**
         * Permet de chiffrer une suite de bytes
         */
        unsigned char* AES_ENC::encrypt(const unsigned char* mess, size_t dataSize, size_t& newSize) {
            unsigned char* crypted = new unsigned char[dataSize * BLOCK_SIZE];
            newSize = dataSize * BLOCK_SIZE;
            for (unsigned int i = 0; i < dataSize; i++) {
                std::string key = invHashTable.find(mess[i])->second;
                for (unsigned int j = 0; j < BLOCK_SIZE; j++) {
                    crypted[i * BLOCK_SIZE + j] = key[j];
                }
            }
            return crypted;
        }

        /**
         * Permet de dechiffrer une suite de bytes
         */
        unsigned char* AES_ENC::decrypt(const unsigned char* mess, size_t dataSize, size_t& newSize) {
            unsigned char* decrypted = new unsigned char[dataSize / BLOCK_SIZE];
            newSize = dataSize / BLOCK_SIZE;
            for (unsigned int i = 0; i < newSize; i++) {
                std::string key;
                for (unsigned int j = 0; j < BLOCK_SIZE; j++) {
                    key += mess[i * BLOCK_SIZE + j];
                }
                decrypted[i] = hashTable.find(key)->second;
            }
            return decrypted;
        }
    }
}
