#ifndef ODFAEG_STREAM_HPP
#define ODFAEG_STREAM_HPP
#include <SFML/Audio.hpp>
#include "../../../src/odfaeg/Sound/SoundFile.hpp"
#include <vector>
#include <string>
namespace odfaeg {
    namespace sound {
        class Stream : public sf::SoundStream
        {
        public:

            void load(const sf::SoundBuffer& buffer);
            bool openFromFile(const std::string& file);
            bool openFromMemory(const void* data, std::size_t size);
            bool openFromStream(sf::InputStream& stream);
        private:

            virtual bool onGetData(Chunk& data);

            virtual void onSeek(sf::Time timeOffset);
            void initialize();
            priv::SoundFile* m_file;
            sf::Time m_duration;
            std::vector<sf::Int16> m_samples;
            std::size_t m_currentSample;
            sf::Mutex m_mutex;
        };
    }
}
#endif
