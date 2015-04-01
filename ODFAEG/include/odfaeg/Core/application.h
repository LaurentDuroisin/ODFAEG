#ifndef ODFAEG_APPLICATION_HPP
#define ODFAEG_APPLICATION_HPP
#include "resourceCache.h"
#include "../Graphics/renderWindow.h"
#include "../Graphics/renderStates.h"
#include "../Graphics/fastRenderComponentManager.h"
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
            *   \param int : the number of render components used by the application.
            *   \param bool : activate the default opengl depthtest.
            *   \param sf::Uint32 : the window's style.
            *   \param ContextSettings : advanced opengl settings.
            */
            Application(sf::VideoMode vm, std::string title, bool useThread = false, bool depthTest = true, sf::Uint32 style = sf::Style::Default, sf::ContextSettings settings = sf::ContextSettings())

            {
                clearColor = sf::Color::Black;
                window = std::make_unique<graphic::RenderWindow>(vm, title, style, settings, depthTest);
                componentManager = std::make_unique<graphic::FastRenderComponentManager>(*window);
                app = this;
                running = false;
                sf::Clock loopSpeed;
                addClock(loopSpeed, "LoopTime");
                listener = std::make_unique<Listener>(useThread);
                eventContextActivated = true;
            }
            /** \fn Application()
            *   \brief create a console odfaeg application.
            */
            Application () {
                window = nullptr;
                app = this;
                running = false;
                sf::Clock loopTime;
                addClock(loopTime, "LoopTime");
                sf::Clock timeClock;
                addClock(timeClock, "TimeClock");
                eventContextActivated = true;
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
                    if (window != nullptr && window->isOpen()) {
                        render();
                        update();
                    }
                    if (network::Network::getCliInstance().isRunning() && !network::Network::getCliInstance().isUsingThread()) {
                        network::Network::getCliInstance().checkMessages();
                    }
                    if (network::Network::getSrvInstance().isRunning() && !network::Network::getSrvInstance().isUsingThread()) {

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
                if (window != nullptr)
                    window->close();
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
                if (window != nullptr && window->isOpen()) {
                    window->clear(clearColor);
                    onRender(componentManager.get());
                    componentManager->clearComponents();
                    componentManager->updateComponents();
                    componentManager->drawRenderComponents();
                    onDisplay(window.get());
                    componentManager->drawGuiComponents();
                    window->display();
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
                if (window != nullptr) {
                    sf::Event event;
                    events.clear();
                    while (window->pollEvent(event)) {
                        events.insert(events.begin(),event);
                    }
                    if (events.size() > 0) {
                        while (events.size() != 0) {
                            event = events.back();
                            onUpdate(event);
                            if (eventContextActivated)
                                listener->pushEvent(event);
                            for (unsigned int i = 0; i < componentManager->getNbComponents(); i++) {
                                componentManager->getComponent(i)->onUpdate(event);
                                if (componentManager->getComponent(i)->isEventContextActivated()) {
                                    componentManager->getComponent(i)->pushEvent(event);
                                }
                            }
                            events.pop_back();
                        }
                    } else {
                        if (eventContextActivated) {
                            listener->processEvents();
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
            * \param FastRenderComponentManager : the manager of all render components.
            */
            virtual void onRender (graphic::FastRenderComponentManager* cm){}
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
            virtual void onUpdate (sf::Event& event) {}
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
                return *window;
            }
            /** \fn FastRenderComponentManager& getRenderComponentManager()
            *   \brief return a reference to the render component manager.
            *   \return the render component manager of the application.
            */
            graphic::FastRenderComponentManager& getRenderComponentManager() {
                return *componentManager;
            }
            /** \fn View& getView()
            *   \brief return a reference to the current view of the window.
            *   \return a reference to the view.
            */
            graphic::View& getView() {
                return window->getView();
            }
            /** \fn View& getDefaultView()
            *   \brief return the default view of the window.
            *   \return the default view.
            */
            graphic::View getDefaultView() {
                return window->getDefaultView();
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
            }
            static sf::Clock& getTimeClk() {
                return timeClk;
            }
            /** > a pointer to the current odfaeg application*/
            static Application* app;
        private :
            std::unique_ptr<graphic::RenderWindow> window; /** > the render window*/
            std::unique_ptr<graphic::FastRenderComponentManager> componentManager; /** > the render component manager which draw components on the window*/
            std::map<std::string, sf::Clock> clocks; /** > all the clocks used by the application to measure the time.*/
            bool running; /** > determine if the application running or not.*/
            sf::Color clearColor; /** > keep the clear color of the window*/
            std::vector<sf::Event> events; /** > store the windows events generated by the application*/
            std::vector<sf::Event>::iterator it; /** > an iterator to the sf::events generated by the application*/
            std::unique_ptr<Listener> listener;
            std::thread rendering_thread;
            bool eventContextActivated;
            static sf::Clock timeClk;
        };
    }
}
#endif // APPLICATION
