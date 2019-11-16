#ifndef ODFAEG_ARCHIVE
#define ODFAEG_ARCHIVE
#include <vector>
#include <map>
#include <iostream>
#include <typeinfo>
#include "factory.h"
#include <sstream>
#include <memory>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "resourceCache.h"
#include "export.hpp"
/**
  *\namespace odfaeg
  * the namespace of the Opensource Development Framework Adapted for Every Games.
  */
namespace odfaeg {
    namespace core {
        /**
        * maximum decompressed data input length (1MB)
        */
        #define MAX_DECOMPRESSED_LENGTH 1048576

        /**
        * maximum compressed data input length
        */
        #define MAX_COMPRESSED_LENGTH (MAX_DECOMPRESSED_LENGTH + (MAX_DECOMPRESSED_LENGTH/2))
        /**
        * hash table length
        */
        #define DICT_LEN 16384

        #define DecodeOffset(x) DecodeLength(x)
        /**
        * utf16 character
        */
        typedef unsigned short u16;

        /**
        * one byte
        */
        typedef unsigned char u8;

        /**
        * 32 bit unsigned integer
        */
        typedef unsigned int u32;
        template <typename T>
        struct has_typedef_key {
            // Types "yes" and "no" are guaranteed to have different sizes,
            // specifically sizeof(yes) == 1 and sizeof(no) == 2.
            typedef char yes[1];
            typedef char no[2];

            template <typename C>
            static yes& test(typename C::KEYTYPE*);
            template <typename>
            static no& test(...);

