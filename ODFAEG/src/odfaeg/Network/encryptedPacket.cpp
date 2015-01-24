#include "../../../include/odfaeg/Network/encryptedPacket.h"
#include "../../../include/odfaeg/Network/network.h"
namespace odfaeg {
    namespace network {
        using namespace std;
        using namespace sf;
        Rsa EncryptedPacket::rsa = Rsa();
        const void* EncryptedPacket::onSend (size_t& dataSize) {
            unsigned char* buffer;
            if (Network::getSrvInstance().isRunning())
                buffer = rsa.encryptWithPrKey(static_cast<const unsigned char*> (getData()), getDataSize(), reinterpret_cast<int*> (&dataSize));
            else
                buffer = rsa.encryptWithPbKey(static_cast<const unsigned char*> (getData()), getDataSize(), reinterpret_cast<int*> (&dataSize));
            return &buffer[0];
        }
        void EncryptedPacket::onReceive (const void* data, size_t dataSize) {
            unsigned char* buffer;
            std::size_t dstSize = 0;
            if (Network::getSrvInstance().isRunning())
                buffer = rsa.decryptWithPrKey(static_cast<const unsigned char*> (data), dataSize, reinterpret_cast<int*> (&dstSize));
            else
                buffer = rsa.decryptWithPbKey(static_cast<const unsigned char*> (data), dataSize, reinterpret_cast<int*> (&dstSize));
            append(&buffer[0], dstSize);
        }
    }
}
