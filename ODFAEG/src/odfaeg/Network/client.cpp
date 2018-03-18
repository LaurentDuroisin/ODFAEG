#include "../../../include/odfaeg/Network/client.h"
#include "../../../include/odfaeg/Network/network.h"
#include "../../../include/odfaeg/Network/cliEncryptedPacket.hpp"
#include "../../../include/odfaeg/Core/application.h"
namespace odfaeg {
    namespace network {
        using namespace sf;
        using namespace std;
        SrkClient::SrkClient () {
            running = false;
            remotePortUDP = 0;
        }
        void SrkClient::getPublicKey () {
            Packet packet;
            std::string message;
           /* unsigned char* cliPbKey = nullptr;
            int size = CliEncryptedPacket::getCertificate(&cliPbKey);
            std::string cert (reinterpret_cast<char*>(cliPbKey), size);
            std::string message = "SETCLIPBKEY"+cert;*/
            message = "GetPbKeyRsa";
            packet<<message;
            clientTCP.send(packet);
            packet.clear();
            bool done, pbKeyRsaReceived, certifiate;
            done = pbKeyRsaReceived = certifiate = false;
            EncryptedPacket enc_packet;

            while (!done || !pbKeyRsaReceived) {
                /*if (!certifiate && !pbKeyRsaReceived && !done) {
                    CliEncryptedPacket cliEncryptedPacket;
                    if (clientTCP.receive(cliEncryptedPacket)) {
                        cliEncryptedPacket>>message;
                        cliEncryptedPacket.clear();
                        if (message == "GETCERTIFIATECLIENT") {
                            message == Network::getCertifiateClientMess();
                            cliEncryptedPacket<<message;
                            clientTCP.send(cliEncryptedPacket);
                        }
                        if (message == "CERTIFIEDCLIENT") {
                            message = "GetPbKeyRsa";
                            packet<<message;
                            clientTCP.send(packet);
                            certifiate = true;
                        }
                    }
                }*/
                if (/*certifiate &&*/ !pbKeyRsaReceived && !done) {
                    if (clientTCP.receive(packet) == Socket::Done) {
                        packet>>message;
                        if (message.find("RSAKEY") != std::string::npos) {
                            message.erase(0, 6);
                            Network::setPbKey(message);
                            pbKeyRsaReceived = true;
                            packet.clear();
                            message = "GetPbKey";
                            enc_packet<<message;
                            clientTCP.send(enc_packet);
                            enc_packet.clear();
                        }
                    }
                }
                if (/*certifiate &&*/ pbKeyRsaReceived && !done) {
                    if (clientTCP.receive(enc_packet) == Socket::Done) {
                        string message;
                        enc_packet>>message;
                        if (message.find("AESKEY")!= std::string::npos) {
                            message.erase(0, 6);
                            Network::setSymPbKey(message);
                            done = true;
                            enc_packet.clear();
                        }
                    }
                }
            }
        }
        bool SrkClient::startCli(int portTCP, int portUDP, IpAddress address, bool useSecuredConnexion) {
             if (!running) {
                remotePortUDP = portUDP;
                this->srvAddress = address;
                this->useSecuredConnexion = useSecuredConnexion;
                if(clientTCP.connect(address, portTCP) != Socket::Done) {
                    cout<<"Erreur : impossible de se connecter au serveur!"<<endl;
                    return false;
                }
                if (clientUDP.bind(Socket::AnyPort) != Socket::Done) {
                    cout<<"Erreur : impossible d'écouter sur le port : "<<portUDP<<endl;
                    return false;
                }
                Packet packet;
                std::string message = "updateUdpPort*" + core::conversionIntString(clientUDP.getLocalPort());
                packet<<message;
                clientTCP.send(packet);
                if (useSecuredConnexion)
                    getPublicKey();
                selector.add(clientTCP);
                selector.add(clientUDP);
                running = true;
                std::cout<<"Client started!"<<std::endl;
                return true;
            } else {
                cout<<"Client already started!"<<endl;
                return false;
            }
        }
        void SrkClient::stopCli() {
            if(running) {
                running = false;
            } else {
                cout<<"Client already stopped"<<endl;
            }
        }
        void SrkClient::sendTCPPacket(Packet &packet) {
            //if (useThread)
                //lock_guard<recursive_mutex> locker(rec_mutex);
            if(clientTCP.send(packet) != Socket::Done)
                cout<<"Erreur!"<<endl;
        }
        void SrkClient::sendUDPPacket(Packet &packet) {
            //if (useThread)
                //lock_guard<recursive_mutex> locker(rec_mutex);
            if(clientUDP.send(packet, srvAddress, remotePortUDP) != Socket::Done)
                cout<<"Erreur!"<<endl;
        }
        bool SrkClient::isRunning () {
            return running;
        }
        void SrkClient::checkMessages() {
            if (running) {
                short unsigned int port;
                IpAddress address;
                if (selector.wait(milliseconds(10))) {
                     if (selector.isReady(clientTCP)) {
                        if (useSecuredConnexion) {
                            SymEncPacket packet;
                            if(clientTCP.receive(packet) == Socket::Done) {
                                string message;
                                packet>>message;
                                Network::addResponse(message);

                            }
                        } else {
                            Packet packet;
                            if(clientTCP.receive(packet) == Socket::Done) {

                                string message;
                                packet>>message;
                                Network::addResponse(message);

                            }
                        }
                    }

                    if (selector.isReady(clientUDP)) {
                        Packet packet;
                        if (clientUDP.receive(packet, address, port) == Socket::Done) {
                            if (address == srvAddress) {
                                string message;
                                packet>>message;
                                if (message == "PING") {
                                    packet.clear();
                                    message = "PONG";
                                    packet<<message;
                                    Network::sendUdpPacket(packet);
                                } else if (message == "GET_TIME") {
                                    packet.clear();
                                    sf::Int64 time = core::Application::getTimeClk().getElapsedTime().asMicroseconds();
                                    message = "SET_TIME*"+core::conversionLongString(time);
                                    packet<<message;
                                    Network::sendUdpPacket(packet);
                                } else {
                                    Network::addResponse(message);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
