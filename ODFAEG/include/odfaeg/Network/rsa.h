#ifndef SSL_RSA
#define SSL_RSA
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include "../Core/utilities.h"
#include <fstream>
#include <ctime>
#include "../Math/maths.h"
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>

#include <openssl/engine.h>
#include <openssl/conf.h>
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "export.hpp"
/**
  *\namespace odfaeg
  * the namespace of the Opensource Development Framework Adapted for Every Games.
  */
namespace odfaeg {
    namespace network {
        class ODFAEG_NETWORK_API Rsa {
        /**
          * \file rsa.h
          * \class RSA
          * \brief rsa encryption, with openssl, the keys are contained in a certificate.
          * \author Duroisin.L
          * \version 1.0
          * \date 1/02/2014
          */
            public :
                Rsa() {
                    x = X509_new();
                    evp_pkey = EVP_PKEY_new();
                    pub = BIO_new(BIO_s_mem());
                    pub_len = 0;
                    pub_key = nullptr;
                    keypair = generateKeys(2048);
                }
                /**
                * \fn unsigned char* decryptWithPrKey (const unsigned char* encData, int dataSize, int* newSize)
                * \brief decrypt a message with the private key.
                * \param encData : the encrypted data.
                * \param dataSize : the size of the encrypted data.
                * \param newSize : the size of the data.
                */
                unsigned char* decryptWithPrKey (const unsigned char* encData, int dataSize, int* newSize);
                /**
                * \fn unsigned char* encryptWithPrKey (const unsigned char* encData, int dataSize, int* newSize)
                * \brief encrypt a message with the private key.
                * \param data : the data.
                * \param dataSize : the size of the data.
                * \param newSize : the size of the encrypted data.
                */
                unsigned char* encryptWithPrKey (const unsigned char* data, int dataSize, int* newSize);
                 /**
                * \fn unsigned char* decryptWithPbKey (const unsigned char* encData, int dataSize, int* newSize)
                * \brief decrypt a message with the public key.
                * \param encData : the encrypted data.
                * \param dataSize : the size of the encrypted data.
                * \param newSize : the size of the data.
                */
                unsigned char* decryptWithPbKey (const unsigned char* encData, int dataSize, int* newSize);
                /**
                * \fn unsigned char* encryptWithPbKey (const unsigned char* encData, int dataSize, int* newSize)
                * \brief encrypt a message with the public key.
                * \param data : the data.
                * \param dataSize : the size of the data.
                * \param newSize : the size of the encrypted data.
                */
                unsigned char* encryptWithPbKey (const unsigned char* data, int dataSize, int* newSize);
                /**
                * \fn int getCertificate(unsigned char** out)
                * \brief get the certificate which contains the keys.
                * \param the certificate.
                * \return the size of the certificate.
                */
                int getCertificate(unsigned char** out);
                /**
                * \fn void setCertificate(const unsigned char* certificate, int length)
                * \brief set the certificate containing the keys.
                * \param certificate : the certificate.
                * \param length : the length.
                */
                void setCertificate(const unsigned char* certificate, int length);
                ~Rsa() {
                    EVP_PKEY_free(evp_pkey);
                    RSA_free(keypair);
                    X509_free(x);
                    BIO_free(pub);
                    delete[] pub_key;
                }
            private :
                /**
                * \fn RSA* generateKeys (int size)
                * \brief generate keys with the given size.
                * \param size : the size of the keys.
                * \return the keys.
                */
                RSA* generateKeys (int size) {
                    Rsa::size = size;


                    RSA* key_pair = RSA_generate_key(size, 3, NULL, NULL);
                    PEM_write_bio_RSAPublicKey(pub, keypair);
                    pub_len = BIO_pending(pub);
                    pub_key = (char*) malloc(pub_len + 1);
                    BIO_read(pub, pub_key, pub_len);
                    pub_key[pub_len] = '\0';
                    EVP_PKEY_set1_RSA(evp_pkey,key_pair);
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
                /*static void callback(int p, int n, void *arg)
                {
                    char c='B';
                    if (p == 0) c='.';
                    if (p == 1) c='+';
                    if (p == 2) c='*';
                    if (p == 3) c='\n';
                };*/
                /*static int add_ext(X509 *cert, int nid, char *value) {
                    X509_EXTENSION *ex;
                    X509V3_CTX ctx;

                    X509V3_set_ctx_nodb(&ctx);

                    X509V3_set_ctx(&ctx, cert, cert, NULL, NULL, 0);
                    ex = X509V3_EXT_conf_nid(NULL, &ctx, nid, value);
                    if (!ex)
                        return 0;

                    X509_add_ext(cert,ex,-1);
                    X509_EXTENSION_free(ex);
                    return 1;
                }*/
                int size; /**> the size of the keys.(in bytes)*/
                RSA* keypair; /**> the keys.*/
                X509 *x; /**> the certificate.*/
                EVP_PKEY* evp_pkey; /**The envelop*/
                BIO* pub; /**> an openssl structure used to print keys.*/
                char* pub_key; /**> the public key.*/
                size_t pub_len; /**> the length of the public key.*/
        };
    }
}
#endif
