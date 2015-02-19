#ifndef ODFAEG_OBJECT_HPP
#define ODFAEG_OBJECT_HPP
#include "erreur.h"
#include "command.h"
#include <thread>
#include <map>
#include <mutex>
#include <condition_variable>
#include "export.hpp"
#include "singleton.h"
#include <chrono>
/**
 *\namespace odfaeg
 * the namespace of the Opensource Development Framework Adapted for Every Games.
 */
namespace odfaeg {
    namespace core {
        class ODFAEG_CORE_API Listener {
        /**
        * \file listener.h
        * \class Listener
        * \brief this class connect one or more command to an id, get the sf::Event and check if the command is triggered.
        * this class use a thread to execute the commands, and wait until we call a methode to check if the commands are trigered.
        * \author Duroisin.L
        * \version 1.0
        * \date 1/02/2014
        */
        public :
            /**
            * \fn Listener()
            * \brief constructor.
            */
             Listener(bool useThread) {
                running = true, g_notified = true, one_triggered = false,
                this->useThread = useThread;
                if (useThread)
                   t = std::thread(&Listener::run, this);
             }
             bool isUsingThread() {
                 return useThread;
             }
             /**
             * \fn void connect (std::string key, Command command)
             * \brief connect an identifier to a command.
             * \param std:string key : the key to add.
             * \param Command command : the command to add.
             */
             void connect(std::string key, Command command) {
                 std::map<std::string, Command>::iterator it = commands.find(key);
                 if (it != commands.end()) {
                    throw Erreur (20, "This connection already exists!", 0);
                 }
                 commands.insert(std::pair<std::string, Command>(key, command));
             }
             /**
             * \fn void setCommandParams (std::string key,
             * \brief connect an identifier to a command.
             * \param std:string key : the key to add.
             * \param Command command : the command to add.
             */
             template <typename...A>
             void setCommandSigParams (std::string key, A&&... args) {

                 std::map<std::string, Command>::iterator it = commands.find(key);
                 if (it == commands.end())
                    throw Erreur(2, "No such connexion!", 6);
                 it->second.setSigParams(std::forward<A>(args)...);
             }
             template <typename...A>
             void setCommandSlotParams (std::string key, A&&... args) {

                 std::map<std::string, Command>::iterator it = commands.find(key);
                 if (it == commands.end())
                    throw Erreur(2, "No such connexion!", 6);
                 it->second.setSlotParams(std::forward<A>(args)...);
             }
             template <typename...A>
             void bindCommandSigParams (std::string key, A&&... args) {
                 std::map<std::string, Command>::iterator it = commands.find(key);
                 if (it == commands.end())
                    throw Erreur(2, "No such connexion!", 6);
                 it->second.bindSigParams(std::forward<A>(args)...);
             }
             template <typename...A>
             void bindCommandSlotParams (std::string key, A&&... args) {
                 std::map<std::string, Command>::iterator it = commands.find(key);
                 if (it == commands.end())
                    throw Erreur(2, "No such connexion!", 6);
                 it->second.bindSlotParams(std::forward<A>(args)...);
             }
             /** bool isOneTriggered()
             *   \fn bool isOneTriggered()
             *   \brief return true if a least one command is triggered
             *   \return if a command is triggered.
             */
             bool isOneTriggered() {
                 std::map<std::string, Command>::iterator it;
                 for (it = commands.begin(); it != commands.end(); it++) {
                    if (it->second.isTriggered()) {
                       return true;
                    }
                 }
                 return false;
             }
             /**\fn bool void run()
             *  \brief block the current thread, check if the command are triggered and execute them.
             */
             void run () {
                 std::map<std::string, Command>::iterator it;
                 while (running) {
                     //std::cout<<"listener running"<<std::endl;

                     std::unique_lock<std::mutex> locker(g_lock_listen);
                     std::chrono::milliseconds dura(100);
                     g_signal.wait_for(locker, dura, [&](){return one_triggered || !running;});
                     for (it = commands.begin(); it != commands.end(); it++) {
                        if (it->second.isTriggered())
                            (it->second)();
                     }
                     Command::clearEventsStack();
                     g_notified = true;
                     one_triggered = false;
                     g_signal.notify_all();
                 }
             }
             /** \fn void pushEvent (sf::Event event)
             *   \brief push an event into the event stack.
             *   \param sf::Event : the sf::event to pass into the stack.
             */
             void pushEvent(sf::Event event) {
                 std::map<std::string, Command>::iterator it;
                 for (it = commands.begin(); it != commands.end(); it++) {
                    if(it->second.containsBufferEvent(event)) {
                        Command::pushEvent(event);
                    }
                    Action* action = it->second.getAction();
                    if(action != nullptr) {
                        action->setPressed(event);
                    }

                 }
                 processEvents();
             }
             /** \fn void processEvents ()
             *   \brief check if the commands are triggered and execute them.
             *   the current thread is blocked until this method is finished.
             */
             void processEvents() {
                if (useThread) {
                    if (!one_triggered) {
                        g_notified = false;
                        one_triggered = true;
                        g_signal.notify_all();
                        std::unique_lock<std::mutex> locker(g_lock_listen);
                        g_signal.wait(locker, [&](){return g_notified;});
                    }
                } else {
                     std::map<std::string, Command>::iterator it;
                     for (it = commands.begin(); it != commands.end(); it++) {
                        if (it->second.isTriggered()) {
                            (it->second)();
                        }
                     }
                     Command::clearEventsStack();
                }
             }
             void stopListen()  {
                 running = false;
                 if (useThread) {
                     g_signal.notify_all();
                     t.join();
                 }
             }
             /** \fn ~Listener()
             * \brief destrucor : stop the thread.
             */
             ~Listener () {
                 stopListen();
             }
             private :
             /** \fn void stopListen()
             *   \brief stop the thread which triggers and execute commands.
             */

             std::map<std::string, Command> commands; /**> stores and execute commands.*/
             std::thread t; /**> a thread with trigger and executes the commands.*/
             bool running, g_notified, one_triggered, useThread; /**> tells if the thread is running, if we have to process the commands and if a command is triggered.*/
             std::mutex g_lock_listen; /**> mutex used to avoid to have concurrent access to variables with multi threads*/
             std::condition_variable g_signal; /** primitive synchronisation used to bloc the current thread while the commands are executed.*/
        };
    }
}
#endif
