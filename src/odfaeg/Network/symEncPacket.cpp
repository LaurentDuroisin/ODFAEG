#include "../../../include/odfaeg/Network/symEncPacket.h"
#include <climits>
namespace odfaeg {
    namespace network {
        using namespace std;
        using namespace sf;
        AES_ENC SymEncPacket::aes = AES_ENC();
        const void* SymEncPacket::onSend (size_t& dataSize) {
            unsigned char* buffer;
            buffer = aes.ossl_encrypt(static_cast<const unsigned char*> (getData()), getDataSize(), reinterpret_cast<int&>(dataSize));
            std::cout<<"send ! "<<std::endl;
            for (unsigned int i = 0; i < dataSize; i++) {
                std::cout<<std::hex<<(int) buffer[i];
            }
            std::cout<<std::endl;
            return &buffer[0];
        }
        void SymEncPacket::onReceive (const void* data, size_t dataSize) {
            unsigned char* buffer;
            std::size_t dstSize = 0;
            const unsigned char* datas = static_cast<const unsigned char*> (data);
            std::cout<<"receive : "<<std::endl;
            for (unsigned int i = 0; i < dataSize; i++) {
                std::cout<<std::hex<<(int) datas[i];
            }
            std::cout<<std::endl;
            buffer = aes.ossl_decrypt(static_cast<const unsigned char*> (data), dataSize, reinterpret_cast<int&>(dstSize));
            append(&buffer[0], dstSize);
        }
    }
}
