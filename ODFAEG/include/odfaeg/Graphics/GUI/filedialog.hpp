#ifndef FILEDIALOG_HPP
#define FILEDIALOG_HPP
#include "../lightComponent.h"
#include "../renderWindow.h"
#include "../rectangleShape.h"
#include "../text.h"
#include "../font.h"
#include "panel.hpp"
#include "label.hpp"
#include "button.hpp"
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
namespace odfaeg {
    namespace graphic {
        namespace gui {
            class FileDialog : public LightComponent, public ActionListener {
                public :
                    FileDialog(math::Vec3f position, math::Vec3f size, const Font* font);
                    void onDirSelected(Label* label);
                    void onFileSelected(Label* label);
                    void clear();
                    void onDraw(RenderTarget& target, RenderStates states = RenderStates::Default);
                    void onEventPushed (sf::Event event, RenderWindow& window);
                    void checkSubWindowEvents();
                    std::string getPathChosen();
                    std::string getAppiDir();
                    void actionPerformed(Button* button);
                    void onVisibilityChanged(bool visible);
                private :
                    const Font* font;
                    RenderWindow rw;
                    Panel pTop, pBottom, pDirectories, pFiles;
                    Label lTop;
                    Button bChoose, bCancel;
                    std::string appliDir;
                    std::string pathChosen;
            };
        }
    }
}
#endif
