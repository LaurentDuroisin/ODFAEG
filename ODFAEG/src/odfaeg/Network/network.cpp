#include "../../../include/odfaeg/Network/network.h"
#include "../../../include/odfaeg/Core/application.h"
namespace odfaeg {
    namespace network {
        using namespace sf;
        using namespace std;
        SrkClient& Network::cli = Network::getCliInstance();
        SrkServer& Network::srv = Network::getSrvInstance();
        Int64 Network::timeBtw2Pings = sf::seconds(1.f).asMicroseconds();
        Int64 Network::timeBtw2Sync = sf::seconds(1.f).asMicroseconds();
        Clock Network::timeBtw2SyncClk = Clock();
        Clock Network::timeBtw2PingsClk = sf::Clock();
        Clock Network::timeoutClk = Clock();
        std::vector<std::pair<User*, std::string>> Network::requests = std::vector<std::pair<User*, std::string>>();
        std::vector<User*> Network::users = std::vector<User*>();
        vector<string> Network::responses = vector<string>();
        bool Network::startCli (int portTCP, int portUDP, IpAddress address, bool useThread, bool useSecuredConnexion) {

            if (srv.isRunning()) {
                return false;
            }
            return cli.startCli(portTCP, portUDP, address, useThread, useSecuredConnexion);
        }
        bool Network::startSrv (int portTCP, int portUDP, bool useThread) {
            if (cli.isRunning()) {
                return false;
            }
            return srv.startSrv(portTCP, portUDP, useThread);
        }
        void Network::addRequest(User* user, std::string request) {
            std::pair<User*, std::string> userreq(user, request);
            requests.push_back(userreq);
        }

        void Network::stopCli () {
            cli.stopCli ();
        }
        void Network::addResponse(string response) {
            responses.push_back(response);
        }
        void Network::sendTcpPacket (Packet &packet) {
            if(cli.isRunning()) {
               if (cli.isUsingThread()) {
                   std::lock_guard<std::recursive_mutex> locker(rec_mutex);
                   cli.sendTCPPacket(packet);
               } else {
                   cli.sendTCPPacket(packet);
               }
            } else if (srv.isRunning()) {
               for (unsigned int i = 0; i < users.size(); i++) {
                    users[i]->sendTcpPacket(packet);
               }
            }
        }
        void Network::sendUdpPacket(Packet &packet) {
           if(cli.isRunning()) {
               if (cli.isUsingThread()) {
                   std::lock_guard<std::recursive_mutex> locker(rec_mutex);
                   cli.sendUDPPacket(packet);
               } else {
                   cli.sendUDPPacket(packet);
               }
           } else if (srv.isRunning()) {
               for (unsigned int i = 0; i < users.size(); i++) {
                    users[i]->sendUdpPacket(packet);
               }
           }
        }
        bool Network::getResponse(string tag, string &response) {
            if (cli.isUsingThread()) {
                std::lock_guard<std::recursive_mutex> locker(rec_mutex);
                vector<string>::iterator it;
                for (it = responses.begin(); it != responses.end();it++) {
                    if (it->find(tag) != string::npos) {
                        response = it->substr(tag.length(), it->length() - tag.length());
                        it = responses.erase(it);
                        return true;
                    }
                }
            } else {
                vector<string>::iterator it;
                for (it = responses.begin(); it != responses.end();it++) {
                    if (it->find(tag) != string::npos) {
                        response = it->substr(tag.length(), it->length() - tag.length());
                        it = responses.erase(it);
                        return true;
                    }
                }
            }
            return false;
        }
        std::string Network::getLastRequest(User** user, sf::Time timeOut) {
            std::string request="";
            if (srv.isUsingThread()) {
                std::lock_guard<std::recursive_mutex> locker(rec_mutex);
                timeoutClk.restart();
                std::vector<std::pair<User*, std::string>>::iterator it;
                while (request == "" && timeoutClk.getElapsedTime().asSeconds() < timeOut.asSeconds()) {
                    if (requests.size() > 0) {
                        it = requests.begin();
                        request = it->second;
                        *user = it->first;
                        it = requests.erase(it);
                    }
                }
            } else {
                timeoutClk.restart();
                std::vector<std::pair<User*, std::string>>::iterator it;
                while (request == "" && timeoutClk.getElapsedTime().asSeconds() < timeOut.asSeconds()) {
                    srv.checkMessages();
                    if (requests.size() > 0) {
                        it = requests.begin();
                        request = it->second;
                        *user = it->first;
                        it = requests.erase(it);
                    }
                }
            }
            return request;
        }
        bool Network::hasPbKey(TcpSocket& socket) {
            for (unsigned int i = 0; i < users.size(); i++) {
                if (&users[i]->getTcpSocket() == &socket) {
                    return users[i]->hPbKey();
                }
            }
            return false;
        }
        bool Network::hasPbKeyRsa(TcpSocket& socket) {
            for (unsigned int i = 0; i < users.size(); i++) {
                if (&users[i]->getTcpSocket() == &socket) {
                    return users[i]->hPbKeyRsa();
                }
            }
            return false;
        }
        bool Network::hasResponse() {
            return responses.size() != 0;
        }
        bool Network::hasRequest() {
            return requests.size() != 0;
        }
        string Network::getLastResponse (sf::Time timeOut) {
            string response = "";
            if (cli.isUsingThread()) {
                std::lock_guard<std::recursive_mutex> locker(rec_mutex);
                timeoutClk.restart();
                vector<string>::iterator it;

                while (response == "" && timeoutClk.getElapsedTime().asSeconds() < timeOut.asSeconds()) {
                    if (responses.size() > 0) {
                        it = responses.begin();
                        response = *it;
                        it = responses.erase(it);
                    }

                }
            } else {
                timeoutClk.restart();
                vector<string>::iterator it;
                while (response == "" && timeoutClk.getElapsedTime().asSeconds() < timeOut.asSeconds()) {
                    cli.checkMessages();
                    if (responses.size() > 0) {
                        it = responses.begin();
                        response = *it;
                        it = responses.erase(it);
                    }
                }
            }
            return response;
        }

