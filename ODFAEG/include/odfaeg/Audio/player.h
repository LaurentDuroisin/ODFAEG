#ifndef ODFAEG_PLAYER_HPP
#define ODFAEG_PLAYER_HPP
#include "stream.h"
#include <memory>
/**
  *\namespace odfaeg
  * the namespace of the Opensource Development Framework Adapted for Every Games.
  */
namespace odfaeg {
    namespace audio {
        class Player {
        public :
            Player();
            Player (sf::SoundBuffer& soundBuffer);
            void setAudioStream(Stream* stream);
            Stream* getAudioStream();
            void play (bool loop = false);
            void stop ();
            void pause ();
            bool isPlaying();
            void setPitch(float pitch);
            void setVolume(float volume);
            void setPosition(float x, float y, float z);
            void setPosition(sf::Vector3f position);
            void setRelativeToListener(bool relative);
            void setMinDistance (float minDistance);
            void setAttenuation(float attenuation);
            float getPitch() const;
            float getVolume() const;
            sf::Vector3f getPosition() const;
            bool isRelativeToListener() const;
            float getMinDistance() const;
            float getAttenuation() const;
            bool stereo();
            ~Player();
        private:
            unsigned int m_buffer;
            Stream* stream;
            bool inPause;
            bool playing;
        };

    }
}
#endif // ODFAEG_PLAYER_HPP
