#ifndef ODFAEG_ENTITY_SYSTEM_HPP
#define ODFAEG_ENTITY_SYSTEM_HPP
#include <condition_variable>
#include <mutex>
#include <thread>
#include "export.hpp"
#include <chrono>
/**
*\namespace odfaeg
* the namespace of the Opensource Development Framework Adapted for Every Games.
*/
namespace odfaeg {
    namespace core {
        /**
        * \file entitySystem.h
        * \class EntitiesSystem
        * \brief base class of all entities systems of the odfaeg.
        * \author Duroisin.L
        * \version 1.0
        * \date 1/02/2014
        */
        class ODFAEG_CORE_API EntitySystem {
        public :
            /**
            *\fn EntitySystem ()
            *\brief constructor.
            */
            EntitySystem (bool useThread) : needToUpdate(false), useThread(useThread) {
                running = false;
                g_notified = true;
                running = true;
                if (useThread)
                    m_thread = std::thread (&EntitySystem::run, this);
            }
            /**
            *\fn void update()
            *\brief function called when we need to update the entities, this function block the current thread until it's finished.
            */
            void update () {
                if (useThread) {
                    if (!needToUpdate) {
                        g_notified = false;
                        needToUpdate = true;
                        g_signal.notify_all();
                        std::unique_lock<std::mutex> locker (g_lock_update);
                        g_signal.wait(locker, [&](){return g_notified;});
                    }
                } else {
                    onUpdate();
                }
            }
            /**
            *\fn void run()
            *\brief this function block this thread until we need to update the entities by calling the update function.
            * or if we want to stop the thread.
            */
            void run () {

                while (running) {
                    //std::cout<<"entity system running"<<std::endl;
                    std::unique_lock<std::mutex> locker(g_lock_update);
                    std::chrono::milliseconds dura(500);
                    g_signal.wait_for(locker, dura, [&](){return needToUpdate || !running;});
                    if (needToUpdate)
                        onUpdate();
                    g_notified = true;
                    needToUpdate = false;
                    g_signal.notify_all();
                }
            }
            /**
            *\fn bool isRunning()
            *\brief tells is the thread is running.
            *\return true if the thread is running.
            */
            bool isRunning () {
                return running;
            }
            /**
            *\fn void stop()
            *\brief stop this thread.
            */
            void stop () {
                running = false;
                if (useThread) {
                    g_signal.notify_all();
                    /*std::unique_lock<std::mutex> locker(g_lock_update);
                    std::notify_all_at_thread_exit(g_signal, std::move(g_lock_update));*/
                }
            }
            /**
            *\fn void onUpdate()
            *\brief function to refefines to updates the entities.
            */
            virtual void onUpdate() = 0;
        private :
            std::thread m_thread; /**< an internal thread which updates the entities*/
            bool running, needToUpdate, g_notified, useThread; /**< tells if the thread is running, if the entities need to be update by the thread, and, if the thread have finished to update the entities.*/
            std::condition_variable g_signal; /**<condition variable used for the primitive synchronisation of the thread.*/
            std::mutex g_lock_update; /**<mutex used in the condition variable to avoid concurrent access to a variable between different threads*/
        };
    }
}
#endif // ENTITY_SYSTEM
