////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2013 Laurent Gomila (laurent.gom@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "../../../include/odfaeg/Graphics/GlResource.hpp"
#include "GlContext.hpp"
#include <SFML/System/Mutex.hpp>
#include <SFML/System/Lock.hpp>
#include <iostream>
#include <cstdlib>
#include "GlDebug.hpp"
#include "glCheck.h"
namespace
{
    // OpenGL resources counter and its mutex
    unsigned int count = 0;
    sf::Mutex mutex;
}
using namespace sf;

namespace odfaeg
{
////////////////////////////////////////////////////////////
GlResource::GlResource(){

        // Protect from concurrent access

        // If this is the very first resource, trigger the global context initialization
        Lock lock(mutex);
        if (count == 0)
            priv::GlContext::globalInit();
        /*odfaeg::priv::ensureGlewInit();
        #ifdef SFML_DEBUG
        #ifndef DEBUG_FUNCTIONS_LOADED
        std::cout<<"loading internal sfml debug function"<<std::endl;
        OpenGL::LoadDebugFunctions();
        #define  DEBUG_FUNCTIONS_LOADED
        #endif
        #endif*/

        // Increment the resources counter
        count++;


    // Now make sure that there is an active OpenGL context in the current thread
    priv::GlContext::ensureContext();

    /*#ifdef SFML_DEBUG
    #ifndef DEBUG_FUNCTIONS_INITIALIZED
    std::cout<<"initializing internal opengl debug functions"<<std::endl;
    OpenGL::InitialiseDebugFunctions();
    #define DEBUG_FUNCTIONS_INITIALIZED
    #endif
    #endif*/ // DEBUG_FUNCTIONS_INITIALIZED
}


////////////////////////////////////////////////////////////
GlResource::~GlResource()
{
    // Protect from concurrent access
    Lock lock(mutex);

    // Decrement the resources counter
    count--;

    // If there's no more resource alive, we can trigger the global context cleanup
    if (count == 0)
        priv::GlContext::globalCleanup();
}


////////////////////////////////////////////////////////////
void GlResource::ensureGlContext()
{
    priv::GlContext::ensureContext();
}

} // namespace sf