            // If the "sizeof" of the result of calling test<T>(0) would be equal to sizeof(yes),
            // the first overload worked and T has a nested type named foobar.
            static const int value = sizeof(test<T>(0)) == sizeof(yes);
        };
        /**
        * \file archive.h
        * \class OTextArchive
        * \brief Write everything into the output archive's buffer in text format.
        * \author Duroisin.L
        * \version 1.0
        * \date 1/02/2014
        */
        class ODFAEG_CORE_API OTextArchive {
        public :
            /**\fn OTextArchive (std::ostream& buffer)
            *  \brief pass a c++ ouptut buffer to the archive.
            *  \param std::ostream& buffer : the output buffer.
            */
            OTextArchive(std::ostream& buffer) : buffer(buffer) {
                nbSerialized = 0;
            }
            bool isInputArchive() {
                return false;
            }
            void clear() {
                buffer.clear();
                adresses.clear();
                nbSerialized = 0;
            }
            /** \fn void compress_stdin
            *   \brief compress the data contained into the ouptut archive's buffer.
            */
            void compress_stdin(void)
            {
              /*std::ostringstream output;
              output<<buffer;
              int i, out_len;
              char *buffer, *output_buffer;
              i = output.str().size();*/
              /* allocate input and output buffers */
              /*buffer = new char[MAX_DECOMPRESSED_LENGTH];
              output_buffer = new char [MAX_COMPRESSED_LENGTH];*/

              /* read input from stdin */
              /*if (i == MAX_DECOMPRESSED_LENGTH)
              {
                std::cerr<<"ERROR: Input is too long!"<<std::endl;
                return;
              }
              buffer = const_cast<char*> (output.str().c_str());
              output.str("");*/

              /* each utf16 character is 2 bytes long so the input number of bytes must be odd */
              /*if (i % 2 != 0)
                fprintf(stderr, "WARNING: odd number of input bytes!\n");*/

              /* compress */
              /*out_len = utf16_compress((u16 *)buffer, i / 2, (u8 *)output_buffer);

              //delete[] buffer;
              this->buffer.rdbuf()->pubsetbuf(output_buffer, out_len);
              //delete[] output_buffer;*/
            }
            //Fundamentals.
            /**
            * \fn void operator(T& data, D...)
            * \brief write a fundamental type into the archive.
            * \param T& the data to write.
            * \param D... used for SFINAE.
            */
            template <typename T,
                      class... D,
                      class = typename std::enable_if<std::is_fundamental<T>::value>::type>
            void operator() (T& data, D...) {
                std::ostringstream oss;
                oss<<typeid(data).name()<<"*"<<reinterpret_cast<unsigned long long int>(&data);
                std::map<std::string, long long int>::iterator it = adresses.find(oss.str());
                if (it != adresses.end()) {
                    //std::cout<<"id : "<<it->second<<std::endl;
                    buffer<<it->second<<std::endl;
                } else {
                    std::pair<std::string, long long int> newAddress (oss.str(), nbSerialized);
                    adresses.insert(newAddress);
                    buffer<<newAddress.second<<std::endl;
                    //std::cout<<"id : "<<newAddress.second<<std::endl;
                    //std::cout<<"fundamental data : "<<data<<std::endl;
                    buffer<<data<<std::endl;
                    nbSerialized++;
                }
            }
            /**
            * \fn void operator(T* data, D...)
            * \brief write pointer to a fundamental type into the archive.
            * \param T* the pointer to write.
            * \param D... used for SFINAE.
            */
            template <typename T,
                  class... D,
                  class = typename std::enable_if<std::is_fundamental<T>::value>::type>
            void operator() (T* data, D...) {
                if (data != nullptr) {
                    std::ostringstream oss;
                    oss<<typeid(*data).name()<<"*"<<reinterpret_cast<unsigned long long int>(data);
                    std::map<std::string, long long int>::iterator it = adresses.find(oss.str());
                    if (it != adresses.end()) {
                        //std::cout<<"id : "<<it->second<<std::endl;
                        buffer<<it->second<<std::endl;
                    } else {
                        std::pair<std::string, long long int> newAddress (oss.str(), nbSerialized);
                        adresses.insert(newAddress);
                        buffer<<newAddress.second<<std::endl;
                        //std::cout<<"id : "<<newAddress.second<<std::endl;
                        buffer<<(*data)<<std::endl;
                        //std::cout<<"fundamental data : "<<(*data)<<std::endl;
                        nbSerialized++;
                    }
                } else {
                    long long int id = -1;
                    buffer<<id<<std::endl;
                }
            }
            template <typename E,
            class... D,
            class = typename std::enable_if<!std::is_fundamental<E>::value>::type,
            class = typename std::enable_if<std::is_enum<E>::value>::type>
            void operator() (E& data, D...) {
                std::ostringstream oss;
                oss<<typeid(data).name()<<"*"<<reinterpret_cast<unsigned long long int>(&data);
                std::map<std::string, long long int>::iterator it = adresses.find(oss.str());
                if (it != adresses.end()) {
                    buffer<<it->second<<std::endl;
                    //std::cout<<"id : "<<it->second<<std::endl;
                } else {
                    std::pair<std::string, long long int> newAddress (oss.str(), nbSerialized);
                    adresses.insert(newAddress);
                    buffer<<newAddress.second<<std::endl;
                    //std::cout<<"id : "<<newAddress.second<<std::endl;
                    buffer<<data<<std::endl;
                    //std::cout<<"enum data : "<<data<<std::endl;
                    nbSerialized++;
                }
            }
            template <typename E,
            class... D,
            class = typename std::enable_if<!std::is_fundamental<E>::value>::type,
            class = typename std::enable_if<std::is_enum<E>::value>::type>
            void operator() (E* data, D...) {
                if (data != nullptr) {
                    std::ostringstream oss;
                    oss<<typeid(*data).name()<<"*"<<reinterpret_cast<unsigned long long int>(data);
                    std::map<std::string, long long int>::iterator it = adresses.find(oss.str());
                    if (it != adresses.end()) {
                        buffer<<it->second<<std::endl;
                        //std::cout<<"id : "<<it->second<<std::endl;
                    } else {
                        std::pair<std::string, long long int> newAddress (oss.str(), nbSerialized);
                        adresses.insert(newAddress);
                        buffer<<newAddress.second<<std::endl;
                        //std::cout<<"id : "<<newAddress.second<<std::endl;
                        buffer<<(*data)<<std::endl;
                        //std::cout<<"enum data : "<<(*data)<<std::endl;
                        nbSerialized++;
                    }
                } else {
                    long long int id = -1;
                    buffer<<id<<std::endl;
                }
            }
            //std::string.
            /**
            *\fn void operator(T& data, D...)
            *\brief write an std::string into the archive.
            *\param T& data : the std::string to write.
            *\param D... : used fo SFINAE.
            */
            template <typename T,
                  class... D,
                  class = typename std::enable_if<!std::is_fundamental<T>::value>::type,
                  class = typename std::enable_if<std::is_same<T, std::string>::value>::type,
                  class = typename std::enable_if<!std::is_enum<T>::value>::type>
            void operator() (T& data, D...) {
                std::ostringstream oss;
                oss<<typeid(data).name()<<"*"<<reinterpret_cast<unsigned long long int>(&data);
                std::map<std::string, long long int>::iterator it = adresses.find(oss.str());
                if (it != adresses.end()) {
                    buffer<<it->second<<std::endl;
                    //std::cout<<"id : "<<it->second<<std::endl;
                } else {
                    std::pair<std::string, long long int> newAddress (oss.str(), nbSerialized);
                    adresses.insert(newAddress);
                    buffer<<newAddress.second<<std::endl;
                    //std::cout<<"id : "<<newAddress.second<<std::endl;
                    std::size_t str_size = data.length();
                    buffer<<str_size<<std::endl;
                    //std::cout<<"str size : "<<str_size<<std::endl;
                    const char* datas = data.c_str();
                    nbSerialized++;
                    for (unsigned int i = 0; i < str_size; i++)
                        (*this)(datas[i]);
                }
            }
            /**
            *\fn void operator(T* data, D...)
            *\brief The pointer to the std::string to write.
            *\param T* data : the pointer to the data to write.
            *\param D... : used for SFINAE.
            */
            template <typename T,
                  class... D,
                  class = typename std::enable_if<!std::is_fundamental<T>::value>::type,
                  class = typename std::enable_if<std::is_same<T, std::string>::value>::type,
                  class = typename std::enable_if<!std::is_enum<T>::value>::type>
            void operator() (T* data, D...) {
                if (data != nullptr) {
                    std::ostringstream oss;
                    oss<<typeid(*data).name()<<"*"<<reinterpret_cast<unsigned long long int>(data);
                    std::map<std::string, long long int>::iterator it = adresses.find(oss.str());
                    if (it != adresses.end()) {
                        buffer<<it->second<<std::endl;
                        //std::cout<<"id : "<<it->second<<std::endl;
                    } else {
                        std::pair<std::string, long long int> newAddress (oss.str(), nbSerialized);
                        adresses.insert(newAddress);
                        buffer<<newAddress.second<<std::endl;
                        //std::cout<<"id : "<<newAddress.second<<std::endl;
                        std::size_t str_size = data->length();
                        buffer<<str_size<<std::endl;
                        //std::cout<<"str size : "<<str_size<<std::endl;
                        const char* datas = data->c_str();
                        nbSerialized++;
                        for (unsigned int i = 0; i < str_size; i++)
                            (*this)(datas[i]);
                    }
                } else {
                    long long int id = -1;
                    buffer<<id<<std::endl;
                }
            }
            //Static objects.
            /**
            *\fn void operator(O& data, D...)
            *\brief register a static object onto the archive.
            *\param O& the object to register.
            *\param D... : used for SFINAE.
            */
            template <class O,
                      class... D,
                      class = typename std::enable_if<!std::is_fundamental<O>::value>::type,
                      class = typename std::enable_if<!std::is_same<O, std::string>::value && !std::is_pointer<O>::value>::type,
                      class = typename std::enable_if<!has_typedef_key<O>::value>::type,
                      class = typename std::enable_if<!std::is_enum<O>::value>::type>
            void operator() (O& object, D...) {
                std::ostringstream oss;
                oss<<typeid(object).name()<<"*"<<reinterpret_cast<unsigned long long int>(&object);
                std::map<std::string, long long int>::iterator it = adresses.find(oss.str());
                if (it != adresses.end()) {
                    buffer<<it->second<<std::endl;
                    //std::cout<<"id : "<<it->second<<std::endl;
                } else {
                    std::pair<std::string, long long int> newAddress (oss.str(), nbSerialized);
                    adresses.insert(newAddress);
                    buffer<<newAddress.second<<std::endl;
                    //std::cout<<"id : "<<newAddress.second<<std::endl;
                    nbSerialized++;
                    object.serialize(*this);
                }
            }
            template <class O,
                      class... D,
                      class = typename std::enable_if<!std::is_fundamental<O>::value>::type,
                      class = typename std::enable_if<!std::is_same<O, std::string>::value>::type,
                      class = typename std::enable_if<!has_typedef_key<O>::value>::type,
                      class = typename std::enable_if<!std::is_enum<O>::value>::type>
            /**
            *\fn void operator(O* data, D...)
            *\brief register a static object onto the archive.
            *\param O* the pointer to the object to register.
            *\param D... : used for SFINAE.
            */
            void operator() (O* object, D...) {
                if (object != nullptr) {
                    std::ostringstream oss;
                    oss<<typeid(*object).name()<<"*"<<reinterpret_cast<unsigned long long int>(object);
                    std::map<std::string, long long int>::iterator it = adresses.find(oss.str());
                    if (it != adresses.end()) {
                        buffer<<it->second<<std::endl;
                        //std::cout<<"id : "<<it->second<<std::endl;
                    } else {
                        std::pair<std::string, long long int> newAddress (oss.str(), nbSerialized);
                        adresses.insert(newAddress);
                        buffer<<newAddress.second<<std::endl;
                        //std::cout<<"id : "<<newAddress.second<<std::endl;
                        nbSerialized++;
                        object->serialize(*this);
                    }
                } else {
                    long long int id = -1;
                    buffer<<id<<std::endl;
                }
            }
            //Dynamic objects.
            /**
            *\fn void operator(O& data, D...)
            *\brief register a dynamic object onto the archive.
            *\param O& the object to register.
            *\param D... : used for SFINAE.
            */
            template <class O,
                      class... D,
                      class = typename std::enable_if<!std::is_fundamental<O>::value>::type,
                      class = typename std::enable_if<!std::is_same<O, std::string>::value>::type,
                      class = typename std::enable_if<has_typedef_key<O>::value>::type,
                      class = typename std::enable_if<!std::is_enum<O>::value>::type,
                      class = typename std::enable_if<!sizeof...(D)>::type>
            void operator() (O& object, D...) {
                std::ostringstream oss;
                oss<<typeid(object).name()<<"*"<<reinterpret_cast<unsigned long long int>(&object);
                std::map<std::string, long long int>::iterator it = adresses.find(oss.str());
                if (it != adresses.end()) {
                    buffer<<it->second<<std::endl;
                    //std::cout<<"id : "<<it->second<<std::endl;
                } else {
                    std::pair<std::string, long long int> newAddress (oss.str(), nbSerialized);
                    adresses.insert(newAddress);
                    buffer<<newAddress.second<<std::endl;
                    //std::cout<<"id : "<<newAddress.second<<std::endl;
                    nbSerialized++;
                    if (typeid(decltype(object)).name() == typeid(object).name()) {
                        object.vtserialize(*this);
                    } else {
                        object.key.register_object(&object);
                        object.key.serialize_object("serialize", "OTextArchive", *this);
                    }
                }
            }
            /**
            *\fn void operator(O& data, D...)
            *\brief register pointer to a dynamic object onto the archive.
            *\param O& the object to register.
            *\param D... : used for SFINAE.
            */
            template <class O,
                      class... D,
                      class = typename std::enable_if<!std::is_fundamental<O>::value>::type,
                      class = typename std::enable_if<!std::is_same<O, std::string>::value>::type,
                      class = typename std::enable_if<has_typedef_key<O>::value>::type,
                      class = typename std::enable_if<!std::is_enum<O>::value>::type,
                      class = typename std::enable_if<!sizeof...(D)>::type>
            void operator() (O* object, D...) {
                if (object != nullptr) {
                    std::ostringstream oss;
                    oss<<typeid(*object).name()<<"*"<<reinterpret_cast<unsigned long long int>(object);
                    std::map<std::string, long long int>::iterator it = adresses.find(oss.str());
                    if (it != adresses.end()) {
                        buffer<<it->second<<std::endl;
                        //std::cout<<"id : "<<it->second<<std::endl;
                    } else {
                        std::pair<std::string, long long int> newAddress (oss.str(), nbSerialized);
                        adresses.insert(newAddress);
                        buffer<<newAddress.second<<std::endl;
                        //std::cout<<"id : "<<newAddress.second<<std::endl;
                        std::string typeName = "BaseType";
                        if (typeid(decltype(*object)).name() == typeid(*object).name()) {
                            buffer<<typeName<<std::endl;
                            //std::cout<<"type name : "<<typeName<<std::endl;
                            nbSerialized++;
                            object->vtserialize(*this);
                        } else {
                            object->key.register_object(object);
                            typeName = object->key.getTypeName();
                            buffer<<typeName<<std::endl;
                            //std::cout<<"type name : "<<typeName<<std::endl;
                            nbSerialized++;
                            object->key.serialize_object("serialize", "OTextArchive", *this);
                        }
                    }
                } else {
                    long long int id = -1;
                    buffer<<id<<std::endl;
                }
            }
            //std::vectors.
            /**
            *\fn void operator(O& data, D...)
            *\brief register a list of objects onto the archive.
            *\param std::vector<O>& the list of objects to register.
            *\param D... : used for SFINAE.
            */
            template <class O>
            void operator() (std::vector<O>& data) {
                std::size_t size = data.size();
                buffer<<size<<std::endl;
                //std::cout<<"vector size : "<<size<<std::endl;
                for (unsigned int i = 0; i < data.size(); i++)
                     (*this)(data[i]);
            }
            template <class T>
            void operator() (std::unique_ptr<T>& ptr) {
                (*this)(ptr.get());
            }
            template <class T1, class T2>
            void operator() (std::pair<T1, T2>& pair) {
                (*this)(pair.first);
                (*this)(pair.second);
            }
            template <class T1, class T2>
            void operator() (std::map<T1, T2>&  map) {
                std::size_t size = map.size();
                buffer<<size<<std::endl;
                //std::cout<<"map size : "<<size<<std::endl;
                typename std::map<T1, T2>::iterator it;
                for (it = map.begin(); it != map.end(); it++) {
                    (*this)(*it);
                }
            }
            /**
            * Simple hash function. Takes two utf16 characters and enumerate
            * their hash value. The value is used as dictionary array index.
            *
            * @param first - utf16 character
            * @param second - utf16 character
            * @return hash value in interval from 0 to DICT_LEN
            */
            u16 hash(u16 char0, u16 char1)
            {
              return ((char1 * 37 ^ ((char0 >> 7) * 5) ^ (char0)) * 33) & DICT_LEN;
            }

