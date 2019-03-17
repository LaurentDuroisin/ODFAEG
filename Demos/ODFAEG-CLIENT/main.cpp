#include "application.h"
using namespace odfaeg::core;
using namespace odfaeg::graphic;
using namespace odfaeg::physic;
using namespace odfaeg::math;
using namespace odfaeg::network;
using namespace odfaeg::window;
using namespace sorrok;
using namespace std;
class Chien {
};
class Chat {
};
class Attaquer : public odfaeg::core::Visitor<> {
    public :
    void operator()(const Chat&, const Chat&, int i) const
    { std::cout <<  "griffe - griffe " <<i<<std::endl;}
    void operator()(const Chat&, const Chien&, int i) const
    { std::cout <<  "griffe - mord  " <<i<<std::endl;}
    void operator()(const Chien&, const Chat&, int i) const
    { std::cout <<  "mord - griffe " <<i<<std::endl;}
    void operator()(const Chien&, const Chien&, int i) const
    { std::cout << "mord - mord  " <<i<<std::endl;}
};
int main(int argv, char* argc[]) {
    /*Chien chien;
    Chat chat;
    std::vector<std::pair<odfaeg::core::Variant<Chien, Chat>,odfaeg::core::Variant<Chien, Chat>>> list;
    list.push_back(std::make_pair(chat, chat));
    list.push_back(std::make_pair(chat, chien));
    list.push_back(std::make_pair(chien, chat));
    list.push_back(std::make_pair(chien, chien));
    int nb = 10;
    for (unsigned int i = 0; i < list.size(); i++) {
      odfaeg::core::apply_nary_visitor(Attaquer(), list[i].first, list[i].second, nb);
    }
    return 0;*/
    /*bool b = true;
    bool b2;
    std::ostringstream oss;
    OTextArchive ota(oss);
    ota(b);
    std::istringstream iss;
    iss.str(oss.str());
    ITextArchive ita(iss);
    ita(b2);
    std::cout<<"b : "<<b2<<std::endl;
    return 0;*/

    EXPORT_CLASS_GUID(BoundingVolumeBoundingBox, BoundingVolume, BoundingBox)
    EXPORT_CLASS_GUID(EntityTile, Entity, Tile)
    EXPORT_CLASS_GUID(EntityTile, Entity, BigTile)
    EXPORT_CLASS_GUID(EntityWall, Entity, g2d::Wall)
    EXPORT_CLASS_GUID(EntityDecor, Entity, g2d::Decor)
    EXPORT_CLASS_GUID(EntityAnimation, Entity, Anim)
    EXPORT_CLASS_GUID(EntityHero, Entity, Hero)
    EXPORT_CLASS_GUID(EntityMonster, Entity, Monster)
    EXPORT_CLASS_GUID(EntityMesh, Entity, Mesh)
    MyAppli app(sf::VideoMode(800, 600, 32), "Test odfaeg");
    return app.exec();
    /*TcpSocket clientTCP;
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
    return 0;*/
    /*RenderWindow window(sf::VideoMode(800, 600), "test oit", sf::Style::Default, ContextSettings(0, 0, 8, 3, 0));
    //window.getView().move(400, 300, 300);
    std::string path = "tilesets/vlad_sword.png";
    Texture text;
    text.loadFromFile(path);
    int textRectX = 0, textRectY = 0, textRectWidth = 50, textRectHeight = 100;
    int textWidth = text.getSize().x;
    Anim* animation = new Anim(0.1f, Vec3f(-25, -50, 0), Vec3f(50, 100, 0), 0);
    for (unsigned int j = 0; j < 8; j++) {
        sf::IntRect textRect (textRectX, textRectY, textRectWidth, textRectHeight);
        Tile *tile = new Tile(&text, Vec3f(-25, -50, 0), Vec3f(50, 100, 0), textRect);
        tile->getFaces()[0]->getMaterial().setTexId("VLADSWORD");
        g2d::Decor *frame = new g2d::Decor(tile, &g2d::AmbientLight::getAmbientLight());
        frame->setShadowCenter(Vec3f(0, 200, 0));
        if (textRectX + textRectWidth > textWidth) {
            textRectX = 0;
            textRectY += textRectHeight;
        } else {
            textRectX += textRectWidth;
        }
        animation->addFrame(frame);
    }
    while(window.isOpen()) {
        IEvent event;
        while (window.pollEvent(event)) {
            if (event.type == IEvent::WINDOW_EVENT && event.window.type == IEvent::WINDOW_EVENT_CLOSED)
                window.close();
        }
        window.clear();
        window.draw(*animation->getCurrentFrame());
        animation->computeNextFrame();
        window.display();
    }
    delete animation;
    return 0;*/
}
