#ifndef PONG_PLAYER_HPP
#define PONG_PLAYER_HPP
#include "odfaeg/Network/user.h"
#include "odfaeg/Physics/boundingBox.h"
#include "odfaeg/Graphics/entity.h"
class Player : public odfaeg::graphic::Entity {
public :
    Player(odfaeg::network::User& user, std::string pseudo, odfaeg::math::Vec2f position);
    sf::IpAddress ipAddress();
    std::string pseudo();
    void setLastSrvTime(sf::Int64 time);
    sf::Int64 lastSrvTime();
    void setSyncTime(sf::Int64 time);
    sf::Int64 syncTime();
    sf::Clock& timeSinceLastSync();
    bool isLeaf() const {
        return true;
    }
    bool operator== (odfaeg::graphic::Entity& entity) {
        return false;
    }
    bool isAnimated() const {
        return false;
    }
    bool isModel() const {
        return false;
    }
    bool selectable() const {
        return false;
    }
    bool isLight() const {
        return false;
    }
    bool isShadow() const {
        return false;
    }
    void onMove(odfaeg::math::Vec3f& t);
    bool isConnected();
    void setMoving(bool moving);
    bool isMoving();
    void setDir(odfaeg::math::Vec2f dir);
    odfaeg::math::Vec2f dir();
private :
    odfaeg::math::Vec2f m_dir;
    bool moving;
    sf::Int64 m_lastSrvTime;
    sf::Int64 m_syncTime;
    std::string m_pseudo;
    odfaeg::network::User& m_user;
    sf::Clock m_timeSinceLastSync;
};
#endif // PLAYER_HPP
