#include "application.h"
using namespace odfaeg::core;
using namespace odfaeg::graphic;
using namespace odfaeg::physic;
using namespace odfaeg::math;
using namespace odfaeg::network;
MyAppli::MyAppli(sf::VideoMode wm, std::string title) : Application (wm, title, false, true, sf::Style::Default, sf::ContextSettings(0, 0, 4, 3, 0)) {
    running = false;
    actualKey = sf::Keyboard::Key::Unknown;
    previousKey = sf::Keyboard::Key::Unknown;
    getView().setScale(1, -1, 1);
    sf::Clock clock1, clock2;
    addClock(clock1, "RequestTime");
    addClock(clock2, "TransferTime");
    caracter = nullptr;
    monster = nullptr;
}
void MyAppli::keyHeldDown (sf::Keyboard::Key key) {
    //BoundingRectangle rect (pos.x, pos.y, getView().getSize().x, getView().getSize().y);
    if (actualKey != sf::Keyboard::Key::Unknown && key == sf::Keyboard::Key::Z) {
        if (!caracter->isMoving()) {
            if (actualKey != previousKey) {
                    Vec2f dir(0, -1);
                    caracter->setDir(dir);
            }
            Vec2f dir = caracter->getDir();
            std::string message = "MOVEFROMKEYBOARD*"+conversionFloatString(dir.x)+"*"+conversionFloatString(dir.y);
            SymEncPacket packet;
            packet<<message;
            Network::sendTcpPacket(packet);
            message = "GETCARPOS";
            packet.clear();
            packet<<message;
            getClock("TransferTime").restart();
            getClock("RequestTime").restart();
            Network::sendTcpPacket(packet);
            caracter->setMoving(true);
            caracter->setIsMovingFromKeyboard(true);
        }
    } else if (actualKey != sf::Keyboard::Key::Unknown && key == sf::Keyboard::Key::Q) {
        if (!caracter->isMoving()) {
            if (actualKey != previousKey) {
                Vec2f dir(-1, 0);
                caracter->setDir(dir);
            }
            Vec2f dir = caracter->getDir();
            std::string message = "MOVEFROMKEYBOARD*"+conversionFloatString(dir.x)+"*"+conversionFloatString(dir.y);
            SymEncPacket packet;
            packet<<message;
            Network::sendTcpPacket(packet);
            message = "GETCARPOS";
            packet.clear();
            packet<<message;
            getClock("TransferTime").restart();
            getClock("RequestTime").restart();
            Network::sendTcpPacket(packet);
            caracter->setMoving(true);
            caracter->setIsMovingFromKeyboard(true);
        }
    } else if (actualKey != sf::Keyboard::Key::Unknown && actualKey == sf::Keyboard::Key::S) {
        if (!caracter->isMoving()) {
            if (actualKey != previousKey) {
                Vec2f dir(0, 1);
                caracter->setDir(dir);
            }
            Vec2f dir = caracter->getDir();
            std::string message = "MOVEFROMKEYBOARD*"+conversionFloatString(dir.x)+"*"+conversionFloatString(dir.y);
            SymEncPacket packet;
            packet<<message;
            Network::sendTcpPacket(packet);
            message = "GETCARPOS";
            packet.clear();
            packet<<message;
            getClock("TransferTime").restart();
            getClock("RequestTime").restart();
            Network::sendTcpPacket(packet);
            caracter->setMoving(true);
            caracter->setIsMovingFromKeyboard(true);
        }
    } else if (actualKey != sf::Keyboard::Key::Unknown && key == sf::Keyboard::Key::D) {
        if (!caracter->isMoving()) {
            if (actualKey != previousKey) {
                Vec2f dir(1, 0);
                caracter->setDir(dir);
            }
            Vec2f dir = caracter->getDir();
            std::string message = "MOVEFROMKEYBOARD*"+conversionFloatString(dir.x)+"*"+conversionFloatString(dir.y);
            SymEncPacket packet;
            packet<<message;
            Network::sendTcpPacket(packet);
            message = "GETCARPOS";
            packet.clear();
            packet<<message;
            getClock("TransferTime").restart();
            getClock("RequestTime").restart();
            Network::sendTcpPacket(packet);
            caracter->setIsMovingFromKeyboard(true);
            caracter->setMoving(true);
        }
    }
}
void MyAppli::leftMouseButtonPressed(sf::Vector2f mousePos) {
    Vec3f finalPos(mousePos.x, getRenderWindow().getSize().y - mousePos.y, 0);
    finalPos = getRenderWindow().mapPixelToCoords(finalPos);
    finalPos = Vec3f(finalPos.x, finalPos.y, 0);
    /*std::vector<Vec2f> path = World::getPath(caracter, finalPos);
    if (path.size() > 0) {
        caracter->setPath(path);
        caracter->setIsMovingFromKeyboard(false);
        caracter->setMoving(true);
    }*/
    std::string message = "MOVEFROMPATH*"+conversionFloatString(finalPos.x)+"*"+conversionFloatString(finalPos.y);
    SymEncPacket packet;
    packet<<message;
    Network::sendTcpPacket(packet);
    message = "GETCARPOS";
    packet.clear();
    packet<<message;
    getClock("TransferTime").restart();
    getClock("RequestTime").restart();
    Network::sendTcpPacket(packet);
}
bool MyAppli::mouseInside (sf::Vector2f mousePos) {
    BoundingBox bx (0, 0, 0, 100, 100, 0);
    if (bx.isPointInside(Vec3f(mousePos.x, mousePos.y, 0))) {
        return true;
    }
    return false;
}
void MyAppli::onMouseInside (sf::Vector2f mousePos) {
    std::cout<<"Mouse inside : "<<mousePos.x<<" "<<mousePos.y<<std::endl;
}
void MyAppli::onLoad() {
    TextureManager<> tm;
    tm.fromFileWithAlias("tilesets/herbe.png", "GRASS");
    tm.fromFileWithAlias("tilesets/murs.png", "WALLS");
    tm.fromFileWithAlias("tilesets/maison.png", "HOUSE");
    tm.fromFileWithAlias("tilesets/flemmes1.png", "FIRE1");
    tm.fromFileWithAlias("tilesets/flemmes2.png", "FIRE2");
    tm.fromFileWithAlias("tilesets/flemmes3.png", "FIRE3");
    cache.addResourceManager(tm, "TextureManager");
    //shader.loadFromFile("Shaders/SimpleVertexShader.vertexshader", "Shaders/SimpleFragmentShader.fragmentshader");
}
void MyAppli::onInit () {
    TextureManager<> &tm = cache.resourceManager<Texture, std::string>("TextureManager");
    Vec2f pos (getView().getPosition().x - getView().getSize().x * 0.5f, getView().getPosition().y - getView().getSize().y * 0.5f);
    BoundingBox bx (pos.x, pos.y, 0, getView().getSize().x, getView().getSize().y, 0);
    theMap = new Map(&getRenderComponentManager(), "Map test", 100, 50);
    World::addEntityManager(theMap);
    World::setCurrentEntityManager("Map test");
    eu = new EntitiesUpdater(false);
    World::addEntitiesUpdater(eu);
    au = new AnimUpdater(false);
    au->setInterval(sf::seconds(0.01f));
    World::addAnimUpdater(au);
    tiles.push_back(new Tile(tm.getResourceByAlias("GRASS"), Vec3f(0, 0, 0), Vec3f(120, 60, 0),sf::IntRect(0, 0, 100, 50)));
    walls.push_back(new Tile(tm.getResourceByAlias("WALLS"), Vec3f(0, 0, 0), Vec3f(100, 100, 0), sf::IntRect(100, 0, 100, 100)));
    walls.push_back(new Tile(tm.getResourceByAlias("WALLS"), Vec3f(0, 0, 0), Vec3f(100, 100, 0), sf::IntRect(100, 100, 100, 100)));
    walls.push_back(new Tile(tm.getResourceByAlias("WALLS"), Vec3f(0, 0, 0), Vec3f(100, 100, 0), sf::IntRect(100, 200, 100, 100)));
    walls.push_back(new Tile(tm.getResourceByAlias("WALLS"), Vec3f(0, 0, 0), Vec3f(100, 100, 0), sf::IntRect(100, 300, 100, 100)));
    walls.push_back(new Tile(tm.getResourceByAlias("WALLS"), Vec3f(0, 0, 0), Vec3f(100, 100, 0), sf::IntRect(100, 400, 100, 100)));
    walls.push_back(new Tile(tm.getResourceByAlias("WALLS"), Vec3f(0, 0, 0), Vec3f(100, 100, 0), sf::IntRect(100, 500, 100, 100)));
    tiles[0]->getFaces()[0]->getMaterial().setTexId("GRASS");
    walls[0]->getFaces()[0]->getMaterial().setTexId("WALLS");
    walls[1]->getFaces()[0]->getMaterial().setTexId("WALLS");
    walls[2]->getFaces()[0]->getMaterial().setTexId("WALLS");
    walls[3]->getFaces()[0]->getMaterial().setTexId("WALLS");
    walls[4]->getFaces()[0]->getMaterial().setTexId("WALLS");
    walls[5]->getFaces()[0]->getMaterial().setTexId("WALLS");
    std::ifstream ifs("FichierDeSerialisation");
    BoundingBox mapZone(0, 0, 0, 1500, 1000, 0);
    World::generate_map(tiles, walls, Vec2f(100, 50), mapZone, false);
    w = new g2d::Wall(3, 80, walls[3],&g2d::AmbientLight::getAmbientLight(), Shadow::SHADOW_TYPE::SHADOW_TILE);
    w->setPosition(Vec3f(0, 130, 130 + w->getSize().y * 0.5f));
    World::addEntity(w);
    Tile* thouse = new Tile(tm.getResourceByAlias("HOUSE"), Vec3f(0, 0, 0), Vec3f(250, 300, 0), sf::IntRect(0, 0, 250, 300));
    thouse->getFaces()[0]->getMaterial().setTexId("HOUSE");
    g2d::Decor* decor = new g2d::Decor(thouse, &g2d::AmbientLight::getAmbientLight(), 300, Shadow::SHADOW_TYPE::SHADOW_TILE);
    decor->setPosition(Vec3f(-100, 250, 400));
    BoundingVolume *bb = new BoundingBox(decor->getGlobalBounds().getPosition().x, decor->getGlobalBounds().getPosition().y + decor->getGlobalBounds().getSize().y * 0.4f, 0,
    decor->getGlobalBounds().getSize().x, decor->getGlobalBounds().getSize().y * 0.25f, 0);
    decor->setCollisionVolume(bb);
    decor->setShadowCenter(Vec3f(-10, 500, 0));
    World::addEntity(decor);
    Anim* fire = new Anim(0.1f, Vec3f(0, 100, 150), Vec3f(100, 100, 0), 0);
    Tile* tf1 = new Tile(tm.getResourceByAlias("FIRE1"), Vec3f(0, 100, 150), Vec3f(100, 100, 0), sf::IntRect(0, 0, 150, 200));
    tf1->getFaces()[0]->getMaterial().setTexId("FIRE1");
    g2d::Decor *fire1 = new g2d::Decor(tf1, &g2d::AmbientLight::getAmbientLight(), 100, Shadow::SHADOW_TYPE::SHADOW_TILE);
    fire1->setShadowCenter(Vec3f(0, 400, 0));
    //decor->setShadowCenter(Vec2f(0, 60));
    //decor->changeGravityCenter(Vec3f(50, 50, 0));
    Tile* tf2 = new Tile(tm.getResourceByAlias("FIRE2"), Vec3f(0, 100, 150), Vec3f(100, 100, 0), sf::IntRect(0, 0, 150, 200));
    tf2->getFaces()[0]->getMaterial().setTexId("FIRE2");
    g2d::Decor *fire2 = new g2d::Decor(tf2, &g2d::AmbientLight::getAmbientLight(), 100, Shadow::SHADOW_TYPE::SHADOW_TILE);
    fire2->setShadowCenter(Vec3f(0, 400, 0));
    //decor->setShadowCenter(Vec2f(0, 60));
    //decor->changeGravityCenter(Vec3f(50, 50, 0));
    Tile* tf3 = new Tile(tm.getResourceByAlias("FIRE3"), Vec3f(0, 100, 150), Vec3f(100, 100, 0), sf::IntRect(0, 0, 150, 200));
    tf3->getFaces()[0]->getMaterial().setTexId("FIRE3");
    g2d::Decor *fire3 = new g2d::Decor(tf3, &g2d::AmbientLight::getAmbientLight(), 100, Shadow::SHADOW_TYPE::SHADOW_TILE);
    fire3->setShadowCenter(Vec3f(0, 400, 0));
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
    au->addAnim(fire);
    //PonctualLight* light = new PonctualLight(Vec2f(50, 150),100,50,0,200,sf::Color(255,255,0),16,0);
    //World::addEntity(light);
    SymEncPacket packet;
    packet<<"GETCARINFOS";
    Network::sendTcpPacket(packet);
    std::string response = Network::waitForLastResponse("CARINFOS");
    std::istringstream iss(response);
    ITextArchive ia(iss);
    ia(caracter);
    std::string path = "tilesets/vlad_sword.png";
    cache.resourceManager<Texture, std::string>("TextureManager").fromFileWithAlias(path, "VLADSWORD");
    const Texture *text = cache.resourceManager<Texture, std::string>("TextureManager").getResourceByPath(path);
    int textRectX = 0, textRectY = 0, textRectWidth = 50, textRectHeight = 100;
    int textWidth = text->getSize().x;
    Vec3f tmpCenter = caracter->getCenter();
    caracter->setCenter(Vec3f(0, 0, 0));
    for (unsigned int i = 0; i < 64; i+=8) {
        Anim* animation = new Anim(0.1f, Vec3f(-25, -50, 0), Vec3f(50, 100, 0), 0);
        for (unsigned int j = 0; j < 8; j++) {
            sf::IntRect textRect (textRectX, textRectY, textRectWidth, textRectHeight);
            Tile *tile = new Tile(text, Vec3f(-25, -50, 0), Vec3f(50, 100, 0), textRect);
            tile->getFaces()[0]->getMaterial().setTexId("VLADSWORD");
            g2d::Decor *frame = new g2d::Decor(tile, &g2d::AmbientLight::getAmbientLight(), 100, Shadow::SHADOW_TYPE::SHADOW_TILE);
            frame->setShadowCenter(Vec3f(0, 100, 0));
            //decor->setShadowCenter(Vec2f(80, 130));
            //decor->changeGravityCenter(Vec3f(50, 50, 0));
            textRectX += textRectWidth;
            if (textRectX + textRectWidth > textWidth) {
                textRectX = 0;
                textRectY += textRectHeight;
            }
            animation->addEntity(frame);
        }
        caracter->addAnimation(animation);
        au->addAnim(animation);
    }
    caracter->setCenter(tmpCenter);
    getView().move(caracter->getCenter().x, caracter->getCenter().y, caracter->getCenter().z - 300);
    BoundingVolume* bb2 = new BoundingBox(caracter->getGlobalBounds().getPosition().x, caracter->getGlobalBounds().getPosition().y + caracter->getGlobalBounds().getSize().y * 0.4f, 0,
    caracter->getGlobalBounds().getSize().x, caracter->getGlobalBounds().getSize().y * 0.25f, 0);
    caracter->setCollisionVolume(bb2);
    World::addEntity(caracter);
    Network::sendTcpPacket(packet);
    response = Network::waitForLastResponse("MONSTERSINFOS");
    iss.str(response);
    ia(monster);
    path = "tilesets/ogro.png";
    //for (unsigned int n = 0; n < monsters.size(); n++) {
        tmpCenter = monster->getCenter();
        monster->setCenter(Vec3f(0, 0, 0));
        cache.resourceManager<Texture, std::string>("TextureManager").fromFileWithAlias(path, "OGRO");
        text = cache.resourceManager<Texture, std::string>("TextureManager").getResourceByPath(path);
        textRectX = 0, textRectY = 0, textRectWidth = 50, textRectHeight = 100;
        textWidth = text->getSize().x;
        for (unsigned int i = 0; i < 64; i+=8) {
            Anim* animation = new Anim(0.1f, Vec3f(-25, -50, 0), Vec3f(50, 100, 0), 0);
            for (unsigned int j = 0; j < 8; j++) {
                sf::IntRect textRect (textRectX, textRectY, textRectWidth, textRectHeight);
                Tile *tile = new Tile(text, Vec3f(-25, -50, 0), Vec3f(50, 100, 0), textRect);
                tile->getFaces()[0]->getMaterial().setTexId("OGRO");
                g2d::Decor *frame = new g2d::Decor(tile, &g2d::AmbientLight::getAmbientLight(), 100, Shadow::SHADOW_TYPE::SHADOW_TILE);
                frame->setShadowCenter(Vec3f(0, 100, 0));
                //decor->changeGravityCenter(Vec3f(50, 50, 0));
                textRectX += textRectWidth;
                if (textRectX + textRectWidth > textWidth) {
                    textRectX = 0;
                    textRectY += textRectHeight;
                }
                animation->addEntity(frame);
            }
            monster->addAnimation(animation);
            au->addAnim(animation);
        }
        monster->setCenter(tmpCenter);
        World::addEntity(monster);
    //}

    //caracter->setCenter(Vec3f(getView().getPosition().x, getView().getPosition().y, 300));
    g2d::PonctualLight* light1 = new g2d::PonctualLight(Vec3f(0, 420, 420), 100, 50, 50, 255, sf::Color::Yellow, 16);
    light2 = new g2d::PonctualLight(Vec3f(50, 160, 160), 100, 50, 50, 255, sf::Color::Yellow, 16);
    World::addEntity(light1);
    World::addEntity(light2);
    FastRenderComponent *frc1 = new FastRenderComponent(getRenderWindow(),0, "E_BIGTILE", false);
    FastRenderComponent *frc2 = new FastRenderComponent(getRenderWindow(),1, "E_WALL+E_DECOR+E_ANIMATION+E_CARACTER+E_MONSTER", false);
    getRenderComponentManager().addRenderComponent(frc1);
    getRenderComponentManager().addRenderComponent(frc2);
    //getView().move(d.x * 0.5f, d.y * 0.5f, 0);
    //World::computeIntersectionsWithWalls();
    World::update();
    //World::computeIntersectionsWithWalls();
    Action a1 (Action::EVENT_TYPE::KEY_HELD_DOWN, sf::Keyboard::Key::Z);
    Action a2 (Action::EVENT_TYPE::KEY_HELD_DOWN, sf::Keyboard::Key::Q);
    Action a3 (Action::EVENT_TYPE::KEY_HELD_DOWN, sf::Keyboard::Key::S);
    Action a4 (Action::EVENT_TYPE::KEY_HELD_DOWN, sf::Keyboard::Key::D);
    Action a5 (Action::EVENT_TYPE::MOUSE_BUTTON_PRESSED_ONCE, sf::Mouse::Left);
    Action combined  = a1 || a2 || a3 || a4;
    Command moveAction(combined, FastDelegate<void>(&MyAppli::keyHeldDown, this, sf::Keyboard::Key::Unknown));
    getListener().connect("MoveAction", moveAction);
    g2d::AmbientLight::getAmbientLight().setColor(sf::Color(0, 0, 255));
    Command mouseInsideAction(FastDelegate<bool>(&MyAppli::mouseInside,this, sf::Vector2f(-1, -1)), FastDelegate<void>(&MyAppli::onMouseInside, this, sf::Vector2f(-1,-1)));
    getListener().connect("MouseInside",mouseInsideAction);
    Command leftMouseButtonPressedAction (a5, FastDelegate<void>(&MyAppli::leftMouseButtonPressed, this, sf::Vector2f(-1, -1)));
    getListener().connect("LeftMouseButtonPressedAction", leftMouseButtonPressedAction);
}
void MyAppli::onRender(FastRenderComponentManager *cm) {
    /* std::vector<Vec3f> path = caracter->getPath();
    VertexArray m_vertices(sf::PrimitiveType::LinesStrip);
    for (unsigned int i = 0; i < path.size(); i++) {
        m_vertices.append(path[i]);
        //glVertex2f(path[i+1].x, path[i+1].y);
    }
    getRenderWindow().draw(m_vertices);*/
    /*Entity& shadows = World::getShadowMap<Entity>();
    getRenderWindow().draw(shadows, BlendMode::BlendMultiply);*/
    // draw everything here...
    World::drawOnComponents("E_BIGTILE", 0);
    World::drawOnComponents("E_WALL+E_DECOR+E_ANIMATION+E_CARACTER+E_MONSTER", 1);
    /*ConvexShape convexshape(4);
    convexshape.setPoint(0, Vector3f(0, 0, 0));
    convexshape.setPoint(1, Vector3f(100, 0, 0));
    convexshape.setPoint(2, Vector3f(100, 50, 0));
    convexshape.setPoint(3, Vector3f(0, 50, 0));
    convexshape.setFillColor(sf::Color::Red);
    //shape.setScale(Vec3f(2, 2, 0));
    getRenderWindow().draw(convexshape);*/
    /*CircleShape circle(20);
    getRenderWindow().draw(circle);*/
    /*RectangleShape rectangle(Vec3f(100, 100, 0));
    getRenderWindow().draw(rectangle);*/
    /*std::vector<ConvexShape*> cvs = theMap->getCollisionVolumes();
    for (unsigned int i = 0; i < cvs.size(); i++)
    getRenderWindow().draw(*cvs[i]);*/
    /*Entity& lights = World::getLightMap<Entity>();
    getRenderWindow().draw(lights, BlendMode::BlendMultiply);*/
    //std::cout<<getRenderWindow().mapPixelToCoords(entities[0]->getPosition());
    //getRenderWindow().draw(*tiles[0]);
    /*Face face(Vec3f(0, 0, 0), Vec3f(120, 60, 0), Vec3f(60, 30, 0), sf::Quads, "E_FACE");
    Vertex v1 (Vector3f(0, 0, 0),Color(255, 255, 255, 0), Vector2f(0, 0));
    Vertex v2 (Vector3f(120, 0, 0),Color(255, 255, 255, 0), Vector2f(100, 0));
    Vertex v3 (Vector3f(120, 60, 0),Color(255, 255, 255, 0), Vector2f(100, 50));
    Vertex v4 (Vector3f(0, 60, 0),Color(255, 255, 255, 0), Vector2f(0, 50));
    face.append(v1);
    face.append(v2);
    face.append(v3);
    face.append(v4);
    TextureManager<TEXTURES> &tm = getCache().resourceManager<Texture, TEXTURES>("TextureManager");
    face.addTexture(IntRect(0, 0,100,50),tm.getResourceByAlias(GRASS));*/
    //getComponentManager().draw(*tiles[0]);
    //std::cout<<getRenderWindow().mapCoordsToPixel(tiles[0]->getPosition())<<std::endl;
    //Entity& shadows = World::getShadowMap<Entity>();
    //draw(shadows, sf::BlendMode::BlendMultiply);
    /* for (unsigned int i = 0; i < entities.size(); i++) {
        getRenderWindow().draw(*entities[i]);
    }*/
    //Entity& lights = World::getLightMap<Entity>();
    //draw(lights, BlendMultiply);
}
void MyAppli::onDisplay(RenderWindow* window) {
    RenderStates states(sf::BlendMode(sf::BlendMultiply));
    Entity& shadowMap = World::getShadowMap("E_WALL+E_DECOR+E_ANIMATION+E_CARACTER+E_MONSTER", 2, 1);
    window->draw(shadowMap,states);
   /* std::vector<g2d::Entity*> entity = World::getEntities<g2d::Entity>("E_MONSTER");
    window->draw(*entity[0]);*/
    /*std::vector<g2d::Entity*> ground = World::getVisibleEntities<g2d::Entity>("E_BIGTILE");
    for (unsigned int i = 0; i < ground.size(); i++) {
        Matrix4f matrix = ground[i]->getTransform().get3DMatrix()
        * getView().getViewMatrix().get3DMatrix()
        * getView().getProjMatrix().get3DMatrix();
        shader.setParameter("mvp", matrix.transpose());
        RenderStates states;
        states.shader = &shader;
        window->draw(*ground[i], states);
    }*/
    /*g2d::Entity& lightMap = World::getLightMap<g2d::Entity>("E_PONCTUAL_LIGHT", 2, 0, 1);
    window->draw(lightMap, sf::BlendMultiply);*/
    /*std::vector<Ray*> segments = w->getSegments();
    VertexArray va(sf::Lines);
    for (unsigned int i = 0; i < segments.size(); i++) {
        Vertex vertex1(sf::Vector3f(segments[i]->getOrig().x, segments[i]->getOrig().y, segments[i]->getOrig().z), sf::Color::Red);
        Vertex vertex2(sf::Vector3f(segments[i]->getExt().x, segments[i]->getExt().y, segments[i]->getExt().z), sf::Color::Red);
        va.append(vertex1);
        va.append(vertex2);
    }
    window->draw(va);*/
    //window->draw(light2->getIntPoints());
    /*RectangleShape shape(Vec3f(100, 100, 0));
    shape.setFillColor(Color::Red);
    shape.setOutlineThickness(10);
    shape.setOutlineColor(Color(0, 0, 0));
    shape.setPosition(Vec3f(0, 300, 300));
    window->draw(shape);*/
    //window->draw(World::getLightMap<Entity>());
    /*std::vector<g2d::Entity*> entities = World::getVisibleEntities<g2d::Entity>("E_BIGTILE");
    for (unsigned int i = 0; i < entities.size(); i++) {
        window->draw(*entities[i]);
    }*/
    /*entities = World::getVisibleEntities<g2d::Entity>("E_WALL+E_DECOR+E_ANIMATION+E_CARACTER");
    for (unsigned int i = 0; i < entities.size(); i++) {
        if(entities[i]->getType() != "E_SHADOW_WALL" && entities[i]->getType() != "E_SHADOW_TILE")
            window->draw(*entities[i]);
    }*/
    /*g2d::Entity* decor = World::getEntities<g2d::Entity>("E_DECOR")[0];
    g2d::Entity* caracter = World::getEntities<g2d::Entity>("E_CARACTER")[0];
    RectangleShape rect(Vec3f(decor->getCollisionVolume()->getSize().x, decor->getCollisionVolume()->getSize().y, 0));
    rect.setPosition(Vec3f(Vec3f(decor->getCollisionVolume()->getPosition().x, decor->getCollisionVolume()->getPosition().y, decor->getPosition().z)));
    window->draw(rect);
    RectangleShape rect2(Vec3f(caracter->getCollisionVolume()->getSize().x, caracter->getCollisionVolume()->getSize().y, 0));
    rect2.setPosition(Vec3f(Vec3f(caracter->getCollisionVolume()->getPosition().x, caracter->getCollisionVolume()->getPosition().y, caracter->getPosition().z)));
    window->draw(rect2);*/
}
void MyAppli::onUpdate (sf::Event& event) {
    // check all the window's events that were triggered since the last iteration of the loop
    if (event.type == sf::Event::Closed) {
        Network::stopCli();
        au->stop();
        eu->stop();
        stop();
    }
    if (event.type == sf::Event::KeyPressed) {
        previousKey = actualKey;
        actualKey = event.key.code;
        getListener().setCommandSlotParams("MoveAction", this, event.key.code);
    }
    if (event.type == sf::Event::KeyReleased && caracter->isMovingFromKeyboard()) {
        caracter->setMoving(false);
        caracter->setIsMovingFromKeyboard(false);
        previousKey = event.key.code;
        actualKey = sf::Keyboard::Key::Unknown;
        sf::Int64 cli_time = getClock("TimeClock").getElapsedTime().asMicroseconds();
        std::string message = "STOPCARMOVE*"+conversionLongString(cli_time);
        SymEncPacket packet;
        packet<<message;
        Network::sendTcpPacket(packet);
    }
    /*if (event.type == sf::Event::MouseMoved) {
        Vector2f mousePos = Vector2f(event.mouseMove.x, event.mouseMove.y);
        InputSystem::getListener().setCommandParams("MouseInside", this, this, mousePos);
    }*/
    if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2f mousePos (event.mouseButton.x, event.mouseButton.y);
        getListener().setCommandSlotParams("LeftMouseButtonPressedAction", this, mousePos);
    }
}
void MyAppli::onExec () {
    if (getClock("RequestTime").getElapsedTime().asSeconds() >= timeBtwnTwoReq.asSeconds()) {
        std::string request = "GETCARPOS";
        sf::Packet packet;
        packet<<request;
        Network::sendUdpPacket(packet);
        getClock("RequestTime").restart();
    }
    std::string response;
    if (Network::getResponse("NEWPATH", response)) {
        std::vector<std::string> infos = split(response, "*");
        std::vector<Vec2f> path;
        int size = conversionStringInt(infos[0]);
        for (int i = 0; i < size; i++)
            path.push_back(Vec2f(conversionStringFloat(infos[i*2+1]), conversionStringFloat(infos[i*2+2])));
        caracter->setPath(path);
        caracter->setIsMovingFromKeyboard(false);
        caracter->setMoving(true);
    }
    if (Network::getResponse("NEWPOS", response) || getClock("TransferTime").getElapsedTime().asSeconds() >= timeBtwnTwoReq.asSeconds() + 0.5f) {
        std::vector<std::string> infos = split(response, "*");
        if (infos.size() == 3) {
            ping = conversionStringLong(infos[0]);
            Vec3f actualPos = Vec3f(caracter->getCenter().x, caracter->getCenter().y, 0);
            Vec3f newPos (conversionStringFloat(infos[1]), conversionStringFloat(infos[2]), 0);
            for (unsigned int i = 0; i < getRenderComponentManager().getNbComponents(); i++) {
                View view = getRenderComponentManager().getRenderComponent(i)->getView();
                Vec3f d = newPos - actualPos;
                view.move(d.x, d.y, d.y);
                getRenderComponentManager().getRenderComponent(i)->setView(view);
            }
            Vec3f d = newPos - actualPos;
            World::moveEntity(caracter, d.x, d.y, d.y);
            getView().move (d.x, d.y, d.y);
            World::update();
        }
        interpolation.first = Vec3f(caracter->getCenter().x, caracter->getCenter().y, 0);
        if (caracter->isMoving()) {
            if (caracter->isMovingFromKeyboard()) {
                interpolation.second = interpolation.first + Vec3f(caracter->getDir().x,caracter->getDir().y,0)  * caracter->getSpeed() * (ping + timeBtwnTwoReq.asMicroseconds());
            } else {
                interpolation.second = Computer::getPosOnPathFromTime(interpolation.first, caracter->getPath(),ping + timeBtwnTwoReq.asMicroseconds(),caracter->getSpeed());
            }
        } else {
            interpolation.second = interpolation.first;
        }
        getClock("TransferTime").restart();
   } else if (Network::getResponse("GET_TIME", response)) {
        sf::Int64 cli_time = getClock("TimeClock").getElapsedTime().asMicroseconds();
        std::string message = "SET_TIME*"+conversionLongString(cli_time);
        SymEncPacket packet;
        packet<<message;
        Network::sendTcpPacket(packet);
   } else {
       if (getClock("LoopTime").getElapsedTime().asMilliseconds() < 100) {
            std::chrono::milliseconds dura(100 - getClock("LoopTime").getElapsedTime().asMilliseconds());
            std::this_thread::sleep_for(dura);
       }
       if (caracter->isMoving()) {
            if (caracter->isMovingFromKeyboard()) {

                Vec3f actualPos = Vec3f(caracter->getCenter().x, caracter->getCenter().y, 0);
                sf::Int64 elapsedTime = getClock("TransferTime").getElapsedTime().asMicroseconds();
                Vec3f newPos = interpolation.first + (interpolation.second - interpolation.first) * ((float) elapsedTime / (float) (ping + timeBtwnTwoReq.asMicroseconds()));
                caracter->setCenter(Vec3f(newPos.x, newPos.y, caracter->getCenter().z));
                if (World::collide(caracter)) {
                    newPos = actualPos;
                }
                caracter->setCenter(Vec3f(actualPos.x, actualPos.y, caracter->getCenter().z));
                for (unsigned int i = 0; i < getRenderComponentManager().getNbComponents(); i++) {
                    View view = getRenderComponentManager().getRenderComponent(i)->getView();
                    Vec3f d = newPos - view.getPosition();
                    view.move(d.x, d.y, d.y);
                    getRenderComponentManager().getRenderComponent(i)->setView(view);
                }
                Vec3f d = newPos - actualPos;
                World::moveEntity(caracter, d.x, d.y, d.y);
                getView().move(d.x, d.y, d.y);
                World::update();
            } else {
                Vec2f actualPos = caracter->getCenter();
                sf::Int64 elapsedTime = getClock("TransferTime").getElapsedTime().asMicroseconds();
                Vec2f newPos = interpolation.first + (interpolation.second - interpolation.first) * ((float) elapsedTime / (float) (ping + timeBtwnTwoReq.asMicroseconds()));
                if (newPos.computeDist(caracter->getPath()[caracter->getPath().size() - 1]) <= 1) {
                    caracter->setMoving(false);
                    newPos = caracter->getPath()[caracter->getPath().size() - 1];
                }
                for (unsigned int i = 0; i < getRenderComponentManager().getNbComponents(); i++) {
                    View view = getRenderComponentManager().getRenderComponent(i)->getView();
                    Vec3f d = newPos - view.getPosition();
                    view.move(d.x, d.y, d.y);
                    getRenderComponentManager().getRenderComponent(i)->setView(view);
                }
                Vec2f d = newPos - actualPos;
                Vec2f dir = d.normalize();
                if (dir != caracter->getDir())
                    caracter->setDir(dir);
                World::moveEntity(caracter, d.x, d.y, d.y);
                getView().move(d.x, d.y, d.y);
                World::update();

                //if (newPos == caracter->getPath()[caracter->getPath().size() - 1])
                   // std::cout<<"car pos : "<<caracter->getCenter()<<std::endl;
                //std::cout<<"new pos : "<<newPos<<"actual pos : "<<actualPos<<std::endl;
            }
       }
    }
}
