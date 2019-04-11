#ifndef ODFAEG_PLAYER_HPP
#define ODFAEG_PLAYER_HPP
#include "stream.h"
#include <memory>
namespace odfaeg {
    namespace sound {
        class Player {
        public :
            Player();
            Player (sf::SoundBuffer& soundBuffer);
            void setAudioStream(sf::SoundStream* stream);
            sf::SoundStream* getAudioStream();
            void play ();
            void stop ();
            void pause ();
            ~Player();
        private:
            sf::SoundStream* stream;
        };
    }
}
#endif // ODFAEG_PLAYER_HPP
