#ifndef ODFAEG_APPLICATION_HPP
#define ODFAEG_APPLICATION_HPP
#include "resourceCache.h"
#include "../Graphics/renderWindow.h"
#include "../Graphics/renderStates.h"
#include "../Graphics/renderComponentManager.h"
#include "../Graphics/world.h"
#include "export.hpp"
#include "singleton.h"
#include "../Network/network.h"

/**
  *\namespace odfaeg
  * the namespace of the Opensource Development Framework Adapted for Every Games.
  */
namespace odfaeg {
    namespace core {
        /**
        * \file application.h
        * \class Application
        * \brief base class dedicated for all ODFAEG applications.
        * \author Duroisin.L
        * \version 1.0
        * \date 1/02/2014
        */
        class ODFAEG_CORE_API Application {
        public :
            /** \fn Application(sf::VideoMode, std::string title, int nbComponents, bool depthDepth, sf::Uint32 style, sf::ContetSettings settings)
            *   \brief create a graphic odfaeg application.
            *   \param sf::VideoMode : the video mode. (the size of the window)
            *   \param std::string : the title of the window.
            *   \param sf::Uint32 : the window's style.
            *   \param ContextSettings : advanced opengl settings.
            */
            Application(sf::VideoMode vm, std::string title, sf::Uint32 style = sf::Style::Default, sf::ContextSettings settings = sf::ContextSettings())

