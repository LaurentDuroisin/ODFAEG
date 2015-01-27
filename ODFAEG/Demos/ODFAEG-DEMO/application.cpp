#include "application.h"
#include "odfaeg/Math/distributions.h"
using namespace odfaeg;
using namespace odfaeg::graphic;
using namespace odfaeg::math;
using namespace odfaeg::physic;
using namespace odfaeg::core;
using namespace odfaeg::audio;
MyAppli::MyAppli(sf::VideoMode wm, std::string title) : Application (wm, title, false, true, sf::Style::Default, sf::ContextSettings(0, 0, 4, 3, 0)) {
    running = false;
    actualKey = sf::Keyboard::Key::Unknown;
    previousKey = sf::Keyboard::Key::Unknown;
    getView().move(0, 300, 0);

    fpsCounter = 0;
    addClock(sf::Clock(), "FPS");
    day = false;
    sf::Listener::setUpVector(0.f, 0.f, 1.f);
}
void MyAppli::keyHeldDown (sf::Keyboard::Key key) {
    //BoundingRectangle rect (pos.x, pos.y, getView().getSize().x, getView().getSize().y);
    if (actualKey != sf::Keyboard::Key::Unknown && key == sf::Keyboard::Key::Z) {
        if (!caracter->isMoving()) {
            if (actualKey != previousKey) {
                Vec2f dir(0, -1);
                caracter->setDir(dir);
                sf::Listener::setDirection(dir.x, dir.y, 0);
            }
            caracter->setMoving(true);
            caracter->setIsMovingFromKeyboard(true);
        }
    } else if (actualKey != sf::Keyboard::Key::Unknown && key == sf::Keyboard::Key::Q) {
        if (!caracter->isMoving()) {
            if (actualKey != previousKey) {
                Vec2f dir(-1, 0);
                caracter->setDir(dir);
                sf::Listener::setDirection(dir.x, dir.y, 0);
            }
            caracter->setMoving(true);
            caracter->setIsMovingFromKeyboard(true);
        }
    } else if (actualKey != sf::Keyboard::Key::Unknown && actualKey == sf::Keyboard::Key::S) {
        if (!caracter->isMoving()) {
            if (actualKey != previousKey) {
                Vec2f dir(0, 1);
                caracter->setDir(dir);
                sf::Listener::setDirection(dir.x, dir.y, 0);
            }
            caracter->setMoving(true);
            caracter->setIsMovingFromKeyboard(true);
        }
    } else if (actualKey != sf::Keyboard::Key::Unknown && key == sf::Keyboard::Key::D) {
        if (!caracter->isMoving()) {
            if (actualKey != previousKey) {
                Vec2f dir(1, 0);
                caracter->setDir(dir);
                sf::Listener::setDirection(dir.x, dir.y, 0);
            }
            caracter->setIsMovingFromKeyboard(true);
            caracter->setMoving(true);
        }
    }
}
void MyAppli::leftMouseButtonPressed(sf::Vector2f mousePos) {
    Vec2f mouse(mousePos.x, mousePos.y);
    Vec3f finalPos = getRenderWindow().mapPixelToCoords(Vec3f(mouse.x, getRenderWindow().getSize().y-mouse.y, 0));
    std::vector<Vec2f> path = World::getPath(caracter, finalPos);
    if (path.size() > 0) {
        caracter->setPath(path);
        caracter->setIsMovingFromKeyboard(false);
        caracter->setMoving(true);
    }
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
    tm.fromFileWithAlias("tilesets/particule.png", "PARTICLE");
    SoundBufferManager<> sm;
    sm.fromFileWithAlias("sounds/walk_crop.wav", "step foot");
    sm.fromFileWithAlias("sounds/fire.wav", "fire");
    Stream* stream = new Stream();
    stream->load(*sm.getResourceByAlias("step foot"));
    Stream* stream2 = new Stream();
    stream2->load(*sm.getResourceByAlias("fire"));
    player.setAudioStream(stream);
    pfire.setAudioStream(stream2);
    cache.addResourceManager(tm, "TextureManager");
    cache.addResourceManager(sm, "SoundManager");
}
void MyAppli::onInit () {
    if (day)
        g2d::AmbientLight::getAmbientLight().setColor(sf::Color::White);
    TextureManager<> &tm = cache.resourceManager<Texture, std::string>("TextureManager");
    Vec2f pos (getView().getPosition().x - getView().getSize().x * 0.5f, getView().getPosition().y - getView().getSize().y * 0.5f);
    BoundingBox bx (pos.x, pos.y, 0, getView().getSize().x, getView().getSize().y, 0);
    theMap = new Map(&getRenderComponentManager(), "Map test", 100, 50);
    World::addEntityManager(theMap);
    World::setCurrentEntityManager("Map test");
    eu = new EntitiesUpdater(true);
    World::addEntitiesUpdater(eu);
    au = new AnimUpdater(true);
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
    if(ifs) {
        ITextArchive ia(ifs);
        std::vector<Entity*> entities;
        ia(entities);
        for (unsigned int i = 0; i < entities.size(); i++) {
            World::addEntity(entities[i]);
            if (entities[i]->getType() == "E_BIGTILE") {
                for (unsigned int j = 0; j < entities[i]->getChildren().size(); j++) {
                    std::string texId =  entities[i]->getChildren()[j]->getFaces()[0]->getMaterial().getTexId();
                    sf::IntRect texRect = entities[i]->getChildren()[j]->getFaces()[0]->getMaterial().getTexRect();
                    entities[i]->getChildren()[j]->getFaces()[0]->getMaterial().clearTextures();
                    entities[i]->getChildren()[j]->getFaces()[0]->getMaterial().addTexture(tm.getResourceByAlias(texId), texRect);
                    entities[i]->getChildren()[j]->getFaces()[0]->getMaterial().setTexId(texId);
                    entities[i]->getChildren()[j]->setParent(entities[i]);
                }

            } else if (entities[i]->getType() == "E_WALL") {
                std::string texId =  entities[i]->getChildren()[0]->getFaces()[0]->getMaterial().getTexId();
                sf::IntRect texRect = entities[i]->getChildren()[0]->getFaces()[0]->getMaterial().getTexRect();
                entities[i]->getChildren()[0]->getFaces()[0]->getMaterial().clearTextures();
                entities[i]->getChildren()[0]->getFaces()[0]->getMaterial().addTexture(tm.getResourceByAlias(texId), texRect);
                entities[i]->getChildren()[0]->getFaces()[0]->getMaterial().setTexId(texId);
                entities[i]->getChildren()[0]->setParent(entities[i]);
                entities[i]->getChildren()[1]->setParent(entities[i]);
                static_cast<Shadow*>(entities[i]->getChildren()[1])->createShadow(g2d::AmbientLight::getAmbientLight(), static_cast<Model*>(entities[i]));
                World::getGridCellAt(Vec3f(entities[i]->getCenter().x, entities[i]->getCenter().y, 0))->setPassable(false);
            } else if (entities[i]->getType() == "E_DECOR") {
                std::string texId =  entities[i]->getChildren()[0]->getFaces()[0]->getMaterial().getTexId();
                sf::IntRect texRect = entities[i]->getChildren()[0]->getFaces()[0]->getMaterial().getTexRect();
                entities[i]->getChildren()[0]->getFaces()[0]->getMaterial().clearTextures();
                entities[i]->getChildren()[0]->getFaces()[0]->getMaterial().addTexture(tm.getResourceByAlias(texId), texRect);
                entities[i]->getChildren()[0]->getFaces()[0]->getMaterial().setTexId(texId);
                entities[i]->getChildren()[0]->setParent(entities[i]);
                entities[i]->getChildren()[1]->setParent(entities[i]);
                static_cast<Shadow*>(entities[i]->getChildren()[1])->createShadow(g2d::AmbientLight::getAmbientLight(),static_cast<Model*>(entities[i]));
            } else if (entities[i]->getType() == "E_ANIMATION") {
                Anim* anim = static_cast<Anim*> (entities[i]);
                for (unsigned int j = 0; j < anim->getChildren().size(); j++) {
                    std::string texId = entities[i]->getChildren()[j]->getChildren()[0]->getFaces()[0]->getMaterial().getTexId();
                    sf::IntRect texRect = entities[i]->getChildren()[j]->getChildren()[0]->getFaces()[0]->getMaterial().getTexRect();
                    entities[i]->getChildren()[j]->getChildren()[0]->getFaces()[0]->getMaterial().clearTextures();
                    entities[i]->getChildren()[j]->getChildren()[0]->getFaces()[0]->getMaterial().addTexture(tm.getResourceByAlias(texId), texRect);
                    entities[i]->getChildren()[j]->getChildren()[0]->getFaces()[0]->getMaterial().setTexId(texId);
                    entities[i]->getChildren()[j]->setParent(entities[i]);
                    entities[i]->getChildren()[j]->setParent(entities[i]);
                    entities[i]->getChildren()[j]->getChildren()[0]->setParent(entities[i]->getChildren()[j]);
                    entities[i]->getChildren()[j]->getChildren()[1]->setParent(entities[i]->getChildren()[j]);
                    static_cast<Shadow*>(entities[i]->getChildren()[j]->getChildren()[1])->createShadow(g2d::AmbientLight::getAmbientLight(),static_cast<Model*>(entities[i]->getChildren()[0]));
                }
                anim->play(true);
                au->addAnim(anim);
            }
        }
        ifs.close();
    } else {
        BoundingBox mapZone(0, 0, 0, 1500, 1000, 0);
        World::generate_map(tiles, walls, Vec2f(100, 50), mapZone, false);
        Tile* thouse = new Tile(tm.getResourceByAlias("HOUSE"), Vec3f(0, 0, 0), Vec3f(250, 300, 0), sf::IntRect(0, 0, 250, 300));
        thouse->getFaces()[0]->getMaterial().setTexId("HOUSE");
        g2d::Decor* decor = new g2d::Decor(thouse, &g2d::AmbientLight::getAmbientLight(), 300, Shadow::SHADOW_TYPE::SHADOW_TILE);
        decor->setPosition(Vec3f(-100, 250, 400));
        BoundingVolume *bb = new BoundingBox(decor->getGlobalBounds().getPosition().x, decor->getGlobalBounds().getPosition().y + decor->getGlobalBounds().getSize().y * 0.4f, 0,
        decor->getGlobalBounds().getSize().x, decor->getGlobalBounds().getSize().y * 0.25f, 0);
        //std::cout<<bb->getPosition()<<" "<<bb->getSize()<<std::endl;
        decor->setCollisionVolume(bb);
        decor->setShadowCenter(Vec3f(0, 500, 0));
        World::addEntity(decor);
        Anim* fire = new Anim(0.1f, Vec3f(0, 100, 150), Vec3f(100, 100, 0), 0);
        Tile* tf1 = new Tile(tm.getResourceByAlias("FIRE1"), Vec3f(0, 100, 150), Vec3f(100, 100, 0), sf::IntRect(0, 0, 150, 200));
        tf1->getFaces()[0]->getMaterial().setTexId("FIRE1");
        g2d::Decor *fire1 = new g2d::Decor(tf1, &g2d::AmbientLight::getAmbientLight(), 100, Shadow::SHADOW_TYPE::SHADOW_TILE);
        fire1->setShadowCenter(Vec3f(0, 200, 0));
        //decor->changeGravityCenter(Vec3f(50, 50, 0));
        Tile* tf2 = new Tile(tm.getResourceByAlias("FIRE2"), Vec3f(0, 100, 150), Vec3f(100, 100, 0), sf::IntRect(0, 0, 150, 200));
        tf2->getFaces()[0]->getMaterial().setTexId("FIRE2");
        g2d::Decor *fire2 = new g2d::Decor(tf2, &g2d::AmbientLight::getAmbientLight(), 100, Shadow::SHADOW_TYPE::SHADOW_TILE);
        fire2->setShadowCenter(Vec3f(0, 200, 0));
        //decor->changeGravityCenter(Vec3f(50, 50, 0));
        Tile* tf3 = new Tile(tm.getResourceByAlias("FIRE3"), Vec3f(0, 100, 150), Vec3f(100, 100, 0), sf::IntRect(0, 0, 150, 200));
        tf3->getFaces()[0]->getMaterial().setTexId("FIRE3");
        g2d::Decor *fire3 = new g2d::Decor(tf3, &g2d::AmbientLight::getAmbientLight(), 100, Shadow::SHADOW_TYPE::SHADOW_TILE);
        fire3->setShadowCenter(Vec3f(0, 200, 0));
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
        w = new g2d::Wall(3, 80, walls[3],&g2d::AmbientLight::getAmbientLight(), Shadow::SHADOW_TYPE::SHADOW_TILE);
        w->setPosition(Vec3f(0, 130, 130 + w->getSize().y * 0.5f));
        World::addEntity(w);
    }
    ps.setTexture(*tm.getResourceByAlias("PARTICLE"));
    for (unsigned int i = 0; i < 10; i++) {
        ps.addTextureRect(sf::IntRect(i*10, 0, 10, 10));
    }
    emitter.setEmissionRate(30);
    emitter.setParticleLifetime(Distributions::uniform(sf::seconds(5), sf::seconds(7)));
    emitter.setParticlePosition(Distributions::rect(Vec3f(50, 90, 0), Vec3f(25, 5, 0)));   // Emit particles in given circle
    emitter.setParticleVelocity(Distributions::deflect(Vec3f(0, -10, 0),  0)); // Emit towards direction with deviation of 15°
    emitter.setParticleRotation(Distributions::uniform(0.f, 0.f));
    emitter.setParticleTextureIndex(Distributions::uniformui(0, 9));
    emitter.setParticleScale(Distributions::rect(Vec3f(2.1f, 2.1f, 1.f), Vec3f(2.f, 2.f, 1.f)));
    //emitter.setParticleColor(Distributions::color(Vec3f(0, 0, 0, 255), Vec3f(0, 0, 0, 255)));
   /* sf::Vector3f acceleration(0, -1, 0);
    ForceAffector affector(acceleration);
    billboard->getParticleSystem().addAffector(affector);*/
    ps.addEmitter(refEmitter(emitter));
    View view = getView();
    //view.rotate(0, 0, 20);
    FastRenderComponent *frc1 = new FastRenderComponent(getRenderWindow(),0, "E_BIGTILE", false);
    FastRenderComponent *frc2 = new FastRenderComponent(getRenderWindow(),1, "E_WALL+E_DECOR+E_ANIMATION+E_CARACTER", false);
    frc1->setView(view);
    frc2->setView(view);
    getRenderComponentManager().addRenderComponent(frc1);
    getRenderComponentManager().addRenderComponent(frc2);
    caracter = new Caracter("Sorrok", "Nagi", "M", "Map test", "Brain", "Green", "White","Normal","Novice", 1);
    std::string path = "tilesets/vlad_sword.png";
    cache.resourceManager<Texture, std::string>("TextureManager").fromFileWithAlias(path, "VLADSWORD");
    const Texture *text = cache.resourceManager<Texture, std::string>("TextureManager").getResourceByPath(path);
    int textRectX = 0, textRectY = 0, textRectWidth = 50, textRectHeight = 100;
    int textWidth = text->getSize().x;
    for (unsigned int i = 0; i < 64; i+=8) {
        Anim* animation = new Anim(0.1f, Vec3f(-25, -50, 0), Vec3f(50, 100, 0), 0);
        for (unsigned int j = 0; j < 8; j++) {
            sf::IntRect textRect (textRectX, textRectY, textRectWidth, textRectHeight);
            Tile *tile = new Tile(text, Vec3f(-25, -50, 0), Vec3f(textRectWidth, textRectHeight, 0), textRect);
            tile->getFaces()[0]->getMaterial().setTexId("VLADSWORD");
            g2d::Decor *frame = new g2d::Decor(tile, &g2d::AmbientLight::getAmbientLight(), 100, Shadow::SHADOW_TYPE::SHADOW_TILE);
            frame->setShadowCenter(Vec3f(0, 100, 100));
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
    BoundingVolume* bb2 = new BoundingBox(caracter->getGlobalBounds().getPosition().x, caracter->getGlobalBounds().getPosition().y + caracter->getGlobalBounds().getSize().y * 0.4f, 0,
    caracter->getGlobalBounds().getSize().x, caracter->getGlobalBounds().getSize().y * 0.25f, 0);
    caracter->setCollisionVolume(bb2);
    caracter->setCenter(Vec3f(getView().getPosition().x, getView().getPosition().y, 300));
    //std::cout<<bb2->getPosition()<<" "<<bb2->getSize()<<std::endl;
    g2d::PonctualLight* light1 = new g2d::PonctualLight(Vec3f(-50, 420, 420), 100, 50, 0, 255, sf::Color::Yellow, 16);
    light2 = new g2d::PonctualLight(Vec3f(50, 160, 160), 100, 50, 0, 255, sf::Color::Yellow, 16);
    World::addEntity(light1);
    World::addEntity(light2);
    //getView().move(d.x * 0.5f, d.y * 0.5f, 0);
    World::addEntity(caracter);

    //World::computeIntersectionsWithWalls();
    World::update();
    Action a1 (Action::EVENT_TYPE::KEY_PRESSED_ONCE, sf::Keyboard::Key::Z);
    Action a2 (Action::EVENT_TYPE::KEY_PRESSED_ONCE, sf::Keyboard::Key::Q);
    Action a3 (Action::EVENT_TYPE::KEY_PRESSED_ONCE, sf::Keyboard::Key::S);
    Action a4 (Action::EVENT_TYPE::KEY_PRESSED_ONCE, sf::Keyboard::Key::D);
    Action a5 (Action::EVENT_TYPE::MOUSE_BUTTON_PRESSED_ONCE, sf::Mouse::Left);
    Action combined  = a1 || a2 || a3 || a4;
    Command moveAction(combined, FastDelegate<void>(&MyAppli::keyHeldDown, this, sf::Keyboard::Unknown));
    getListener().connect("MoveAction", moveAction);
    if (!day)
        g2d::AmbientLight::getAmbientLight().setColor(sf::Color(0, 0, 255));
    Command mouseInsideAction(FastDelegate<bool>(&MyAppli::mouseInside,this, sf::Vector2f(-1, -1)), FastDelegate<void>(&MyAppli::onMouseInside, this, sf::Vector2f(-1,-1)));
    getListener().connect("MouseInside",mouseInsideAction);
    Command leftMouseButtonPressedAction (a5, FastDelegate<void>(&MyAppli::leftMouseButtonPressed, this, sf::Vector2f(-1, -1)));
    getListener().connect("LeftMouseButtonPressedAction", leftMouseButtonPressedAction);
    player.setRelativeToListener(true);
    player.setPosition(0.f, 0.f, 0.f);
    player.setMinDistance(5.f);
    player.setAttenuation(10.f);
    pfire.setPosition(0, 100, 0);
    pfire.setMinDistance(200);
    pfire.setAttenuation(10.f);
    pfire.play(true);
    sf::Listener::setPosition(caracter->getCenter().x, caracter->getCenter().y, 0);
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
    World::drawOnComponents("E_WALL+E_DECOR+E_ANIMATION+E_CARACTER", 1);
    fpsCounter++;
    if (getClock("FPS").getElapsedTime() >= sf::seconds(1.f)) {
        std::cout<<"FPS : "<<fpsCounter<<std::endl;
        fpsCounter = 0;
        getClock("FPS").restart();
    }

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
    Entity& shadowMap = World::getShadowMap("E_WALL+E_DECOR+E_ANIMATION+E_CARACTER", 2, 1);
    window->draw(shadowMap, sf::BlendMultiply);
    //getView().rotate(0, 0, 20);
    window->draw(ps);
    //getView().rotate(0, 0, 0);
    Entity& lightMap = World::getLightMap("E_PONCTUAL_LIGHT", 2, 1);
    window->draw(lightMap, sf::BlendMultiply);
    /*Entity& normalMap = theMap->getNormalMapTile();
    window->draw(normalMap);*/
    /*std::vector<Vec2f> segments = caracter->getPath();
    VertexArray va(sf::LinesStrip);
    for (unsigned int i = 0; i < segments.size(); i++) {
        Vertex vertex1(sf::Vector3f(segments[i].x, segments[i].y, segments[i].y), sf::Color::Red);

        va.append(vertex1);

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
    /*std::vector<Entity*> entities = World::getVisibleEntities("E_BIGTILE");
    //std::cout<<entities.size()<<std::endl;
    for (unsigned int i = 0; i < entities.size(); i++) {
        window->draw(*entities[i]);
    }
    entities = World::getVisibleEntities("E_WALL+E_DECOR+E_ANIMATION+E_CARACTER");
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
        stop();
        eu->stop();
        au->stop();
        pfire.stop();
        std::vector<Entity*> entities = World::getEntities("E_BIGTILE+E_WALL+E_DECOR+E_ANIMATION");
        std::ofstream ofs("FichierDeSerialisation");
        OTextArchive oa(ofs);
        oa(entities);
        ofs.close();
    }
    if (event.type == sf::Event::KeyPressed) {
        previousKey = actualKey;
        actualKey = event.key.code;
        getListener().setCommandSlotParams("MoveAction", this, event.key.code);
    }
    if (event.type == sf::Event::KeyReleased && caracter->isMovingFromKeyboard()) {
        if (player.isPlaying())
            player.stop();

        caracter->setMoving(false);
        caracter->setIsMovingFromKeyboard(false);
        previousKey = event.key.code;
        actualKey = sf::Keyboard::Key::Unknown;
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
    sf::Int64 t = getClock("LoopTime").getElapsedTime().asMicroseconds() * caracter->getSpeed();
    if (caracter->isMoving()) {
        if (!player.isPlaying()) {
            player.play(true);
        }
        if (caracter->isMovingFromKeyboard()) {
            World::moveEntity(caracter, caracter->getDir().x * t * caracter->getSpeed(), caracter->getDir().y * t * caracter->getSpeed(),caracter->getDir().y * t * caracter->getSpeed());
            if (World::collide(caracter)) {
                World::moveEntity(caracter, -caracter->getDir().x * t * caracter->getSpeed(), -caracter->getDir().y * t * caracter->getSpeed(),-caracter->getDir().y * t * caracter->getSpeed());
            }
            for (unsigned int i = 0; i < getRenderComponentManager().getNbComponents(); i++) {
                View view = getRenderComponentManager().getRenderComponent(i)->getView();
                Vec3f d = caracter->getCenter() - view.getPosition();
                view.move(d.x, d.y, d.y);
                getRenderComponentManager().getRenderComponent(i)->setView(view);
            }
            Vec3f d = caracter->getCenter() - getView().getPosition();
            getView().move(d.x, d.y, d.y);
            Vec2f actualPos = Vec2f(caracter->getCenter().x, caracter->getCenter().y);
            sf::Listener::setPosition(actualPos.x, actualPos.y, 0);
            World::update();
        } else {
            Vec3f actualPos = caracter->getCenter();
            Vec2f pos = Computer::getPosOnPathFromTime(actualPos, caracter->getPath(),t,caracter->getSpeed());
            Vec2f d = pos - actualPos;
            Vec2f dir = d.normalize();
            World::moveEntity(caracter, d.x, d.y, d.y);
            if (dir != caracter->getDir())
                caracter->setDir(dir);
            for (unsigned int i = 0; i < getRenderComponentManager().getNbComponents(); i++) {
                View view = getRenderComponentManager().getRenderComponent(i)->getView();
                view.move(d.x, d.y, d.y);
                getRenderComponentManager().getRenderComponent(i)->setView(view);
            }
            getView().move(d.x, d.y, d.y);
            actualPos = Vec2f(caracter->getCenter().x, caracter->getCenter().y);
            sf::Listener::setDirection(dir.x, dir.y, 0);
            sf::Listener::setPosition(actualPos.x, actualPos.y, 0);
            if (actualPos.computeDist(caracter->getPath()[caracter->getPath().size() - 1]) <= 1.f) {
                caracter->setMoving(false);
                if (player.isPlaying())
                    player.stop();
            }
        }
        World::update();
    }
    ps.update(getClock("LoopTime").getElapsedTime());
}
