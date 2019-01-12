#ifndef ODFAEG_ACTION_MAP_HPP
#define ODFAEG_ACTION_MAP_HPP
#include "action.h"
#include "fastDelegate.h"
/**
  *\namespace odfaeg
  * the namespace of the Opensource Development Framework Adapted for Every Games.
  */
namespace odfaeg
{
    namespace core {

        /**
        * \file command.h
        * \class Command
        * \brief This class manage a command : a command must have an action or a trigger function or both to trigger.
        * this class then can check if a command is triggered, and call the slot function which is linked to the command.
        * this class can also have a list of triggered SFML events, and the parameters of the command can be changed.
        * If the parameters are changed, the slot function'll be called with the new command's parameters as arguments.
        * \author Duroisin.L
        * \version 1.0
        * \date 1/02/2014
        * Store an action, a triggered function and a slot function.
        * Link the action, the triggered function to a slot function.
        */

        class ODFAEG_CORE_API Command
        {
        //friend class Action;
        public :
            /** \fn Command(Action action, FastDelegate<void> delegate)
            *  \brief Constructor :Construct a command with an action and a slot function.
            *  \param action : the action on which we want to bind the slot function.
            *  \param FastDelegate<void> the function to invoke throw the delegate when the action of the command is triggered.
            */
            Command (Action action, FastDelegate<void> delegate);
            /**
                \fn Command(FastDelegate<bool> trigger, FastDelegate<void> command)
              \brief Constructor :Construct a command with a trigger and a slot function
            *  \param FastDelegate<bool> : the function to invoke to check if the command is triggered.
            *  \param FastDelegate<void> the function to invoke throw the delegate when the command is triggered.
            */
            Command (FastDelegate<bool> trigger, FastDelegate<void> slot);
            /** \fn Command(Action action, FastDelegate<bool> trigger, FastDelegate<void> command)
            *  \brief Constructor : Construct a command with an action, a trigger function and a slot function.
            *  \param action : the action on which we want to bind the function.
            *  \param FastDelegate<bool> : the function to invoke to check if the command is triggered.
            *  \param FastDelegate<void> the function to invoke throw the delegate when the command is triggered.
            */
            Command (Action action, FastDelegate<bool> trigger, FastDelegate<void> slot);
            /**\fn containsEvent (sf::Event &event)
            *  \brief check if the sf::Event is in the events stack of the command.
            *  \param the sf::Event to check in.
            *  \return true if the sf::Event is in the events stack, false otherwise.
            */
            Command (const Command& other);
            bool containsEvent (sf::Event &event);
            /** \fn bool isTriggered()
            *   \brief return true if the command is triggered.
            *   \return true if command is triggered, false otherwise.
            */
            bool isTriggered();
            /** \fn bool containsBufferEvent(sf::Event& event)
            *   \brief check if the sf::Event is stored in the event buffer of the action
            *   \param the sf::Event to check with. (If there isn't an action linked to the command, it always return false)
            */
            bool containsBufferEvent(sf::Event& event);
            /** \fn void clearEventsStack ()
            *   \brief clear all the sf::events in the events stack.
            */
            static void clearEventsStack ();
            /** \fn void pushEvent (sf::Event& event)
            *   \brief store all the incoming sf::events which are triggered.
            *   \param the triggered sf::Event.
            */
            static void pushEvent (sf::Event& event);
            /** \fn void getEvents()
            *   \brief return all sfml events which are generated since the last event loop.
            *   \return the sfml event which are generated since the last loop.
            */
            static std::vector<sf::Event> getEvents();

            /** \fn void operator()()
            *   \brief call the slot function linked to command.
            */
            void operator()();
            /**\fn
            * \brief remove an sf::Event from the stack.
            *  \param sf::Event event : the sfml event to remove from the stack.
            */
            static void removeEvent(sf::Event& event);
            template <typename... A>
            void setSigParams(A&&... args);
            template <typename... A>
            void setSlotParams(A&&... args);
            template <typename... A>
            void bindSigParams(A&&... args);
            template <typename... A>
            void bindSlotParams(A&&... args);
            /** \fn void setParams(A... args)
            *   \brief change params of a command.
            *   \param A... args : the arguments of the command's members functions to pass when the command is called.
            */
            Action* getAction();
            Command& operator= (const Command& other);
            static bool equalEvent (sf::Event event, sf::Event other);
            void setName(std::string name);
        private :

            /** \fn bool equalEvent (sf::Event event, sf::Event other)
            *   \brief test if two events are equal.
            *   \param sf::Event event : the event.
            *   \param sf::Event other : the otherevent.
            *   \return return true if the two events are equal.
            */

            /** < the action mapped to the command.*/
            std::unique_ptr<Action> action;
            /** < the slot function mapped to the command.*/
            FastDelegate<void> slot;
            /** < the trigger mapped to the command.*/
            std::unique_ptr<FastDelegate<bool>> trigger;
            /** < the SFML events generated.*/
            static std::vector<sf::Event> events;
            std::string name;
        };
    }
}
#include "command.tpp"
#endif // ACTION_MAP
