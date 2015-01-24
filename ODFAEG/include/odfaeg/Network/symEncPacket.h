#ifndef SYM_ENC_PACKET
#define SYM_ENC_PACKET
#include <SFML/Network.hpp>
#include "aes.h"
#include "export.hpp"
namespace odfaeg {
    namespace network {
        class ODFAEG_NETWORK_API SymEncPacket : public sf::Packet {
            public :
                static char* getKey() {
                    return aes.getKey();
                }
                static char* getIv() {
                    return aes.getIv();
                }
                static void setKey(char* key) {
                    aes.setKey(key);
                }
                static void setIv(char* iv) {
                    aes.setIv(iv);
                }
            private :
                virtual const void* onSend(std::size_t& dataSize);
                virtual void onReceive (const void* data, std::size_t dataSize);
                static AES_ENC aes;
        };
    }
}
#endif