            /**
            * Initialize the compressor. It is called from utf16_compress only.
            */
            void utf16_compressor_init(void)
            {
              if (utf16_compressor_initialized)
               return;
              utf16_compressor_initialized = 1;
              /* clear the hash table. It needs to be called only once to prevent access
              unaligned pointer which causes crash on some architectures. */
              memset(dictionary, 0, sizeof(dictionary));
            }
            /**
            * Writes one output byte.
            *
            * @param out - output byte
            * @param output - pointer to output array.
            */
            void OutputByte(u8 out, u8 **output)
            {
              **output = out;
              *output += 1;
            }
            /**
            * Encode pair of length and offset values. OutpurByte function is called on each output byte.
            *
            * @param offset - offset
            * @param len - length
            * @param pointer to output array
            */
            void OutputMatch(u16 offset, u16 len, u8 **output)
            {
              OutputByte((u8)(len & 0x3F) | ((len > 0x3F) << 6) | 0x80, output);
              len >>= 6;

              while (len > 0) {
                OutputByte((u8)(len & 0x7F) | ((len > 0x7F) << 7), output);
                len >>= 7;
              }

              OutputByte((u8)(offset & 0x3F) | ((offset > 0x3F) << 6) | 0x80, output);
              offset >>= 6;

              while (offset > 0) {
                OutputByte((u8)(offset & 0x7F) | ((offset > 0x7F) << 7), output);
                offset >>= 7;
              }
            }

