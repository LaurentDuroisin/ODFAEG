#ifndef ODFAEG_TABLE_HPP
#define ODFAEG_TABLE_HPP
#include "../lightComponent.h"
#include "../rectangleShape.h"
namespace odfaeg {
    namespace graphic {
        namespace gui {
            class Table : public LightComponent {
                public :
                Table (RenderWindow& window, math::Vec3f position, math::Vec3f size, unsigned int nbRows, unsigned int nbColumns);
                void addElement (LightComponent* element, unsigned int rowIndex, unsigned int columnIndex);
                void onMove (math::Vec3f& t);
                void onDraw(RenderTarget& target, RenderStates states);
                void clear();
                private :
                unsigned int nbRows;
                unsigned int nbColumns;
                RectangleShape rect;
            };
        }
    }
}
#endif
