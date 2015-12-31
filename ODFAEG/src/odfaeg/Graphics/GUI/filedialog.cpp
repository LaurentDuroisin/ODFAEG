#include "../../../../include/odfaeg/Graphics/GUI/filedialog.hpp"
namespace odfaeg {
    namespace graphic {
        namespace gui {
            FileDialog::FileDialog(math::Vec3f position, math::Vec3f size, const Font* font) :
            rw(sf::VideoMode(size.x, size.y), "File Dialog", sf::Style::Default, sf::ContextSettings(0, 0, 0, 3, 0)),
            LightComponent (rw, position, size, size * 0.5),
            pTop(rw, position, size),
            pBottom (rw, position, size),
            pDirectories(rw, position, size),
            pFiles(rw, position, size),
            lTop(rw, position, size, font, ""),
            bChoose (position, size, font, "Choose", rw),
            bCancel (position, size, font, "Cancel", rw),
            font(font) {
                pTop.setRelToParentLayout(math::Vec3f(0, 0, 1, 0.1));
                pDirectories.setRelToParentLayout(math::Vec3f(0, 0.1, 0.5, 0.8));
                pFiles.setRelToParentLayout(math::Vec3f(0.5, 0.1, 0.5, 0.8));
                pBottom.setRelToParentLayout(math::Vec3f(0, 0.9, 1, 0.1));
                lTop.setParent(&pTop);
                bChoose.setParent(&pBottom);
                bCancel.setParent(&pBottom);
                bChoose.addActionListener(this);
                bCancel.addActionListener(this);
                char cCurrentPath[FILENAME_MAX];
                getcwd(cCurrentPath,sizeof(cCurrentPath));
                lTop.setRelToParentLayout(math::Vec3f(0, 0, 1, 1));
                pTop.addChild(&lTop);
                std::string currentPath (cCurrentPath);
                lTop.setText(currentPath);
                appliDir = lTop.getText();
                std::string textDir;
                if (DIR* root = opendir("/")) {
                    dirent* ent;
                    unsigned int i = 0;
                    while((ent = readdir(root)) != NULL) {
                        if (strcmp(ent->d_name, ".") && strcmp(ent->d_name, "..")) {
                            textDir = std::string(ent->d_name);
                            Label* lDirectory = new Label(rw, position, size, font, "");
                            lDirectory->setParent(&pDirectories);
                            lDirectory->setRelToParentLayout(math::Vec3f(0, 0.04 * i, 1, 0.04));
                            pDirectories.addChild(lDirectory);
                            lDirectory->setText(sf::String(textDir));
                            core::Action a (core::Action::EVENT_TYPE::MOUSE_BUTTON_PRESSED_ONCE, sf::Mouse::Right);
                            core::Command cmd (a, core::FastDelegate<bool>(&Label::isMouseInside, lDirectory), core::FastDelegate<void>(&FileDialog::onDirSelected, this, lDirectory));
                            if(ent->d_type == DT_DIR) {
                                lDirectory->setForegroundColor(sf::Color::Red);
                                getListener().connect("1d"+textDir, cmd);
                            } else {
                                getListener().connect("1f"+textDir, cmd);
                            }
                            i++;
                        }
                    }
                }
                currentPath = lTop.getText();
                if (DIR* current = opendir(currentPath.c_str())) {
                    dirent *ent;
                    std::string fileNames;
                    unsigned int i = 0;
                    while ((ent = readdir(current)) != NULL) {
                        if (strcmp(ent->d_name, ".") && strcmp(ent->d_name, "..")) {
                            fileNames = std::string(ent->d_name);
                            Label* lFile = new Label(rw, position, size, font, "");
                            lFile->setParent(&pFiles);
                            lFile->setRelToParentLayout(math::Vec3f(0, 0.04 * i, 1, 0.04));
                            pFiles.addChild(lFile);
                            lFile->setText(sf::String(fileNames));
                            core::Action a (core::Action::EVENT_TYPE::MOUSE_BUTTON_PRESSED_ONCE, sf::Mouse::Right);
                            core::Command cmd (a, core::FastDelegate<bool>(&Label::isMouseInside, lFile), core::FastDelegate<void>(&FileDialog::onFileSelected, this, lFile));
                            if(ent->d_type == DT_DIR) {
                                lFile->setForegroundColor(sf::Color::Red);
                                getListener().connect("2d"+fileNames, cmd);
                            } else {
                                getListener().connect("2f"+fileNames, cmd);
                            }
                            i++;
                        }
                    }
                }
                bChoose.setRelToParentLayout(math::Vec3f(0.7, 0.1, 0.1, 0.8));
                pBottom.addChild(&bChoose);
                bCancel.setRelToParentLayout(math::Vec3f(0.85, 0.1, 0.1, 0.8));
                pBottom.addChild(&bCancel);
                View defaultView = rw.getDefaultView();
                defaultView.setCenter(math::Vec3f(rw.getSize().x * 0.5f, rw.getSize().y * 0.5f, 0));
                rw.setView(defaultView);
            }
            void FileDialog::clear() {
                rw.clear();
                pTop.clear();
                pBottom.clear();
                pDirectories.clear();
                pFiles.clear();
            }
            void FileDialog::draw(RenderTarget& target, RenderStates states) {
                if (rw.isOpen()) {
                    rw.draw(pTop, states);
                    rw.draw(pBottom, states);
                    rw.draw(pDirectories, states);
                    rw.draw(pFiles, states);
                    rw.display();
                }
            }
            void FileDialog::onDirSelected(Label* label) {
                for (unsigned int i = 0; i < pFiles.getChildren().size(); i++) {
                    if (static_cast<Label*>(pFiles.getChildren()[i])->getForegroundColor() == sf::Color::Red) {
                        getListener().removeCommand("2d"+static_cast<Label*>(pFiles.getChildren()[i])->getText());
                    } else {
                        getListener().removeCommand("2f"+static_cast<Label*>(pFiles.getChildren()[i])->getText());
                    }
                }
                pFiles.removeAll();
                for (unsigned int i = 0; i < pDirectories.getChildren().size(); i++) {
                    static_cast<Label*>(pDirectories.getChildren()[i])->setBackgroundColor(sf::Color::Black);
                }
                std::string dirName = (label->getText().at(0) != '/') ? "/" + label->getText() : label->getText();
                std::string currentDir = dirName;
                lTop.setText(currentDir);
                label->setBackgroundColor(sf::Color::Blue);
                if (label->getForegroundColor() == sf::Color::Red) {
                    if (DIR* current = opendir(dirName.c_str())) {
                        dirent *ent;
                        std::string fileNames;
                        unsigned int i = 0;
                        while ((ent = readdir(current)) != NULL) {
                            if (strcmp(ent->d_name, ".") && strcmp(ent->d_name, "..")) {
                                fileNames = std::string(ent->d_name);
                                Label* lFile = new Label(rw, getPosition(), getSize(), font, "");
                                lFile->setParent(&pFiles);
                                lFile->setRelToParentLayout(math::Vec3f(0, 0.04 * i, 1, 0.04));
                                pFiles.addChild(lFile);
                                lFile->setText(fileNames);
                                core::Action a (core::Action::EVENT_TYPE::MOUSE_BUTTON_PRESSED_ONCE, sf::Mouse::Right);
                                core::Command cmd (a, core::FastDelegate<bool>(&Label::isMouseInside, lFile), core::FastDelegate<void>(&FileDialog::onFileSelected, this, lFile));
                                if(ent->d_type == DT_DIR) {
                                    lFile->setForegroundColor(sf::Color::Red);
                                    getListener().connect("2d"+fileNames, cmd);
                                } else {
                                    getListener().connect("2f"+fileNames, cmd);
                                }
                                i++;
                            }
                        }
                        closedir(current);
                    }
                }
            }
            void FileDialog::onFileSelected(Label* label) {
                std::string fileName = label->getText();
                sf::Color color = label->getForegroundColor();
                if (color == sf::Color::Red) {
                    for (unsigned int i = 0; i < pFiles.getChildren().size(); i++) {
                        if (static_cast<Label*>(pFiles.getChildren()[i])->getForegroundColor() == sf::Color::Red) {
                            if (static_cast<Label*>(pFiles.getChildren()[i])->getText() == label->getText())
                                getListener().removeLater("2d"+static_cast<Label*>(pFiles.getChildren()[i])->getText());
                            else
                                getListener().removeCommand("2d"+static_cast<Label*>(pFiles.getChildren()[i])->getText());
                        } else {
                             if (static_cast<Label*>(pFiles.getChildren()[i])->getText() == label->getText())
                                getListener().removeLater("2f"+static_cast<Label*>(pFiles.getChildren()[i])->getText());
                             else
                                getListener().removeCommand("2f"+static_cast<Label*>(pFiles.getChildren()[i])->getText());
                        }
                    }
                    pFiles.removeAll();
                    std::string currentDir = lTop.getText();
                    currentDir += "/"+fileName;
                    lTop.setText(currentDir);
                    if (DIR* current = opendir(currentDir.c_str())) {
                        dirent *ent;
                        std::string fileNames;
                        unsigned int i = 0;
                        while ((ent = readdir(current)) != NULL) {
                            if (strcmp(ent->d_name, ".") && strcmp(ent->d_name, "..")) {
                                fileNames = std::string(ent->d_name);
                                Label* lFile = new Label(rw, getPosition(), getSize(), font, "");
                                lFile->setParent(&pFiles);
                                lFile->setRelToParentLayout(math::Vec3f(0, 0.04 * i, 1, 0.04));
                                pFiles.addChild(lFile);
                                lFile->setText(fileNames);
                                core::Action a (core::Action::EVENT_TYPE::MOUSE_BUTTON_PRESSED_ONCE, sf::Mouse::Right);
                                core::Command cmd (a, core::FastDelegate<bool>(&Label::isMouseInside, lFile), core::FastDelegate<void>(&FileDialog::onFileSelected, this, lFile));
                                if(ent->d_type == DT_DIR) {
                                    lFile->setForegroundColor(sf::Color::Red);
                                    getListener().connect("2d"+fileNames, cmd);
                                } else {
                                    getListener().connect("2f"+fileNames, cmd);
                                }
                                i++;
                            }
                        }
                        closedir(current);
                    }
                } else {
                    for (unsigned int i = 0; i < pFiles.getChildren().size(); i++) {
                        static_cast<Label*>(pFiles.getChildren()[i])->setBackgroundColor(sf::Color::Black);
                    }
                    label->setBackgroundColor(sf::Color::Blue);
                }
            }
            std::string FileDialog::getPathChosen() {
                std::string path = pathChosen;
                pathChosen = "";
                return path;
            }
            void FileDialog::pushEvent(sf::Event event) {

            }
            void FileDialog::checkSubWindowEvents() {
                sf::Event event;
                while(rw.pollEvent(event)) {
                    if (event.type == sf::Event::Closed) {
                        rw.setVisible(false);
                    }
                    for (unsigned int i = 0; i < pDirectories.getChildren().size(); i++) {
                        pDirectories.getChildren()[i]->onUpdate(&getWindow(), event);
                    }
                    for (unsigned int i = 0; i < pFiles.getChildren().size(); i++) {
                        pFiles.getChildren()[i]->onUpdate(&getWindow(), event);
                    }
                    bChoose.onUpdate(&getWindow(), event);
                    bCancel.onUpdate(&getWindow(), event);
                    getListener().pushEvent(event);
                    bChoose.getListener().pushEvent(event);
                    bCancel.getListener().pushEvent(event);
                }
                bChoose.getListener().processEvents();
                bCancel.getListener().processEvents();
                pDirectories.getListener().processEvents();
                pFiles.getListener().processEvents();
            }
            void FileDialog::onVisibilityChanged(bool visible) {
                rw.setVisible(false);
            }
            void FileDialog::actionPerformed(Button* button) {
                if (button->getText() == "Choose") {
                    for (unsigned int i = 0; i < pDirectories.getChildren().size(); i++) {
                        Label* label = static_cast<Label*>(pDirectories.getChildren()[i]);
                        if (label->getForegroundColor() == sf::Color::White
                            && label->getBackgroundColor() == sf::Color::Blue) {
                            pathChosen = lTop.getText();
                            return;
                        }
                    }
                    for (unsigned int i = 0; i < pFiles.getChildren().size(); i++) {
                        Label* label = static_cast<Label*>(pFiles.getChildren()[i]);
                        if (label->getForegroundColor() == sf::Color::White
                            && label->getBackgroundColor() == sf::Color::Blue) {
                            pathChosen = lTop.getText()+"/"+label->getText();
                        }
                    }
                } else if (button->getText() == "Cancel") {
                    rw.setVisible(false);
                }
            }
        }
    }
}