            {
                clearColor = sf::Color::Black;
                graphic::RenderWindow* window = new graphic::RenderWindow (vm, title, style, settings);
                windows.push_back(window);
                componentManager = std::make_unique<graphic::RenderComponentManager>(*window);
                app = this;
                running = false;
                sf::Clock loopSpeed;
                addClock(loopSpeed, "LoopTime");
                listener = std::make_unique<Listener>();
                eventContextActivated = true;
            }
            /** \fn Application()
            *   \brief create a console odfaeg application.
            */
            Application () {
                app = this;
                running = false;
                sf::Clock loopTime;
                addClock(loopTime, "LoopTime");
                sf::Clock timeClock;
                addClock(timeClock, "TimeClock");
                eventContextActivated = true;
            }
            void addWindow(graphic::RenderWindow* window) {
                if (windows.size() != 0) {
                    windows.push_back(window);
                }
            }
            /** \fn int exec()
            *   \brief main loop of the odfaeg application.
            *   \return true if the application runs without errors, false otherwise.
            */
            int exec() {
                load();
                init();
                running = true;
                //window->setActive(false);
                //rendering_thread = std::thread(&Application::render, this);
                while (running) {
                    //std::lock_guard<std::recursive_mutex> lock(rec_mutex);
                    if (windows.size() != 0 && windows[0]->isOpen()) {
                        render();
                        update();
                    }
                    if (network::Network::getCliInstance().isRunning()) {
                        network::Network::getCliInstance().checkMessages();
                    }
                    if (network::Network::getSrvInstance().isRunning()) {

                        network::Network::getSrvInstance().checkMessages();
                    }
                    onExec();
                    getClock("LoopTime").restart();
                }
                return EXIT_SUCCESS;
            }
            /** \fn void stop()
            *   \brief stop the odfaeg application and close the window if it's a graphic application.
            */
            void stop() {
                running = false;
                //rendering_thread.join();
                for(unsigned int i = 0; i < windows.size(); i++)
                    windows[i]->close();
            }
            /** \fn void load()
            *   \brief call the onLoad function, this is where all resources used by the application are loaded.
            */
            void load() {
                onLoad();
            }
            /** \fn void init()
            *   \brief call the onInit function, this is where all the entities used by the application are initialized.
            */
            void init() {
                onInit();
            }
            /** \fn void render()
            *   \brief call the rendering functions used to render entities on components or on the window.
            */
            void render() {
                if (windows.size() != 0 && windows[0]->isOpen()) {
                    for (unsigned int i = 0; i < windows.size(); i++)
                        windows[i]->clear(clearColor);
                    onRender(componentManager.get());
                    componentManager->clearComponents();
                    if (eventContextActivated) {
                       listener->processEvents();
                    }
                    componentManager->updateComponents();
                    componentManager->drawRenderComponents();
                    onDisplay(windows[0]);
                    componentManager->drawGuiComponents();
                    for (unsigned int i = 0; i < windows.size(); i++)
                        windows[i]->display();
                }
            }
            void setEventContextActivated(bool eventContextActivated) {
                this->eventContextActivated = eventContextActivated;
            }
            bool isEventContextActivated() {
                return eventContextActivated;
            }
            /** \fn void update()
            *   \brief call the updates functions to update the entities
            *   filter the sf::Events and pass window events which are generated to the listener.
            */
            void update() {
                if (windows.size() != 0) {
                    sf::Event event;
                    events.clear();
                    for (unsigned int i = 0; i < windows.size(); i++) {
                        if (windows[i]->hasFocus()) {
                            while (windows[i]->pollEvent(event)) {
                                events.insert(std::make_pair(windows[i], event));
                            }
                        }
                    }
                    if (events.size() > 0) {
                        for (it = events.begin(); it != events.end(); it++) {
                            onUpdate(it->first, it->second);
                            if (eventContextActivated) {
                                listener->pushEvent(event);
                            }
                            for (unsigned int i = 0; i < componentManager->getNbComponents(); i++) {
                                componentManager->getComponent(i)->onUpdate(it->first, it->second);
                                if (componentManager->getComponent(i)->isEventContextActivated()) {
                                    componentManager->getComponent(i)->pushEvent(event);
                                }
                            }
                        }
                    }
                }
                graphic::World::updateTimers();
            }
            /**
            * \fn void onLoad()
            * \brief function which can be redefined if the application have to load resources at the start.
            */
            virtual void onLoad (){}
            /**
            * \fn void onLoad()
            * \brief function which can be redefined if the application have to init entities at the start.
            */
            virtual void onInit() {}
            /**
            * \fn void onLoad()
            * \brief function which can be redefined if the application have to render entities on components.
            * \param RenderComponentManager : the manager of all render components.
            */
            virtual void onRender (graphic::RenderComponentManager* cm){}
            /**
            * \fn void onLoad()
            * \brief function which can be redefined if the application have to render entities on the window.
            */
            virtual void onDisplay(graphic::RenderWindow *rw){}
            /**
            * \fn void onUpdate()
            * \brief function which can be redefined if the application have to update entities when window's events are generated.
            * \param the generated event.
            */
            virtual void onUpdate (graphic::RenderWindow* window, sf::Event& event) {}
            /**
            * \fn void onExec()
            * \brief function which can be redefined if the application need to do something at each loop.
            * by example if the application need to do something when a networking message is arrived.
            */
            virtual void onExec() {}
            /** \fn void addClock(sf::Clock clock, std::string name)
            *   \brief add a clock to the application, the clock is so accessible everywhere in the source code.
            *   \param Clock : the clock to add.
            *   \param name : a name which identify the clock.
            */
            void addClock(sf::Clock clock, std::string name)  {

                clocks.insert(std::pair<std::string, sf::Clock>(name, clock));
            }
            /** \fn sf::Clock& getClock(std::string name)
            *   \brief return a clock from its name.
            *   \param std::string : the name of the clock.
            *   \return the clock corresponding to the given name.
            */
            sf::Clock& getClock(std::string name) {
                std::map<std::string, sf::Clock>::iterator it = clocks.find(name);
                if (it == clocks.end())
                    throw Erreur (14, "Clock not found!", 0);
                return it->second;
            }
            /** \fn RenderWindow& getRenderWindow()
            *   \brief return a reference to the windows of the appliation
            *   \return the render window of the application.
            */
            graphic::RenderWindow& getRenderWindow() {
                return *windows[0];
            }
            /** \fn RenderComponentManager& getRenderComponentManager()
            *   \brief return a reference to the render component manager.
            *   \return the render component manager of the application.
            */
            graphic::RenderComponentManager& getRenderComponentManager() {
                return *componentManager;
            }
            /** \fn View& getView()
            *   \brief return a reference to the current view of the window.
            *   \return a reference to the view.
            */
            graphic::View& getView() {
                return windows[0]->getView();
            }
            /** \fn View& getDefaultView()
            *   \brief return the default view of the window.
            *   \return the default view.
            */
            graphic::View getDefaultView() {
                return windows[0]->getDefaultView();
            }
            /** \fn setClearColor (sf::Color clearColor)
            *   \brief define the clear color of the window.
            *   \param sf::Color : the clear color.
            */
            void setClearColor(sf::Color clearColor) {
                this->clearColor = clearColor;
            }
            Listener& getListener() {
                return *listener;
            }
            /** \fn ~Application()
            *   \brief destructor : stop the application and destroy the window if any.
            */
            virtual void onDisconnected(network::User* user) {
            }
            ~Application() {
                stop();
                for (unsigned int i = 0; i < windows.size(); i++)
                   delete windows[i];
            }
            static sf::Clock& getTimeClk() {
                return timeClk;
            }
            /** > a pointer to the current odfaeg application*/
            static Application* app;
        private :
            std::vector<graphic::RenderWindow*> windows; /** > the render window*/
            std::unique_ptr<graphic::RenderComponentManager> componentManager; /** > the render component manager which draw components on the window*/
            std::map<std::string, sf::Clock> clocks; /** > all the clocks used by the application to measure the time.*/
            bool running; /** > determine if the application running or not.*/
            sf::Color clearColor; /** > keep the clear color of the window*/
            std::multimap<graphic::RenderWindow*, sf::Event> events; /** > store the windows events generated by the application*/
            std::multimap<graphic::RenderWindow*, sf::Event>::iterator it; /** > an iterator to the sf::events generated by the application*/
            std::unique_ptr<Listener> listener;
            bool eventContextActivated;
            static sf::Clock timeClk;
        };
    }
}
#endif // APPLICATION