            /**
            * Encode array of literals. OutpurByte function is called on each output byte.
            *
            * @param input - array of utf16 literals
            * @param len - number of input utf16 literals
            * @param output - pointer to output byte array.
            */

            void OutputLiteral(u16 *input, u16 len, u8 **output)
            {
              u16 previous, l = len;
              int diff;

              /* most significant bit is 0 to mark a literal */
              OutputByte((u8)(len & 0x3F) | ((len > 0x3F) << 6), output);

              len >>= 6;

              while (len > 0) {
                OutputByte((u8)(len & 0x7F) | ((len > 0x7F) << 7), output);
                len >>= 7;
              }

              /* save the first Unicode character */
              previous = *input++;
              OutputByte(previous & 0xFF, output);
              OutputByte(previous >> 8, output);

              /* save differences between the characters */
              len = l - 1;
              while (len-- > 0) {
                diff = previous - *input;
                if ((diff & 0xFFFFFFC0) == 0 || (diff | 0x3F) == -1)
                  OutputByte((u8)(diff & 0x7F), output);
                else {
                  OutputByte((u8)((diff & 0x7F) | 0x80), output);
                  diff >>= 7;
                  if ((diff & 0xFFFFFFC0) == 0 || (diff | 0x3F) == -1)
                    OutputByte((u8)(diff & 0x7F), output);
                  else {
                    OutputByte((u8)((diff & 0x7F) | 0x80), output);
                    OutputByte(diff >> 7, output);
                  }
                }
                previous = *input++;
              }
            }

            /**
            * utf16 Compress function. This function implements described compression algorithm.
            *
            * @param input - array of input utf16 characters
            * @param length - number of input utf16 characters
            * @param output - output buffer
            * @return number of output bytes
            */
            int utf16_compress(u16 *input, int length, u8 *output)
            {
              int i;
              u16 *input_orig = input;
              u8 *o = output;
              u16 *literal, *match, match_index;

              utf16_compressor_init();

              literal = input;
              while (length-- > 0)
              {
                /* check for previous occurrence */
                match_index = hash(*input, *(input + 1));
                match = dictionary[match_index];
                dictionary[match_index] = input;
                if (match >= input_orig && match < input && *match == *input && *(match + 1) == *(input + 1))
                {
                  /* Previous occurrence was found. Encode literals...*/
                  if (literal < input)
                    OutputLiteral(literal, input - literal, &output);
                  i = 2;
                  /* ...find the occurrence length...*/
                  while (*(match + i) == *(input + i))
                    ++i;
                  /* ...and encode the (offset, length) pair */
                  OutputMatch(input - match, i, &output);
                  input += i;
                  length -= (i - 1);
                  literal = input;
                }
                else
                  ++input;
              }
              /* if there are some remaining literals, encode them */
              if (literal < input)
                OutputLiteral(literal, input - literal, &output);

              return output - o;
            }
            /** \fn ostream& operator<<(std::ostream& out)
            *   \brief compress the archive's data and put it into an output stream.
            *   \param std::ostream& out : the output stream where to compress the data.
            *   \return std::ostream& : the output stream where the datas are compressed.
            */
            friend std::ostream& operator<<(std::ostream& out, OTextArchive& oa) {
                /*oa.compress_stdin();
                out<<oa.buffer;
                return out;*/
            }
        private :
            std::ostream& buffer; /**< the output buffer containing the datas.*/
            int utf16_compressor_initialized = 0; /**<A boolean to test if the compressor is initialized*/
            u16 * dictionary[DICT_LEN]; /**<A the dictionnary used for the compression algorithm*/
            std::map<std::string, long long int> adresses; /**< an std::map used to store the adresses and the id of the serialized pointers.*/
            unsigned long long int nbSerialized; /** <the number data which are serialized into the archive*/
        };
        /**
        * \file archive.h
        * \class ITextArchive
        * \brief Read everything from the input archive's buffer.
        * \author Duroisin.L
        * \version 1.0
        * \date 1/02/2014
        */
        class ITextArchive {
        public :
            /**
            *\fn ITextArchive(std::istream& buffer)
            *\brief pass the input stream to the input test archive.
            *\param std::istream& buffer : the input buffer where to read the datas.
            */
            ITextArchive (std::istream& buffer) : buffer(buffer) {
                nbDeserialized = 0;
            }
            bool isInputArchive() {
                return true;
            }
            void clear() {
                buffer.clear();
                adresses.clear();
                nbDeserialized = 0;
            }
            /**
            * reads compressed utf16 from stdin, decompress it and writes output to stdout
            */
            void decompress_stdin(void)
            {
              /*std::ostringstream output;
              output(buffer);
              int i, out_len;
              char *buffer, *output_buffer;
              i = output.str().size();*/

              /* allocate input and output buffers */
              /*buffer = new char[MAX_COMPRESSED_LENGTH];
              output_buffer = new char[MAX_DECOMPRESSED_LENGTH];*/

              /* read input from stdin */
              /*if (i == MAX_DECOMPRESSED_LENGTH)
              {
                  std::cerr<<"ERROR: Input is too long!"<<std::endl;
                  return;
              }
              buffer = const_cast<char*> (output.str().c_str());
              output.str("");*/
              /* make decompression */
              /*out_len = utf16_decompress((u8 *)buffer, i, (u16 *)output_buffer);
              //delete[] buffer;
              this->buffer.rdbuf()->pubsetbuf(output_buffer, out_len);
              //delete[] output_buffer;*/
            }
            //Fundamentals.
            /**
            * \fn void operator(T& data, D...)
            * \brief read a fundamental type from the archive.
            * \param T& the data to read.
            * \param D... used for SFINAE.
            */
            template <typename T,
                  class... D,
                  class = typename std::enable_if<std::is_fundamental<T>::value>::type>
            void operator() (T& data, D...) {
                long long int id;
                buffer>>id;
                char space;
                buffer.get(space);
                //std::cout<<"id : "<<id<<std::endl;
                std::map<long long int, std::string>::iterator it = adresses.find(id);
                if (it != adresses.end()) {
                    std::istringstream iss(it->second);
                    std::vector<std::string> parts = split(iss.str(), "*");
                    data = *reinterpret_cast<T*> (conversionStringULong(parts[1]));
                } else {
                    std::ostringstream oss;
                    oss<<typeid(data).name()<<"*"<<reinterpret_cast<unsigned long long int>(&data);
                    std::pair<long long int, std::string> newAddress (id, oss.str());
                    adresses.insert(newAddress);
                    nbDeserialized++;
                    buffer>>data;
                    char space;
                    buffer.get(space);
                    //std::cout<<"fundamental data : "<<data<<std::endl;
                }
            }
            /**
            * \fn void operator(T& data, D...)
            * \brief read a char from the archive. (we need to read unformatted input here to also read special chars like \n, spaces, etc...)
            * \param T& the data to read.
            * \param D... used for SFINAE.
            */
            void operator() (char& data) {
                long long int id;
                buffer>>id;
                char space;
                buffer.get(space);
                //std::cout<<"id : "<<id<<std::endl;
                std::map<long long int, std::string>::iterator it = adresses.find(id);
                if (it != adresses.end()) {
                    std::istringstream iss(it->second);
                    std::vector<std::string> parts = split(iss.str(), "*");
                    data = *reinterpret_cast<char*> (conversionStringULong(parts[1]));
                } else {
                    std::ostringstream oss;
                    oss<<typeid(data).name()<<"*"<<reinterpret_cast<unsigned long long int>(&data);
                    std::pair<long long int, std::string> newAddress (id, oss.str());
                    adresses.insert(newAddress);
                    nbDeserialized++;
                    buffer.get(data);
                    char space;
                    buffer.get(space);
                    //std::cout<<"char data : "<<data<<std::endl;
                }
            }
            /**
            * \fn void operator(T& data, D...)
            * \brief read an unsigned char from the archive. (we need to read unformatted input here to also read special chars like \n, spaces, etc...)
            * \param T& the data to read.
            * \param D... used for SFINAE.
            */
            void operator() (unsigned char& data) {
                long long int id;
                buffer>>id;
                char space;
                buffer.get(space);
                //std::cout<<"id : "<<id<<std::endl;
                std::map<long long int, std::string>::iterator it = adresses.find(id);
                if (it != adresses.end()) {
                    std::istringstream iss(it->second);
                    std::vector<std::string> parts = split(iss.str(), "*");
                    data = *reinterpret_cast<unsigned char*> (conversionStringULong(parts[1]));
                } else {
                    std::ostringstream oss;
                    oss<<typeid(data).name()<<"*"<<reinterpret_cast<unsigned long long int>(&data);
                    std::pair<long long int, std::string> newAddress (id, oss.str());
                    adresses.insert(newAddress);
                    nbDeserialized++;
                    buffer.get((char&) data);
                    char space;
                    buffer.get(space);
                    //std::cout<<"unsigned char data : "<<data<<std::endl;
                }
            }

