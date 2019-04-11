#ifndef ODFAEG_INPUT_SYSTEM_HPP
#define ODFAEG_INPUT_SYSTEM_HPP
#include "listener.h"
#include "export.hpp"
/**
  *\namespace odfaeg
  * the namespace of the Opensource Development Framework Adapted for Every Games.
  */
namespace odfaeg {
/**
* \file inputSystem.h
* \class InputSystem
* \brief class used to holds the unique listener in a multithread context. (Monostate pattern)
* \author Duroisin.L
* \version 1.0
* \date 1/02/2014
*/
class ODFAEG_CORE_API InputSystem {
    public :
    /** \fn Listener& getListener()
    *   \brief get the listener.
    *   \return Listener& return a reference to a unique listener.
    */
    static Listener& getListener();
    private :
    static Listener& listener; /**> holds a reference to the unique listener.*/
};
}
#endif // INPUT_SYSTEM
