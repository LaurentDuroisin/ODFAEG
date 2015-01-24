#include "../../../include/odfaeg/Network/rsa.h"
#include <stdlib.h>
#include <iostream>
namespace odfaeg {
    namespace network {
        int Rsa::getCertificate (unsigned char **out) {
            int len= i2d_X509(x, out);
            return len;
        }

        void Rsa::setCertificate(const unsigned char* in, int length) {
            x = d2i_X509(NULL,&in,length);
            evp_pkey = X509_get_pubkey(x);
            keypair = EVP_PKEY_get1_RSA(evp_pkey);
            PEM_write_bio_RSAPublicKey(pub, keypair);
            pub_len = BIO_pending(pub);
            free(pub_key);
            pub_key = (char*)malloc(pub_len + 1);
            BIO_read(pub, pub_key, pub_len);
            pub_key[pub_len] = '\0';
        }
        unsigned char* Rsa::encryptWithPrKey (const unsigned char *data, int dataSize, int *newSize) {
            unsigned char* encData = new unsigned char[RSA_size(keypair)];
            *newSize = RSA_private_encrypt(dataSize, data, encData, keypair, RSA_PKCS1_PADDING);
            if (*newSize == -1) {
                char* err = (char*) malloc(130);
                ERR_load_crypto_strings();
                ERR_error_string(ERR_get_error(), err);
                fprintf(stderr, "Error encrypting message: %s\n", err);
                free(err);
                return nullptr;
            }
            return encData;
        }
        unsigned char* Rsa::decryptWithPrKey (const unsigned char *encData, int dataSize, int* newSize) {
            unsigned char *data = new unsigned char[dataSize];
            *newSize = RSA_private_decrypt(RSA_size(keypair), encData, data, keypair, RSA_PKCS1_OAEP_PADDING);
            if (*newSize == -1) {
                char* err = (char*) malloc(130);
                ERR_load_crypto_strings();
                ERR_error_string(ERR_get_error(), err);
                fprintf(stderr, "Error encrypting message: %s\n", err);
                free(err);
                return nullptr;
            }
            return data;
        }
        unsigned char* Rsa::encryptWithPbKey (const unsigned char *data,int dataSize, int* newSize) {
            unsigned char *encData = new unsigned char[RSA_size(keypair)];
            *newSize = RSA_public_encrypt(dataSize, data, encData, keypair, RSA_PKCS1_OAEP_PADDING);
            if (*newSize == -1) {
                char* err = (char*) malloc(130);
                ERR_load_crypto_strings();
                ERR_error_string(ERR_get_error(), err);
                fprintf(stderr, "Error encrypting message: %s\n", err);
                free(err);
                return nullptr;
            }
            return encData;
        }
        unsigned char* Rsa::decryptWithPbKey (const unsigned char *encData, int dataSize, int *newSize) {
            unsigned char *data = new unsigned char[dataSize];
            *newSize = RSA_public_decrypt(RSA_size(keypair), encData, data, keypair, RSA_PKCS1_PADDING);
            if (*newSize == -1) {
                char* err = (char*) malloc(130);

                ERR_load_crypto_strings();
                ERR_error_string(ERR_get_error(), err);
                fprintf(stderr, "Error encrypting message: %s\n", err);
                free(err);
                return nullptr;
            }
            return data;
        }
    }
}
