#ifndef AES_ENC_H
#define AES_ENC_H
#include <iostream>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <ctime>
#include <openssl/rand.h>
#include <openssl/aes.h>
#include "../Core/utilities.h"
#include "../Math/maths.h"
#include "export.hpp"
#include <string.h>
#include <stdlib.h>
#include "bigInt.hpp"
#include <map>
#include <memory>
/**
  *\namespace odfaeg
  * the namespace of the Opensource Development Framework Adapted for Every Games.
  */
namespace odfaeg {
    namespace network {
        /**
          * \file aes.h
          * \class AES_ENC
          * \brief Perform symetric encryptions with the aes algorithm.
          * \author Duroisin.L
          * \version 1.0
          * \date 1/02/2014
          *
          */
        class ODFAEG_NETWORK_API AES_ENC {
        public:

            AES_ENC() {
                ossl_key = new unsigned char[32];
                iv = new unsigned char[32];
                aesSalt = new unsigned char[8];
                e_ctx = new EVP_CIPHER_CTX;
                d_ctx = new EVP_CIPHER_CTX;
                aesPass = ossl_generateKey(256);
                generateKey();
            }
            /**
            * \fn unisgned char encrypt(const unsigned char* data, int dataSize, int* newSize)
            * \brief encrypt the data.
            * \param data : the data to encrypt.
            * \param dataSize : the size of the data to encrypt.
            * \param newSize : the size of the encrypted data.
            * \return the encrypted data.
            */
            unsigned char* ossl_encrypt(const unsigned char* data, unsigned int dataSize, unsigned int& newSize);
            /**
            * \fn unsigned char* decrypt(const unsigned char* encData, int dataSize, int* newSize)
            * \brief decrypt the data.
            * \param encData : the encrypted data.
            * \param dataSize : the size of the encrypted data.
            * \param newSize : the size of the data.
            * \return the data.
            */
            unsigned char* ossl_decrypt(const unsigned char* encData, unsigned int dataSize, unsigned int& newSize);
            /**
            * \fn char* getKey()
            * \brief get the key used to hash the aes key.
            * \return the key.
            */
            char* ossl_getKey ();
            /**
            * \fn char* getIv()
            * \brief get the iv used to hash the key.
            * \return the iv.
            */
            char* ossl_getIv();
            /**
            * \fn void setKey (char* key);
            * \brief set the key used to hash the aes key.
            * \param key : the key.
            */
            void ossl_setKey (char* key);
            /**
            * \fn void setIv (char* iv);
            * \brief set the iv used to hash the key.
            * \param iv : the iv.
            */
            void ossl_setIv (char* iv);
            unsigned char* encrypt(const unsigned char* mess, std::size_t dataSize, std::size_t& newSize);
            unsigned char* decrypt(const unsigned char* mess, size_t dataSize, size_t& newSize);
            std::string getKeys();
            void updateKeys (std::string keys);
            ~AES_ENC() {
                delete[] ossl_key;
                delete[] iv;
                delete[] aesSalt;
                delete[] aesPass;
                EVP_CIPHER_CTX_cleanup(d_ctx);
                EVP_CIPHER_CTX_cleanup(e_ctx);
                delete e_ctx;
                delete d_ctx;
            }
        private:
            /**
            * \fn unsigned char* generateKey(int length)
            * \brief generate an aes key with the given length.
            * \param the length of the key.
            * \return the pass used to hash the key.
            */
            unsigned char* ossl_generateKey(int length) {
                size = length;
                aesPass = new unsigned char[size / 8];
                if (RAND_bytes(aesPass, size / 8) == 0)
                    std::cerr<<"fail to generate aes pass"<<std::endl;
                int nrounds = 5;
                if (RAND_bytes(aesSalt, 8) == 0)
                    std::cerr<<"fail to generate random salt"<<std::endl;
                /*

                * Gen key & IV for AES 256 CBC mode. A SHA1 digest is used to hash the 			* supplied key material.
                * nrounds is the number of times the we hash the material. More rounds 			* are more secure but
                * slower.
                */
                if (EVP_BytesToKey(EVP_aes_256_ecb(), EVP_sha1(), aesSalt, aesPass, 			    size / 8, nrounds, ossl_key, iv) == 0)
                    std::cerr<<"Failed to hash the aes key."<<std::endl;
                EVP_CIPHER_CTX_init(e_ctx);
                //EVP_EncryptInit_ex(e_ctx, EVP_aes_256_cbc(), NULL, key, iv);
                EVP_CIPHER_CTX_init(d_ctx);
                //EVP_DecryptInit_ex(d_ctx, EVP_aes_256_cbc(), NULL, key, iv);
                return aesPass;
            }
            void generateKey();
            int size; /**> the size of the key.*/
            std::string aes_key; /**> the aes key.*/
            unsigned char *ossl_key, *iv, *aesSalt, *aesPass; /**> the key, the iv, the salt and the pass used to hash the aes key.*/
            EVP_CIPHER_CTX* e_ctx, *d_ctx; /**>The ciphers used to encrypt and decrypt the data. */
            std::map<std::string, unsigned char> hashTable;
            std::map<unsigned char, std::string> invHashTable;
            const unsigned int BLOCK_SIZE = 32;
        };
    }
}
#endif // AES_ENC
