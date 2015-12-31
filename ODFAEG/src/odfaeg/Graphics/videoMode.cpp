#include "../../../include/odfaeg/Graphics/videoMode.h"
#include <algorithm>
#include <iostream>
namespace odfaeg {
    VideoMode::VideoMode(unsigned int width, unsigned int height, unsigned int bitsPerPixel) :
        width(width), height (height), bitsPerPixel(bitsPerPixel) {

    }
    std::vector<VideoMode> VideoMode::getFullscreenModes() {
        std::vector<VideoMode> vModes;
        SDL_DisplayMode* current;
        for (unsigned int i = 0; SDL_GetNumVideoDisplays(); ++i) {
            int error = SDL_GetNumDisplayModes(i);
            if (error == 0) {
                for (unsigned int j = 0; j < SDL_GetNumDisplayModes(i); j++) {
                    int should_be_zero = SDL_GetDisplayMode(i, j, current);
                    if (should_be_zero == 0) {
                        VideoMode mode (static_cast<unsigned int>(current->w), static_cast<unsigned int>(current->h), static_cast<unsigned int>(current->format));
                        if (std::find(vModes.begin(), vModes.end(), mode) == vModes.end())
                            vModes.push_back(mode);
                    }
                }
            }
        }
        return vModes;
    }
    VideoMode VideoMode::getDesktopMode() {
        SDL_DisplayMode* current;
        for (unsigned int i = 0; i < SDL_GetNumVideoDisplays(); i++) {
            int error = SDL_GetDesktopDisplayMode(i, current);

            if (error == 0) {
                return VideoMode (static_cast<unsigned int>(current->w), static_cast<unsigned int>(current->h), static_cast<unsigned int>(current->format));
            }
        }
    }
    unsigned int VideoMode::getWidth() {
        return width;
    }
    unsigned int VideoMode::getHeight() {
        return height;
    }
    unsigned int VideoMode::getBitsPerPixel() {
        return bitsPerPixel;
    }
    ////////////////////////////////////////////////////////////
    bool VideoMode::isValid() const
    {
        const std::vector<VideoMode>& modes = getFullscreenModes();

        return std::find(modes.begin(), modes.end(), *this) != modes.end();
    }
    ////////////////////////////////////////////////////////////
    bool operator ==(const VideoMode& left, const VideoMode& right)
    {
        return (left.width        == right.width)        &&
               (left.height       == right.height)       &&
               (left.bitsPerPixel == right.bitsPerPixel);
    }


    ////////////////////////////////////////////////////////////
    bool operator !=(const VideoMode& left, const VideoMode& right)
    {
        return !(left == right);
    }


    ////////////////////////////////////////////////////////////
    bool operator <(const VideoMode& left, const VideoMode& right)
    {
        if (left.bitsPerPixel == right.bitsPerPixel)
        {
            if (left.width == right.width)
            {
                return left.height < right.height;
            }
            else
            {
                return left.width < right.width;
            }
        }
        else
        {
            return left.bitsPerPixel < right.bitsPerPixel;
        }
    }


    ////////////////////////////////////////////////////////////
    bool operator >(const VideoMode& left, const VideoMode& right)
    {
        return right < left;
    }


    ////////////////////////////////////////////////////////////
    bool operator <=(const VideoMode& left, const VideoMode& right)
    {
        return !(right < left);
    }


    ////////////////////////////////////////////////////////////
    bool operator >=(const VideoMode& left, const VideoMode& right)
    {
    return !(left < right);
    }

}
