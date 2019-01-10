#include "../../../../include/odfaeg/Graphics/GUI/filedialog.hpp"
#include "../../../../include/odfaeg/Math/maths.h"
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
            lTop(rw, position, size, font, "",15),
            bChoose (position, size, font, "Choose", 15, rw),
            bCancel (position, size, font, "Cancel", 15, rw),
            font(font) {
                pTop.setRelPosition(0.f, 0.f);
                pTop.setRelSize(1.f, 0.1f);
                pDirectories.setRelPosition(0.f, 0.1f);
                pDirectories.setRelSize(0.5f, 0.8f);
                pFiles.setRelPosition(0.5f, 0.1f);
                pFiles.setRelSize(0.5f, 0.8f);
                pBottom.setRelPosition(0.f, 0.9f);
                pBottom.setRelSize(1.f, 0.1f);
                pTop.setParent(this);
                pBottom.setParent(this);
                pDirectories.setParent(this);
                pFiles.setParent(this);
                addChild(&pTop);
                addChild(&pBottom);
                addChild(&pDirectories);
                addChild(&pFiles);
                lTop.setParent(&pTop);
                bChoose.setParent(&pBottom);
                bCancel.setParent(&pBottom);
                bChoose.addActionListener(this);
                bCancel.addActionListener(this);
                char cCurrentPath[FILENAME_MAX];
                getcwd(cCurrentPath,sizeof(cCurrentPath));
                lTop.setRelPosition(0.f, 0.f);
                lTop.setRelSize(1.f, 1.f);
                pTop.addChild(&lTop);
                std::string currentPath (cCurrentPath);
                lTop.setText(currentPath);
                lTop.setBackgroundColor(sf::Color::Red);
                appliDir = lTop.getText();
                std::string textDir;
                if (DIR* root = opendir("/")) {
                    dirent* ent;
                    unsigned int i = 0;
                    while((ent = readdir(root)) != NULL) {
                        if (strcmp(ent->d_name, ".") && strcmp(ent->d_name, "..")) {
                            textDir = std::string(ent->d_name);
                            Label* lDirectory = new Label(rw, position, size, font, "",15);
                            //lDirectory->setBackgroundColor(sf::Color(math::Math::random(0, 255),math::Math::random(0, 255), math::Math::random(0, 255), 255));
                            lDirectory->setParent(&pDirectories);
                            lDirectory->setRelPosition(0.f, 0.04f * i);
                            lDirectory->setRelSize(1, 0.04f);
                            pDirectories.addChild(lDirectory);
                            lDirectory->setText(sf::String(textDir));
                            core::Action a (core::Action::EVENT_TYPE::MOUSE_BUTTON_PRESSED_ONCE, sf::Mouse::Left);
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
                            Label* lFile = new Label(rw, position, size, font, "",15);
                            //lFile->setBackgroundColor(sf::Color(math::Math::random(0, 255),math::Math::random(0, 255), math::Math::random(0, 255), 255));
                            lFile->setParent(&pFiles);
                            lFile->setRelPosition(0.f, 0.04f * i);
                            lFile->setRelSize(1.f, 0.04f);
                            pFiles.addChild(lFile);
                            lFile->setText(sf::String(fileNames));
                            core::Action a (core::Action::EVENT_TYPE::MOUSE_BUTTON_PRESSED_ONCE, sf::Mouse::Left);
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
                bChoose.setRelPosition(0.7f, 0.1f);
                bChoose.setRelSize(0.1f, 0.8f);
                bChoose.setParent(&pBottom);
                pBottom.addChild(&bChoose);
                bCancel.setRelPosition(0.85f, 0.1f);
                bCancel.setRelSize(0.1f, 0.8f);
                bCancel.setParent(&pBottom);
                pBottom.addChild(&bCancel);
                setRelSize(1, 1);
                setAutoResized(true);
            }
            void FileDialog::clear() {
                rw.clear();
                pTop.clear();
                pBottom.clear();
                pDirectories.clear();
                pFiles.clear();
            }
            void FileDialog::onDraw(RenderTarget& target, RenderStates states) {
                if (rw.isOpen()) {
                    rw.draw(pTop, states);
                    rw.draw(pDirectories, states);
                    rw.draw(pFiles, states);
                    rw.draw(pBottom, states);
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
                                Label* lFile = new Label(rw, getPosition(), getSize(), font, "",15);
                                //lFile->setBackgroundColor(sf::Color(math::Math::random(0, 255),math::Math::random(0, 255), math::Math::random(0, 255), 255));
                                lFile->setParent(&pFiles);
                                lFile->setRelPosition(0.f, 0.04f * i);
                                lFile->setRelSize(1.f, 0.04f);
                                pFiles.addChild(lFile);
                                lFile->setText(fileNames);
                                core::Action a (core::Action::EVENT_TYPE::MOUSE_BUTTON_PRESSED_ONCE, sf::Mouse::Left);
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
                setAutoResized(true);
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
                                Label* lFile = new Label(rw, getPosition(), getSize(), font, "",15);
                                //lFile->setBackgroundColor(sf::Color(math::Math::random(0, 255),math::Math::random(0, 255), math::Math::random(0, 255), 255));
                                lFile->setParent(&pFiles);
                                lFile->setRelPosition(0.f, 0.04f * i);
                                lFile->setRelSize(1.f, 0.04f);
                                pFiles.addChild(lFile);
                                lFile->setText(fileNames);
                                core::Action a (core::Action::EVENT_TYPE::MOUSE_BUTTON_PRESSED_ONCE, sf::Mouse::Left);
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
                setAutoResized(true);
            }
            std::string FileDialog::getPathChosen() {
                std::string path = pathChosen;
                pathChosen = "";
                return path;
            }
            std::string FileDialog::getAppiDir() {
                return appliDir;
            }
            void FileDialog::onEventPushed(sf::Event event, RenderWindow& window) {
                if (event.type == sf::Event::Closed) {
                    rw.setVisible(false);
                }
                /*for (unsigned int i = 0; i < pDirectories.getChildren().size(); i++) {
                    pDirectories.getChildren()[i]->onUpdate(&getWindow(), event);
                }
                for (unsigned int i = 0; i < pFiles.getChildren().size(); i++) {
                    pFiles.getChildren()[i]->onUpdate(&getWindow(), event);
                }
                bChoose.onUpdate(&getWindow(), event);
                bCancel.onUpdate(&getWindow(), event);
                getListener().pushEvent(event);
                bChoose.getListener().pushEvent(event);
                bCancel.getListener().pushEvent(event);*/
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
