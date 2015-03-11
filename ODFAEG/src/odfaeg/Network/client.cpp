#include "../../../include/odfaeg/Network/client.h"
#include "../../../include/odfaeg/Network/network.h"
#include "../../../include/odfaeg/Core/application.h"
namespace odfaeg {
    namespace network {
        using namespace sf;
        using namespace std;
        SrkClient::SrkClient () {
            running = false;
            useThread = false;
            remotePortUDP = 0;
        }
        bool SrkClient::isUsingThread() {
            return useThread;
        }
        void SrkClient::getPublicKey () {
            Packet packet;
            string message = "GetPbKeyRsa";
            packet<<message;
            clientTCP.send(packet);
            packet.clear();
            bool done, pbKeyRsaReceived;
            done = pbKeyRsaReceived = false;
            EncryptedPacket enc_packet;

            while (!done || !pbKeyRsaReceived) {

                if (!pbKeyRsaReceived && !done) {
                    if (clientTCP.receive(packet) == Socket::Done) {
                        packet>>message;
                        Network::setPbKey(message);
                        pbKeyRsaReceived = true;
                        packet.clear();
                        message = "GetPbKey";
                        enc_packet<<message;
                        clientTCP.send(enc_packet);
                        enc_packet.clear();
                    }
                }
                if (pbKeyRsaReceived && !done) {
                    if (clientTCP.receive(enc_packet) == Socket::Done) {
                        string message;
                        enc_packet>>message;
                        Network::setSymPbKey(message);
                        done = true;
                        enc_packet.clear();
                    }
                }
            }
        }
        bool SrkClient::startCli(int portTCP, int portUDP, IpAddress address, bool useThread, bool useSecuredConnexion) {
             if (!running) {
                remotePortUDP = portUDP;
                this->useThread = useThread;
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
                if (useSecuredConnexion)
                    getPublicKey();
                Packet packet;
                std::string message = "updateUdpPort*" + core::conversionIntString(clientUDP.getLocalPort());
                packet<<message;
                clientTCP.send(packet);
                selector.add(clientTCP);
                selector.add(clientUDP);
                running = true;
                if (useThread)
                    m_thread = thread (&SrkClient::run, this);
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
                if (useThread)
                    m_thread.join();
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
        void SrkClient::run() {
            short unsigned int port;
            IpAddress address;
            while (running) {
                if (selector.wait(milliseconds(100))) {
                    lock_guard<recursive_mutex> locker(rec_mutex);
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
                                    packet<<"PONG";
                                    Network::sendUdpPacket(packet);
                                } else if (message == "GET_TIME") {
                                    std::cout<<"GET TIME"<<std::endl;
                                    packet.clear();
                                    packet<<"SET_TIME*"+core::conversionLongString(core::Application::getTimeClk().getElapsedTime().asMicroseconds());
                                    Network::sendUdpPacket(packet);
                                } else {
                                    std::cout<<"message : "<<message<<std::endl;
                                    Network::addResponse(message);
                                }
                            } else {
                                cout<<"This message don't provide from the server."<<endl;
                            }

                        }
                    }
                }
            }
        }
    }
}