            /**
            * \fn void operator(T& data, D...)
            * \brief read a pointer to a fundamental type from the archive.
            * \param T& the data to read.
            * \param D... used for SFINAE.
            */
            template <typename T,
                  class... D,
                  class = typename std::enable_if<std::is_fundamental<T>::value>::type>
            void operator() (T*& data, D...) {
                long long int id;
                buffer>>id;
                char space;
                buffer.get(space);
                //std::cout<<"id : "<<id<<std::endl;
                if (id != -1) {
                    std::map<long long int, std::string>::iterator it = adresses.find(id);
                    if (it != adresses.end()) {
                        std::istringstream iss(it->second);
                        std::vector<std::string> parts = split(iss.str(), "*");
                        data = reinterpret_cast<T*> (conversionStringULong(parts[1]));
                    } else {
                        data = new T();
                        std::ostringstream oss;
                        oss<<typeid(*data).name()<<"*"<<reinterpret_cast<unsigned long long int>(data);
                        std::pair<long long int, std::string> newAddress (id, oss.str());
                        adresses.insert(newAddress);
                        nbDeserialized++;
                        buffer>>(*data);
                        //std::cout<<"fundamental data : "<<(*data)<<std::endl;
                        char space;
                        buffer.get(space);
                    }
                } else {
                    data = nullptr;
                }
            }
             /**
            * \fn void operator(T& data, D...)
            * \brief read a pointer to a fundamental type from the archive.
            * \param T& the data to read.
            * \param D... used for SFINAE.
            */
            template <typename T,
                  class... D,
                  class = typename std::enable_if<std::is_fundamental<T>::value>::type>
            void operator() (char*& data, D...) {
                long long int id;
                buffer>>id;
                char space;
                buffer.get(space);
                //std::cout<<"id : "<<id<<std::endl;
                if (id != -1) {
                    std::map<long long int, std::string>::iterator it = adresses.find(id);
                    if (it != adresses.end()) {
                        std::istringstream iss(it->second);
                        std::vector<std::string> parts = split(iss.str(), "*");
                        data = reinterpret_cast<T*> (conversionStringULong(parts[1]));
                    } else {
                        data = new char();
                        std::ostringstream oss;
                        oss<<typeid(*data).name()<<"*"<<reinterpret_cast<unsigned long long int>(data);
                        std::pair<long long int, std::string> newAddress (id, oss.str());
                        adresses.insert(newAddress);
                        nbDeserialized++;
                        buffer>>(*data);
                        char space;
                        buffer.get(space);
                        //std::cout<<"char data : "<<(*data)<<std::endl;
                    }
                } else {
                    data = nullptr;
                }
            }
             /**
            * \fn void operator(T& data, D...)
            * \brief read a pointer to a fundamental type from the archive.
            * \param T& the data to read.
            * \param D... used for SFINAE.
            */
            template <typename T,
                  class... D,
                  class = typename std::enable_if<std::is_fundamental<T>::value>::type>
            void operator() (unsigned char*& data, D...) {
                long long int id;
                buffer>>id;
                char space;
                buffer.get(space);
                //std::cout<<"id : "<<id<<std::endl;
                if (id != -1) {
                    std::map<long long int, std::string>::iterator it = adresses.find(id);
                    if (it != adresses.end()) {
                        std::istringstream iss(it->second);
                        std::vector<std::string> parts = split(iss.str(), "*");
                        data = reinterpret_cast<T*> (conversionStringULong(parts[1]));
                    } else {
                        data = new unsigned char();
                        std::ostringstream oss;
                        oss<<typeid(*data).name()<<"*"<<reinterpret_cast<unsigned long long int>(data);
                        std::pair<long long int, std::string> newAddress (id, oss.str());
                        adresses.insert(newAddress);
                        nbDeserialized++;
                        buffer>>((char&) *data);
                        char space;
                        buffer.get(space);
                        //std::cout<<"unsigned char data"<<(*data)<<std::endl;
                    }
                } else {
                    data = nullptr;
                }
            }
            template <typename E,
                    class... D,
                    class = typename std::enable_if<!std::is_fundamental<E>::value>::type,
                    class = typename std::enable_if<std::is_enum<E>::value>::type>
            void operator()(E& data, D...) {
                long long int id;
                buffer>>id;
                char space;
                buffer.get(space);
                //std::cout<<"id : "<<id<<std::endl;
                std::map<long long int, std::string>::iterator it = adresses.find(id);
                if (it != adresses.end()) {
                    std::istringstream iss(it->second);
                    std::vector<std::string> parts = split(iss.str(), "*");
                    data = *reinterpret_cast<E*> (conversionStringULong(parts[1]));
                } else {
                    int eVal;
                    buffer>>eVal;
                    data = static_cast<E>(eVal);
                    char space;
                    buffer.get(space);
                    //std::cout<<"enum val : "<<eVal<<std::endl;
                    std::ostringstream oss;
                    oss<<typeid(data).name()<<"*"<<reinterpret_cast<unsigned long long int>(&data);
                    std::pair<long long int, std::string> newAddress (id, oss.str());
                    adresses.insert(newAddress);
                    nbDeserialized++;
                }
            }
            template <typename E,
                    class... D,
                    class = typename std::enable_if<!std::is_fundamental<E>::value>::type,
                    class = typename std::enable_if<std::is_enum<E>::value>::type>
            void operator() (E*& data, D...) {
                long long int id;
                buffer>>id;
                char space;
                buffer.get(space);
                //std::cout<<"id : "<<id<<std::endl;
                if (id != -1) {
                    std::map<long long int, std::string>::iterator it = adresses.find(id);
                    if (it != adresses.end()) {
                        std::istringstream iss(it->second);
                        std::vector<std::string> parts = split(iss.str(), "*");
                        data = reinterpret_cast<E*> (conversionStringULong(parts[1]));
                    } else {
                        data = new E();
                        int eVal;
                        buffer>>eVal;
                        *data = static_cast<E>(eVal);
                        char space;
                        buffer.get(space);
                        //std::cout<<"enum val : "<<eVal<<std::endl;
                        std::ostringstream oss;
                        oss<<typeid(*data).name()<<"*"<<reinterpret_cast<unsigned long long int>(data);
                        std::pair<long long int, std::string> newAddress (id, oss.str());
                        adresses.insert(newAddress);
                        nbDeserialized++;
                    }
                } else {
                    data = nullptr;
                }
            }
            //std::string.
            /**
            * \fn void operator(T& data, D...)
            * \brief read an std::string from the archive.
            * \param T& the data to read.
            * \param D... used for SFINAE.
            */
            template <typename T,
                  class... D,
                  class = typename std::enable_if<!std::is_fundamental<T>::value>::type,
                  class = typename std::enable_if<std::is_same<T, std::string>::value>::type,
                  class = typename std::enable_if<!std::is_enum<T>::value>::type>
            void operator() (T& data, D...) {
                long long int id;
                buffer>>id;
                char space;
                buffer.get(space);
                //std::cout<<"id : "<<id<<std::endl;
                std::map<long long int, std::string>::iterator it = adresses.find(id);
                if (it != adresses.end()) {
                    std::istringstream iss(it->second);
                    std::vector<std::string> parts = split(iss.str(), "*");
                    data = *reinterpret_cast<T*> (conversionStringULong(parts[1]));
                } else {
                    std::size_t str_size;
                    buffer>>str_size;
                    char space;
                    buffer.get(space);
                    //std::cout<<"str size : "<<str_size<<std::endl;
                    char* datas = new char[str_size];
                    nbDeserialized++;
                    for (unsigned int i = 0; i < str_size; i++) {
                        (*this)(datas[i]);
                    }
                    data = std::string(datas, str_size);
                    std::ostringstream oss;
                    oss<<typeid(data).name()<<"*"<<reinterpret_cast<unsigned long long int>(&data);
                    std::pair<long long int, std::string> newAddress (id, oss.str());
                    adresses.insert(newAddress);
                }
            }
            /**
            * \fn void operator(T& data, D...)
            * \brief read a pointer to an std::string from the archive.
            * \param T* the data to read.
            * \param D... used for SFINAE.
            */
            template <typename T,
                  class... D,
                  class = typename std::enable_if<!std::is_fundamental<T>::value>::type,
                  class = typename std::enable_if<std::is_same<T, std::string>::value>::type,
                  class = typename std::enable_if<!std::is_enum<T>::value>::type>
            void operator() (T*& data, D...) {
                long long int id;
                buffer>>id;
                char space;
                buffer.get(space);
                //std::cout<<"id : "<<id<<std::endl;
                if (id != -1) {
                    std::map<long long int, std::string>::iterator it = adresses.find(id);
                    if (it != adresses.end()) {
                        std::istringstream iss(it->second);
                        std::vector<std::string> parts = split(iss.str(), "*");
                        data = reinterpret_cast<T*> (conversionStringULong(parts[1]));
                    } else {
                        std::size_t str_size;
                        buffer>>str_size;
                        char space;
                        buffer.get(space);
                        //std::cout<<"str size : "<<str_size<<std::endl;
                        char* datas = new char [str_size];
                        nbDeserialized++;
                        for (unsigned int i = 0; i < str_size; i++)
                            (*this)(datas[i]);
                        data = new std::string(datas, str_size);
                        std::ostringstream oss;
                        oss<<typeid(*data).name()<<"*"<<reinterpret_cast<unsigned long long int>(data);
                        std::pair<long long int, std::string> newAddress (id, oss.str());
                        adresses.insert(newAddress);
                    }
                } else {
                    data = nullptr;
                }
            }
            //Static objects.
            /**
            * \fn void operator(O& data, D...)
            * \brief read a static object from the archive.
            * \param O& the data to read.
            * \param D... used for SFINAE.
            */
            template <class O,
                      class... D,
                      class = typename std::enable_if<!std::is_fundamental<O>::value>::type,
                      class = typename std::enable_if<!std::is_same<O, std::string>::value && !std::is_pointer<O>::value>::type,
                      class = typename std::enable_if<!has_typedef_key<O>::value>::type,
                      class = typename std::enable_if<!std::is_enum<O>::value>::type>
            void operator() (O& object, D...) {
                long long int id;
                buffer>>id;
                char space;
                buffer.get(space);
                //std::cout<<"id : "<<id<<std::endl;
                std::map<long long int, std::string>::iterator it = adresses.find(id);
                if (it != adresses.end()) {
                    std::istringstream iss(it->second);
                    std::vector<std::string> parts = split(iss.str(), "*");
                    object = *reinterpret_cast<O*> (conversionStringULong(parts[1]));
                } else {
                    std::ostringstream oss;
                    oss<<typeid(object).name()<<"*"<<reinterpret_cast<unsigned long long int>(&object);
                    std::pair<long long int, std::string> newAddress (id, oss.str());
                    adresses.insert(newAddress);
                    nbDeserialized++;
                    object.serialize(*this);
                }
            }
            /**
            * \fn void operator(O& data, D...)
            * \brief read a pointer to a static object from the archive.
            * \param O* the data to read.
            * \param D... used for SFINAE.
            */
            template <class O,
                      class... D,
                      class = typename std::enable_if<!std::is_fundamental<O>::value>::type,
                      class = typename std::enable_if<!std::is_same<O, std::string>::value>::type,
                      class = typename std::enable_if<!has_typedef_key<O>::value>::type,
                      class = typename std::enable_if<!std::is_enum<O>::value>::type>
            void operator() (O*& object, D...) {
                long long int id;
                buffer>>id;
                char space;
                buffer.get(space);
                //std::cout<<"id : "<<id<<std::endl;
                if (id != -1) {
                    std::map<long long int, std::string>::iterator it = adresses.find(id);
                    if (it != adresses.end()) {
                        std::istringstream iss(it->second);
                        std::vector<std::string> parts = split(iss.str(), "*");
                        object = reinterpret_cast<O*> (conversionStringULong(parts[1]));
                    } else {
                        object = new O();
                        std::ostringstream oss;
                        oss<<typeid(*object).name()<<"*"<<reinterpret_cast<unsigned long long int>(object);
                        std::pair<long long int, std::string> newAddress (id, oss.str());
                        adresses.insert(newAddress);
                        nbDeserialized++;
                        object->serialize(*this);
                    }
                } else {
                    object = nullptr;
                }
            }
            //Dynamic objects.
            /**
            * \fn void operator(O* data, D...)
            * \brief read a dynamic object from the archive.
            * \param O& the data to read.
            * \param D... used for SFINAE.
            */
            template <class O,
                      class... D,
                      class = typename std::enable_if<!std::is_fundamental<O>::value>::type,
                      class = typename std::enable_if<!std::is_same<O, std::string>::value>::type,
                      class = typename std::enable_if<has_typedef_key<O>::value>::type,
                      class = typename std::enable_if<!std::is_enum<O>::value>::type,
                      class = typename std::enable_if<!sizeof...(D)>::type>
            void operator() (O& object, D...) {
                long long int id;
                buffer>>id;
                char space;
                buffer.get(space);
                //std::cout<<"id : "<<id<<std::endl;
                std::map<long long int, std::string>::iterator it = adresses.find(id);
                if (it != adresses.end()) {
                    std::istringstream iss(it->second);
                    std::vector<std::string> parts = split(iss.str(), "*");
                    object = *reinterpret_cast<O*> (conversionStringULong(parts[1]));
                } else {
                    std::ostringstream oss;
                    oss<<typeid(object).name()<<"*"<<reinterpret_cast<unsigned long long int>(&object);
                    std::pair<long long int, std::string> newAddress (id, oss.str());
                    adresses.insert(newAddress);
                    nbDeserialized++;
                    if (typeid(decltype(object)) == typeid(object)) {
                        object.vtserialize(*this);
                    } else {
                        object.key.register_object(&object);
                        object.key.serialize_object("serialize", "ITextArchive", *this);
                    }
                }
            }
            /**
            * \fn void operator(O* data, D...)
            * \brief read a pointer to a non abstract dynamic object from the archive.
            * \param O* the data to read.
            * \param D... used for SFINAE.
            */
            template <class O,
                      class... D,
                      class = typename std::enable_if<!std::is_fundamental<O>::value>::type,
                      class = typename std::enable_if<!std::is_same<O, std::string>::value>::type,
                      class = typename std::enable_if<!std::is_enum<O>::value>::type,
                      class = typename std::enable_if<has_typedef_key<O>::value && !std::is_abstract<O>::value>::type,
                      class = typename std::enable_if<!sizeof...(D)>::type>
            void operator() (O*& object, D...) {
                long long int id;
                buffer>>id;
                char space;
                buffer.get(space);
                //std::cout<<"id : "<<id<<std::endl;
                if (id != -1) {
                    std::map<long long int, std::string>::iterator it = adresses.find(id);
                    if (it != adresses.end()) {
                        std::istringstream iss(it->second);
                        std::vector<std::string> parts = split(iss.str(), "*");
                        object = reinterpret_cast<O*> (conversionStringULong(parts[1]));
                    } else {
                        std::string typeName;
                        getline(buffer, typeName);
                        std::cout<<"type name : "<<typeName<<std::endl;
                        if (typeName == "BaseType") {
                            object = new O();
                            std::ostringstream oss;
                            oss<<typeid(*object).name()<<"*"<<reinterpret_cast<unsigned long long int>(object);
                            std::pair<long long int, std::string> newAddress (id, oss.str());
                            adresses.insert(newAddress);
                            nbDeserialized++;
                            object->vtserialize(*this);
                        } else {
                            object = dynamic_cast<O*>(O::allocate(typeName));
                            std::ostringstream oss;
                            oss<<typeid(*object).name()<<"*"<<reinterpret_cast<unsigned long long int>(object);
                            std::pair<long long int, std::string> newAddress (id, oss.str());
                            adresses.insert(newAddress);
                            nbDeserialized++;
                            object->key.register_object(object);
                            object->key.serialize_object("serialize", "ITextArchive", *this);
                        }
                    }
                } else {
                    object = nullptr;
                }
            }
            /**
            * \fn void operator(O* data, D...)
            * \brief read a pointer to an abstract dynamic object from the archive.
            * \param O* the data to read.
            * \param D... used for SFINAE.
            */
            template <class O,
                      class... D,
                      class = typename std::enable_if<!std::is_fundamental<O>::value>::type,
                      class = typename std::enable_if<!std::is_same<O, std::string>::value>::type,
                      class = typename std::enable_if<!std::is_enum<O>::value>::type,
                      class = typename std::enable_if<has_typedef_key<O>::value>::type,
                      class = typename std::enable_if<std::is_abstract<O>::value>::type,
                      class = typename std::enable_if<!sizeof...(D)>::type>
            void operator() (O*& object, D...) {
                long long int id;
                buffer>>id;
                char space;
                buffer.get(space);
                //std::cout<<"id : "<<id<<std::endl;
                if (id != -1) {
                    std::map<long long int, std::string>::iterator it = adresses.find(id);
                    if (it != adresses.end()) {
                        std::istringstream iss(it->second);
                        std::vector<std::string> parts = split(iss.str(), "*");
                        object = reinterpret_cast<O*> (conversionStringULong(parts[1]));
                    } else {
                        std::string typeName;
                        getline(buffer, typeName);
                        //std::cout<<"typename : "<<typeName<<std::endl;
                        object = dynamic_cast<O*>(O::allocate(typeName));
                        std::ostringstream oss;
                        oss<<typeid(*object).name()<<"*"<<reinterpret_cast<unsigned long long int>(object);
                        std::pair<long long int, std::string> newAddress (id, oss.str());
                        adresses.insert(newAddress);
                        nbDeserialized++;
                        object->key.register_object(object);
                        object->key.serialize_object("serialize", "ITextArchive", *this);
                    }
                } else {
                    object = nullptr;
                }
            }
            /**
            * \fn void operator(O* data, D...)
            * \brief read a list of objects from the archive.
            * \param O* the data to read.
            * \param D... used for SFINAE.
            */
            template <class O>
            void operator() (std::vector<O>& objects) {
                std::size_t size;
                buffer>>size;
                char space;
                buffer.get(space);
                //std::cout<<"vector size : "<<size<<std::endl;
                for (unsigned int i = 0; i < size; i++) {
                    O object;
                    (*this)(object);
                    objects.push_back(std::move(object));
                }
            }
            template <class T>
            void operator() (std::unique_ptr<T>& ptr) {
                T* tmp;
                (*this)(tmp);
                if (tmp != nullptr)
                    ptr.reset(tmp);
            }
            template <class T1, class T2>
            void operator()(std::pair<T1, T2>& pair) {
                T1 type1;
                T2 type2;
                (*this)(type1);
                (*this)(type2);
                pair = std::make_pair(type1, type2);
            }
            template <class T1, class T2>
            void operator()(std::map<T1, T2>& map) {
                std::size_t size;
                buffer>>size;
                char space;
                buffer.get(space);
                //std::cout<<"map size : "<<size<<std::endl;
                for (unsigned int i = 0; i < size; i++) {
                    std::pair<T1, T2> pair;
                    (*this)(pair);
                    map.insert(pair);
                }
            }
            /**
            * Decode length value. The offset is decoded the same way.
            *
            * @param input - pointer to array of bytes which encodes the length.
            * @return the encoded length/offset value. *input pointer is shifted by the read length.
            */
            int DecodeLength(u8 **input)
            {
              int ret = 0;
              int shift = 6;

              u8 *b = *input;

              (*input)++;
              ret += *b & 0x3f;

              if ((*b & 0x40) == 0)
                return ret;

              do
              {
                b = *input;
                (*input)++;
                ret |= ((int)(*b & 0x7f)) << shift;
                shift+=7;
              } while ((*b & 0x80) != 0);

              return ret;
            }

