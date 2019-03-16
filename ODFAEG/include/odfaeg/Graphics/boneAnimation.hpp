#ifndef ODFAEG_BONE_ANIMATION_HPP
#define ODFAEG_BONE_ANIMATION_HPP
#include "animatedEntity.h"
namespace odfaeg {
    namespace graphic {
        class BoneAnimation : public AnimatedEntity {
            public :
            BoneAnimation(math::Vec3f position, math::Vec3f size, std::string type);
            void setBoneIndex(unsigned int index);
            unsigned int getBoneIndex();
            private :
                unsigned int boneIndex;
        };
    }
}
#endif // BONE_ANIMATION_HPP
