#include "application.h"
#include <stdlib.h>     /* atexit */
#include "odfaeg/Network/cliEncryptedPacket.hpp"
using namespace odfaeg::core;
using namespace odfaeg::math;
using namespace odfaeg::graphic;
using namespace odfaeg::physic;
using namespace odfaeg::network;
using namespace sorrok;
using namespace sf;
using namespace std;
int main (int argv, char* argc[]) {
    EXPORT_CLASS_GUID(BoundingVolumeBoundingBox, BoundingVolume, BoundingBox)
    EXPORT_CLASS_GUID(EntityTile, Entity, Tile)
    EXPORT_CLASS_GUID(EntityTile, Entity, BigTile)
    EXPORT_CLASS_GUID(EntityWall, Entity, g2d::Wall)
    EXPORT_CLASS_GUID(EntityDecor, Entity, g2d::Decor)
    EXPORT_CLASS_GUID(EntityAnimation, Entity, Anim)
    EXPORT_CLASS_GUID(EntityHero, Entity, Hero)
    EXPORT_CLASS_GUID(EntityMonster, Entity, Monster)
    EXPORT_CLASS_GUID(EntityPnj, Entity, Pnj)
    EXPORT_CLASS_GUID(EntityMesh, Entity, Mesh)
    EXPORT_CLASS_GUID(EntityPLIGHT, Entity, g2d::PonctualLight)
    MyAppli app;
    return app.exec();
    bool running = false;
    TcpListener listener;
    SocketSelector selector;
    if (!listener.listen(10000) == Socket::Done) {
        cerr<<"Impossible d'écouter sur ce port : "<<endl;
    } else {
        cout<<"Server started!"<<endl;
        selector.add(listener);
        running = true;
        CliEncryptedPacket::setServerRunning(running);
    }
    std::vector<TcpSocket*> clients;
    Network::setCertifiateClientMess("SORROKCLIENT");
    bool pbCliRsaKeyReceived = false, certifiedClient = false;
    while (running) {
        if (selector.wait(milliseconds(10))) {
            if (selector.isReady(listener)) {
                TcpSocket *client = new TcpSocket();
                if (listener.accept(*client) == Socket::Done) {
                    std::cout<<"client connected!"<<std::endl;
                    selector.add(*client);
                    clients.push_back(client);
                } else {
                    delete client;
                }
            }
            std::vector<TcpSocket*>::iterator it;
            for (it = clients.begin(); it != clients.end();it++) {
                    TcpSocket& client = **it;
                    if (selector.isReady(client)) {
                        if (!pbCliRsaKeyReceived && !certifiedClient) {
                            Packet packet;
                            if (client.receive(packet) == Socket::Done) {
                                std::string request;
                                packet>>request;
                                if (request.find("SETCLIPBKEY") != std::string::npos) {
                                    std::cout<<"set cli pb key"<<std::endl;
                                    request.erase(0, 11);
                                    CliEncryptedPacket::setCertificate(reinterpret_cast<const unsigned char*>(request.c_str()), request.size());
                                    pbCliRsaKeyReceived = true;
                                    CliEncryptedPacket cliEncryptedPacket;
                                    std::string message = "GETCERTIFIATECLIENT";
                                    cliEncryptedPacket<<message;
                                    client.send(cliEncryptedPacket);
                                }
                            } else {
                                selector.remove(client);
                                it = clients.erase(it);
                                delete *it;
                                it--;
                            }
                        } else if (pbCliRsaKeyReceived && !certifiedClient) {
                            CliEncryptedPacket packet;
                            if (client.receive(packet) == Socket::Done) {
                                std::string request;
                                packet>>request;
                                packet.clear();
                                std::cout<<"authentified message : "<<request<<std::endl;
                                if (request == Network::getCertifiateClientMess()) {
                                    std::cout<<"Ok"<<std::endl;
                                    packet<<"CERTIFIEDCLIENT";
                                    client.send(packet);
                                    certifiedClient=true;
                                }
                            } else {
                                selector.remove(client);
                                it = clients.erase(it);
                                delete *it;
                                it--;
                            }
                        }
                    }
            }
        }
    }
    return 0 ;
}

