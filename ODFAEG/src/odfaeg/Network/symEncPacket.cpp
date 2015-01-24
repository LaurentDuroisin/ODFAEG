#include "../../../include/odfaeg/Network/symEncPacket.h"
#include <climits>
namespace odfaeg {
    namespace network {
        using namespace std;
        using namespace sf;
        AES_ENC SymEncPacket::aes = AES_ENC();
        const void* SymEncPacket::onSend (size_t& dataSize) {
            unsigned char* buffer;
            /*std::cout<<"data : "<<std::endl;
            for (unsigned int i = 0; i < getDataSize(); i++) {
                std::cout<<(int) static_cast<const unsigned char*> (getData())[i]<<std::endl;
            }
            std::cout<<std::endl<<"data size : "<<getDataSize()<<std::endl;*/
            buffer = aes.encrypt(reinterpret_cast<const unsigned char*> (getData()), getDataSize(), reinterpret_cast<int*>(&dataSize));
            /*std::cout<<"data : "<<std::endl;
            for (unsigned int i = 0; i < dataSize; i++) {
                std::cout<<(int) buffer[i]<<" ";
            }
            std::cout<<std::endl;
            std::cout<<"size : "<<dataSize<<std::endl;*/
            return &buffer[0];
        }
        void SymEncPacket::onReceive (const void* data, size_t dataSize) {
            unsigned char* buffer;
            std::size_t dstSize = 0;
            /*std::cout<<"data : "<<std::endl;
            for (unsigned int i = 0; i < dataSize; i++) {
                std::cout<<(int) static_cast<const unsigned char*>(data)[i]<<" ";
            }
            std::cout<<std::endl;
            std::cout<<"size : "<<dataSize<<std::endl;*/
            buffer = aes.decrypt(reinterpret_cast<const unsigned char*> (data), dataSize, reinterpret_cast<int*>(&dstSize));
            /*std::cout<<"data : "<<std::endl;
            for (unsigned int i = 0; i < dstSize; i++)
                std::cout<<(int) buffer[i]<<" ";
            std::cout<<std::endl<<"data size : "<<dstSize<<std::endl;*/
            append(&buffer[0], dstSize);
        }
    }
}
