#include "../../../include/odfaeg/Window/windowFactory.hpp"
#if defined(SFML)
#include "../../../include/odfaeg/Window/sfmlWindowImpl.hpp"
typedef odfaeg::window::sfmlWindowImpl WindowType;
#elif defined(SDL)
#include "../../../include/odfaeg/Window/sdlWindowImpl.hpp"
typedef odfaeg::window::sdlWindowImpl WindowType;
#elif defined(GLFW)
#include "../../../include/odfaeg/Window/glfwWindowImpl.hpp"
typedef odfaeg::window::glfwWindowImpl WindowType;
#else
#include "../../../include/odfaeg/Window/windowImpl.hpp"
typedef odfaeg::window::WindowImpl WindowType;
#endif
namespace odfaeg {
    namespace window {
        IWindow* WindowFactory::create() {
            return new WindowType();
        }
    }
}
