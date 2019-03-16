#include "../../../include/odfaeg/Window/glResource.hpp"
#include "../../../include/odfaeg/Window/contextImpl.hpp"
#include <iostream>
namespace odfaeg {
    namespace window {
        GLResource::GLResource() {
            //ContextImpl::initResource();
        }
        GLResource::~GLResource() {
            //ContextImpl::cleanupResource();
        }
        ////////////////////////////////////////////////////////////
        GLResource::TransientContextLock::TransientContextLock()
        {
            //ContextImpl::acquireTransientContext();
        }


        ////////////////////////////////////////////////////////////
        GLResource::TransientContextLock::~TransientContextLock()
        {
            //ContextImpl::releaseTransientContext();
        }
    }
}