            /**
            * Writes one output byte.
            *
            * @param out - output byte
            * @param output - pointer to output array.
            */
            void OutputByte(u8 out, u8 **output)
            {
              **output = out;
              *output += 1;
            }
             /**
            * utf16 Decompress function. This function implements described decompression algorithm.
            *
            * @param input - input compressed data
            * @param len - length of input
            * @param output - output buffer
            * @return number of extracted utf16 characters
            */
            int utf16_decompress(u8 *input, int len, u16 *output)
            {
              u8 *o = (u8 *)output;
              int offset, length;
              u8 *end = input + len;
              int c;

              while (input < end) {
              /* decite what to decode. Match or set of literals?*/
                if (*input & 0x80)
                {  /* match */
                  length = DecodeLength(&input);
                  offset = DecodeOffset(&input);  // same algorithm as DecodeLength
                  while (length-- > 0)
                  {
                    *output = *(output - offset);
                    ++output;
                  }
                }
                else
                {
                  /* literal */
                  length = DecodeLength(&input);
                  *output = *input++;
                  *output++ |= ((unsigned int)(*input++)) << 8;
                  --length;

                  while (length-- > 0) {
                    c = *input & 0x7F;

                    if (*input++ & 0x80) {    /* two bytes */
                      c |= ((unsigned int)*input & 0x7F) << 7;

                      if (*input++ & 0x80) {  /* three bytes */
                        c |= *input++ << 14;

                        if (c & 0x10000)      /* negative number */
                          c |= 0xFFFF0000;
                      }
                      else if (c & 0x2000)    /* negative number */
                        c |= 0xFFFFC000;
                    }
                    else if (c & 0x40)        /* negative number */
                      c |= 0xFFFFFF80;

                    *output = *(output - 1) - c;
                    ++output;
                  }
                }
              }
              return (u8 *) output - o;
            }
            /** \fn istream& operator<<(std::istream& in)
            *   \brief decompress the archive's data and put it into an input stream.
            *   \param std::istream& in : the input stream where to decompress the data.
            *   \return the input stream where the datas are decompressed.
            */
            friend std::istream& operator>>(std::istream& in, ITextArchive& ia) {
                /*std::streambuf * pbuf = in.rdbuf();
                long size = pbuf->pubseekoff(0,in.end);
                char* contents = new char [size];
                pbuf->sgetn (contents,size);
                ia.buffer.rdbuf()->pubsetbuf(contents, size);
                ia.decompress_stdin();
                return in;*/
            }
        private :
            std::istream& buffer; /**< the buffer where to read the data.*/
            std::map<long long int, std::string> adresses; /**< an std::map used to store ids and adresses of readed pointers.*/
            unsigned long long int nbDeserialized; /** the nb object which have been deserailized.*/
        };
    }
}
#endif // ODFAEG_ARCHIVE

