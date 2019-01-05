#include "application.h"
using namespace odfaeg::core;
using namespace odfaeg::graphic;
using namespace odfaeg::physic;
using namespace odfaeg::math;
using namespace odfaeg::network;
using namespace sorrok;
using namespace sf;
using namespace std;
int main(int argv, char* argc[]) {
    EXPORT_CLASS_GUID(BoundingVolumeBoundingBox, BoundingVolume, BoundingBox)
    EXPORT_CLASS_GUID(EntityTile, Entity, Tile)
    EXPORT_CLASS_GUID(EntityTile, Entity, BigTile)
    EXPORT_CLASS_GUID(EntityWall, Entity, g2d::Wall)
    EXPORT_CLASS_GUID(EntityDecor, Entity, g2d::Decor)
    EXPORT_CLASS_GUID(EntityAnimation, Entity, Anim)
    EXPORT_CLASS_GUID(EntityHero, Entity, Hero)
    EXPORT_CLASS_GUID(EntityMonster, Entity, Monster)
    MyAppli app(sf::VideoMode(800, 600, 32), "Test odfaeg");
    return app.exec();
    TcpSocket clientTCP;
    bool running = false;
    if(clientTCP.connect(IpAddress::LocalHost, 10000) != Socket::Done) {
        cout<<"Erreur : impossible de se connecter au serveur!"<<endl;
        return false;
    } else {
        running = true;
    }
    bool certified = false;
    Packet packet;
    unsigned char* cliPbKey = nullptr;
    int size = CliEncryptedPacket::getCertificate(&cliPbKey);
    std::string message (reinterpret_cast<char*>(cliPbKey), size);
    message.insert(0, "SETCLIPBKEY");
    packet<<message;
    clientTCP.send(packet);
    Network::setCertifiateClientMess("SORROKCLIENT");
    bool certifiateMessageSend = false;
    while (running) {
        if (!certifiateMessageSend && !certified) {
            CliEncryptedPacket cliEncryptedPacket;
            if (clientTCP.receive(cliEncryptedPacket) == Socket::Done) {
                std::string response;
                cliEncryptedPacket>>response;
                cliEncryptedPacket.clear();
                if (response == "GETCERTIFIATECLIENT") {
                    std::string message = Network::getCertifiateClientMess();
                    std::cout<<"get client certified : "<<message<<std::endl;
                    cliEncryptedPacket<<message;
                    clientTCP.send(cliEncryptedPacket);
                    certifiateMessageSend = true;
                }
            }
        }
        if (certifiateMessageSend && !certified) {
            CliEncryptedPacket cliEncryptedPacket;
            if (clientTCP.receive(cliEncryptedPacket) == Socket::Done) {
                std::string response;
                cliEncryptedPacket>>response;
                cliEncryptedPacket.clear();
                if (response == "CERTIFIEDCLIENT") {
                    std::cout<<"client certified"<<std::endl;
                    certified = true;
                    running = false;
                }
            }
        }
    }
    return 0;
}
