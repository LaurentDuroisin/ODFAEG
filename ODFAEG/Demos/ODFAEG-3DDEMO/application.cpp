#include "application.h"
#include "odfaeg/Math/distributions.h"
using namespace odfaeg::math;
using namespace odfaeg::physic;
using namespace odfaeg::core;
using namespace odfaeg::graphic;
MyAppli::MyAppli(Vec2f size, std::string title) :
    Application(sf::VideoMode(size.x, size.y), title, false, true, sf::Style::Default, sf::ContextSettings(0, 0, 4, 3, 0)) {
    //In perspective projection the x and y coordinates of the view are always between -1 and 1 with opengl.

    //Rotate the cube around a vector.

    //The default view have a perspective projection, but you can set another view with the setView function.
    View view(size.x, size.y, 80, 0.1f, 1000);
    view.move(0, 50, 0);
    billboard = new BillBoard(view, ps);
    view.setConstrains(0, 10);

    //getRenderWindow().setView(view);
    //getView().setPerspective(-size.x * 0.5f, size.x * 0.5f, -size.y * 0.5f, size.y * 0.5f, -1000, 1000);
    FastRenderComponent* frc = new FastRenderComponent(getRenderWindow(), 0, "E_BIGTILE+E_CUBE", false);
    frc->setView(view);

    //getView().setPerspective(-size.x * 0.5f, size.x * 0.5f, -size.y * 0.5f, size.y * 0.5f,-1000, 1000);
    getRenderComponentManager().addRenderComponent(frc);
    speed = 10.f;
    sensivity = 0.1f;
    oldX = sf::Mouse::getPosition(getRenderWindow()).x;
    oldY = sf::Mouse::getPosition(getRenderWindow()).y;
    verticalMotionActive = false;
    verticalMotionDirection = 0;
}
void MyAppli::onLoad() {
    TextureManager<TEXTURES> tm;
    tm.fromFileWithAlias("tilesets/Terre2.jpg", GRASS);
    tm.fromFileWithAlias("tilesets/particule.png", PARTICLE);
    cache.addResourceManager(tm, "TextureManager");
}
void MyAppli::onInit() {
    TextureManager<TEXTURES> &tm = cache.resourceManager<Texture, TEXTURES>("TextureManager");
    theMap = new Map(&getRenderComponentManager(), "Map test", 100, 100);
    BaseChangementMatrix bcm;
    theMap->setBaseChangementMatrix(bcm);
    World::addEntityManager(theMap);
    World::setCurrentEntityManager("Map test");
    cube = new g3d::Cube(Vec3f(-1, -1, -1), 2, 2, 2, sf::Color(255, 0, 0));
    cube->move(Vec3f(0.f, 0.f, 20.f));
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
    heightmap = static_cast<BigTile*>(World::getEntities("E_BIGTILE")[0]);

    ps.setTexture(*tm.getResourceByAlias(PARTICLE));
    for (unsigned int i = 0; i < 10; i++) {
        ps.addTextureRect(sf::IntRect(i*10, 0, 10, 10));
    }
    UniversalEmitter emitter;
    emitter.setEmissionRate(30);
    emitter.setParticleLifetime(Distributions::uniform(sf::seconds(5), sf::seconds(7)));
    emitter.setParticlePosition(Distributions::rect(Vec3f(0, 0, 50), Vec3f(10, 0, 100)));   // Emit particles in given circle
    emitter.setParticleVelocity(Distributions::deflect(Vec3f(0, 0, 10),  0)); // Emit towards direction with deviation of 15°
    emitter.setParticleRotation(Distributions::uniform(0.f, 0.f));
    emitter.setParticleTextureIndex(Distributions::uniformui(0, 9));
    emitter.setParticleScale(Distributions::rect(Vec3f(2.1f, 2.1f, 2.f), Vec3f(2.f, 2.f, 2.f)));
    ps.addEmitter(emitter);
    g2d::PonctualLight* light = new g2d::PonctualLight(Vec3f(0, 0, 10), 200, 200, 200, 255, sf::Color::Yellow, 16);
    World::addEntity(light);
    eu = new EntitiesUpdater(false);
    World::addEntitiesUpdater(eu);
    World::update();
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
    g2d::AmbientLight::getAmbientLight().setColor(sf::Color::Blue);
}
void MyAppli::onRender(FastRenderComponentManager* frcm) {
    World::drawOnComponents("E_BIGTILE+E_CUBE", 0);
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
    Entity& lightMap = World::getLightMap("E_PONCTUAL_LIGHT", 1, 0);
    window->draw(lightMap, sf::BlendMultiply);
    //std::cout<<()<<std::endl;

}
void MyAppli::onUpdate (sf::Event& event) {

        if (event.type == sf::Event::Closed)
        {
            stop();
        }
        if (event.type == sf::Event::Resized)
        {
            // Ajust the viewport size when the window is resized.
            getView().reset(BoundingBox(0, 0, getView().getViewport().getPosition().z,event.size.width, event.size.height, getView().getViewport().getDepth()));
        } else if (event.type == sf::Event::MouseMoved && sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
            int relX = (event.mouseMove.x - oldX) * sensivity;
            int relY = (event.mouseMove.y - oldY) * sensivity;
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
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
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
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
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
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
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
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
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
    ps.update(clock.getElapsedTime());
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
    oldX = sf::Mouse::getPosition(getRenderWindow()).x;
    oldY = sf::Mouse::getPosition(getRenderWindow()).y;
    clock.restart();
}
