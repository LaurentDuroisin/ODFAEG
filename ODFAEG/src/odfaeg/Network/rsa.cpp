#include "../../../include/odfaeg/Network/rsa.h"
#include <stdlib.h>
#include <iostream>
#include <ctime>
using namespace std;
namespace odfaeg {
    namespace network {
        Rsa::Rsa() {
            x = X509_new();
            evp_pkey = EVP_PKEY_new();
            keypair = ssl_generateKeys(2048);
            generateKeys(256);
        }
        int Rsa::ossl_getCertificate (unsigned char** out) {
            return i2d_X509(x, out);
        }

        void Rsa::ossl_setCertificate(const unsigned char* in, int l) {
            x = d2i_X509(NULL,&in,l);
            EVP_PKEY_free(evp_pkey);
            evp_pkey = EVP_PKEY_new();
            evp_pkey = X509_get_pubkey(x);
            keypair = EVP_PKEY_get1_RSA(evp_pkey);
        }
        void Rsa::setCertificate(string certificate) {
            vector<string> pbKeyParts = core::split(certificate, "-");
            e.setStr(pbKeyParts[0], 16);
            n.setStr(pbKeyParts[1], 16);
        }
        std::string Rsa::getCertificate() {
            return e.toStr(16)+"-"+n.toStr(16);
        }
        void Rsa::generateKeys(unsigned int size) {
            BigInt t;
            do {
                BigInt p = BigInt::genPrime(size/8, 16);
                BigInt q = BigInt::genPrime(size/8, 16);
                n = p * q;
                BigInt f = (p - 1) * (q - 1);
                do {
                    do {
                        e = BigInt::genRandom(size/8, 16);
                    } while (BigInt::pgcd(e,f) != 1 && BigInt::pgcd(e, n) != 1);
                    d = e.m_invert(f);
                } while (d == 0);
                t = BigInt(5, true, 16);
                BigInt c = t.modOfPow(e, n);
                t = c.modOfPow(d, n);
            } while (t != 5);

        }
        RSA* Rsa::ssl_generateKeys (int size) {
            Rsa::size = size;


            RSA* key_pair = RSA_generate_key(size, 3, NULL, NULL);
            /*PEM_write_bio_RSAPublicKey(pub, keypair);
            pub_len = BIO_pending(pub);
            pub_key = (char*) malloc(pub_len + 1);
            BIO_read(pub, pub_key, pub_len);
            pub_key[pub_len] = '\0';*/
            EVP_PKEY_set1_RSA(evp_pkey, key_pair);
            X509_set_version(x,2);
            ASN1_INTEGER_set(X509_get_serialNumber(x),0);
            X509_gmtime_adj(X509_get_notBefore(x),0);
            X509_gmtime_adj(X509_get_notAfter(x),(long)60*60*24*365);
            X509_set_pubkey(x,evp_pkey);
            //X509_NAME* name = X509_get_subject_name(x);

            /* This function creates and adds the entry, working out the
             * correct string type and performing checks on its length.
             * Normally we'd check the return value for errors...
             */
           /* X509_NAME_add_entry_by_txt(name,"C", MBSTRING_ASC, (unsigned char*) "BE", -1, -1, 0);
            X509_NAME_add_entry_by_txt(name,"Falior",
            MBSTRING_ASC, (unsigned char*) "Falior Group", -1, -1, 0);*/

            /* Its self signed so set the issuer name to be the same as the
                * subject.
            */
            //X509_set_issuer_name(x,name);
            /* Add various extensions: standard extensions */
            /*add_ext(x, NID_basic_constraints, "critical,CA:TRUE");
            add_ext(x, NID_key_usage, "critical,keyCertSign,cRLSign");

            add_ext(x, NID_subject_key_identifier, "hash");


            add_ext(x, NID_netscape_cert_type, "sslCA");

            add_ext(x, NID_netscape_comment, "example comment extension");*/
            X509_sign(x,evp_pkey,EVP_sha1());
            return key_pair;
        }
        unsigned char* Rsa::encryptWithPbKey(const unsigned char* data, std::size_t dataSize, std::size_t& newSize) {
            newSize = BLOC_SIZE * dataSize;
            unsigned char* crypted = new unsigned char[newSize+1];
            unsigned int p = 0;
            for (unsigned int i = 0; i < dataSize; i++) {
                int ascii = data[i];
                BigInt c = BigInt(ascii, true, 16).modOfPow(e, n);
                std::string s = c.toStr(16);
                for (unsigned int j = 0; j < s.length(); j++) {
                    crypted[p] = s.at(j);
                    p++;
                }
                for (unsigned int i = 0; i < BLOC_SIZE - s.length(); i++) {
                    crypted[p] = 0;
                    p++;
                }
            }
            crypted[p] = 32;
            return crypted;
        }
        unsigned char* Rsa::decryptWithPrKey(const unsigned char* data, std::size_t dataSize, std::size_t& newSize) {
            unsigned int p = 0;
            std::vector<std::string> strs;
            while(data[p] != 32) {
                std::string str = "";
                while (data[p] != 0) {
                    str += data[p];
                    p++;
                }
                while (data[p] == 0) {
                    p++;
                }
                strs.push_back(str);
            }
            newSize = strs.size();
            unsigned char* decrypted = new unsigned char[newSize];
            for (unsigned int i = 0; i < strs.size(); i++) {
                BigInt c (strs[i], 16);
                BigInt dec = c.modOfPow(d, n);
                int ascii = core::conversionStringInt(dec.toStr(10));
                decrypted[i] = ascii;
            }
            return decrypted;
        }
        unsigned char* Rsa::encryptWithPrKey(const unsigned char* data, std::size_t dataSize, std::size_t& newSize) {
            newSize = BLOC_SIZE * dataSize;
            unsigned char* crypted = new unsigned char[newSize+1];
            unsigned int p = 0;
            for (unsigned int i = 0; i < dataSize; i++) {
                int ascii = data[i];
                BigInt c = BigInt(ascii, true, 16).modOfPow(d, n);
                std::string s = c.toStr(16);
                for (unsigned int j = 0; j < s.length(); j++) {
                    crypted[p] = s.at(j);
                    p++;
                }
                for (unsigned int i = 0; i < BLOC_SIZE - s.length(); i++) {
                    crypted[p] = 0;
                    p++;
                }
            }
            crypted[p] = 32;
            return crypted;
        }
        unsigned char* Rsa::decryptWithPbKey(const unsigned char* data, std::size_t dataSize, std::size_t& newSize) {
            unsigned int p = 0;
            std::vector<std::string> strs;
            while(data[p] != 32) {
                std::string str = "";
                while (data[p] != 0) {
                    str += data[p];
                    p++;
                }
                while (data[p] == 0) {
                    p++;
                }
                strs.push_back(str);
            }
            newSize = strs.size();
            unsigned char* decrypted = new unsigned char[newSize];
            for (unsigned int i = 0; i < strs.size(); i++) {
                BigInt c (strs[i], 16);
                BigInt dec = c.modOfPow(e, n);
                int ascii = core::conversionStringInt(dec.toStr(10));
                decrypted[i] = ascii;
            }
            return decrypted;
        }
        unsigned char* Rsa::ossl_encryptWithPrKey (const unsigned char *data, int dataSize, int& newSize) {
            unsigned char* encData = new unsigned char[RSA_size(keypair)];
            newSize = RSA_private_encrypt(dataSize, data, encData, keypair, RSA_PKCS1_PADDING);
            if (newSize == -1) {
                char* err = (char*) malloc(130);
                ERR_load_crypto_strings();
                ERR_error_string(ERR_get_error(), err);
                fprintf(stderr, "Error encrypting message: %s\n", err);
                free(err);
                return nullptr;
            }
            return encData;
        }
        unsigned char* Rsa::ossl_decryptWithPrKey (const unsigned char *encData, int dataSize, int& newSize) {
            unsigned char *data = new unsigned char[RSA_size(keypair)];
            newSize = RSA_private_decrypt(RSA_size(keypair), encData, data, keypair, RSA_PKCS1_OAEP_PADDING);
            if (newSize == -1) {
                char* err = (char*) malloc(130);
                ERR_load_crypto_strings();
                ERR_error_string(ERR_get_error(), err);
                fprintf(stderr, "Error encrypting message: %s\n", err);
                free(err);
                return nullptr;
            }
            return data;
        }
        unsigned char* Rsa::ossl_encryptWithPbKey (const unsigned char *data,int dataSize, int& newSize) {
            unsigned char *encData = new unsigned char[RSA_size(keypair)];
            newSize = RSA_public_encrypt(dataSize, data, encData, keypair, RSA_PKCS1_OAEP_PADDING);
            if (newSize == -1) {
                char* err = (char*) malloc(130);
                ERR_load_crypto_strings();
                ERR_error_string(ERR_get_error(), err);
                fprintf(stderr, "Error encrypting message: %s\n", err);
                free(err);
                return nullptr;
            }
            return encData;
        }
        unsigned char* Rsa::ossl_decryptWithPbKey (const unsigned char *encData, int dataSize, int &newSize) {
            unsigned char *data = new unsigned char[RSA_size(keypair)];
            newSize = RSA_public_decrypt(RSA_size(keypair), encData, data, keypair, RSA_PKCS1_PADDING);
            if (newSize == -1) {
                char* err = (char*) malloc(130);

                ERR_load_crypto_strings();
                ERR_error_string(ERR_get_error(), err);
                fprintf(stderr, "Error encrypting message: %s\n", err);
                free(err);
                return nullptr;
            }
            return data;
        }
        Rsa::~Rsa() {
            EVP_PKEY_free(evp_pkey);
            RSA_free(keypair);
            X509_free(x);
        }
    }
}
