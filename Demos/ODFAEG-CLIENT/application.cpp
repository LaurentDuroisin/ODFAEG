#include "application.h"
using namespace odfaeg::core;
using namespace odfaeg::graphic;
using namespace odfaeg::graphic::gui;
using namespace odfaeg::physic;
using namespace odfaeg::math;
using namespace odfaeg::network;
using namespace odfaeg::window;
namespace sorrok {
    MyAppli::MyAppli(sf::VideoMode wm, std::string title) : Application (wm, title, sf::Style::Default, ContextSettings(0, 0, 4, 3, 0)) {
        running = false;
        actualKey = IKeyboard::Key::Unknown;
        previousKey = IKeyboard::Key::Unknown;
        getView().setScale(1, -1, 1);
        sf::Clock clock1;
        addClock(clock1, "RequestTime");
        hero = nullptr;
        monster = nullptr;
        received = false;
        Network::setCertifiateClientMess("SORROKCLIENT");
        isClientAuthentified = false;
    }
    void MyAppli::onIconClicked(Icon* icon) {
        TextureManager<Item::Type> &tm2 = cache.resourceManager<Texture, Item::Type>("TextureManager2");
        Item::Type itemType = tm2.getAliasByResource(const_cast<Texture*>(icon->getSprite().getTexture()))[0];
        std::map<Item::Type, std::vector<Item>>& items = static_cast<Hero*>(hero)->getInventory();
        std::map<Item::Type, std::vector<Item>>::iterator it = items.find(itemType);
        Item item = it->second.back();
        if (itemType == Item::HP_POTION) {
            it->second.pop_back();
            Label* label;
            bool found = false;
            for (unsigned int i = 0; i < pInventory->getChildren().size() && !found; i+=2) {
                Icon* icn = dynamic_cast<Icon*> (pInventory->getChildren()[i]);
                label = dynamic_cast<Label*> (pInventory->getChildren()[i+1]);
                if (icon == icn) {
                    label->setText(conversionIntString(it->second.size()));
                    found = true;
                }
            }
            if (it->second.empty()) {
                for (unsigned int i = 0; i < pInventory->getChildren().size(); i++) {
                    if (pInventory->getChildren()[i] == icon) {
                        pInventory->removeChild(*icon);
                        pInventory->removeChild(*label);
                    }
                }
                items.erase(it);
            }
            ItemAction* ia = new ItemAction();
            FastDelegate<void> action(&ItemAction::useHpPotion, ia, static_cast<Hero*>(hero), item);
            gameActions.push_back(std::make_pair(static_cast<Hero*>(hero)->getJobVariant(), std::make_pair(item, action)));
            itemActions.push_back(ia);
        }
    }
    void MyAppli::showInventory() {
        wInventory->setVisible(true);
        std::map<Item::Type, std::vector<Item>>& items = static_cast<Hero*>(hero)->getInventory();
        TextureManager<Item::Type> &tm2 = cache.resourceManager<Texture, Item::Type>("TextureManager2");
        FontManager<Fonts> &fm = cache.resourceManager<Font, Fonts>("FontManager");
        const Font* font = fm.getResourceByAlias(Fonts::Serif);
        unsigned int nbRows = 1 + items.size() / 10;
        unsigned int rowIndex = 0, colIndex = 0;
        Table table(4, 10);
        std::map<Item::Type, std::vector<Item>>::iterator it;
        for(it = items.begin(); it != items.end(); it++) {
            Sprite sprite (*tm2.getResourceByAlias(it->first),pItems->getPosition(),Vec3f(50, 50, 0), sf::IntRect(0, 0, 50, 50));
            Icon* icon = new Icon(*wInventory,Vec3f(0, 0, 0),Vec3f(0, 0, 0),sprite);
            icon->setName("HPPOTION_ICON");
            Action a (Action::MOUSE_BUTTON_PRESSED_ONCE, IMouse::Left);
            Command cmd(a, FastDelegate<bool>(&Icon::isMouseInside, icon), FastDelegate<void>(&MyAppli::onIconClicked, this, icon));
            icon->getListener().connect("UseItem"+conversionIntString(it->first), cmd);
            Label* label = new Label(*wInventory,Vec3f(0, 0, 0),Vec3f(0, 0 ,0),font,conversionUIntString(it->second.size()),10);
            label->setBackgroundColor(sf::Color::Transparent);
            table.addElement(icon, rowIndex, colIndex);
            table.addElement(label, rowIndex, colIndex);
            colIndex++;
            if (colIndex == 10) {
                rowIndex++;
                colIndex = 0;
            }
            pInventory->addChild(icon);
            pInventory->addChild(label);
            icon->setParent(pInventory);
            label->setParent(pInventory);
        }
    }
    void MyAppli::dropItems(Label* label) {
        std::vector<Item>::iterator it;
        for (it = selectedCristal.second.begin(); it != selectedCristal.second.end(); ) {
            if (it->getName() == label->getText()) {
                static_cast<Hero*>(hero)->addItem(*it);
                it = selectedCristal.second.erase(it);
            } else {
                it++;
            }
        }
        std::vector<Item> itemsToDisplay = selectedCristal.second;
        pItems->removeAll();
        if (itemsToDisplay.size() > 0) {
            FontManager<Fonts> &fm = cache.resourceManager<Font,Fonts>("FontManager");
            Table table (4,2);
            TextureManager<Item::Type> &tm2 = cache.resourceManager<Texture, Item::Type>("TextureManager2");
            for (unsigned int i = 0; i < itemsToDisplay.size(); i++) {
                Label* itemName = new Label(*wPickupItems, Vec3f(0, 0, 0), Vec3f(0, 0, 0), fm.getResourceByAlias(Fonts::Serif), itemsToDisplay[i].getName(),15);
                Action a (Action::EVENT_TYPE::MOUSE_BUTTON_PRESSED_ONCE, IMouse::Left);
                Command cmd (a, FastDelegate<bool>(&Label::isMouseInside, itemName), FastDelegate<void>(&MyAppli::dropItems, this, itemName));
                itemName->getListener().connect("PICKUPITEM", cmd);
                itemName->setParent(pItems);
                table.addElement(itemName,i, 1);
                Sprite sprite (*tm2.getResourceByAlias(Item::HP_POTION),pItems->getPosition(),Vec3f(50, 50, 0), sf::IntRect(0, 0, 50, 50));
                Icon* icon = new Icon(*wPickupItems,Vec3f(0, 0, 0),Vec3f(0, 0, 0),sprite);
                icon->setParent(pItems);
                table.addElement(icon,i,0);
                pItems->addChild(itemName);
                pItems->addChild(icon);
            }
        } else {
            std::vector<std::pair<Sprite*, std::vector<Item>>>::iterator it;
            for (it = cristals.begin(); it != cristals.end();) {
                if (it->first == selectedCristal.first) {
                    delete it->first;
                    it = cristals.erase(it);
                } else {
                    it++;
                }
            }
        }
    }
    void MyAppli::pickUpItems (IKeyboard::Key key) {
        if (key != IKeyboard::Key::Unknown && key == IKeyboard::Key::A) {
            std::vector<std::pair<Sprite*, std::vector<Item>>>::iterator it, itf;
            std::vector<Item> itemsToDisplay;
            if (cristals.size() > 0) {
                itf = cristals.begin();
                float minDist = itf->first->getPosition().computeDist(hero->getPosition());
                if (minDist < 100) {
                    itemsToDisplay = itf->second;
                    selectedCristal = *itf;
                }
                itf++;
                for (it = itf; it != cristals.end(); it++) {
                    float dist = it->first->getPosition().computeDist(hero->getPosition());
                    if (dist < 100
                        && dist < minDist) {
                        itemsToDisplay = it->second;
                        minDist = dist;
                        selectedCristal = *it;
                    }
                }
                wPickupItems->setPosition(sf::Vector2i(selectedCristal.first->getPosition().x, selectedCristal.first->getPosition().y));
            }
            if (itemsToDisplay.size() > 0) {
                FontManager<Fonts> &fm = cache.resourceManager<Font,Fonts>("FontManager");
                TextureManager<Item::Type> &tm2 = cache.resourceManager<Texture, Item::Type>("TextureManager2");
                Table table (4,2);
                for (unsigned int i = 0; i < itemsToDisplay.size(); i++) {
                    Label* itemName = new Label(*wPickupItems, Vec3f(0, 0, 0), Vec3f(0, 0, 0), fm.getResourceByAlias(Fonts::Serif), itemsToDisplay[i].getName(),15);
                    Action a (Action::EVENT_TYPE::MOUSE_BUTTON_PRESSED_ONCE, IMouse::Left);
                    Command cmd (a, FastDelegate<bool>(&Label::isMouseInside, itemName), FastDelegate<void>(&MyAppli::dropItems, this, itemName));
                    itemName->getListener().connect("PICKUPITEM", cmd);
                    itemName->setParent(pItems);
                    table.addElement(itemName,i, 1);
                    Sprite sprite (*tm2.getResourceByAlias(Item::HP_POTION),pItems->getPosition(),Vec3f(50, 50, 0), sf::IntRect(0, 0, 50, 50));
                    Icon* icon = new Icon(*wPickupItems,Vec3f(0, 0, 0),Vec3f(0, 0, 0),sprite);
                    icon->setParent(pItems);
                    table.addElement(icon,i,0);
                    pItems->addChild(itemName);
                    pItems->addChild(icon);
                }
                wPickupItems->setVisible(true);
            }
        }
    }
    void MyAppli::keyHeldDown (IKeyboard::Key key) {
        //BoundingRectangle rect (pos.x, pos.y, getView().getSize().x, getView().getSize().y);
        if (actualKey != IKeyboard::Key::Unknown && key == IKeyboard::Key::Z) {
            if (!hero->isMoving()) {
                //if (actualKey != previousKey) {
                    Vec2f dir(0, -1);
                    hero->setDir(dir);
                    sf::Int64 cli_time = Application::getTimeClk().getElapsedTime().asMicroseconds();
                    hero->setIsMovingFromKeyboard(true);
                    std::string message = "MOVEFROMKEYBOARD*"+conversionIntString(hero->getId())+"*"+conversionFloatString(dir.x)+"*"+conversionFloatString(dir.y)+"*"+conversionLongString(cli_time);
                    SymEncPacket packet;
                    packet<<message;
                    Network::sendTcpPacket(packet);
                //}

            }
        } else if (actualKey != IKeyboard::Key::Unknown && key == IKeyboard::Key::Q) {
            if (!hero->isMoving()) {
                //if (actualKey != previousKey) {
                    Vec2f dir(-1, 0);
                    hero->setDir(dir);
                    sf::Int64 cli_time = Application::getTimeClk().getElapsedTime().asMicroseconds();
                    hero->setIsMovingFromKeyboard(true);
                    std::string message = "MOVEFROMKEYBOARD*"+conversionIntString(hero->getId())+"*"+conversionFloatString(dir.x)+"*"+conversionFloatString(dir.y)+"*"+conversionLongString(cli_time);
                    SymEncPacket packet;
                    packet<<message;
                    Network::sendTcpPacket(packet);
                //}
            }
        } else if (actualKey != IKeyboard::Key::Unknown && actualKey == IKeyboard::Key::S) {
            if (!hero->isMoving()) {
                //if (actualKey != previousKey) {
                    Vec2f dir(0, 1);
                    hero->setDir(dir);
                    sf::Int64 cli_time = Application::getTimeClk().getElapsedTime().asMicroseconds();
                    hero->setIsMovingFromKeyboard(true);
                    std::string message = "MOVEFROMKEYBOARD*"+conversionIntString(hero->getId())+"*"+conversionFloatString(dir.x)+"*"+conversionFloatString(dir.y)+"*"+conversionLongString(cli_time);
                    SymEncPacket packet;
                    packet<<message;
                    Network::sendTcpPacket(packet);
                //}
            }
        } else if (actualKey != IKeyboard::Key::Unknown && key == IKeyboard::Key::D) {
            if (!hero->isMoving()) {
                //if (actualKey != previousKey) {
                    Vec2f dir(1, 0);
                    hero->setDir(dir);
                    sf::Int64 cli_time = Application::getTimeClk().getElapsedTime().asMicroseconds();
                    hero->setIsMovingFromKeyboard(true);
                    std::string message = "MOVEFROMKEYBOARD*"+conversionIntString(hero->getId())+"*"+conversionFloatString(dir.x)+"*"+conversionFloatString(dir.y)+"*"+conversionLongString(cli_time);
                    SymEncPacket packet;
                    packet<<message;
                    Network::sendTcpPacket(packet);
                //}
            }
        }
    }
    void MyAppli::leftMouseButtonPressed(sf::Vector2f mousePos) {
        Vec3f finalPos(mousePos.x, getRenderWindow().getSize().y - mousePos.y, 0);
        finalPos = getRenderWindow().mapPixelToCoords(finalPos);
        finalPos = Vec3f(finalPos.x, finalPos.y, 0);
        std::string message = "MOVEFROMPATH*"+conversionIntString(hero->getId())+"*"+conversionFloatString(finalPos.x)+"*"+conversionFloatString(finalPos.y);
        SymEncPacket packet;
        packet<<message;
        Network::sendTcpPacket(packet);
    }
    void MyAppli::rightMouseButtonPressed(sf::Vector2f mousePos) {
        Vec3f finalPos (mousePos.x, getRenderWindow().getSize().y - mousePos.y, 0);
        finalPos = getRenderWindow().mapPixelToCoords(finalPos);
        finalPos = Vec3f(finalPos.x, finalPos.y, 0);
        int id = hero->getId();
        std::string message = "ATTACK*"+conversionIntString(id)+"*"+conversionFloatString(finalPos.x)+"*"+conversionFloatString(finalPos.y)+"*"+conversionLongString(Application::getTimeClk().getElapsedTime().asMicroseconds());
        SymEncPacket packet;
        packet<<message;
        Network::sendTcpPacket(packet);
    }
    bool MyAppli::mouseInside (sf::Vector2f mousePos) {
        BoundingBox bx (0, 0, 0, 100, 100, 0);
        if (bx.isPointInside(Vec3f(mousePos.x, mousePos.y, 0))) {
            return true;
        }
        return false;
    }
    void MyAppli::onLoad() {
        TextureManager<> tm;
        tm.fromFileWithAlias("tilesets/herbe.png", "GRASS");
        tm.fromFileWithAlias("tilesets/murs.png", "WALLS");
        tm.fromFileWithAlias("tilesets/maison.png", "HOUSE");
        tm.fromFileWithAlias("tilesets/flemmes1.png", "FIRE1");
        tm.fromFileWithAlias("tilesets/flemmes2.png", "FIRE2");
        tm.fromFileWithAlias("tilesets/flemmes3.png", "FIRE3");
        tm.fromFileWithAlias("tilesets/cristal.png", "CRISTAL");
        TextureManager<Item::Type> tm2;
        tm2.fromFileWithAlias("tilesets/hppotion-icon.png", Item::HP_POTION);
        FontManager<Fonts> fm;
        fm.fromFileWithAlias("fonts/FreeSerif.ttf", Serif);
        cache.addResourceManager(fm, "FontManager");
        cache.addResourceManager(tm, "TextureManager");
        cache.addResourceManager(tm2, "TextureManager2");
        //shader.loadFromFile("Shaders/SimpleVertexShader.vertexshader", "Shaders/SimpleFragmentShader.fragmentshader");
    }
    void MyAppli::onInit () {
        FontManager<Fonts>& fm = cache.resourceManager<Font, Fonts>("FontManager");
        Network::startCli(10'000, 10'001,sf::IpAddress::LocalHost);
        TextureManager<> &tm = cache.resourceManager<Texture, std::string>("TextureManager");
        Vec2f pos (getView().getPosition().x - getView().getSize().x * 0.5f, getView().getPosition().y - getView().getSize().y * 0.5f);
        BoundingBox bx (pos.x, pos.y, 0, getView().getSize().x, getView().getSize().y, 0);
        theMap = new Map(&getRenderComponentManager(), "Map test", 100, 50, 0);
        BaseChangementMatrix bcm;
        bcm.set2DIsoMatrix();
        theMap->setBaseChangementMatrix(bcm);
        World::addEntityManager(theMap);
        World::setCurrentEntityManager("Map test");
        eu = new EntitiesUpdater();
        World::addWorker(eu);
        au = new AnimUpdater();
        au->setInterval(sf::seconds(0.01f));
        World::addTimer(au);
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

        //}

        //caracter->setCenter(Vec3f(getView().getPosition().x, getView().getPosition().y, 300));
        g2d::PonctualLight* light1 = new g2d::PonctualLight(Vec3f(0, 420, 420), 100, 50, 50, 255, sf::Color::Yellow, 16);
        light2 = new g2d::PonctualLight(Vec3f(50, 160, 160), 100, 50, 50, 255, sf::Color::Yellow, 16);
        World::addEntity(light1);
        World::addEntity(light2);
        ZSortingRenderComponent *frc1 = new ZSortingRenderComponent(getRenderWindow(),0, "",*theMap);
        ShadowRenderComponent *frc2 = new ShadowRenderComponent(getRenderWindow(),1, "");
        PerPixelLinkedListRenderComponent* frc3 = new PerPixelLinkedListRenderComponent(getRenderWindow(),2,"", ContextSettings(24, 0, 0, 3, 0));
        LightRenderComponent* frc4 = new LightRenderComponent(getRenderWindow(),3,"");
        /*View view = getView();
        frc1->setView(view);*/
        /*frc2->setView(view);
        frc3->setView(view);
        frc4->setView(view);*/
        /*frc1->setRelPosition(0.f, 0.f);
        frc1->setRelSize(1.f, 1.f);
        /*frc2->setRelPosition(0.f, 0.f);
        frc2->setRelSize(1.f, 1.f);
        frc3->setRelPosition(0.f, 0.f);
        frc3->setRelSize(1.f, 1.f);
        frc4->setRelPosition(0.f, 0.f);
        frc4->setRelSize(1.f, 1.f);*/
        getRenderComponentManager().addComponent(frc1);
        getRenderComponentManager().addComponent(frc2);
        getRenderComponentManager().addComponent(frc3);
        getRenderComponentManager().addComponent(frc4);
        //getView().move(d.x * 0.5f, d.y * 0.5f, 0);
        //World::computeIntersectionsWithWalls();
        //World::update();
        //World::computeIntersectionsWithWalls();
        Action a1 (Action::EVENT_TYPE::KEY_HELD_DOWN, IKeyboard::Key::Z);
        Action a2 (Action::EVENT_TYPE::KEY_HELD_DOWN, IKeyboard::Key::Q);
        Action a3 (Action::EVENT_TYPE::KEY_HELD_DOWN, IKeyboard::Key::S);
        Action a4 (Action::EVENT_TYPE::KEY_HELD_DOWN, IKeyboard::Key::D);
        Action a5 (Action::EVENT_TYPE::MOUSE_BUTTON_PRESSED_ONCE, IMouse::Left);
        Action a6 (Action::EVENT_TYPE::MOUSE_BUTTON_PRESSED_ONCE, IMouse::Right);
        Action combined  = a1 || a2 || a3 || a4;
        Command moveAction(combined, FastDelegate<void>(&MyAppli::keyHeldDown, this, IKeyboard::Key::Unknown));
        getListener().connect("MoveAction", moveAction);
        g2d::AmbientLight::getAmbientLight().setColor(sf::Color(0, 0, 255));
        Command leftMouseButtonPressedCommand (a5, FastDelegate<void>(&MyAppli::leftMouseButtonPressed, this, sf::Vector2f(-1, -1)));
        Command rightMouseButtonPressedCommand (a6, FastDelegate<void>(&MyAppli::rightMouseButtonPressed, this, sf::Vector2f(-1, -1)));
        getListener().connect("LeftMouseButtonPressedAction", leftMouseButtonPressedCommand);
        getListener().connect("RightMouseButtonPressedAction", rightMouseButtonPressedCommand);
        Action aPickUpItems (Action::EVENT_TYPE::KEY_PRESSED_ONCE, IKeyboard::Key::A);
        Command cmdPickUpItems(aPickUpItems,FastDelegate<void>(&MyAppli::pickUpItems, this, IKeyboard::Key::Unknown));
        getListener().connect("PickUpItems", cmdPickUpItems);
        Action aShowInventory(Action::KEY_PRESSED_ONCE, IKeyboard::Key::I);
        Command cmdShowInventory(aShowInventory, FastDelegate<void>(&MyAppli::showInventory, this));
        getListener().connect("ShowInventory", cmdShowInventory);
        wResuHero = new RenderWindow (sf::VideoMode(400, 300), "Create ODFAEG Application", sf::Style::Titlebar, ContextSettings(0, 0, 4, 3, 0));
        label = new gui::Label(*wResuHero, Vec3f(0, 0, 0), Vec3f(200, 50, 0),fm.getResourceByAlias(Fonts::Serif),"5", 15);
        getRenderComponentManager().addComponent(label);
        button = new gui::Button(Vec3f(0, 200, 0), Vec3f(400, 100, 0), fm.getResourceByAlias(Fonts::Serif),"Respawn", 15, *wResuHero);
        getRenderComponentManager().addComponent(button);
        button->setEventContextActivated(false);
        button->addActionListener(this);
        wResuHero->setVisible(false);
        wResuHero->setPosition(sf::Vector2i(500, 400));
        wPickupItems = new RenderWindow(sf::VideoMode(100, 200), "PickupItems", sf::Style::Close, ContextSettings(0, 0, 4, 3, 0));
        addWindow(wPickupItems);
        wPickupItems->setVisible(false);
        World::update();
        addWindow(wResuHero);
        pItems = new Panel(*wPickupItems,Vec3f(0, 0, 0),Vec3f(100, 200, 0));
        getRenderComponentManager().addComponent(pItems);
        wInventory = new RenderWindow(sf::VideoMode(500, 300), "Inventory", sf::Style::Close, ContextSettings(0, 0, 4, 3, 0));
        addWindow(wInventory);
        wInventory->setVisible(false);
        pInventory = new Panel(*wInventory, Vec3f(0, 0, 0), Vec3f(500, 300, 0));
        getRenderComponentManager().addComponent(pInventory);
        sf::sleep(sf::seconds(0.5f));
        wIdentification = new RenderWindow(sf::VideoMode(400, 300), "Identification", sf::Style::Titlebar, ContextSettings(0, 0, 4, 3, 0));
        View iView = wIdentification->getDefaultView();
        iView.setCenter(Vec3f(wIdentification->getSize().x * 0.5f, wIdentification->getSize().y * 0.5f, 0));
        wIdentification->setView(iView);
        addWindow(wIdentification);
        labPseudo = new gui::Label(*wIdentification, Vec3f(0, 0, 0), Vec3f(200, 50, 0), fm.getResourceByAlias(Fonts::Serif), "Pseudo : ", 15);
        getRenderComponentManager().addComponent(labPseudo);
        labMdp = new gui::Label(*wIdentification, Vec3f(0, 60, 0), Vec3f(200, 50, 0), fm.getResourceByAlias(Fonts::Serif), "Password : ", 15);
        getRenderComponentManager().addComponent(labMdp);
        taPseudo = new gui::TextArea(Vec3f(200, 0, 0), Vec3f(200, 50, 0), fm.getResourceByAlias(Fonts::Serif),"Pseudo", *wIdentification);
        getRenderComponentManager().addComponent(taPseudo);
        taPassword = new gui::PasswordField(Vec3f(200, 60, 0), Vec3f(200, 50, 0), fm.getResourceByAlias(Fonts::Serif),"Password", *wIdentification);
        getRenderComponentManager().addComponent(taPassword);
        idButton = new gui::Button(Vec3f(0, 120, 0), Vec3f(200, 50, 0), fm.getResourceByAlias(Fonts::Serif), "Connect", 15, *wIdentification);
        getRenderComponentManager().addComponent(idButton);
        invButton = new gui::Button(Vec3f(200, 120, 0), Vec3f(200, 50, 0), fm.getResourceByAlias(Fonts::Serif), "Invite", 15, *wIdentification);
        getRenderComponentManager().addComponent(invButton);
        invButton->addActionListener(this);
        idButton->addActionListener(this);
        hpBar = new gui::ProgressBar(getRenderWindow(), Vec3f(0, 0, 0), Vec3f(100, 10, 0),*fm.getResourceByAlias(Fonts::Serif),15);
        hpBar->setMaximum(100);
        hpBar->setMinimum(0);
        hpBar->setValue(100);
        xpBar = new gui::ProgressBar(getRenderWindow(), Vec3f(0, 590, 0), Vec3f(800, 10, 0),*fm.getResourceByAlias(Fonts::Serif),15);
        xpBar->setMaximum(1500);
        xpBar->setMinimum(0);
        xpBar->setValue(0);
        fcHpBar = new gui::ProgressBar(getRenderWindow(), Vec3f(700, 0, 0), Vec3f(100, 10, 0),*fm.getResourceByAlias(Fonts::Serif),15);
        fcHpBar->setMinimum(0);
        fcHpBar->setMaximum(100);
        fcHpBar->setValue(100);
        fcHpBar->setVisible(false);
        getRenderComponentManager().addComponent(hpBar);
        getRenderComponentManager().addComponent(xpBar);
        getRenderComponentManager().addComponent(fcHpBar);
        setEventContextActivated(false);
    }
    void MyAppli::onRender(RenderComponentManager *cm) {
        // draw everything here...
        if (isClientAuthentified) {
            World::drawOnComponents("E_BIGTILE", 0);
            World::drawOnComponents("E_WALL+E_DECOR+E_ANIMATION+E_HERO", 1);
            World::drawOnComponents("E_WALL+E_DECOR+E_ANIMATION+E_HERO", 2);
            World::drawOnComponents("E_WALL+E_DECOR+E_ANIMATION+E_PONCTUAL_LIGHT", 3);
        } else {
            World::drawOnComponents("", 0);
            World::drawOnComponents("", 1);
            World::drawOnComponents("", 2);
            World::drawOnComponents("", 3);
        }
    }
    void MyAppli::onDisplay(RenderWindow* window) {
        if (window == &getRenderWindow()) {
            std::vector<std::pair<Sprite*, std::vector<Item>>>::iterator it;
            for (it = cristals.begin(); it != cristals.end(); it++) {
                Sprite sprite = *it->first;
                window->draw(sprite);
            }
            std::vector<std::pair<std::pair<Caracter*, Text>, std::pair<sf::Time, sf::Time>>>::iterator it2;
            for (it2 = tmpTexts.begin(); it2 != tmpTexts.end();) {
                sf::Time elapsedTime = Application::getTimeClk().getElapsedTime() - it2->second.first;
                it2->second.first = Application::getTimeClk().getElapsedTime();
                it2->second.second -= elapsedTime;
                if (it2->second.second <= sf::seconds(0)) {
                    it2 = tmpTexts.erase(it2);
                } else {
                    it2->first.second.setPosition(Vec3f(it2->first.first->getPosition().x, it2->first.first->getPosition().y-10, 0));
                    window->draw(it2->first.second);
                    it2++;
                }
            }
        }
    }
    void MyAppli::onUpdate (RenderWindow* window, IEvent& event) {
        // check all the window's events that were triggered since the last iteration of the loop
        if (event.type == IEvent::WINDOW_EVENT && event.window.type == IEvent::WINDOW_EVENT_CLOSED && window == &getRenderWindow()) {
            Network::stopCli();
            stop();
        }
        if (event.type == IEvent::WINDOW_EVENT && event.window.type == IEvent::WINDOW_EVENT_CLOSED && window == wPickupItems) {
            pItems->removeAll();
            wPickupItems->setVisible(false);
        }
        if (event.type == IEvent::WINDOW_EVENT && event.window.type == IEvent::WINDOW_EVENT_CLOSED && window == wInventory) {
            pInventory->removeAll();
            wInventory->setVisible(false);
        }
        if (event.type == IEvent::KEYBOARD_EVENT && event.keyboard.type == IEvent::KEY_EVENT_PRESSED && window == &getRenderWindow()) {
            previousKey = actualKey;
            actualKey = static_cast<IKeyboard::Key>(event.keyboard.code);
            getListener().setCommandSlotParams("MoveAction", this, static_cast<IKeyboard::Key>(event.keyboard.code));
            getListener().setCommandSlotParams("PickUpItems", this, static_cast<IKeyboard::Key>(event.keyboard.code));
        }
        if (event.type == IEvent::KEYBOARD_EVENT && event.keyboard.type == IEvent::KEY_EVENT_RELEASED
            && hero != nullptr && hero->isMovingFromKeyboard() && window == &getRenderWindow()) {
            previousKey = static_cast<IKeyboard::Key>(event.keyboard.code);
            actualKey = IKeyboard::Key::Unknown;
            /*hero->setMoving(false);
            hero->setIsMovingFromKeyboard(false);*/
            sf::Int64 cli_time = Application::getTimeClk().getElapsedTime().asMicroseconds();
            std::string message = "STOPCARMOVE*"+conversionIntString(hero->getId())+"*"+conversionLongString(cli_time);
            SymEncPacket packet;
            packet<<message;
            Network::sendTcpPacket(packet);
        }
        if (event.type == IEvent::MOUSE_BUTTON_EVENT && event.mouseButton.type == IEvent::BUTTON_EVENT_PRESSED && window == &getRenderWindow()) {
            sf::Vector2f mousePos (event.mouseButton.x, event.mouseButton.y);
            getListener().setCommandSlotParams("LeftMouseButtonPressedAction", this, mousePos);
            getListener().setCommandSlotParams("RightMouseButtonPressedAction", this, mousePos);
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
        /*if (Network::getResponse("MOVEFROMKEYBOARD", response)) {
            std::cout<<"move from kb"<<std::endl;
            hero->setIsMovingFromKeyboard(true);
            hero->setMoving(true);
            if (hero->isAttacking())
                hero->setAttacking(false);
            hero->setFightingMode(false);
            std::string request = "GETCARPOS";
            sf::Packet packet;
            packet<<request;
            Network::sendUdpPacket(packet);
            getClock("RequestTime").restart();
        }*/
        if (Network::getResponse("NEWPATH", response)) {
            std::vector<std::string> infos = split(response, "*");
            std::vector<Vec2f> path;
            int size = conversionStringInt(infos[0]);
            int id = conversionStringInt(infos[1]);
            Caracter* caracter = static_cast<Caracter*>(World::getEntity(id));
            if (static_cast<Hero*>(caracter) && static_cast<Hero*>(caracter)->isMovingFromKeyboard()) {
                caracter->setIsMovingFromKeyboard(false);
            }
            Vec2f actualPos (conversionStringFloat(infos[2]), conversionStringFloat(infos[3]));
            sf::Int64 last_cli_time = conversionStringLong(infos[4]);
            sf::Int64 elapsedTime = Application::getTimeClk().getElapsedTime().asMicroseconds() - last_cli_time;
            bool isInFightingMode = conversionStringInt(infos[5]);
            caracter->setFightingMode(isInFightingMode);
            if (caracter->isAttacking())
                caracter->setAttacking(false);
            caracter->setFightingMode(false);
            Vec2f newPos = Computer::getPosOnPathFromTime(actualPos, caracter->getPath(),elapsedTime,caracter->getSpeed());
            for (int i = 0; i < size; i++) {
                path.push_back(Vec2f(conversionStringFloat(infos[i*2+6]), conversionStringFloat(infos[i*2+7])));
            }
            Vec2f d = newPos - actualPos;
            if (id == hero->getId()) {
                for (unsigned int i = 0; i < getRenderComponentManager().getNbComponents(); i++) {
                    if (getRenderComponentManager().getRenderComponent(i) != nullptr) {
                        View view = getRenderComponentManager().getRenderComponent(i)->getView();
                        view.move(d.x, d.y, d.y);
                        getRenderComponentManager().getRenderComponent(i)->setView(view);
                    }
                }
                getView().move (d.x, d.y, d.y);
            }
            Vec2f dir = d.normalize();
            if (dir != caracter->getDir())
                caracter->setDir(dir);
            World::moveEntity(caracter, d.x, d.y, d.y);
            caracter->setPath(path);
            caracter->setMoving(true);
            caracter->interpolation.first = caracter->getCenter();
            caracter->interpolation.second = Computer::getPosOnPathFromTime(caracter->interpolation.first, caracter->getPath(),ping + timeBtwnTwoReq.asMicroseconds(),caracter->getSpeed());
            caracter->getClkTransfertTime().restart();
            std::string request = "GETCARPOS";
            sf::Packet packet;
            packet<<request;
            Network::sendUdpPacket(packet);
            getClock("RequestTime").restart();
        }
        if (Network::getResponse("NEWPOS", response)) {

            std::vector<std::string> infos = split(response, "*");
            int id = conversionStringInt(infos[0]);
            ping = conversionStringLong(infos[1]);
            Caracter* caracter = static_cast<Caracter*>(World::getEntity(id));
            Vec3f actualPos = Vec3f(caracter->getCenter().x, caracter->getCenter().y, 0);
            Vec3f newPos (conversionStringFloat(infos[2]), conversionStringFloat(infos[3]), conversionStringFloat(infos[4]));

            sf::Int64 last_cli_time = conversionStringLong(infos[5]);
            sf::Int64 elapsedTime = Application::getTimeClk().getElapsedTime().asMicroseconds() - last_cli_time;
            bool isMoving = conversionStringInt(infos[6]);
            bool isInFightingMode = conversionStringInt(infos[7]);
            bool isAttacking = conversionStringInt(infos[8]);
            bool isAlive = conversionStringInt(infos[9]);
            int life = conversionStringInt(infos[10]);
            if (last_cli_time > caracter->getAttribute("isMoving").getValue<sf::Int64>()) {
                caracter->setMoving(isMoving);
            }
            if (last_cli_time > caracter->getAttribute("isInFightingMode").getValue<sf::Int64>()) {
                caracter->setFightingMode(isInFightingMode);
            }
            if(last_cli_time > caracter->getAttribute("isAttacking").getValue<sf::Int64>()) {
                caracter->setAttacking(isAttacking);
            }
            if (last_cli_time > caracter->getAttribute("isAlive").getValue<sf::Int64>()) {
                caracter->setAlive(isAlive);
            }
            if (last_cli_time > caracter->getAttribute("life").getValue<sf::Int64>()) {
                caracter->setLife(life);
            }
            if (!caracter->isMoving() && static_cast<Hero*>(caracter)->isMovingFromKeyboard()) {
                static_cast<Hero*>(caracter)->setIsMovingFromKeyboard(false);
            }
            if (static_cast<Hero*> (caracter) && static_cast<Hero*>(caracter)->isMovingFromKeyboard() && caracter->isMoving()) {
                newPos = newPos + Vec3f(caracter->getDir().x, caracter->getDir().y, 0) * caracter->getSpeed() * elapsedTime;
            } else if (caracter->isMoving()) {
                newPos = Computer::getPosOnPathFromTime(newPos, caracter->getPath(),elapsedTime, caracter->getSpeed());
            }
            Vec3f d = newPos - actualPos;

            if (id == hero->getId()) {
                for (unsigned int i = 0; i < getRenderComponentManager().getNbComponents(); i++) {
                    if (getRenderComponentManager().getRenderComponent(i) != nullptr) {
                        View view = getRenderComponentManager().getRenderComponent(i)->getView();
                        view.move(d.x, d.y, d.y);
                        getRenderComponentManager().getRenderComponent(i)->setView(view);
                    }
                }
                getView().move (d.x, d.y, d.y);
            }
            World::moveEntity(caracter, d.x, d.y, d.y);
            World::update();
            caracter->interpolation.first = Vec3f(caracter->getCenter().x, caracter->getCenter().y, caracter->getCenter().z);
            if (caracter->isMoving()) {
                if (caracter->isMovingFromKeyboard()) {
                    caracter->interpolation.second = caracter->interpolation.first + Vec3f(caracter->getDir().x,caracter->getDir().y,0)  * caracter->getSpeed() * (ping + timeBtwnTwoReq.asMicroseconds());
                } else {
                    caracter->interpolation.second = Computer::getPosOnPathFromTime(caracter->interpolation.first, caracter->getPath(),ping + timeBtwnTwoReq.asMicroseconds(),caracter->getSpeed());
                }
            } else {
                caracter->interpolation.second = caracter->interpolation.first;
            }
            caracter->getClkTransfertTime().restart();
       }
       if (Network::getResponse("DEATH", response)) {
           int id = conversionStringInt(response);
           Caracter* caracter = static_cast<Caracter*>(World::getEntity(id));
           caracter->setAttacking(false);
           caracter->setFightingMode(false);
           caracter->setAlive(false);
       }
       if (Network::getResponse("ENTERINFIGHTINGMODE", response)) {
            int id = conversionStringInt(response);
            Entity* entity = World::getEntity(id);
            if (static_cast<Caracter*> (entity))
                static_cast<Caracter*> (entity)->setFightingMode(true);
       }
       if (Network::getResponse("LEAVEINFIGHTINGMODE", response)) {
            int id = conversionStringInt(response);
            Entity* entity = World::getEntity(id);
            if (static_cast<Caracter*> (entity))
                static_cast<Caracter*> (entity)->setFightingMode(false);
       }
       if (Network::getResponse("SETATTACKING", response)) {
            int id = conversionStringInt(response);
            Entity* entity = World::getEntity(id);
            if (static_cast<Caracter*> (entity)) {
                static_cast<Caracter*> (entity)->setAttacking(true);
                static_cast<Caracter*> (entity)->restartAttackSpeed();
            }
       }
       if (Network::getResponse("ALIVE", response)) {
            std::vector<std::string> infos = split(response, "*");
            int id = conversionStringInt(infos[0]);
            Vec3f center(conversionStringFloat(infos[1]), conversionStringFloat(infos[2]), conversionStringFloat(infos[2]));
            Caracter* caracter = static_cast<Caracter*>(World::getEntity(id));
            if (caracter->getType() == "E_HERO") {
                wResuHero->setVisible(false);
                button->setEventContextActivated(false);
                setEventContextActivated(true);
            }
            caracter->setCenter(center);
            caracter->setMoving(false);
            caracter->setFightingMode(false);
            caracter->setAttacking(false);
            caracter->setAlive(true);
            caracter->restartRegenHP();
       }
       if (Network::getResponse("ATTACK", response)) {
            Caracter* monster = static_cast<Caracter*>(World::getEntity(conversionStringInt(response)));
            hero->setFocusedCaracter(monster);
            hero->setFightingMode(true);
            hero->setAttacking(false);
            hero->setMoving(false);
        }
        if (Network::getResponse("IDOK", response)) {
            isClientAuthentified = true;
            wIdentification->setVisible(false);
            idButton->setEventContextActivated(false);
            invButton->setEventContextActivated(false);
            setEventContextActivated(true);
            SymEncPacket packet;
            packet<<"GETMAPINFOS";
            Network::sendTcpPacket(packet);
            std::string response = Network::waitForLastResponse("MAPINFOS", sf::seconds(20.f));
            std::istringstream iss(response);
            ITextArchive ia(iss);
            std::vector<Entity*> entities;
            ia(entities);
            TextureManager<> &tm = cache.resourceManager<Texture, std::string>("TextureManager");
            for (unsigned int i = 0; i < entities.size(); i++) {
                World::addEntity(entities[i]);
                if (entities[i]->getType() == "E_BIGTILE") {
                    for (unsigned int j = 0; j < entities[i]->getChildren().size(); j++) {
                        std::string texId =  entities[i]->getChildren()[j]->getFaces()[0]->getMaterial().getTexId();
                        sf::IntRect texRect = entities[i]->getChildren()[j]->getFaces()[0]->getMaterial().getTexRect();
                        entities[i]->getChildren()[j]->getFaces()[0]->getMaterial().clearTextures();
                        entities[i]->getChildren()[j]->getFaces()[0]->getMaterial().addTexture(tm.getResourceByAlias(texId), texRect);
                        entities[i]->getChildren()[j]->getFaces()[0]->getMaterial().setTexId(texId);
                    }

                } else if (entities[i]->getType() == "E_WALL") {
                    std::string texId =  entities[i]->getChildren()[0]->getFaces()[0]->getMaterial().getTexId();
                    sf::IntRect texRect = entities[i]->getChildren()[0]->getFaces()[0]->getMaterial().getTexRect();
                    entities[i]->getChildren()[0]->getFaces()[0]->getMaterial().clearTextures();
                    entities[i]->getChildren()[0]->getFaces()[0]->getMaterial().addTexture(tm.getResourceByAlias(texId), texRect);
                    entities[i]->getChildren()[0]->getFaces()[0]->getMaterial().setTexId(texId);
                    World::getGridCellAt(Vec3f(entities[i]->getCenter().x, entities[i]->getCenter().y, 0))->setPassable(false);
                } else if (entities[i]->getType() == "E_DECOR") {
                    std::string texId =  entities[i]->getChildren()[0]->getFaces()[0]->getMaterial().getTexId();
                    sf::IntRect texRect = entities[i]->getChildren()[0]->getFaces()[0]->getMaterial().getTexRect();
                    entities[i]->getChildren()[0]->getFaces()[0]->getMaterial().clearTextures();
                    entities[i]->getChildren()[0]->getFaces()[0]->getMaterial().addTexture(tm.getResourceByAlias(texId), texRect);
                    entities[i]->getChildren()[0]->getFaces()[0]->getMaterial().setTexId(texId);
                } else if (entities[i]->getType() == "E_ANIMATION") {
                    Anim* anim = static_cast<Anim*> (entities[i]);
                    for (unsigned int j = 0; j < anim->getChildren().size(); j++) {
                        if (entities[i]->getChildren()[j]->getChildren().size() > 0) {
                            std::string texId = entities[i]->getChildren()[j]->getChildren()[0]->getFaces()[0]->getMaterial().getTexId();
                            sf::IntRect texRect = entities[i]->getChildren()[j]->getChildren()[0]->getFaces()[0]->getMaterial().getTexRect();
                            entities[i]->getChildren()[j]->getChildren()[0]->getFaces()[0]->getMaterial().clearTextures();
                            entities[i]->getChildren()[j]->getChildren()[0]->getFaces()[0]->getMaterial().addTexture(tm.getResourceByAlias(texId), texRect);
                            entities[i]->getChildren()[j]->getChildren()[0]->getFaces()[0]->getMaterial().setTexId(texId);
                        }
                    }
                    anim->play(true);
                    au->addAnim(anim);
                }
            }

            packet.clear();
            packet<<"GETCARINFOS";
            Network::sendTcpPacket(packet);
            response = Network::waitForLastResponse("CARINFOS", sf::seconds(5.f));
            iss.str("");
            ia.clear();
            iss.str(response);
            ia(hero);
            std::string path = "tilesets/vlad_sword.png";
            cache.resourceManager<Texture, std::string>("TextureManager").fromFileWithAlias(path, "VLADSWORD");
            const Texture *text = cache.resourceManager<Texture, std::string>("TextureManager").getResourceByPath(path);
            int textRectX = 0, textRectY = 0, textRectWidth = 50, textRectHeight = 100;
            int textWidth = text->getSize().x;
            Vec3f tmpCenter = hero->getCenter();
            hero->setCenter(Vec3f(0, 0, 0));
            for (unsigned int i = 0; i < 8; i++) {
                Anim* animation = new Anim(0.1f, Vec3f(-25, -50, 0), Vec3f(50, 100, 0), 0);
                for (unsigned int j = 0; j < 8; j++) {
                    sf::IntRect textRect (textRectX, textRectY, textRectWidth, textRectHeight);
                    Tile *tile = new Tile(text, Vec3f(-25, -50, 0), Vec3f(50, 100, 0), textRect);
                    tile->getFaces()[0]->getMaterial().setTexId("VLADSWORD");
                    g2d::Decor *frame = new g2d::Decor(tile, &g2d::AmbientLight::getAmbientLight());
                    frame->setShadowCenter(Vec3f(0, 200, 0));
                    if (textRectX + textRectWidth > textWidth) {
                        textRectX = 0;
                        textRectY += textRectHeight;
                    } else {
                        textRectX += textRectWidth;
                    }
                    animation->getCurrentFrame()->setBoneIndex(i);
                    animation->addFrame(frame);
                }
                hero->addAnimation(animation);
                au->addAnim(animation);
            }
            for (unsigned int i = 0; i < 8; i++) {
                Anim* animation = new Anim(0.1f, Vec3f(-25, -50, 0), Vec3f(50, 100, 0), 0);
                for (unsigned int j = 0; j < 12; j++) {
                    sf::IntRect textRect (textRectX, textRectY, textRectWidth, textRectHeight);
                    Tile *tile = new Tile(text, Vec3f(-25, -50, 0), Vec3f(50, 100, 0), textRect);
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
                animation->getCurrentFrame()->setBoneIndex(i+8);
                hero->addAnimation(animation);
                au->addAnim(animation);
            }
            textRectWidth = 100;
            for (unsigned int i = 0; i < 8; i++) {
                Anim* animation = new Anim(0.1f, Vec3f(-50, -50, 0), Vec3f(100, 100, 0), 0);
                for (unsigned int j = 0; j < 12; j++) {
                    sf::IntRect textRect (textRectX, textRectY, textRectWidth, textRectHeight);
                    Tile *tile = new Tile(text, Vec3f(-50, -50, 0), Vec3f(100, 100, 0), textRect);
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
                animation->getCurrentFrame()->setBoneIndex(i+16);
                hero->addAnimation(animation);
                au->addAnim(animation);
            }
            hero->setCenter(tmpCenter);
            getView().move(hero->getCenter().x, hero->getCenter().y, hero->getCenter().z - 300);
            for (unsigned int i = 0; i < getRenderComponentManager().getNbComponents(); i++) {
                if (getRenderComponentManager().getRenderComponent(i) != nullptr) {
                    View view = getView();
                    getRenderComponentManager().getRenderComponent(i)->setView(view);
                }
            }
            BoundingVolume* bb2 = new BoundingBox(hero->getGlobalBounds().getPosition().x, hero->getGlobalBounds().getPosition().y + hero->getGlobalBounds().getSize().y * 0.4f, 0,
            hero->getGlobalBounds().getSize().x, hero->getGlobalBounds().getSize().y * 0.25f, 0);
            hero->setCollisionVolume(bb2);
            World::addEntity(hero);
            Network::sendTcpPacket(packet);
            response = Network::waitForLastResponse("MONSTERSINFOS", sf::seconds(5.f));
            iss.str("");
            ia.clear();
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
            for (unsigned int i = 0; i < 8; i++) {
                Anim* animation = new Anim(0.1f, Vec3f(-25, -50, 0), Vec3f(50, 100, 0), 0);
                for (unsigned int j = 0; j < 8; j++) {
                    sf::IntRect textRect (textRectX, textRectY, textRectWidth, textRectHeight);
                    Tile *tile = new Tile(text, Vec3f(-25, -50, 0), Vec3f(50, 100, 0), textRect);
                    tile->getFaces()[0]->getMaterial().setTexId("OGRO");
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
                animation->getCurrentFrame()->setBoneIndex(i);
                monster->addAnimation(animation);
                au->addAnim(animation);
            }
            for (unsigned int i = 0; i < 8; i++) {
                Anim* animation = new Anim(0.1f, Vec3f(-25, -50, 0), Vec3f(50, 100, 0), 0);
                for (unsigned int j = 0; j < 6; j++) {
                    sf::IntRect textRect (textRectX, textRectY, textRectWidth, textRectHeight);
                    Tile *tile = new Tile(text, Vec3f(-25, -50, 0), Vec3f(50, 100, 0), textRect);
                    tile->getFaces()[0]->getMaterial().setTexId("OGRO");
                    g2d::Decor *frame = new g2d::Decor(tile, &g2d::AmbientLight::getAmbientLight());
                    frame->setShadowCenter(Vec3f(0, 200, 0));
                    //decor->changeGravityCenter(Vec3f(50, 50, 0));
                    if (textRectX + textRectWidth > textWidth) {
                        textRectX = 0;
                        textRectY += textRectHeight;
                    } else {
                        textRectX += textRectWidth;
                    }
                    animation->addFrame(frame);
                }
                animation->getCurrentFrame()->setBoneIndex(i+8);
                monster->addAnimation(animation);
                au->addAnim(animation);
            }
            textRectWidth = 100;
            for (unsigned int i = 0; i < 8; i++) {
                Anim* animation = new Anim(0.1f, Vec3f(-50, -50, 0), Vec3f(100, 100, 0), 0);
                for (unsigned int j = 0; j < 11; j++) {
                    sf::IntRect textRect(textRectX, textRectY, textRectWidth, textRectHeight);
                    Tile *tile = new Tile(text, Vec3f(-50, -50, 0), Vec3f(100, 100, 0), textRect);
                    tile->getFaces()[0]->getMaterial().setTexId("OGRO");
                    g2d::Decor *frame = new g2d::Decor(tile, &g2d::AmbientLight::getAmbientLight());
                    frame->setShadowCenter(Vec3f(0, 200, 0));
                    //decor->changeGravityCenter(Vec3f(50, 50, 0));
                    if (textRectX + textRectWidth > textWidth) {
                        textRectX = 0;
                        textRectY += textRectHeight;
                    } else {
                        textRectX += textRectWidth;
                    }
                    animation->addFrame(frame);
                }
                animation->getCurrentFrame()->setBoneIndex(i+16);
                monster->addAnimation(animation);
                au->addAnim(animation);
            }
            monster->setCenter(tmpCenter);
            World::addEntity(monster);
            hero->setXpHpBar(xpBar, hpBar);
            monster->setXpHpBar(nullptr, fcHpBar);
            packet.clear();
            packet<<"GETCARPOS";
            hero->getClkTransfertTime().restart();
            getClock("RequestTime").restart();
            Network::sendTcpPacket(packet);
            received = false;
            World::update();
       }
       if (Network::getResponse("ITEMS", response)) {
            std::istringstream iss(response);
            ITextArchive ita(iss);
            std::vector<Item> items;
            ita(items);
            if (items.size() > 0) {
                cristals.clear();
                TextureManager<> &tm = cache.resourceManager<Texture, std::string>("TextureManager");
                Sprite* cristal = new Sprite(*tm.getResourceByAlias("CRISTAL"),hero->getFocusedCaracter()->getPosition(),Vec3f(50, 100, 0),sf::IntRect(0, 0, 50, 100));
                cristals.push_back(std::make_pair(cristal, items));
            }
       }
       if (getClock("LoopTime").getElapsedTime().asMilliseconds() < 100)
            sf::sleep(sf::milliseconds(100 - getClock("LoopTime").getElapsedTime().asMilliseconds()));

       std::vector<Entity*> caracters = World::getEntities("E_MONSTER+E_HERO");
       for (unsigned int i = 0; i < caracters.size(); i++) {
            Caracter* caracter = static_cast<Caracter*>(caracters[i]);
            /*if (caracter->getType() == "E_HERO") {
                std::cout<<"hero life : "<<caracter->getLife()<<std::endl;
            }*/
            if (caracter->isAlive()) {
                if (caracter->isMoving()) {
                    if (dynamic_cast<Hero*>(caracter) && dynamic_cast<Hero*>(caracter)->isMovingFromKeyboard()) {
                        Vec3f actualPos = Vec3f(caracter->getCenter().x, caracter->getCenter().y, 0);
                        sf::Int64 elapsedTime = caracter->getClkTransfertTime().getElapsedTime().asMicroseconds();
                        Vec3f newPos = caracter->interpolation.first + (caracter->interpolation.second - caracter->interpolation.first) * ((float) elapsedTime / (float) (ping + timeBtwnTwoReq.asMicroseconds()));
                        Ray ray(actualPos, newPos);
                        if (World::collide(caracter, ray)) {
                            newPos = actualPos;
                        }
                        for (unsigned int j = 0; j < getRenderComponentManager().getNbComponents(); j++) {
                            if (getRenderComponentManager().getRenderComponent(j) != nullptr) {
                                View view = getRenderComponentManager().getRenderComponent(j)->getView();
                                Vec3f d = newPos - view.getPosition();
                                view.move(d.x, d.y, d.y);
                                getRenderComponentManager().getRenderComponent(j)->setView(view);
                            }
                        }
                        Vec3f d = newPos - actualPos;
                        World::moveEntity(caracter, d.x, d.y, d.y);
                        getView().move(d.x, d.y, d.y);
                        World::update();
                    } else {
                        Vec2f actualPos = Vec2f(caracter->getCenter().x, caracter->getCenter().y);
                        sf::Int64 elapsedTime = caracter->getClkTransfertTime().getElapsedTime().asMicroseconds();
                        Vec2f newPos = Computer::getPosOnPathFromTime(caracter->interpolation.first, caracter->getPath(),elapsedTime,caracter->getSpeed());
                        Vec3f d = newPos - actualPos;
                        Vec2f dir = d.normalize();
                        if (caracter->getFocusedCaracter() != nullptr && caracter->isInFightingMode() &&
                            Vec2f(caracter->getCenter().x, caracter->getCenter().y).computeDist(Vec2f(caracter->getFocusedCaracter()->getCenter().x, caracter->getFocusedCaracter()->getCenter().y)) <= caracter->getRange()) {
                            int delta = caracter->getRange() - Vec2f(caracter->getCenter().x, caracter->getCenter().y).computeDist(Vec2f(caracter->getFocusedCaracter()->getCenter().x, caracter->getFocusedCaracter()->getCenter().y));
                            newPos -= dir * delta;
                            d = newPos - actualPos;
                            caracter->setMoving(false);
                        }
                        if (caracter->isMoving() &&
                            caracter->getPath().size() > 1 &&
                            newPos.computeDist(caracter->getPath()[caracter->getPath().size() - 1]) <= PATH_ERROR_MARGIN) {
                            caracter->setMoving(false);
                            newPos = caracter->getPath()[caracter->getPath().size() - 1];
                            d = newPos - actualPos;
                        }

                        if (dir != caracter->getDir())
                            caracter->setDir(dir);
                        if (caracter->getId() == hero->getId()) {
                            for (unsigned int j = 0; j < getRenderComponentManager().getNbComponents(); j++) {
                                if (getRenderComponentManager().getRenderComponent(j) != nullptr) {
                                    View view = getRenderComponentManager().getRenderComponent(j)->getView();
                                    view.move(d.x, d.y, d.y);
                                    getRenderComponentManager().getRenderComponent(j)->setView(view);
                                }
                            }
                            getView().move(d.x, d.y, d.y);
                        }
                        World::moveEntity(caracter, d.x, d.y, d.y);
                        World::update();
                    }
                }
                if (caracter->isInFightingMode()
                && caracter->getFocusedCaracter() != nullptr
                && Vec2f(caracter->getCenter().x, caracter->getCenter().y).computeDist(Vec2f(caracter->getFocusedCaracter()->getCenter().x,caracter->getFocusedCaracter()->getCenter().y)) <= caracter->getRange()) {
                    if (dynamic_cast<Monster*>(caracter->getFocusedCaracter())
                        && !dynamic_cast<Monster*>(caracter->getFocusedCaracter())->isInFightingMode()) {
                        caracter->getFocusedCaracter()->setFightingMode(true);
                        caracter->getFocusedCaracter()->setFocusedCaracter(caracter);
                    }
                    Vec2f dir = Vec2f(caracter->getFocusedCaracter()->getCenter().x, caracter->getFocusedCaracter()->getCenter().y) - Vec2f(caracter->getCenter().x, caracter->getCenter().y);
                    dir = dir.normalize();
                    if (caracter->getDir() != dir)
                        caracter->setDir(dir);
                    if (caracter->getDamages().empty()) {
                        //std::cout<<"wait for receiving damages"<<std::endl;
                        std::vector<int> damages;
                        std::string response;
                        if (Network::getResponse("DMG"+conversionIntString(caracter->getId()), response)) {
                            std::vector<std::string> infos = split(response, "*");
                            int nb = conversionStringInt(infos[0]);
                            for (unsigned int i = 0; i < nb; i++) {
                                damages.push_back(conversionStringInt(infos[i+1]));
                            }
                            caracter->setDamages(damages);
                            sf::Int64 time = Application::getTimeClk().getElapsedTime().asMicroseconds() - conversionStringLong(infos[nb+1]);
                            caracter->setDmgTransferTime(time);
                            caracter->setLife(conversionStringInt(infos[nb+2]));
                        }
                    }
                    sf::Int64 time = caracter->getDmgTransferTime();
                    if (!caracter->getDamages().empty() && caracter->getTimeOfLastAttack().asSeconds() >= caracter->getAttackSpeed() - time / 1e+6) {
                        if (time  > 0) {
                            time -= caracter->getAttackSpeed() * 1e+6;
                            if (time < 0)
                                time = 0;
                            caracter->setDmgTransferTime(time);
                        }
                        caracter->restartAttackSpeed();
                        int dmg = caracter->getDamages().back();
                        caracter->getDamages().pop_back();
                        FontManager<Fonts>& fm = cache.resourceManager<Font, Fonts>("FontManager");
                        Text text;
                        text.setFont(*fm.getResourceByAlias(Fonts::Serif));
                        text.setString("-"+conversionIntString(dmg));
                        text.setColor(sf::Color::Red);
                        text.setCharacterSize(10);
                        text.setPosition(Vec3f(caracter->getFocusedCaracter()->getPosition().x,caracter->getFocusedCaracter()->getPosition().y-10, 0));
                        text.setSize(Vec3f(10, 10, 0));
                        tmpTexts.push_back(std::make_pair(std::make_pair(caracter->getFocusedCaracter(), text), std::make_pair(Application::getTimeClk().getElapsedTime(), sf::seconds(0.5))));
                        caracter->attackFocusedCaracter(dmg);
                        if (!caracter->getFocusedCaracter()->isAlive()) {
                            caracter->setAttacking(false);
                            caracter->setFightingMode(false);
                        }
                    }
                } else {
                    if (caracter->isAttacking())
                        caracter->setAttacking(false);
                }
                if (!caracter->isInFightingMode() && !caracter->isMoving() && !caracter->isAttacked()) {
                    //std::cout<<"rgn size : "<<caracter->getRegen().size()<<std::endl;
                    if (caracter->getRegen().empty()) {
                        //std::cout<<"wait for received regen"<<std::endl;
                        std::string response;

                        if (Network::getResponse("RGN"+conversionIntString(caracter->getId()), response)) {
                            std::vector<int> regen;
                            std::vector<std::string> infos = split(response, "*");
                            int nb = conversionStringInt(infos[0]);
                            for (unsigned int i = 0; i < nb; i++) {
                                regen.push_back(conversionStringInt(infos[i+1]));
                            }
                            caracter->setRegen(regen);
                            sf::Int64 time = Application::getTimeClk().getElapsedTime().asMicroseconds() - conversionStringLong(infos[nb+1]);
                            caracter->setRgnTransferTime(time);
                            caracter->setLife(conversionStringInt(infos[nb+2]));
                        }
                    }
                    sf::Int64 time = caracter->getRgnTransferTime();
                    if (!caracter->getRegen().empty() && caracter->getTimeOfLastHpRegen().asSeconds() >= caracter->getRegenHpSpeed() - time / 1e+6) {
                        while (time > 0) {
                            time -= caracter->getRegenHpSpeed() * 1e+6;
                            if (time < 0)
                                time = 0;
                            caracter->setRgnTransferTime(time);
                        }
                        caracter->restartRegenHP();
                        int rgn = caracter->getRegen().back();
                        caracter->getRegen().pop_back();
                        FontManager<Fonts>& fm = cache.resourceManager<Font, Fonts>("FontManager");
                        Text text;
                        text.setFont(*fm.getResourceByAlias(Fonts::Serif));
                        text.setString(conversionIntString(rgn));
                        text.setColor(sf::Color::Green);
                        text.setCharacterSize(10);
                        text.setPosition(Vec3f(caracter->getPosition().x,caracter->getPosition().y-10, 0));
                        text.setSize(Vec3f(10, 10, 0));
                        tmpTexts.push_back(std::make_pair(std::make_pair(caracter, text), std::make_pair(Application::getTimeClk().getElapsedTime(), sf::seconds(0.5))));
                        if (caracter->getLife() + rgn >= caracter->getMaxLife()) {
                            caracter->setLife(caracter->getMaxLife());
                        } else {
                            caracter->setLife(caracter->getLife() + rgn);
                        }
                    }
                }
            } else {
                if (caracter == hero) {
                    setEventContextActivated(false);
                    wResuHero->setVisible(true);
                    int time = Math::roundToInt(hero->getTimeBeforeLastRespawn().asSeconds() - hero->getTimeSinceLastRespawn().asSeconds());
                    if (time < 0) {
                        time = 0;
                    }
                    label->setText(conversionIntString(time));
                    if (time <= 0) {
                        button->setEventContextActivated(true);
                    }
                }
            }
        }
        while(!gameActions.empty()) {
            std::cout<<"game action!"<<std::endl;
            apply_nary_visitor(GameAction(), gameActions.back().first, gameActions.back().second.first, gameActions.back().second.second);
            gameActions.pop_back();
            delete itemActions.back();
            itemActions.pop_back();
        }
    }
    void MyAppli::actionPerformed(gui::Button* item) {
        if (item->getText() == "Respawn") {
            SymEncPacket packet;
            std::string message = "ALIVE*"+conversionIntString(hero->getId());
            packet<<message;
            Network::sendTcpPacket(packet);
        }
        if (item->getText() == "Invite") {
            std::cout<<"client authentified"<<std::endl;
            isClientAuthentified = true;
            wIdentification->setVisible(false);
            idButton->setEventContextActivated(false);
            invButton->setEventContextActivated(false);
            setEventContextActivated(true);
            SymEncPacket packet;
            packet<<"INV";
            Network::sendTcpPacket(packet);
        }
        if (item->getText() == "Connect") {
            std::cout<<"try to connect"<<std::endl;
            std::string pseudo = labPseudo->getText();
            std::string pswd = labMdp->getText();
            SymEncPacket packet;
            packet<<"CONNECT*"+pseudo+"*"+pswd;
            Network::sendTcpPacket(packet);
        }
    }
}
