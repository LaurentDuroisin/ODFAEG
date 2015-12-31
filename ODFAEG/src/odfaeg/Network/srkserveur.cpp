#include "../../../include/odfaeg/Network/srkserveur.h"
#include "../../../include/odfaeg/Network/network.h"
#include "../../../include/odfaeg/Core/application.h"
namespace odfaeg {
    namespace network {
        using namespace sf;
        using namespace std;

        SrkServer::SrkServer()
        {
            running = false;
        }
        bool SrkServer::startSrv(int portTCP, int portUDP) {
            if (!running) {
                if (!udpSocket.bind(portUDP) == Socket::Done) {
                    cerr<<"Impossible d'écouter sur ce port : "<<portUDP<<endl;
                    return false;
                } else {
                    selector.add(udpSocket);
                }
                if (!listener.listen(portTCP) == Socket::Done) {
                    cerr<<"Impossible d'écouter sur ce port : "<<portTCP<<endl;
                    return false;
                } else {
                    cout<<"Server started!"<<endl;
                    selector.add(listener);
                    running = true;
                    return true;
                }

            } else {
                cout<<"Server already started!"<<endl;
                return false;
            }

        }
        void SrkServer::stopSrv () {
            if (running) {
                running = false;
            } else {
                cout<<"Server already stopped!"<<endl;
            }
        }
        bool SrkServer::isRunning () {
            return running;
        }
        void SrkServer::checkMessages() {
            if (running) {
                vector<TcpSocket*>::iterator it;
                if (Network::getTimeBtw2PingsClk().getElapsedTime().asMicroseconds() >= Network::getTimeBtw2Pings()) {
                    for (it = clients.begin(); it != clients.end();it++) {
                        TcpSocket& client = **it;
                        User* user = Network::getUser(client);
                        if (user != nullptr && user->getRemotePortUDP()) {
                            Packet packet;
                            packet<<"PING";
                            user->getPingClock().restart();
                            user->sendUdpPacket(packet);
                        }
                    }
                    Network::getTimeBtw2PingsClk().restart();
                }
                if (Network::getTimeBtw2SyncClk().getElapsedTime().asMicroseconds() >= Network::getTimeBtw2Sync()) {
                    for (it = clients.begin(); it != clients.end();it++) {
                        TcpSocket& client = **it;
                        User* user = Network::getUser(client);
                        if (user != nullptr && user->getRemotePortUDP()) {
                            Packet packet;
                            packet<<"GET_TIME";
                            sf::Int64 lastSrvTime = core::Application::getTimeClk().getElapsedTime().asMicroseconds();
                            user->setLastSrvTime(lastSrvTime);
                            user->sendUdpPacket(packet);
                        }
                    }
                    Network::getTimeBtw2SyncClk().restart();
                }
                if (selector.wait(sf::milliseconds(10))) {
                    lock_guard<recursive_mutex> locker (rec_mutex);
                    if (selector.isReady(listener)) {
                        TcpSocket *client = new TcpSocket();
                        if (listener.accept(*client) == Socket::Done) {
                            std::cout<<"client connected!"<<std::endl;
                            selector.add(*client);
                            clients.push_back(client);
                            Network::addUser(*client, udpSocket);
                        } else {
                            delete client;
                        }
                    }
                    for (it = clients.begin(); it != clients.end();it++) {
                        TcpSocket& client = **it;
                        if (selector.isReady(client)) {
                            bool pbKeyRsaSend = Network::hasPbKeyRsa(client);
                            bool pbKeySend = Network::hasPbKey(client);
                            User* user = Network::getUser(client);
                            if (pbKeyRsaSend == pbKeySend &&  user != nullptr &&
                                (!user->getRemotePortUDP() || !user->isUsingSecuredConnexion())) {
                                Packet packet;
                                if (client.receive(packet) == Socket::Done) {
                                    std::string request;
                                    packet>>request;
                                    if (request == "GetPbKeyRsa") {
                                        user->setUseSecuredConnexion(true);
                                        Network::sendPbKeyRsa(*user);
                                        pbKeyRsaSend = true;
                                    } else  if (request.find("updateUdpPort") != std::string::npos) {
                                        std::vector<std::string> parts = core::split(request, "*");
                                        user->setRemotePortUDP(core::conversionStringInt(parts[1]));
                                    } else {
                                        Network::addRequest (user, request);
                                    }
                                } else {
                                    Network::removeUser(client);
                                    selector.remove(client);
                                    it = clients.erase(it);
                                    delete *it;
                                    it--;
                                }
                            }
                            if (pbKeyRsaSend && !pbKeySend && user != nullptr && user->isUsingSecuredConnexion()) {
                                EncryptedPacket packet;
                                if (client.receive(packet) == Socket::Done) {
                                    std::string request;
                                    packet>>request;
                                    if (request == "GetPbKey") {
                                         Network::sendPbKey(*user);
                                         pbKeySend = true;
                                    } else {
                                        Network::removeUser(client);
                                        selector.remove(client);
                                        it = clients.erase(it);
                                        delete *it;
                                        it--;
                                    }
                                }
                            }
                            if (pbKeySend && pbKeyRsaSend && user != nullptr && user->getRemotePortUDP() && user->isUsingSecuredConnexion()) {
                                SymEncPacket packet;
                                if (client.receive(packet) == Socket::Done) {
                                    std::string request;
                                    packet>>request;
                                    Network::addRequest (user, request);
                                } else {
                                    Network::removeUser(client);
                                    selector.remove(client);
                                    it = clients.erase(it);
                                    delete *it;
                                    it--;
                                }
                            }
                        }
                    }
                    if (selector.isReady(udpSocket)) {
                        Packet packet;
                        string request;
                        IpAddress sender;
                        short unsigned int port;
                        if (udpSocket.receive(packet, sender, port) == Socket::Done) {
                            packet>>request;
                            User* user = Network::getUser(sender, port);
                            if (user != nullptr) {
                                std::vector<std::string> infos = core::split(request, "*");
                                if (infos[0] == "PONG") {
                                    user->addPingTime(user->getPingClock().getElapsedTime().asMicroseconds() * 0.5f);
                                } else if (infos[0] == "SET_TIME") {
                                    sf::Int64 cliTime = core::conversionStringLong(infos[1]);
                                    sf::Int64 srvTime = core::Application::getTimeClk().getElapsedTime().asMicroseconds();
                                    sf::Int64 syncTime = cliTime + (srvTime - user->getLastSrvTime()) * 0.5f;
                                    user->setClientTime(syncTime);
                                } else {
                                    Network::addRequest(user, request);
                                }
                            }
                        }
                    }
                }
            }
        }
        void SrkServer::removeClient(TcpSocket* client) {
            vector<TcpSocket*>::iterator it;
            for (it = clients.begin(); it != clients.end();) {
                if (*it == client)
                    it = clients.erase(it);
                else
                    it++;
            }
            delete client;
        }

        const TcpListener& SrkServer::getListner () const {
            return listener;
        }

        SrkServer::~SrkServer()
        {

            for (unsigned int i = 0; i < clients.size(); i++) {
                selector.remove(*clients[i]);
                delete clients[i];
            }
            listener.close();
            //dtor
        }
    }
}



