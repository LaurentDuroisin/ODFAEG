#include "../../../include/odfaeg/Graphics/drawable.h"
namespace odfaeg {
    namespace graphic {
        unsigned int Drawable::nbDrawables = 0;
        Drawable::Drawable() {
            id = nbDrawables;
            nbDrawables++;
        }
        const unsigned int& Drawable::getId() {
            return id;
        }
    }
}
