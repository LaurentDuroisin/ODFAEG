#ifndef TAB_PANE
#define TAB_PANE
#include "../lightComponent.h"
#include "panel.hpp"
#include "label.hpp"
namespace odfaeg {
    namespace graphic {
        namespace gui {
            class TabPane : public LightComponent {
                public :
                    TabPane(RenderWindow& window, math::Vec3f position, math::Vec3f size);
                    void addTab(Panel* panel, std::string tabName, const Font& font);
                    void clear() {
                        for (unsigned int i = 0; i < getChildren().size(); i++)
                            getChildren()[i]->clear();
                    }
                    /*void onDraw(RenderTarget& target, RenderStates states) {
                        std::cout<<getChildren().size()<<std::endl;
                        for (unsigned int i = 0; i < getChildren().size(); i++) {
                            std::cout<<getChildren()[i]->getPosition()<<getChildren()[i]->getSize()<<std::endl;
                        }
                    }*/
                private :
                    void onTabChanged(Panel* panel);
            };
        }
    }
}
#endif
