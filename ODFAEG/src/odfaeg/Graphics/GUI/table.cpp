#include "../../../../include/odfaeg/Graphics/GUI/table.hpp"
namespace odfaeg {
    namespace graphic {
        namespace gui {
            Table::Table (RenderWindow& window, math::Vec3f position, math::Vec3f size, unsigned int nbRows, unsigned int nbColumns) : LightComponent(window,position,size,size*0.5f) {
                this->nbRows = nbRows;
                this->nbColumns = nbColumns;
                rect = RectangleShape (size);
                rect.setPosition(position);
            }
            void Table::addElement (LightComponent* element, unsigned int rowIndex, unsigned int columnIndex) {
                float relSizeX = 1.f / nbColumns;
                float relSizeY = 1.f / nbRows;
                float relPosX = columnIndex * relSizeX;
                float relPosY = rowIndex * relSizeY;
                element->setParent(this);
                element->setRelPosition(relPosX, relPosY);
                element->setRelSize(relSizeX, relSizeY);
                addChild(element);
                setAutoResized(true);
            }
            void Table::onMove(math::Vec3f &t) {
                for (unsigned int i = 0; i < getChildren().size(); i++) {
                    getChildren()[i]->move(t);
                }
            }
            void Table::onDraw(RenderTarget& target, RenderStates states) {
                target.draw(rect);
                for (unsigned int i = 0; i < getChildren().size(); i++) {
                    target.draw(*getChildren()[i], states);
                }
            }
        }
    }
}
