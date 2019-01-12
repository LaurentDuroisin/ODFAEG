#include "application.hpp"
#include "odfaeg/Core/action.h"
#include "odfaeg/Core/command.h"
#include <sys/stat.h>
#include <stdlib.h>
#include "odfaeg/Core/utilities.h"
using namespace odfaeg::core;
using namespace odfaeg::math;
using namespace odfaeg::graphic;
using namespace odfaeg::graphic::gui;
using namespace odfaeg::physic;
ODFAEGCreator::ODFAEGCreator(sf::VideoMode vm, std::string title) :
Application (vm, title, sf::Style::Resize|sf::Style::Close, sf::ContextSettings(0, 0, 0, 3, 0)), isGuiShown (false), cursor(10) {

}
void ODFAEGCreator::onLoad() {
    FontManager<Fonts> fm;
    fm.fromFileWithAlias("fonts/FreeSerif.ttf", Serif);
    cache.addResourceManager(fm, "FontManager");
}
void ODFAEGCreator::onInit() {
    FontManager<Fonts>& fm = cache.resourceManager<Font, Fonts>("FontManager");
    menuBar = new MenuBar(getRenderWindow());
    getRenderComponentManager().addComponent(menuBar);
    menu1 = new Menu(getRenderWindow(),fm.getResourceByAlias(Fonts::Serif),"File");
    getRenderComponentManager().addComponent(menu1);
    menu2 = new Menu(getRenderWindow(),fm.getResourceByAlias(Fonts::Serif),"Compile");
    getRenderComponentManager().addComponent(menu2);
    menu3 = new Menu(getRenderWindow(),fm.getResourceByAlias(Fonts::Serif),"Add");
    getRenderComponentManager().addComponent(menu3);
    menuBar->addMenu(menu1);
    menuBar->addMenu(menu2);
    menuBar->addMenu(menu3);
    item11 = new MenuItem(getRenderWindow(), fm.getResourceByAlias(Fonts::Serif),"New application");
    item11->addMenuItemListener(this);
    getRenderComponentManager().addComponent(item11);
    item12 = new MenuItem(getRenderWindow(), fm.getResourceByAlias(Fonts::Serif),"Test 1 2");
    getRenderComponentManager().addComponent(item12);
    item13 = new MenuItem(getRenderWindow(), fm.getResourceByAlias(Fonts::Serif),"Test 1 3");
    getRenderComponentManager().addComponent(item13);
    menu1->addMenuItem(item11);
    menu1->addMenuItem(item12);
    menu1->addMenuItem(item13);
    item21 = new MenuItem(getRenderWindow(), fm.getResourceByAlias(Fonts::Serif),"Build");
    item21->addMenuItemListener(this);
    getRenderComponentManager().addComponent(item21);
    item22 = new MenuItem(getRenderWindow(), fm.getResourceByAlias(Fonts::Serif),"Run");
    item22->addMenuItemListener(this);
    getRenderComponentManager().addComponent(item22);
    item23 = new MenuItem(getRenderWindow(), fm.getResourceByAlias(Fonts::Serif),"Build and run");
    item23->addMenuItemListener(this);
    getRenderComponentManager().addComponent(item23);
    menu2->addMenuItem(item21);
    menu2->addMenuItem(item22);
    menu2->addMenuItem(item23);
    item31 = new MenuItem(getRenderWindow(), fm.getResourceByAlias(Fonts::Serif),"Rectangle shape");
    item31->addMenuItemListener(this);
    getRenderComponentManager().addComponent(item31);
    item32 = new MenuItem(getRenderWindow(), fm.getResourceByAlias(Fonts::Serif),"Circle Shape");
    getRenderComponentManager().addComponent(item32);
    item33 = new MenuItem(getRenderWindow(), fm.getResourceByAlias(Fonts::Serif),"Sprite");
    getRenderComponentManager().addComponent(item33);
    menu3->addMenuItem(item31);
    menu3->addMenuItem(item32);
    menu3->addMenuItem(item33);
    Action a1 (Action::EVENT_TYPE::KEY_HELD_DOWN, sf::Keyboard::Key::Z);
    Action a2 (Action::EVENT_TYPE::KEY_HELD_DOWN, sf::Keyboard::Key::Q);
    Action a3 (Action::EVENT_TYPE::KEY_HELD_DOWN, sf::Keyboard::Key::S);
    Action a4 (Action::EVENT_TYPE::KEY_HELD_DOWN, sf::Keyboard::Key::D);
    Action combined = a1 || a2 || a3 || a4;
    Command moveAction (combined, FastDelegate<void>(&ODFAEGCreator::processKeyHeldDown, this, sf::Keyboard::Unknown));
    getListener().connect("MoveAction", moveAction);
    View view(getRenderWindow().getSize().x, getRenderWindow().getSize().y, 80, 0.1f, 1000);
    view.setConstrains(0, 10);
    OITRenderComponent *frc = new OITRenderComponent(getRenderWindow(), 0, "");
    frc->setView(view);
    getRenderComponentManager().addComponent(frc);
    fdTexturePath = new FileDialog(Vec3f(0, 0, 0), Vec3f(800, 600, 0), fm.getResourceByAlias(Fonts::Serif));
    fdTexturePath->setVisible(false);
    addWindow(&fdTexturePath->getWindow());
    getRenderComponentManager().addComponent(fdTexturePath);
    wApplicationNew = new RenderWindow(sf::VideoMode(400, 300), "Create ODFAEG Application", sf::Style::Default, sf::ContextSettings(0, 0, 0, 3, 0));
    View waView = wApplicationNew->getDefaultView();
    waView.setCenter(Vec3f(wApplicationNew->getSize().x * 0.5f, wApplicationNew->getSize().y * 0.5f, 0));
    wApplicationNew->setView(waView);
    Label* label = new Label(*wApplicationNew, Vec3f(0, 0, 0), Vec3f(200, 50, 0),fm.getResourceByAlias(Fonts::Serif),"Application name : ", 15);
    getRenderComponentManager().addComponent(label);
    ta = new TextArea(Vec3f(200, 0, 0), Vec3f(200, 50, 0),fm.getResourceByAlias(Fonts::Serif), "", *wApplicationNew);
    getRenderComponentManager().addComponent(ta);
    Label* label2 = new Label(*wApplicationNew, Vec3f(0, 50, 0), Vec3f(200, 50, 0),fm.getResourceByAlias(Fonts::Serif), "Application type : ", 15);
    getRenderComponentManager().addComponent(label2);
    dpList = new DropDownList(*wApplicationNew, Vec3f(200, 50, 0), Vec3f(200, 50, 0),fm.getResourceByAlias(Fonts::Serif),"Normal", 15);
    dpList->addItem("Client", 15);
    dpList->addItem("Server", 15);
    getRenderComponentManager().addComponent(dpList);
    lWidth = new Label(*wApplicationNew, Vec3f(0, 100, 0), Vec3f(100, 50, 0),fm.getResourceByAlias(Fonts::Serif),"Width : ", 15);
    lHeight = new Label(*wApplicationNew, Vec3f(200, 100, 0), Vec3f(100, 50, 0),fm.getResourceByAlias(Fonts::Serif),"Height : ", 15);
    getRenderComponentManager().addComponent(lWidth);
    getRenderComponentManager().addComponent(lHeight);
    taWidth = new TextArea(Vec3f(50, 100, 0), Vec3f(100, 50, 0),fm.getResourceByAlias(Fonts::Serif),"", *wApplicationNew);
    taHeight =  new TextArea(Vec3f(250, 100, 0), Vec3f(100, 50, 0),fm.getResourceByAlias(Fonts::Serif),"", *wApplicationNew);
    getRenderComponentManager().addComponent(taWidth);
    getRenderComponentManager().addComponent(taHeight);
    Button* button = new Button(Vec3f(0, 200, 0), Vec3f(400, 100, 0), fm.getResourceByAlias(Fonts::Serif),"Create", 15, *wApplicationNew);
    button->addActionListener(this);
    getRenderComponentManager().addComponent(button);
    addWindow(wApplicationNew);
    wApplicationNew->setVisible(false);
    pProjects = new Panel(getRenderWindow(),Vec3f(0, 0, 0), Vec3f(200, 700, 0), 0);
    rootNode = std::make_unique<Node> ("projects", pProjects, Vec2f(0.f, 0.015f), Vec2f(1.f / 6.f, 1.f));
    pProjects->setBorderColor(sf::Color(128, 128, 128));
    pProjects->setBackgroundColor(sf::Color::White);
    pProjects->setBorderThickness(5);
    std::ifstream applis("applis");
    std::string line;
    unsigned int i = 0;
    Label* lab = new Label(getRenderWindow(),Vec3f(0,0,0),Vec3f(200, 35, 0),fm.getResourceByAlias(Fonts::Serif),"GUI", 15);
    lab->setBackgroundColor(sf::Color::White);
    Node* node = new Node("GUI",lab,Vec2f(0.f, 0.f),Vec2f(1.f, 0.05f),rootNode.get());
    lab->setForegroundColor(sf::Color::Red);
    lab->setParent(pProjects);
    pProjects->addChild(lab);
    Action a(Action::EVENT_TYPE::MOUSE_BUTTON_PRESSED_ONCE, sf::Mouse::Left);
    Command cmd(a, FastDelegate<bool>(&Label::isMouseInside, lab), FastDelegate<void>(&ODFAEGCreator::showGUI, this, lab));
    lab->getListener().connect("SHOWGUI", cmd);
    if (applis) {
        while(getline(applis, line)) {
            Label* lab = new Label(getRenderWindow(),Vec3f(0,0,0),Vec3f(200, 17, 0),fm.getResourceByAlias(Fonts::Serif),line, 15);
            Node* node = new Node("test",lab,Vec2f(0, 0),Vec2f(1.f, 0.025f),rootNode.get());
            lab->setParent(pProjects);
            lab->setForegroundColor(sf::Color::Red);
            lab->setBackgroundColor(sf::Color::White);
            pProjects->addChild(lab);
            Action a(Action::EVENT_TYPE::MOUSE_BUTTON_PRESSED_ONCE, sf::Mouse::Left);
            Command cmd(a, FastDelegate<bool>(&Label::isMouseInside, lab), FastDelegate<void>(&ODFAEGCreator::showProjectsFiles, this, lab));
            lab->getListener().connect("SHOWPFILES", cmd);
            i++;
            appliname = line;
        }
    }
    applis.close();
    const char* lcars = appliname.c_str();
    char* ucars = new char[appliname.size()];
    for (unsigned int i = 0; i < appliname.length(); i++) {
        ucars[i] = std::tolower(lcars[i]);
    }
    minAppliname = std::string(ucars, appliname.length());
    getRenderComponentManager().addComponent(pProjects);
    pScriptsEdit = new Panel(getRenderWindow(),Vec3f(200, 10, 0),Vec3f(800, 700, 0));
    pScriptsEdit->setRelPosition(1.f / 6.f, 0.01f);
    pScriptsEdit->setRelSize(0.60f, 1.f);
    pScriptsEdit->setBorderColor(sf::Color(128, 128, 128));
    pScriptsEdit->setBackgroundColor(sf::Color::White);
    pScriptsEdit->setBorderThickness(5);
    getRenderComponentManager().addComponent(pScriptsEdit);
    pScriptsFiles = new Panel(getRenderWindow(),Vec3f(0, 0, 0), Vec3f(200, 700, 0), 0);
    pScriptsFiles->setBorderColor(sf::Color(128, 128, 128));
    pScriptsFiles->setBackgroundColor(sf::Color::White);
    pScriptsFiles->setBorderThickness(5);
    rootPropNode = std::make_unique<Node>("Properties", pScriptsFiles, Vec2f(5.f / 6.5f, 0.015f), Vec2f(1.5f / 6.f, 1.f));
    getRenderComponentManager().addComponent(pScriptsFiles);
    tScriptEdit = new TextArea(Vec3f(200, 20, 0),Vec3f(790,650,0),fm.getResourceByAlias(Fonts::Serif),"",getRenderWindow());
    tScriptEdit->setParent(pScriptsEdit);
    tScriptEdit->setRelPosition(0.f, 0.f);
    tScriptEdit->setRelSize(0.9f, 0.9f);
    pScriptsEdit->addChild(tScriptEdit);
    guiSize.x = getRenderWindow().getSize().x - pProjects->getSize().x - pScriptsFiles->getSize().x;
    guiSize.y = getRenderWindow().getSize().y - menuBar->getSize().y;
    guiPos.x = pProjects->getSize().x;
    guiPos.y = menuBar->getSize().y;
    cursor.setOutlineColor(sf::Color::Red);
    cursor.setOutlineThickness(5);
    cursor.setFillColor(sf::Color::Transparent);
    Action moveCursorAction (Action::EVENT_TYPE::MOUSE_BUTTON_PRESSED_ONCE, sf::Mouse::Left);
    Command moveCursorCommand (moveCursorAction, FastDelegate<void>(&ODFAEGCreator::moveCursor, this, sf::Vector2f(-1, -1)));
    getListener().connect("MoveCursor", moveCursorCommand);
}
void ODFAEGCreator::onRender(RenderComponentManager *cm) {

}
void ODFAEGCreator::onDisplay(RenderWindow* window) {
    for (unsigned int i = 0; i < drawables.size(); i++)
        window->draw(*drawables[i]);
    View currentView = window->getView();
    View defaultView = window->getDefaultView();
    window->setView(defaultView);
    if (isGuiShown)
        window->draw(cursor);
    window->setView(currentView);
}
void ODFAEGCreator::onUpdate(RenderWindow* window, sf::Event& event) {
    if (&getRenderWindow() == window && event.type == sf::Event::KeyPressed)
        getListener().setCommandSlotParams("MoveAction", this, event.key.code);
    if (&getRenderWindow() == window && event.type == sf::Event::Closed) {
        stop();
    }
    if (&getRenderWindow() == window && event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2f mousePos (event.mouseButton.x, event.mouseButton.y);
        getListener().setCommandSlotParams("MoveCursor", this, mousePos);
    }
    if (wApplicationNew == window && event.type == sf::Event::Closed) {
        wApplicationNew->setVisible(false);
    }
}
void ODFAEGCreator::onExec() {
    std::string path = fdTexturePath->getPathChosen();
    if (path != "")
        std::cout<<"Loading file : "<<path<<std::endl;
}
void ODFAEGCreator::showGUI(Label* label) {
    isGuiShown = true;
    pScriptsEdit->setVisible(false);
    getRenderComponentManager().getRenderComponent(0)->setVisible(true);
}
void ODFAEGCreator::showProjectsFiles(Label* label) {
    isGuiShown = false;
    getRenderComponentManager().getRenderComponent(0)->setVisible(false);
    pScriptsEdit->setVisible(true);
    Node* node = rootNode->findNode(label);
    if (node->getNodes().size() == 0) {
        FontManager<Fonts>& fm = cache.resourceManager<Font, Fonts>("FontManager");
        std::vector<LightComponent*> children = pProjects->getChildren();
        Label* lHeaders = new Label(getRenderWindow(),Vec3f(0, 0, 0),Vec3f(200,17,0),fm.getResourceByAlias(Fonts::Serif),"headers", 15);
        Label* lSources = new Label(getRenderWindow(),Vec3f(0, 0,0),Vec3f(200,17,0),fm.getResourceByAlias(Fonts::Serif),"sources", 15);
        lHeaders->setBackgroundColor(sf::Color::White);
        lSources->setBackgroundColor(sf::Color::White);
        lHeaders->setParent(pProjects);
        Node* hNode = new Node ("headers", lHeaders, Vec2f(0, 0), Vec2f(1.f, 0.025f), node);
        pProjects->addChild(lHeaders);
        lSources->setParent(pProjects);
        Node* sNode = new Node("sources",lSources,Vec2f(0, 0), Vec2f(1.f, 0.025f), node);
        pProjects->addChild(lSources);
        lHeaders->setForegroundColor(sf::Color::Green);
        lSources->setForegroundColor(sf::Color::Green);
        pProjects->setAutoResized(true);
        Action a(Action::EVENT_TYPE::MOUSE_BUTTON_PRESSED_ONCE, sf::Mouse::Left);
        Command cmd1(a, FastDelegate<bool>(&Label::isMouseInside, lHeaders), FastDelegate<void>(&ODFAEGCreator::showHeadersFiles, this, lHeaders));
        lHeaders->getListener().connect("SHOWHFILES", cmd1);
        Command cmd2(a, FastDelegate<bool>(&Label::isMouseInside, lSources), FastDelegate<void>(&ODFAEGCreator::showSourcesFiles, this, lSources));
        lSources->getListener().connect("SHOWSFILES", cmd2);
    } else if (!node->isNodeVisible()) {
        node->showAllNodes();
    } else {
        node->hideAllNodes();
    }
}
void ODFAEGCreator::showHeadersFiles(Label* label) {
    Node* node = rootNode->findNode(label);
    if (node->getNodes().size() == 0) {
        FontManager<Fonts>& fm = cache.resourceManager<Font, Fonts>("FontManager");
        std::vector<std::string> files;
        std::string cpath = getCurrentPath();
        findFiles("hpp", files, cpath+"/"+appliname);
        for (unsigned int i = 0; i < files.size(); i++) {
            Label* lab = new Label(getRenderWindow(), Vec3f(0, 0, 0), Vec3f(200, 17, 0),fm.getResourceByAlias(Fonts::Serif), files[i], 15);
            lab->setParent(pProjects);
            lab->setBackgroundColor(sf::Color::White);
            lab->setForegroundColor(sf::Color::Yellow);
            Node* lnode = new Node("header files", lab, Vec2f(0, 0),Vec2f(1,0.025f),node);
            pProjects->addChild(lab);
            Action a(Action::EVENT_TYPE::MOUSE_BUTTON_PRESSED_ONCE, sf::Mouse::Left);
            Command cmd(a, FastDelegate<bool>(&Label::isMouseInside, lab), FastDelegate<void>(&ODFAEGCreator::showFileContent, this, lab));
            lab->getListener().connect("SHOWHFILECONTENT"+lab->getText(), cmd);
        }
    } else if (!node->isNodeVisible()) {
        node->showAllNodes();
    } else {
        node->hideAllNodes();
    }
}
void ODFAEGCreator::showSourcesFiles(Label* label) {
     Node* node = rootNode->findNode(label);
     if (node->getNodes().size() == 0) {
        FontManager<Fonts>& fm = cache.resourceManager<Font, Fonts>("FontManager");
        std::vector<std::string> files;
        std::string cpath = getCurrentPath();
        findFiles("cpp", files, cpath+"/"+appliname);
        for (unsigned int i = 0; i < files.size(); i++) {
            Label* lab = new Label(getRenderWindow(), Vec3f(0, 0, 0), Vec3f(200, 17, 0),fm.getResourceByAlias(Fonts::Serif), files[i], 15);
            Node* lNode = new Node("source files", lab, Vec2f(0, 0), Vec2f(1.f, 0.025f), node);
            lab->setBackgroundColor(sf::Color::White);
            lab->setForegroundColor(sf::Color::Yellow);
            lab->setParent(pProjects);
            pProjects->addChild(lab);
            Action a(Action::EVENT_TYPE::MOUSE_BUTTON_PRESSED_ONCE, sf::Mouse::Left);
            Command cmd(a, FastDelegate<bool>(&Label::isMouseInside, lab), FastDelegate<void>(&ODFAEGCreator::showFileContent, this, lab));
            lab->getListener().connect("SHOWHFILECONTENT"+lab->getText(), cmd);
        }
    } else if (!node->isNodeVisible()){
        node->showAllNodes();
    } else {
        node->hideAllNodes();
    }
}
void ODFAEGCreator::showFileContent(Label* lab) {
    std::ifstream file(appliname+"/"+lab->getText());
    if (file) {
        std::string content, line;
        unsigned int nbLines = 0;
        while (getline(file, line)) {
            content += line + "\n";
            nbLines++;
        }
        tScriptEdit->setTextSize(20);
        tScriptEdit->setText(content);
        pScriptsEdit->updateScrolls();
        file.close();
    }
}
void ODFAEGCreator::processKeyHeldDown (sf::Keyboard::Key key) {

}
void ODFAEGCreator::actionPerformed(Button* button) {
    if (button->getText() == "Create") {
        appliname = ta->getText();
        std::ofstream applis("applis");
        applis<<appliname<<std::endl;
        applis.close();
        applitype = dpList->getSelectedItem();
        path = fdTexturePath->getAppiDir() + "/" + appliname;
        const char* lcars = appliname.c_str();
        char* ucars = new char[appliname.size()];
        for (unsigned int i = 0; i < appliname.length(); i++) {
            ucars[i] = std::toupper(lcars[i]);
        }
        std::string majAppliname (ucars, appliname.length());
        delete ucars;
        ucars = new char[appliname.size()];
        for (unsigned int i = 0; i < appliname.length(); i++) {
            ucars[i] = std::tolower(lcars[i]);
        }
        minAppliname = std::string(ucars, appliname.length());
        if(!mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH))
            std::cerr<<"Failed to create application directory!";
        wApplicationNew->setVisible(false);
        if (applitype == "Normal") {
            std::ofstream header(path+"/"+minAppliname+".hpp");
            std::cerr << "Error: " << strerror(errno);
            header<<"#ifndef "<<majAppliname<<"_HPP"<<std::endl;
            header<<"#define "<<majAppliname<<"_HPP"<<std::endl;
            header<<"#include \"/usr/local/include/odfaeg/Core/application.h\""<<std::endl;
            header<<"class "<<appliname<<" : public odfaeg::core::Application {"<<std::endl;
            header<<"   public : "<<std::endl;
            header<<"   "<<appliname<<"(sf::VideoMode vm, std::string title);"<<std::endl;
            header<<"   void onLoad();"<<std::endl;
            header<<"   void onInit();"<<std::endl;
            header<<"   void onRender(odfaeg::graphic::RenderComponentManager* cm);"<<std::endl;
            header<<"   void onDisplay(odfaeg::graphic::RenderWindow* window);"<<std::endl;
            header<<"   void onUpdate (odfaeg::graphic::RenderWindow*, sf::Event& event);"<<std::endl;
            header<<"   void onExec ();"<<std::endl;
            header<<"};"<<std::endl;
            header<<"#endif"<<std::endl;
            header.close();
            std::ofstream source(path+"/"+minAppliname+".cpp");
            source<<"#include \""+path+"/"+minAppliname+".hpp\""<<std::endl;
            source<<"using namespace odfaeg::graphic;"<<std::endl;
            source<<appliname<<"::"<<appliname<<"(sf::VideoMode vm, std::string title) : "<<std::endl;
            source<<"Application (vm, title, sf::Style::Resize|sf::Style::Close, sf::ContextSettings(0, 0, 0, 3, 0)) {"<<std::endl;
            source<<"}"<<std::endl;
            source<<"void "<<appliname<<"::onLoad() {"<<std::endl;
            source<<"}"<<std::endl;
            source<<"void "<<appliname<<"::onInit() {"<<std::endl;
            source<<"}"<<std::endl;
            source<<"void "<<appliname<<"::onRender(RenderComponentManager *cm) {"<<std::endl;
            source<<"}"<<std::endl;
            source<<"void "<<appliname<<"::onDisplay(RenderWindow* window) {"<<std::endl;
            source<<"}"<<std::endl;
            source<<"void "<<appliname<<"::onUpdate (RenderWindow* window, sf::Event& event) {"<<std::endl;
            source<<" if (&getRenderWindow() == window && event.type == sf::Event::Closed) {"<<std::endl;
            source<<"  stop();"<<std::endl;
            source<<" }"<<std::endl;
            source<<"}"<<std::endl;
            source<<"void "<<appliname<<"::onExec () {"<<std::endl;
            source<<"}"<<std::endl;
            source.close();
            std::string width = taWidth->getText();
            std::string height = taHeight->getText();
            std::ofstream main(path+"/main.cpp");
            main<<"#include \""<<minAppliname<<".hpp\""<<std::endl;
            main<<"int main(int argc, char* argv[]) {"<<std::endl;
            main<<"    "<<appliname<<" app(sf::VideoMode("<<width<<","<<height<<"),\""<<appliname<<"\");"<<std::endl;
            main<<"    return app.exec();"<<std::endl;
            main<<"}"<<std::endl;
            main.close();
        }
    }
}
void ODFAEGCreator::actionPerformed(MenuItem* item) {
    if (item->getText() == "New application") {
        wApplicationNew->setVisible(true);
    }
    if (item->getText() == "Build") {
        std::string command = "clang++-3.6 -Wall -std=c++14 -g "+appliname+"/"+"*.cpp -o "+appliname+"/"+minAppliname+".out "
        "/usr/local/lib/libodfaeg-network-s-d.a /usr/local/lib/libodfaeg-audio-s-d.a /usr/local/lib/libodfaeg-graphics-s-d.a "
        "/usr/local/lib/libodfaeg-physics-s-d.a /usr/local/lib/libodfaeg-math-s-d.a /usr/local/lib/libodfaeg-core-s-d.a "
        "/usr/local/lib/libsfml-audio.so /usr/local/lib/libsfml-network.so /usr/local/lib/libsfml-graphics.so /usr/local/lib/libsfml-window.so "
        "/usr/local/lib/libsfml-system.so /usr/lib/x86_64-linux-gnu/libfreetype.so /usr/lib/x86_64-linux-gnu/libpthread.so /usr/lib/x86_64-linux-gnu/libGLEW.so "
        "-lGL -lssl -lcrypto /usr/lib/x86_64-linux-gnu/libSDL2.so 2> errors.err";
        std::system(command.c_str());
    }
    if (item->getText() == "Run") {
        std::string command = std::string("./"+appliname+"/"+minAppliname+".out");
        std::system(command.c_str());
    }
    if (item->getText() == "Build and run") {
        std::string command = "clang++-3.6 -Wall -std=c++14 -g "+appliname+"/"+"*.cpp -o "+appliname+"/"+minAppliname+".out "
        "/usr/local/lib/libodfaeg-network-s-d.a /usr/local/lib/libodfaeg-audio-s-d.a /usr/local/lib/libodfaeg-graphics-s-d.a "
        "/usr/local/lib/libodfaeg-physics-s-d.a /usr/local/lib/libodfaeg-math-s-d.a /usr/local/lib/libodfaeg-core-s-d.a "
        "/usr/local/lib/libsfml-audio.so /usr/local/lib/libsfml-network.so /usr/local/lib/libsfml-graphics.so /usr/local/lib/libsfml-window.so "
        "/usr/local/lib/libsfml-system.so /usr/lib/x86_64-linux-gnu/libfreetype.so /usr/lib/x86_64-linux-gnu/libpthread.so /usr/lib/x86_64-linux-gnu/libGLEW.so "
        "-lGL -lssl -lcrypto /usr/lib/x86_64-linux-gnu/libSDL2.so 2> errors.err";
        std::system(command.c_str());
        command = std::string("./"+appliname+"/"+minAppliname+".out");
        std::system(command.c_str());
    }
    if (item->getText() == "Rectangle shape") {
        std::unique_ptr<RectangleShape> shape = std::make_unique<RectangleShape>(Vec3f(100, 50, 0));
        shape->setPosition(cursor.getPosition());
        transformables.push_back(shape.get());
        selectedObject = shape.get();
        displayInfos(shape.get());
        drawables.push_back(std::move(shape));
    }
}
void ODFAEGCreator::displayInfos (Transformable* shape) {
    rootPropNode->deleteAllNodes();
    pScriptsEdit->removeAll();
    FontManager<Fonts>& fm = cache.resourceManager<Font, Fonts>("FontManager");
    lTransform = new Label(getRenderWindow(),Vec3f(0,0,0),Vec3f(200, 17, 0),fm.getResourceByAlias(Fonts::Serif),"Transform", 15);
    lTransform->setParent(pScriptsFiles);
    Node* ltranfnode = new Node("LabTransform", lTransform, Vec2f(0, 0), Vec2f(1, 0.025),rootPropNode.get());
    pScriptsFiles->addChild(lTransform);
    lPosition = new Label(getRenderWindow(),Vec3f(0,0,0),Vec3f(200, 17, 0),fm.getResourceByAlias(Fonts::Serif),"Position : ", 15);
    lPosition->setParent(pScriptsFiles);
    Node* lPosNode = new Node("LabPosition",lPosition,Vec2f(0, 0), Vec2f(1, 0.025),rootPropNode.get());
    pScriptsFiles->addChild(lPosition);
    lPosX = new Label(getRenderWindow(), Vec3f(0, 0, 0), Vec3f(200, 17, 0),fm.getResourceByAlias(Fonts::Serif),"x : ",15);
    lPosX->setParent(pScriptsFiles);
    Node* lPosXNode = new Node("LabX",lPosX,Vec2f(0, 0),Vec2f(0.25, 0.025), rootPropNode.get());
    pScriptsFiles->addChild(lPosX);
    tPosX = new TextArea(Vec3f(0, 0, 0), Vec3f(100, 50, 0),fm.getResourceByAlias(Fonts::Serif),conversionFloatString(shape->getPosition().x),getRenderWindow());
    tPosX->setParent(pScriptsFiles);
    tPosX->setTextSize(15);
    lPosXNode->addOtherComponent(tPosX, Vec2f(0.75, 0.025));
    pScriptsFiles->addChild(tPosX);
    lPosY = new Label(getRenderWindow(), Vec3f(0, 0, 0), Vec3f(200, 17, 0),fm.getResourceByAlias(Fonts::Serif),"y : ",15);
    lPosY->setParent(pScriptsFiles);
    Node* lPosYNode = new Node("LabY",lPosY,Vec2f(0, 0),Vec2f(0.25, 0.025), rootPropNode.get());
    pScriptsFiles->addChild(lPosY);
    tPosY = new TextArea(Vec3f(0, 0, 0), Vec3f(100, 50, 0),fm.getResourceByAlias(Fonts::Serif),conversionFloatString(shape->getPosition().y),getRenderWindow());
    tPosY->setParent(pScriptsFiles);
    tPosY->setTextSize(15);
    lPosYNode->addOtherComponent(tPosY, Vec2f(0.75, 0.025));
    pScriptsFiles->addChild(tPosY);
    lPosZ = new Label(getRenderWindow(), Vec3f(0, 0, 0), Vec3f(200, 17, 0),fm.getResourceByAlias(Fonts::Serif),"z : ",15);
    lPosZ->setParent(pScriptsFiles);
    Node* lPosZNode = new Node("LabZ",lPosZ,Vec2f(0, 0),Vec2f(0.25, 0.025), rootPropNode.get());
    pScriptsFiles->addChild(lPosZ);
    tPosZ = new TextArea(Vec3f(0, 0, 0), Vec3f(100, 50, 0),fm.getResourceByAlias(Fonts::Serif),conversionFloatString(shape->getPosition().z),getRenderWindow());
    tPosZ->setParent(pScriptsFiles);
    tPosZ->setTextSize(15);
    lPosZNode->addOtherComponent(tPosZ, Vec2f(0.75, 0.025));
    pScriptsFiles->addChild(tPosZ);
    Action a (Action::EVENT_TYPE::TEXT_ENTERED);
    Command cmdPosX (a, FastDelegate<bool>(&TextArea::isTextChanged, tPosX), FastDelegate<void>(&ODFAEGCreator::onObjectPosChanged, this,tPosX));
    Command cmdPosY (a, FastDelegate<bool>(&TextArea::isTextChanged, tPosY), FastDelegate<void>(&ODFAEGCreator::onObjectPosChanged, this,tPosY));
    Command cmdPosZ (a, FastDelegate<bool>(&TextArea::isTextChanged, tPosZ), FastDelegate<void>(&ODFAEGCreator::onObjectPosChanged, this,tPosZ));
    tPosX->getListener().connect("tPosXChanged", cmdPosX);
    tPosY->getListener().connect("tPosYChanged", cmdPosY);
    tPosZ->getListener().connect("tPosZChanged", cmdPosZ);
}
void ODFAEGCreator::moveCursor(sf::Vector2f mousePos) {
    BoundingBox bb (guiPos.x, guiPos.y, guiPos.z, guiSize.x, guiSize.y, guiSize.z);
    if (bb.isPointInside(Vec3f(mousePos.x, mousePos.y, 0))) {
        cursor.setPosition(Vec3f(mousePos.x-getRenderWindow().getView().getSize().x * 0.5f, mousePos.y-getRenderWindow().getView().getSize().y * 0.5f, 0));
    }
}
void ODFAEGCreator::onObjectPosChanged(TextArea* ta) {
    if (ta == tPosX) {
        if (is_number(ta->getText())) {
            float newXPos = conversionStringFloat(ta->getText());
            selectedObject->setPosition(Vec3f(newXPos, selectedObject->getPosition().y, selectedObject->getPosition().z));
        }
    } else if (ta == tPosY) {
        if(is_number(ta->getText())) {
            float newYPos = conversionStringFloat(ta->getText());
            selectedObject->setPosition(Vec3f(selectedObject->getPosition().x, newYPos, selectedObject->getPosition().z));
        }
    } else if (ta == tPosZ) {
        if(is_number(ta->getText())) {
            float newZPos = conversionStringFloat(ta->getText());
            selectedObject->setPosition(Vec3f(selectedObject->getPosition().x, selectedObject->getPosition().y, newZPos));
        }
    }
}