        void Network::addUser(TcpSocket& tcpSocket, UdpSocket& udpSocket) {
            if (srv.isUsingThread()) {
                std::lock_guard<std::recursive_mutex> locker(rec_mutex);
                User* user = new User(tcpSocket, udpSocket);
                users.push_back(user);
            } else {
                User* user = new User(tcpSocket, udpSocket);
                users.push_back(user);
            }
        }
        void Network::removeUser(sf::TcpSocket& socket) {
            if (srv.isUsingThread()) {
                std::lock_guard<std::recursive_mutex> locker(rec_mutex);
                std::vector<User*>::iterator it;
                for (it = users.begin(); it != users.end();) {
                    if (&(*it)->getTcpSocket() == &socket) {
                        if (core::Application::app != nullptr)
                            core::Application::app->onDisconnected(*it);
                        it = users.erase(it);
                    } else
                        it++;
                }
            } else {
                std::vector<User*>::iterator it;
                for (it = users.begin(); it != users.end();) {
                    if (&(*it)->getTcpSocket() == &socket) {
                        if (core::Application::app != nullptr)
                            core::Application::app->onDisconnected(*it);
                        it = users.erase(it);
                    } else
                        it++;
                }
            }
        }
        void Network::sendPbKeyRsa(User &user) {
            if (srv.isUsingThread()) {
                std::lock_guard<std::recursive_mutex> locker(rec_mutex);
                unsigned char* out = nullptr;
                int length = EncryptedPacket::getCertificate(&out);
                string response ((char*) out, length);
                Packet packet;
                packet<<response;
                user.getTcpSocket().send(packet);
                user.setHasPbKeyRsa(true);
            } else {
                unsigned char* out = nullptr;
                int length = EncryptedPacket::getCertificate(&out);
                string response ((char*) out, length);
                Packet packet;
                packet<<response;
                user.getTcpSocket().send(packet);
                user.setHasPbKeyRsa(true);
            }
        }
        void Network::sendPbKey(User &user) {
            if (srv.isUsingThread()) {
                std::lock_guard<std::recursive_mutex> locker(rec_mutex);
                string response = string(SymEncPacket::getKey()) + "-" + string(SymEncPacket::getIv());
                EncryptedPacket packet;
                packet<<response;
                user.getTcpSocket().send(packet);
                user.setHasPbKey(true);
            } else {
                string response = string(SymEncPacket::getKey()) + "-" + string(SymEncPacket::getIv());
                EncryptedPacket packet;
                packet<<response;
                user.getTcpSocket().send(packet);
                user.setHasPbKey(true);
            }
        }
        User* Network::getUser(sf::TcpSocket& socket) {
            if (srv.isUsingThread()) {
                std::lock_guard<std::recursive_mutex> locker(rec_mutex);
                for (unsigned int i = 0; i < users.size(); i++) {
                    if (&users[i]->getTcpSocket() == &socket) {
                        return users[i];
                    }
                }
            } else {
                for (unsigned int i = 0; i < users.size(); i++) {
                    if (&users[i]->getTcpSocket() == &socket) {
                        return users[i];
                    }
                }
            }
            return nullptr;
        }
        User* Network::getUser(IpAddress address, short unsigned int remoteUDPPort) {
            if (srv.isUsingThread()) {
                std::lock_guard<std::recursive_mutex> locker(rec_mutex);
                for (unsigned int i = 0; i < users.size(); i++) {
                    if (users[i]->getIpAddress() == address && users[i]->getRemotePortUDP() == remoteUDPPort) {
                        return users[i];
                    }
                }
            } else {
                for (unsigned int i = 0; i < users.size(); i++) {
                    if (users[i]->getIpAddress() == address && users[i]->getRemotePortUDP() == remoteUDPPort) {
                        return users[i];
                    }
                }
            }
            return nullptr;
        }
        string Network::waitForLastResponse(string tag, sf::Time timeOut) {
            string response = "";
            if (cli.isUsingThread()) {
                std::lock_guard<std::recursive_mutex> locker(rec_mutex);
                timeoutClk.restart();
                vector<string>::iterator it;
                while (response == "" && timeoutClk.getElapsedTime().asSeconds() <= timeOut.asSeconds()) {
                    if (responses.size() > 0) {
                            if(tag == "") {
                                it = responses.begin();
                                response = *it;
                                it = responses.erase(it);
                                return response;
                            } else if (getResponse(tag, response)) {
                                return response;
                            }
                    }
                }
            } else {
                timeoutClk.restart();
                vector<string>::iterator it;
                while (response == "" && timeoutClk.getElapsedTime().asSeconds() <= timeOut.asSeconds()) {
                    cli.checkMessages();
                    if (responses.size() > 0) {
                            if(tag == "") {
                                it = responses.begin();
                                response = *it;
                                it = responses.erase(it);
                                return response;
                            } else if (getResponse(tag, response)) {
                                return response;
                            }
                    }
                }
            }
            return response;
        }
        void Network::setPbKey (string message) {
            if (cli.isUsingThread()) {
                std::lock_guard<std::recursive_mutex> locker(rec_mutex);
                EncryptedPacket::setCertificate(reinterpret_cast<const unsigned char*>(message.c_str()), message.length());
            } else {
                EncryptedPacket::setCertificate(reinterpret_cast<const unsigned char*>(message.c_str()), message.length());
            }
        }
        void Network::setSymPbKey (string pbKey) {
            if (cli.isUsingThread()) {
                std::lock_guard<std::recursive_mutex> locker(rec_mutex);
                vector<string> parts = core::split(pbKey, "-");
                SymEncPacket::setKey(const_cast<char*>(parts[0].c_str()));
                SymEncPacket::setIv(const_cast<char*>(parts[1].c_str()));
            } else {
                vector<string> parts = core::split(pbKey, "-");
                SymEncPacket::setKey(const_cast<char*>(parts[0].c_str()));
                SymEncPacket::setIv(const_cast<char*>(parts[1].c_str()));
            }
        }
    }
}
