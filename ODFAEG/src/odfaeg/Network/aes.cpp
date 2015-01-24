#include "../../../include/odfaeg/Network/aes.h"
namespace odfaeg {
    namespace network {
        using namespace std;
        unsigned char* AES_ENC::encrypt(const unsigned char* data, int dataSize, int* newSize) {
            int cLen = dataSize+AES_BLOCK_SIZE;
            int fLen = 0;
            unsigned char *encData = new unsigned char[cLen];
            if (!EVP_EncryptInit_ex(e_ctx, EVP_aes_256_cbc(), nullptr, key, iv))
                return nullptr;
            if (!EVP_EncryptUpdate(e_ctx, encData, &cLen, data, dataSize))
                return nullptr;
            if(!EVP_EncryptFinal_ex(e_ctx, encData+cLen, &fLen)) {
                return nullptr;
            }
            *newSize = cLen + fLen;

            return encData;
        }
        unsigned char* AES_ENC::decrypt(const unsigned char* encData, int dataSize, int* newSize) {
            int pLen = dataSize;
            int fLen = 0;
            unsigned char *data = new unsigned char[pLen];
            if (!EVP_DecryptInit_ex(d_ctx, EVP_aes_256_cbc(), nullptr, key, iv))
                return nullptr;
            if (!EVP_DecryptUpdate(d_ctx, data, &pLen, encData, dataSize))
                return nullptr;

            if (!EVP_DecryptFinal_ex(d_ctx, data+pLen, &fLen)) {
                char* err = (char*) malloc(130);
                ERR_load_crypto_strings();
                ERR_error_string(ERR_get_error(), err);
                fprintf(stderr, "Error encrypting message: %s\n", err);
                free(err);
            }
            *newSize = pLen + fLen;
            return data;
        }
        void AES_ENC::setKey(char* sKey) {
           key = new unsigned char[size / 8];
           memcpy(key, (unsigned char*) sKey, strlen(sKey));
           key[strlen(sKey) +  1] = '\0';
        }
        void AES_ENC::setIv(char* sIv) {
           iv = new unsigned char[size / 8];
           memcpy(iv, (unsigned char*) sIv, strlen(sIv));
           iv[strlen(sIv) +  1] = '\0';
        }
        char* AES_ENC::getKey() {
           return (char*) key;
        }
        char* AES_ENC::getIv() {
           return (char*) iv;
        }
    }
}
