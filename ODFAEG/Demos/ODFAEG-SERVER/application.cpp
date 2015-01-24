#include "application.h"
using namespace odfaeg::core;
using namespace odfaeg::graphic;
using namespace odfaeg::math;
using namespace odfaeg::physic;
using namespace odfaeg::network;
MyAppli::MyAppli() : Application () {
    running = false;
    sf::Clock clock1, clock2;
    addClock(clock1, "Sync");
    addClock(clock2, "ElapsedTimeSinceLastSync");
}
void MyAppli::onLoad() {
    //shader.loadFromFile("Shaders/SimpleVertexShader.vertexshader", "Shaders/SimpleFragmentShader.fragmentshader");
}
void MyAppli::onInit () {
    theMap = new Map(nullptr, "Map test", 100, 50);
    World::addEntityManager(theMap);
    World::setCurrentEntityManager("Map test");
    tiles.push_back(new Tile(nullptr, Vec3f(0, 0, 0), Vec3f(120, 60, 0),sf::IntRect(0, 0, 100, 50)));
    walls.push_back(new Tile(nullptr, Vec3f(0, 0, 0), Vec3f(100, 100, 0), sf::IntRect(100, 0, 100, 100)));
    walls.push_back(new Tile(nullptr, Vec3f(0, 0, 0), Vec3f(100, 100, 0), sf::IntRect(100, 100, 100, 100)));
    walls.push_back(new Tile(nullptr, Vec3f(0, 0, 0), Vec3f(100, 100, 0), sf::IntRect(100, 200, 100, 100)));
    walls.push_back(new Tile(nullptr, Vec3f(0, 0, 0), Vec3f(100, 100, 0), sf::IntRect(100, 300, 100, 100)));
    walls.push_back(new Tile(nullptr, Vec3f(0, 0, 0), Vec3f(100, 100, 0), sf::IntRect(100, 400, 100, 100)));
    walls.push_back(new Tile(nullptr, Vec3f(0, 0, 0), Vec3f(100, 100, 0), sf::IntRect(100, 500, 100, 100)));
    tiles[0]->getFaces()[0]->getMaterial().setTexId("GRASS");
    walls[0]->getFaces()[0]->getMaterial().setTexId("WALLS");
    walls[1]->getFaces()[0]->getMaterial().setTexId("WALLS");
    walls[2]->getFaces()[0]->getMaterial().setTexId("WALLS");
    walls[3]->getFaces()[0]->getMaterial().setTexId("WALLS");
    walls[4]->getFaces()[0]->getMaterial().setTexId("WALLS");
    walls[5]->getFaces()[0]->getMaterial().setTexId("WALLS");
    BoundingBox mapZone(0, 0, 0, 1500, 1000, 0);
    World::generate_map(tiles, walls, Vec2f(100, 50), mapZone, false);
    w = new g2d::Wall(3, 80, walls[3],&g2d::AmbientLight::getAmbientLight(), Shadow::SHADOW_TYPE::SHADOW_TILE);
    w->setPosition(Vec3f(0, 130, 130 + w->getSize().y * 0.5f));
    World::addEntity(w);
    Tile* thouse = new Tile(nullptr, Vec3f(0, 0, 0), Vec3f(250, 300, 0), sf::IntRect(0, 0, 250, 300));
    thouse->getFaces()[0]->getMaterial().setTexId("HOUSE");
    g2d::Decor* decor = new g2d::Decor(thouse, &g2d::AmbientLight::getAmbientLight(), 300, Shadow::SHADOW_TYPE::SHADOW_TILE);
    decor->setPosition(Vec3f(-100, 250, 400));
    BoundingVolume *bb = new BoundingBox(decor->getGlobalBounds().getPosition().x, decor->getGlobalBounds().getPosition().y + decor->getGlobalBounds().getSize().y * 0.4f, 0,
    decor->getGlobalBounds().getSize().x, decor->getGlobalBounds().getSize().y * 0.25f, 0);
    decor->setCollisionVolume(bb);
    World::addEntity(decor);
    Anim* fire = new Anim(0.1f, Vec3f(0, 100, 150), Vec3f(100, 100, 0), 0);
    Tile* tf1 = new Tile(nullptr, Vec3f(0, 100, 150), Vec3f(100, 100, 0), sf::IntRect(0, 0, 150, 200));
    tf1->getFaces()[0]->getMaterial().setTexId("FIRE1");
    g2d::Decor *fire1 = new g2d::Decor(tf1, &g2d::AmbientLight::getAmbientLight(), 100, Shadow::SHADOW_TYPE::SHADOW_TILE);
    //decor->setShadowCenter(Vec2f(0, 60));
    //decor->changeGravityCenter(Vec3f(50, 50, 0));
    Tile* tf2 = new Tile(nullptr, Vec3f(0, 100, 150), Vec3f(100, 100, 0), sf::IntRect(0, 0, 150, 200));
    tf2->getFaces()[0]->getMaterial().setTexId("FIRE2");
    g2d::Decor *fire2 = new g2d::Decor(tf2, &g2d::AmbientLight::getAmbientLight(), 100, Shadow::SHADOW_TYPE::SHADOW_TILE);
    //decor->setShadowCenter(Vec2f(0, 60));
    //decor->changeGravityCenter(Vec3f(50, 50, 0));
    Tile* tf3 = new Tile(nullptr, Vec3f(0, 100, 150), Vec3f(100, 100, 0), sf::IntRect(0, 0, 150, 200));
    tf3->getFaces()[0]->getMaterial().setTexId("FIRE3");
    g2d::Decor *fire3 = new g2d::Decor(tf3, &g2d::AmbientLight::getAmbientLight(), 100, Shadow::SHADOW_TYPE::SHADOW_TILE);
    //decor->setShadowCenter(Vec2f(0, 60));
    //decor->changeGravityCenter(Vec3f(50, 50, 0));
    //fire1->setShadowCenter(Vec2f(80, 100));
    //fire2->setShadowCenter(Vec2f(80, 100));
    //fire3->setShadowCenter(Vec2f(80, 100));
    fire->addEntity(fire1);
    fire->addEntity(fire2);
    fire->addEntity(fire3);
    fire->play(true);
    World::addEntity(fire);
    //PonctualLight* light = new PonctualLight(Vec2f(50, 150),100,50,0,200,sf::Color(255,255,0),16,0);
    //World::addEntity(light);

    caracter = new Caracter("Sorrok", "Nagi", "M", "Map test", "Brain", "Green", "White","Normal","Novice", 1);
    BoundingVolume* bb2 = new BoundingBox(caracter->getGlobalBounds().getPosition().x, caracter->getGlobalBounds().getPosition().y + caracter->getGlobalBounds().getSize().y * 0.4f, 0,
    caracter->getGlobalBounds().getSize().x, caracter->getGlobalBounds().getSize().y * 0.25f, 0);
    caracter->setCollisionVolume(bb2);
    caracter->setCenter(Vec3f(0, 300, 300));
    g2d::PonctualLight* light1 = new g2d::PonctualLight(Vec3f(0, 420, 420), 100, 50, 50, 255, sf::Color::Yellow, 16);
    light2 = new g2d::PonctualLight(Vec3f(50, 160, 160), 100, 50, 50, 255, sf::Color::Yellow, 16);
    World::addEntity(light1);
    World::addEntity(light2);
    //getView().move(d.x * 0.5f, d.y * 0.5f, 0);
    World::addEntity(caracter);
    //World::computeIntersectionsWithWalls();
    BoundingPolyhedron monsterZone;
    std::array<Vec3f, 4> pts;
    pts[0] = Vec3f(100, 200, 0);
    pts[1] = Vec3f(300, 200, 0);
    pts[2] = Vec3f(100, 400, 0);
    pts[3] = Vec3f(300, 400, 0);
    for (unsigned int i = 0; i < pts.size(); i++) {
        monsterZone.addPoint(pts[i]);
    }
    Monster* monster = new Monster("Ogro", "Orc","MapTest",1,monsterZone);
    monster->respawn();
    World::addEntity(monster);
}
void MyAppli::onExec () {
    if (Network::hasRequest()) {
        User* user;
        std::string message = Network::getLastRequest(&user);
        std::vector<std::string> infos = split(message, "*");
        std::string request = infos[0];

        if (request == "GETCARINFOS") {
            std::ostringstream oss;
            OTextArchive oa(oss);
            Caracter* caracter = static_cast<Caracter*> (World::getEntities("E_CARACTER")[0]);
            oa(caracter);
            SymEncPacket packet;
            packet<<"CARINFOS"+oss.str();
            user->sendTcpPacket(packet);
            oss.str("");
            Monster* monster = static_cast<Monster*>(World::getEntities("E_MONSTER")[0]);
            oa(monster);
            packet.clear();
            packet<<"MONSTERSINFOS"+oss.str();
            user->sendTcpPacket(packet);
        } else if (request == "MOVEFROMKEYBOARD") {
            Caracter* caracter = static_cast<Caracter*> (World::getEntities("E_CARACTER")[0]);
            Vec3f dir (conversionStringFloat(infos[1]), conversionStringFloat(infos[2]), 0);
            caracter->setDir(Vec2f(dir.x, dir.y));
            caracter->setMoving(true);
            caracter->setIsMovingFromKeyboard(true);
        //Update the caracter states for a mouse move and update it's position from the transfer time.
        } else if (request == "MOVEFROMPATH") {
            Caracter* caracter = static_cast<Caracter*> (World::getEntities("E_CARACTER")[0]);
            Vec3f fPos (conversionStringInt(infos[1]), conversionStringInt(infos[2]), 0);
            std::vector<Vec2f> path = World::getPath(caracter, fPos);
            caracter->setPath(path);
            caracter->setMoving(true);
            caracter->setIsMovingFromKeyboard(false);
            SymEncPacket packet;
            std::string response="";
            int size = path.size();
            response += "NEWPATH"+conversionIntString(size)+"*";
            for (int i = 0; i < size; i++) {
                response += conversionFloatString(path[i].x)+"*"+conversionFloatString(path[i].y);
                if (i != size - 1)
                    response += "*";
            }
            packet<<response;
            Network::sendTcpPacket(packet);
        //Update the path followed by the caracter. (For mouse movements)
        } else if (request == "GETCARPOS") {
            Caracter* caracter = static_cast<Caracter*> (World::getEntities("E_CARACTER")[0]);
            if (caracter->isMoving()) {
                if (caracter->isMovingFromKeyboard()) {
                    Vec3f actualPos = Vec3f(caracter->getCenter().x, caracter->getCenter().y, 0);
                    sf::Int64 transferTime = user->getPingAvg();
                    Vec3f newPos = actualPos + Vec3f(caracter->getDir().x, caracter->getDir().y, 0) * caracter->getSpeed() *  transferTime;
                    caracter->setCenter(Vec3f(newPos.x, newPos.y, caracter->getPosition().z));
                    if (World::collide(caracter)) {
                        newPos = actualPos;
                    }
                    caracter->setCenter(Vec3f(actualPos.x, actualPos.y, caracter->getPosition().z));
                    sf::Packet packet;
                    std::string response = "NEWPOS"+conversionLongString(transferTime)+"*"+conversionFloatString(newPos.x)+"*"+conversionFloatString(newPos.y);
                    packet<<response;
                    user->sendUdpPacket(packet);
                } else {
                    sf::Int64 transferTime = user->getPingAvg();
                    Vec3f actualPos = Vec3f(caracter->getCenter().x, caracter->getCenter().y, 0);
                    Vec3f newPos = Computer::getPosOnPathFromTime(actualPos, caracter->getPath(), transferTime, caracter->getSpeed());
                    if (newPos.computeDist(caracter->getPath()[caracter->getPath().size() - 1]) <= 1) {
                        newPos = caracter->getPath()[caracter->getPath().size() - 1];
                    }

                    sf::Packet packet;
                    std::string response = "NEWPOS"+conversionLongString(transferTime)+"*"+conversionFloatString(newPos.x)+"*"+conversionFloatString(newPos.y);
                    packet<<response;
                    user->sendUdpPacket(packet);
                }
            } else {
                sf::Int64 transferTime = user->getPingAvg();
                Vec3f newPos = caracter->getCenter();
                std::string response = "NEWPOS"+conversionLongString(transferTime)+"*"+conversionFloatString(newPos.x)+"*"+conversionFloatString(newPos.y);
                sf::Packet packet;
                packet<<response;
                user->sendUdpPacket(packet);
            }
        } else if (request == "STOPCARMOVE") {
            Caracter* caracter = static_cast<Caracter*> (World::getEntities("E_CARACTER")[0]);
            if (caracter->isMoving()) {
                sf::Int64 elapsed_srv_time = getClock("ElapsedTimeSinceLastSync").getElapsedTime().asMicroseconds();
                sf::Int64 elapsed_cli_time = conversionStringLong(infos[1]) - sync_time;
                sf::Int64 delta_time = (elapsed_srv_time - elapsed_cli_time > 0) ? elapsed_srv_time - elapsed_cli_time : 0;
                Vec3f newPos = caracter->getCenter();
                if (caracter->isMovingFromKeyboard()) {
                    Caracter* caracter = static_cast<Caracter*> (World::getEntities("E_CARACTER")[0]);
                    caracter->setMoving(false);
                    newPos = caracter->getCenter() - Vec3f(caracter->getDir().x, caracter->getDir().y, 0) * delta_time * caracter->getSpeed();
                    caracter->setCenter(newPos);
                }
                std::string response = "NEWPOS"+conversionFloatString(newPos.x)+"*"+conversionFloatString(newPos.y);
                SymEncPacket packet;
                packet<<response;
                user->sendTcpPacket(packet);
            }
        } else if (request == "SET_TIME") {
            sf::Int64 cli_time = conversionStringLong(infos[1]);
            sf::Int64 srv_time = getClock("TimeClock").getElapsedTime().asMicroseconds();
            sync_time = cli_time + (srv_time - last_srv_time) * 0.5f;
            getClock("ElapsedTimeSinceLastSync").restart();
        }
    }
    if (getClock("LoopTime").getElapsedTime().asMilliseconds() < 100) {
        std::chrono::milliseconds dura(100 - getClock("LoopTime").getElapsedTime().asMilliseconds());
        std::this_thread::sleep_for(dura);
    }
    Caracter* caracter = static_cast<Caracter*> (World::getEntities("E_CARACTER")[0]);
    if (caracter->isMoving()) {
        if (caracter->isMovingFromKeyboard()) {
            sf::Int64 elapsedTime = getClock("LoopTime").getElapsedTime().asMicroseconds();
            Vec3f actualPos = Vec3f(caracter->getCenter().x, caracter->getCenter().y, 0);
            Vec3f newPos = actualPos + Vec3f(caracter->getDir().x, caracter->getDir().y, 0) * caracter->getSpeed() * elapsedTime;
            caracter->setCenter(Vec3f(newPos.x, newPos.y, caracter->getPosition().z));
            if (World::collide(caracter)) {
                newPos = actualPos;
            }
            caracter->setCenter(Vec3f(actualPos.x, actualPos.y, caracter->getPosition().z));
            Vec3f d = newPos - actualPos;
            Vec3f dir = d.normalize();
            if (Vec2f(dir.x, dir.y) != caracter->getDir())
                caracter->setDir(Vec2f(dir.x, dir.y));
            World::moveEntity(caracter, d.x, d.y, d.y);
        } else {
            sf::Int64 elapsedTime = getClock("LoopTime").getElapsedTime().asMicroseconds();
            Vec2f actualPos = caracter->getCenter();
            Vec2f newPos = Computer::getPosOnPathFromTime(actualPos, caracter->getPath(), elapsedTime, caracter->getSpeed());
            if (newPos.computeDist(caracter->getPath()[caracter->getPath().size() - 1]) <= 1) {
                caracter->setMoving(false);
                newPos = caracter->getPath()[caracter->getPath().size() - 1];

            }
            Vec2f d = newPos - actualPos;
            Vec2f dir = d.normalize();
            if (dir != caracter->getDir())
                caracter->setDir(dir);
            World::moveEntity(caracter, d.x, d.y, d.y);
        }
    }
    if (getClock("Sync").getElapsedTime().asSeconds() >= timeBtwnTwoSync.asSeconds()) {
        getClock("Sync").restart();
        SymEncPacket packet;
        std::string message = "GET_TIME";
        packet<<message;
        last_srv_time = getClock("TimeClock").getElapsedTime().asMicroseconds();
        Network::sendTcpPacket(packet);
    }
}
