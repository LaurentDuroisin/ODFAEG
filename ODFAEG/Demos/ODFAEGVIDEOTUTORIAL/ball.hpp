#ifndef PONG_BALL_HPP
#define PONG_BALL_HPP
#include "odfaeg/Physics/boundingSphere.h"
#include "odfaeg/Graphics/entity.h"
class Ball : public odfaeg::graphic::Entity {
public :
    Ball();
    void updateSpeed(float speed);
    void updateDir (odfaeg::math::Vec2f dir);
    odfaeg::math::Vec2f dir ();
    float speed();
    struct Historic {
        sf::Int64 time;
        odfaeg::math::Vec2f pos;
        odfaeg::math::Vec2f dir;
    };
    void onMove(odfaeg::math::Vec3f& t);
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
private :
    Historic historic;
    odfaeg::math::Vec2f m_dir;
    float m_speed;
};
#endif // PONG_BALL_HPP
