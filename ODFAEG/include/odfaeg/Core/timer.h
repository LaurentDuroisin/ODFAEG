#ifndef TIMER
#define TIMER
#include "singleton.h"
/**
 *\namespace odfaeg
 * the namespace of the Opensource Development Framework Adapted for Every Games.
 */
namespace odfaeg {
    namespace core {
        /**
        * \file timer.h
        * \class Timer
        * \brief Each timer inherits from this class : a timer updates the scene with the given time's interval.
        * this class use a thread.
        * \author Duroisin.L
        * \version 1.0
        * \date 1/02/2014
        */
        class ODFAEG_CORE_API Timer {
        public :
            /**\fn Timer()
            *  \brief default constructor.
            */
            Timer(bool useThread) : useThread(useThread) {
                if (useThread) {
                   running = true;
                   m_thread = std::thread (&Timer::run, this);
                }
            }
            /**
            *  \fn setInterval(sf::Time interval)
            *  \brief set an interval of time.
            *  \param sf::Time interval : the time interval between two updates.
            */
            void setInterval(sf::Time interval) {
                this->interval = interval;
            }
            /** \fn void stop()
            *   \brief stop the thread.
            */
            void stop() {
                running = false;
               /* if (useThread) {
                    m_thread.join();
                }*/
            }
            bool isUsingThread() {
                return useThread;
            }
            /** \fn void run()
            *   \brief lock the mutex and updates the scene at each time interval.
            */
            void update() {
                sf::Time elapsedTime = clock.getElapsedTime();
                if (elapsedTime >= interval) {
                    onUpdate();
                    clock.restart();
                }
            }
            void run () {
                while (running) {
                    //std::cout<<"timer running"<<std::endl;
                    std::chrono::microseconds dura(100);
                    std::this_thread::sleep_for(dura);
                    std::lock_guard<std::recursive_mutex> lock(rec_mutex);
                    onUpdate();
                }
            }
            /** \fn virtual void onUpdate() = 0;
            *   \brief the function to redefine when updating the scene.
            */
            virtual void onUpdate() = 0;
            bool isRunning () {
                return running;
            }
        private :
            sf::Clock clock; /**> A clock use to measure the time elapsed since the last update*/
            sf::Time interval; /**> The time interval between two updates.*/
            std::thread m_thread; /**>The thread*/
            bool running, useThread; /**> indicates if the thread is running (or not)*/
        };
    }
}
#endif // TIMER
