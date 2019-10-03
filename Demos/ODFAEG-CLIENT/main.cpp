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
    /*Window window(sf::VideoMode(800, 600), "test oit", sf::Style::Default, ContextSettings(24, 8, 4, 3, 0));
     GLuint maxNodes = 20 * 800 * 600;
    GLint nodeSize = 5 * sizeof(GLfloat) + sizeof(GLuint);
    GLuint atomicBuffer, linkedListBuffer, headPtrTex, clearBuf;
    glGenBuffers(1, &atomicBuffer);
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, atomicBuffer);
    glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
    glGenBuffers(1, &linkedListBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, linkedListBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, maxNodes * nodeSize, NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    glGenTextures(1, &headPtrTex);
    glBindTexture(GL_TEXTURE_2D, headPtrTex);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32UI, 800, 600);
    glBindImageTexture(0, headPtrTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
    glBindTexture(GL_TEXTURE_2D, 0);
    std::vector<GLuint> headPtrClearBuf(800*600, 0);
    glGenBuffers(1, &clearBuf);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, clearBuf);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, headPtrClearBuf.size() * sizeof(GLuint),
            &headPtrClearBuf[0], GL_STATIC_COPY);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);*/
    return 0;
}
