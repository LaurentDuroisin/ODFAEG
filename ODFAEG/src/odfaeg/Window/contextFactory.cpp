#include "../../../include/odfaeg/Window/contextFactory.hpp"
#if defined(SFML)
#include "../../../include/odfaeg/Window/sfmlContextImpl.hpp"
typedef odfaeg::window::sfmContextImpl ContextType;
#elif defined(SDL)
#include "../../../include/odfaeg/Window/sdlContextImpl.hpp"
typedef odfaeg::window::sdlContextImpl ContextType;
#elif defined(GLFW)
#include "../../../include/odfaeg/Window/glfwContextImpl.hpp"
typedef odfaeg::window::glfwContextImpl ContextType;
#else
#include "../../../include/odfaeg/Window/contextImpl.hpp"
typedef odfaeg::window::ContextImpl ContextType;
#endif
namespace odfaeg {
    namespace window {
        IContext* ContextFactory::create() {
            return new ContextType();
        }
    }
}
