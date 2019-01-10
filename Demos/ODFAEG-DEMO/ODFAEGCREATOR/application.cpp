#include "application.h"
#include "odfaeg/Core/action.h"
#include "odfaeg/Core/command.h"
#include <sys/stat.h>
#include <stdlib.h>
#include "odfaeg/Core/utilities.h"
using namespace odfaeg::core;
using namespace odfaeg::math;
using namespace odfaeg::graphic;
using namespace odfaeg::graphic::gui;
ODFAEGCreator::ODFAEGCreator(sf::VideoMode vm, std::string title) :
 Application (vm, title, sf::Style::Resize|sf::Style::Close, sf::ContextSettings(0, 0, 0, 3, 0)) {

}
void ODFAEGCreator::onLoad() {
    FontManager<Fonts> fm;
    fm.fromFileWithAlias("fonts/FreeSerif.ttf", Serif);
    cache.addResourceManager(fm, "FontManager");
}
void ODFAEGCreator::onInit() {
    OITRenderComponent* rc0 = new OITRenderComponent(getRenderWindow(), 0, "");
    getRenderComponentManager().addComponent(rc0);
    FontManager<Fonts>& fm = cache.resourceManager<Font, Fonts>("FontManager");
    menuBar = new MenuBar(getRenderWindow());
    getRenderComponentManager().addComponent(menuBar);
    menu1 = new Menu(getRenderWindow(),fm.getResourceByAlias(Fonts::Serif),"File");
    getRenderComponentManager().addComponent(menu1);
    menu2 = new Menu(getRenderWindow(),fm.getResourceByAlias(Fonts::Serif),"Compile");
    getRenderComponentManager().addComponent(menu2);
    menu3 = new Menu(getRenderWindow(),fm.getResourceByAlias(Fonts::Serif),"Test 3");
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
    item31 = new MenuItem(getRenderWindow(), fm.getResourceByAlias(Fonts::Serif),"Test 3 1");
    getRenderComponentManager().addComponent(item31);
    item32 = new MenuItem(getRenderWindow(), fm.getResourceByAlias(Fonts::Serif),"Test 3 2");
    getRenderComponentManager().addComponent(item32);
    item33 = new MenuItem(getRenderWindow(), fm.getResourceByAlias(Fonts::Serif),"Test 3 3");
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
    //fdTexturePath->setVisible(false);
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
    pProjects->setRelPosition(0.f, 0.f);
    pProjects->setRelSize(1.f / 6.f, 1.f);
    pProjects->setBorderColor(sf::Color(128, 128, 128));
    pProjects->setBackgroundColor(sf::Color::White);
    pProjects->setBorderThickness(5);
    std::ifstream applis("applis");
    std::string line;
    unsigned int i = 0;
    Label* lab = new Label(getRenderWindow(),Vec3f(0,20,0),Vec3f(200, 10, 0),fm.getResourceByAlias(Fonts::Serif),"GUI", 15);
    lab->setForegroundColor(sf::Color::Red);
    lab->setParent(pProjects);
    pProjects->addChild(lab);
    Action a(Action::EVENT_TYPE::MOUSE_BUTTON_PRESSED_ONCE, sf::Mouse::Left);
    Command cmd(a, FastDelegate<bool>(&Label::isMouseInside, lab), FastDelegate<void>(&ODFAEGCreator::showGUI, this, lab));
    getListener().connect("SHOWGUI", cmd);
    if (applis) {
        while(getline(applis, line)) {
            Label* lab = new Label(getRenderWindow(),Vec3f(0,30+i*10,0),Vec3f(200, 10, 0),fm.getResourceByAlias(Fonts::Serif),line, 15);
            lab->setParent(pProjects);
            pProjects->addChild(lab);
            Action a(Action::EVENT_TYPE::MOUSE_BUTTON_PRESSED_ONCE, sf::Mouse::Left);
            Command cmd(a, FastDelegate<bool>(&Label::isMouseInside, lab), FastDelegate<void>(&ODFAEGCreator::showProjectsFiles, this, lab));
            getListener().connect("SHOWPFILES", cmd);
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
    pScriptsEdit->setRelSize(0.75f, 1.f);
    pScriptsEdit->setBorderColor(sf::Color(128, 128, 128));
    pScriptsEdit->setBackgroundColor(sf::Color::White);
    pScriptsEdit->setBorderThickness(5);
    getRenderComponentManager().addComponent(pScriptsEdit);
    pScriptsFiles = new Panel(getRenderWindow(),Vec3f(1000, 0, 0), Vec3f(200, 700, 0), 0);
    pScriptsFiles->setRelPosition(5.f / 6.f, 0.f);
    pScriptsFiles->setRelSize(1.f / 6.f, 1.f);
    pScriptsFiles->setBorderColor(sf::Color(128, 128, 128));
    pScriptsFiles->setBackgroundColor(sf::Color::White);
    pScriptsFiles->setBorderThickness(5);
    tScriptEdit = new TextArea(Vec3f(200, 20, 0),Vec3f(790,650,0),fm.getResourceByAlias(Fonts::Serif),"",getRenderWindow());
    tScriptEdit->setParent(pScriptsEdit);
    tScriptEdit->setRelPosition(0.f, 0.f);
    tScriptEdit->setRelSize(0.9f, 0.9f);
    pScriptsEdit->addChild(tScriptEdit);
}
void ODFAEGCreator::onRender(RenderComponentManager *cm) {

}
void ODFAEGCreator::onDisplay(RenderWindow* window) {

}
void ODFAEGCreator::onUpdate(RenderWindow* window, sf::Event& event) {
    getListener().setCommandSlotParams("MoveAction", this, event.key.code);
    if (&getRenderWindow() == window && event.type == sf::Event::Closed) {
        stop();
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
    pScriptsEdit->setVisible(false);
    getRenderComponentManager().getRenderComponent(0)->setVisible(true);
}
void ODFAEGCreator::showProjectsFiles(Label* label) {
    getRenderComponentManager().getRenderComponent(0)->setVisible(false);
    pScriptsEdit->setVisible(true);
    if (label->getChildren().size() == 0) {
        FontManager<Fonts>& fm = cache.resourceManager<Font, Fonts>("FontManager");
        std::vector<LightComponent*> children = pProjects->getChildren();
        unsigned int y = label->getPosition().y + label->getSize().y;
        Label* lheaders = new Label(getRenderWindow(),Vec3f(0, y, 0),Vec3f(200,10,0),fm.getResourceByAlias(Fonts::Serif),"headers", 15);
        Label* lsources = new Label(getRenderWindow(),Vec3f(0, y+10,0),Vec3f(200,10,0),fm.getResourceByAlias(Fonts::Serif),"sources", 15);
        lheaders->setParent(label);
        lheaders->setRelPosition(0.f, 1.f);
        lheaders->setRelSize(1.f, 1.f);
        label->addChild(lheaders);
        lsources->setParent(label);
        lsources->setRelPosition(0.f, 2.f);
        lsources->setRelSize(1.f, 1.f);
        label->addChild(lsources);
        lheaders->setForegroundColor(sf::Color::Red);
        lsources->setForegroundColor(sf::Color::Red);
        unsigned int toMove = 20;
        for (unsigned int i = 0; i < children.size(); i++) {
            if (children[i]->getPosition().y > label->getPosition().y) {
                children[i]->move(Vec3f(0, toMove, 0));
            }
        }
        Action a(Action::EVENT_TYPE::MOUSE_BUTTON_PRESSED_ONCE, sf::Mouse::Left);
        Command cmd1(a, FastDelegate<bool>(&Label::isMouseInside, lheaders), FastDelegate<void>(&ODFAEGCreator::showHeadersFiles, this, lheaders));
        getListener().connect("SHOWHFILES", cmd1);
        Command cmd2(a, FastDelegate<bool>(&Label::isMouseInside, lsources), FastDelegate<void>(&ODFAEGCreator::showSourcesFiles, this, lsources));
        getListener().connect("SHOWSFILES", cmd2);
    } else {
        getListener().removeCommand("SHOWHFILES");
        getListener().removeCommand("SHOWSFILES");
        label->removeAll();
    }
}
void ODFAEGCreator::showHeadersFiles(Label* label) {
    if (label->getChildren().size() == 0) {
        FontManager<Fonts>& fm = cache.resourceManager<Font, Fonts>("FontManager");
        std::vector<std::string> files;
        std::string cpath = getCurrentPath();
        findFiles("hpp", files, cpath+"/"+appliname);
        unsigned int y = label->getPosition().y + 10;
        unsigned int toMove = 0;
        for (unsigned int i = 0; i < files.size(); i++) {
            Label* lab = new Label(getRenderWindow(), Vec3f(0, y+toMove, 0), Vec3f(200, 10, 0),fm.getResourceByAlias(Fonts::Serif), files[i], 15);
            lab->setParent(label);
            lab->setRelPosition(0.f, (i + 1) * 1.f);
            lab->setRelSize(1.f, 1.f);
            label->addChild(lab);
            Action a(Action::EVENT_TYPE::MOUSE_BUTTON_PRESSED_ONCE, sf::Mouse::Left);
            Command cmd(a, FastDelegate<bool>(&Label::isMouseInside, lab), FastDelegate<void>(&ODFAEGCreator::showFileContent, this, lab));
            getListener().connect("SHOWHFILECONTENT"+lab->getText(), cmd);
            toMove += 10;
        }
        std::vector<LightComponent*> children = label->getParent()->getChildren();
        for (unsigned int i = 1; i < children.size(); i++) {
            if (children[i]->getPosition().y > label->getPosition().y) {
                children[i]->move(Vec3f(0, toMove, 0));
                children[i]->setRelPosition(0.f, (2 + files.size()) * 1.f);
                children[i]->setRelSize(1.f, 1.f);
                std::vector<LightComponent*> schildren = children[i]->getChildren();
                for (unsigned int j = 0; j < schildren.size(); j++) {
                    schildren[j]->move(Vec3f(0, toMove, 0));
                    schildren[j]->setRelPosition(0.f, (j + 1) * 1.f);
                    schildren[j]->setRelSize(1.f, 1.f);
                }
            }
        }
    } else {
        unsigned int y = label->getPosition().y + 10;
        std::vector<LightComponent*> children = label->getParent()->getChildren();
        int toMove = -children.size() * 10;
        for (unsigned int i = 1; i < children.size(); i++) {
            if (children[i]->getPosition().y > y) {
                children[i]->move(Vec3f(0, toMove, 0));
                children[i]->setRelPosition(0.f, 2.f);
                children[i]->setRelSize(1.f, 1.f);
                std::vector<LightComponent*> schildren = children[i]->getChildren();
                for (unsigned int j = 0; j < schildren.size(); j++) {
                    schildren[j]->move(Vec3f(0, toMove, 0));
                    schildren[j]->setRelPosition(0.f, (j + 1) * 1.f);
                    schildren[j]->setRelSize(1.f, 1.f);
                }
            }
        }
        for (unsigned int i = 0; i < label->getChildren().size(); i++) {
            getListener().removeCommand("SHOWFILECONTENT"+static_cast<Label*>(label->getChildren()[i])->getText());
        }
        label->removeAll();
    }
}
void ODFAEGCreator::showSourcesFiles(Label* label) {
     if (label->getChildren().size() == 0) {
        FontManager<Fonts>& fm = cache.resourceManager<Font, Fonts>("FontManager");
        std::vector<std::string> files;
        std::string cpath = getCurrentPath();
        findFiles("cpp", files, cpath+"/"+appliname);
        unsigned int y = label->getPosition().y + 10;
        unsigned int toMove = 0;
        for (unsigned int i = 0; i < files.size(); i++) {
            Label* lab = new Label(getRenderWindow(), Vec3f(0, y+toMove, 0), Vec3f(200, 10, 0),fm.getResourceByAlias(Fonts::Serif), files[i], 15);
            lab->setParent(label);
            lab->setRelPosition(0.f, (i + 1) * 1.f);
            lab->setRelSize(1.f, 1.f);
            label->addChild(lab);
            Action a(Action::EVENT_TYPE::MOUSE_BUTTON_PRESSED_ONCE, sf::Mouse::Left);
            Command cmd(a, FastDelegate<bool>(&Label::isMouseInside, lab), FastDelegate<void>(&ODFAEGCreator::showFileContent, this, lab));
            getListener().connect("SHOWHFILECONTENT"+lab->getText(), cmd);
            toMove += 10;
        }
        /*std::vector<LightComponent*> children = label->getParent()->getChildren();
        for (unsigned int i = 0; i < children.size(); i++) {
            if (children[i]->getPosition().y > label->getPosition().y) {
                children[i]->move(Vec3f(0, toMove, 0));
            }
        }*/
    } else {
        for (unsigned int i = 0; i < label->getChildren().size(); i++) {
            getListener().removeCommand("SHOWFILECONTENT"+static_cast<Label*>(label->getChildren()[i])->getText());
        }
        label->removeAll();
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
        unsigned int height = 20;
        tScriptEdit->setSize(Vec3f(800, height, 0));
        tScriptEdit->setPosition(Vec3f(200, 0, 0));
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
}
