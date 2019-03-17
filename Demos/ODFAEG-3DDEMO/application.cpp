#include "application.h"
#include "odfaeg/Math/distributions.h"
using namespace odfaeg::math;
using namespace odfaeg::physic;
using namespace odfaeg::core;
using namespace odfaeg::graphic;
using namespace odfaeg::window;
MyAppli::MyAppli(Vec2f size, std::string title) :
    Application(sf::VideoMode(size.x, size.y), title, sf::Style::Default, ContextSettings(0, 0, 4, 3, 0)) {
    //In perspective projection the x and y coordinates of the view are always between -1 and 1 with opengl.

    //Rotate the cube around a vector.

    //The default view have a perspective projection, but you can set another view with the setView function.
    View view(size.x, size.y, 80, 0.1f, 1000);
    view.move(0, 50, 0);
    ps = new ParticleSystem(Vec3f(0, 0, 0), Vec3f(100, 100, 100));
    billboard = new BillBoard(view, *ps);
    view.setConstrains(0, 10);
    //getRenderWindow().setView(view);
    //getView().setPerspective(-size.x * 0.5f, size.x * 0.5f, -size.y * 0.5f, size.y * 0.5f, -1000, 1000);
    OITRenderComponent* frc = new OITRenderComponent(getRenderWindow(), 0, "E_BIGTILE");
    frc->setView(view);
    /*ShadowRenderComponent* src = new ShadowRenderComponent(getRenderWindow(), 1, "E_CUBE");
    src->setView(view);*/
    OITRenderComponent* oit = new OITRenderComponent(getRenderWindow(), 1, "E_3DMODEL+E_CUBE");
    oit->setView(view);
    /*LightRenderComponent* lrc = new LightRenderComponent(getRenderWindow(), 3, "E_BIGTILE+E_CUBE+E_3DMODEL+E_PONCTUAL_LIGHT");
    lrc->setView(view);*/
    //getView().setPerspective(-size.x * 0.5f, size.x * 0.5f, -size.y * 0.5f, size.y * 0.5f,-1000, 1000);
    getRenderComponentManager().addComponent(frc);
    //getRenderComponentManager().addComponent(src);
    getRenderComponentManager().addComponent(oit);
    //getRenderComponentManager().addComponent(lrc);
    speed = 10.f;
    sensivity = 0.1f;
    oldX = IMouse::getPosition(getRenderWindow()).x;
    oldY = IMouse::getPosition(getRenderWindow()).y;
    verticalMotionActive = false;
    verticalMotionDirection = 0;
    fpsCounter = 0;
    addClock(sf::Clock(), "FPS");
}
void MyAppli::onLoad() {
    TextureManager<TEXTURES> tm;
    tm.fromFileWithAlias("tilesets/Terre2.jpg", GRASS);
    tm.fromFileWithAlias("tilesets/particule.png", PARTICLE);
    cache.addResourceManager(tm, "TextureManager");
}
void MyAppli::onInit() {
    TextureManager<TEXTURES> &tm = cache.resourceManager<Texture, TEXTURES>("TextureManager");
    theMap = new Map(&getRenderComponentManager(), "Map test", 100, 100, 0);
    BaseChangementMatrix bcm;
    //bcm.set3DMatrix();
    theMap->setBaseChangementMatrix(bcm);
    World::addEntityManager(theMap);
    World::setCurrentEntityManager("Map test");
    cube = new g3d::Cube(Vec3f(-1, -1, -1), 2, 2, 2, sf::Color(255, 0, 0));
    cube->move(Vec3f(0.f, 0.f, 50));
    cube->rotate(45, Vec3f(0, 0, 1));
    cube->setOrigin(Vec3f(0, 0, 0));
    cube->scale(Vec3f(10, 10, 10));
    World::addEntity(cube);
    std::vector<Tile*> tGround;
    std::vector<Tile*> tWall;
    Texture* text = const_cast<Texture*>(tm.getResourceByAlias(GRASS));
    text->setRepeated(true);
    text->loadFromFile("tilesets/Terre2.jpg");
    text->setSmooth(true);
    tGround.push_back(new Tile(text, Vec3f(0, 0, 0), Vec3f(50, 50, 0),sf::IntRect(0, 0, 100*20, 100*20)));
    BoundingBox mapZone (-500, -500, 0, 1000, 1000, 50);
    World::generate_map(tGround, tWall, Vec2f(50, 50), mapZone, true);
    heightmap = static_cast<BigTile*>(World::getEntities("E_BIGTILE")[0]->getRootEntity());
    ps->setTexture(*tm.getResourceByAlias(PARTICLE));
    for (unsigned int i = 0; i < 10; i++) {
        ps->addTextureRect(sf::IntRect(i*10, 0, 10, 10));
    }
    UniversalEmitter emitter;
    emitter.setEmissionRate(30);
    emitter.setParticleLifetime(Distributions::uniform(sf::seconds(5), sf::seconds(7)));
    emitter.setParticlePosition(Distributions::rect(Vec3f(0, 0, 50), Vec3f(10, 0, 100)));   // Emit particles in given circle
    emitter.setParticleVelocity(Distributions::deflect(Vec3f(0, 0, 10),  0)); // Emit towards direction with deviation of 15°
    emitter.setParticleRotation(Distributions::uniform(0.f, 0.f));
    emitter.setParticleTextureIndex(Distributions::uniformui(0, 9));
    emitter.setParticleScale(Distributions::rect(Vec3f(2.1f, 2.1f, 2.f), Vec3f(2.f, 2.f, 2.f)));
    ps->addEmitter(emitter);
    g2d::PonctualLight* light = new g2d::PonctualLight(Vec3f(0, 0, 10), 200, 200, 200, 255, sf::Color::Yellow, 16);
    World::addEntity(light);
    eu = new EntitiesUpdater();
    World::addWorker(eu);

    for (unsigned int i = 0; i < getRenderComponentManager().getNbComponents(); i++) {
        View view = getRenderComponentManager().getRenderComponent(i)->getView();
        float z = heightmap->getHeight(Vec2f(view.getPosition().x, view.getPosition().y));
        view.setCenter(Vec3f(view.getPosition().x, view.getPosition().y, z+20));
        getRenderComponentManager().getRenderComponent(i)->setView(view);
    }
    View view = billboard->getView();
    float z = heightmap->getHeight(Vec2f(view.getPosition().x, view.getPosition().y));
    view.setCenter(Vec3f(view.getPosition().x, view.getPosition().y, z+20));
    billboard->setView(view);
    billboard->setCenter(Vec3f(0, 0, z+20));
    g2d::AmbientLight::getAmbientLight().setColor(sf::Color::White);
    Entity* model = new g3d::Model("tilesets/mesh_puddingmill/puddingmill.obj", Vec3f(0, 0, 0));
    model->move(Vec3f(0, 0, 20));
    World::addEntity(model);
    World::update();
}
void MyAppli::onRender(RenderComponentManager* frcm) {
    World::drawOnComponents("E_BIGTILE", 0);
    //World::drawOnComponents("E_CUBE", 1);
    World::drawOnComponents("E_3DMODEL+E_CUBE", 1);
    //World::drawOnComponents("E_BIGTILE+E_CUBE+E_3DMODEL+E_PONCTUAL_LIGHT", 3);
    fpsCounter++;
    if (getClock("FPS").getElapsedTime() >= sf::seconds(1.f)) {
        std::cout<<"FPS : "<<fpsCounter<<std::endl;
        fpsCounter = 0;
        getClock("FPS").restart();
    }
    /*Entity& lightMap = World::getLightMap("E_PONCTUAL_LIGHT", 1, 0);
    World::drawOnComponents(lightMap, 0, sf::BlendMultiply);*/
    //World::drawOnComponents(*billboard, 0);
    /*std::vector<Entity*> entities = World::getVisibleEntities("E_BIGTILE+E_CUBE");
    frcm->getRenderComponent(0)->loadEntitiesOnComponent(entities);
    frcm->getRenderComponent(0)->drawNextFrame();
    frcm->getRenderComponent(0)->getFrameBufferTile().setCenter(getRenderWindow().getView().getPosition());*/
}
void MyAppli::onDisplay(RenderWindow* window) {
    /*std::vector<Entity*> entities = World::getVisibleEntities("E_BIGTILE+E_CUBE");
    for (unsigned int i = 0; i < entities.size(); i++) {
        window->draw(*entities[i]);
    }*/
    /*Entity* lightMap = World::getLightMap("E_PONCTUAL_LIGHT", 1, 0);
    window->draw(*lightMap, sf::BlendMultiply);*/
    //std::cout<<()<<std::endl;

}
void MyAppli::onUpdate (RenderWindow* window, IEvent& event) {

        if (event.type == IEvent::WINDOW_EVENT && event.window.type == IEvent::WINDOW_EVENT_CLOSED)
        {
            stop();
        }
        if (event.type == IEvent::WINDOW_EVENT && event.window.type == IEvent::WINDOW_EVENT_RESIZED)
        {
            // Ajust the viewport size when the window is resized.
            getView().reset(BoundingBox(0, 0, getView().getViewport().getPosition().z,event.window.data1, event.window.data2, getView().getViewport().getDepth()));
        } else if (event.type == IEvent::MOUSE_MOTION_EVENT && IMouse::isButtonPressed(IMouse::Right)) {
            int relX = (event.mouseMotion.x - oldX) * sensivity;
            int relY = (event.mouseMotion.y - oldY) * sensivity;
            //Rotate the view, (Polar coordinates) but you can also use the lookAt function to look at a point.
            for (unsigned int i = 0; i < getRenderComponentManager().getNbComponents(); i++) {
                View view = getRenderComponentManager().getRenderComponent(i)->getView();
                int teta = view.getTeta() - relX;
                int phi = view.getPhi() - relY;
                view.rotate(teta, phi);
                getRenderComponentManager().getRenderComponent(i)->setView(view);
            }
            View view = billboard->getView();
            int teta = view.getTeta() - relX;
            int phi = view.getPhi() - relY;
            view.rotate(teta, phi);
            billboard->setView(view);

            World::update();
        } /*else if (event.type == sf::Event::MouseWheelMoved) {
            if (event.mouseWheel.delta > 0) {
                verticalMotionActive = true;
                verticalMotionDirection = 1;
                timeBeforeStoppingVerticalMotion = sf::milliseconds(250);
                clock2.restart();
                World::update();
            } else if (event.mouseWheel.delta < 0) {
                verticalMotionActive = true;
                verticalMotionDirection = -1;
                timeBeforeStoppingVerticalMotion = sf::milliseconds(250);
                clock2.restart();
                World::update();
            }

        }*/
}
void MyAppli::onExec() {
    if (IKeyboard::isKeyPressed(IKeyboard::Up)) {
        std::cout<<"key up pressed"<<std::endl;
        //Move the view along a vector, but you case also move the view at a point.
        for (unsigned int i = 0; i < getRenderComponentManager().getNbComponents(); i++) {
            View view = getRenderComponentManager().getRenderComponent(i)->getView();
            view.move(view.getForward(), -speed * clock.getElapsedTime().asSeconds());
            float z = heightmap->getHeight(Vec2f(view.getPosition().x, view.getPosition().y));
            view.setCenter(Vec3f(view.getPosition().x, view.getPosition().y, z+20));
            getRenderComponentManager().getRenderComponent(i)->setView(view);
        }
        View view = billboard->getView();
        view.move(view.getForward(), -speed * clock.getElapsedTime().asSeconds());
        float z = heightmap->getHeight(Vec2f(view.getPosition().x, view.getPosition().y));
        view.setCenter(Vec3f(view.getPosition().x, view.getPosition().y, z+20));
        billboard->setView(view);
        billboard->setCenter(Vec3f(0, 0, z+20));
        /*View view = getRenderWindow().getView();
        view.move(view.getForward(), -speed * clock.getElapsedTime().asSeconds());
        float z = heightmap->getHeight(Vec2f(view.getPosition().x, view.getPosition().y));
        view.setCenter(Vec3f(view.getPosition().x, view.getPosition().y, z+20));
        getRenderWindow().setView(view);*/
        World::update();
    }
    if (IKeyboard::isKeyPressed(IKeyboard::Down)) {
        for (unsigned int i = 0; i < getRenderComponentManager().getNbComponents(); i++) {
            View view = getRenderComponentManager().getRenderComponent(i)->getView();
            view.move(view.getForward(), speed * clock.getElapsedTime().asSeconds());
            float z = heightmap->getHeight(Vec2f(view.getPosition().x, view.getPosition().y));
            view.setCenter(Vec3f(view.getPosition().x, view.getPosition().y, z+20));
            getRenderComponentManager().getRenderComponent(i)->setView(view);
        }
        View view = billboard->getView();
        view.move(view.getForward(), -speed * clock.getElapsedTime().asSeconds());
        float z = heightmap->getHeight(Vec2f(view.getPosition().x, view.getPosition().y));
        view.setCenter(Vec3f(view.getPosition().x, view.getPosition().y, z+20));
        billboard->setView(view);
        billboard->setCenter(Vec3f(0, 0, z+20));
        /*View view = getRenderWindow().getView();
        view.move(view.getForward(), speed * clock.getElapsedTime().asSeconds());
        float z = heightmap->getHeight(Vec2f(view.getPosition().x, view.getPosition().y));
        view.setCenter(Vec3f(view.getPosition().x, view.getPosition().y, z+20));
        getRenderWindow().setView(view);*/
        World::update();
    }
    if (IKeyboard::isKeyPressed(IKeyboard::Right)) {
        for (unsigned int i = 0; i < getRenderComponentManager().getNbComponents(); i++) {
            View view = getRenderComponentManager().getRenderComponent(i)->getView();
            view.move(view.getLeft(), speed * clock.getElapsedTime().asSeconds());
            float z = heightmap->getHeight(Vec2f(view.getPosition().x, view.getPosition().y));
            view.setCenter(Vec3f(view.getPosition().x, view.getPosition().y, z+20));
            getRenderComponentManager().getRenderComponent(i)->setView(view);
        }
        View view = billboard->getView();
        view.move(view.getForward(), -speed * clock.getElapsedTime().asSeconds());
        float z = heightmap->getHeight(Vec2f(view.getPosition().x, view.getPosition().y));
        view.setCenter(Vec3f(view.getPosition().x, view.getPosition().y, z+20));
        billboard->setView(view);
        billboard->setCenter(Vec3f(0, 0, z+20));
        /*View view = getRenderWindow().getView();
        view.move(view.getLeft(), speed * clock.getElapsedTime().asSeconds());
        float z = heightmap->getHeight(Vec2f(view.getPosition().x, view.getPosition().y));
        view.setCenter(Vec3f(view.getPosition().x, view.getPosition().y, z+20));
        getRenderWindow().setView(view);*/
        World::update();
    }
    if (IKeyboard::isKeyPressed(IKeyboard::Left)) {
        for (unsigned int i = 0; i < getRenderComponentManager().getNbComponents(); i++) {
            View view = getRenderComponentManager().getRenderComponent(i)->getView();
            view.move(view.getLeft(), -speed * clock.getElapsedTime().asSeconds());
            float z = heightmap->getHeight(Vec2f(view.getPosition().x, view.getPosition().y));
            view.setCenter(Vec3f(view.getPosition().x, view.getPosition().y, z+20));
            getRenderComponentManager().getRenderComponent(i)->setView(view);
        }
        View view = billboard->getView();
        view.move(view.getForward(), -speed * clock.getElapsedTime().asSeconds());
        float z = heightmap->getHeight(Vec2f(view.getPosition().x, view.getPosition().y));
        view.setCenter(Vec3f(view.getPosition().x, view.getPosition().y, z+20));
        billboard->setView(view);
        billboard->setCenter(Vec3f(0, 0, z+20));
        /*View view = getRenderWindow().getView();
        view.move(view.getForward(), -speed * clock.getElapsedTime().asSeconds());
        float z = heightmap->getHeight(Vec2f(view.getPosition().x, view.getPosition().y));
        view.setCenter(Vec3f(view.getPosition().x, view.getPosition().y, z+20));
        getRenderWindow().setView(view);*/
        World::update();
    }
    ps->update(clock.getElapsedTime());
    /*if (clock2.getElapsedTime() > timeBeforeStoppingVerticalMotion) {
        verticalMotionActive = false;
        verticalMotionDirection = 0;
    } else {
        timeBeforeStoppingVerticalMotion -= clock2.getElapsedTime();
    }
    for (unsigned int i = 0; i < getRenderComponentManager().getNbComponents(); i++) {
        View view = getRenderComponentManager().getRenderComponent(i)->getView();
        view.move(0, 0, -verticalMotionDirection * speed * clock2.getElapsedTime().asSeconds());
        getRenderComponentManager().getRenderComponent(i)->setView(view);
    }*/
    oldX = IMouse::getPosition(getRenderWindow()).x;
    oldY = IMouse::getPosition(getRenderWindow()).y;
    clock.restart();
}
