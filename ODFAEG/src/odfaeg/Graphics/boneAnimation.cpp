#include "../../../include/odfaeg/Graphics/boneAnimation.hpp"
namespace odfaeg {
    namespace graphic {
        BoneAnimation::BoneAnimation(math::Vec3f position, math::Vec3f  size, std::string type) : AnimatedEntity(position, size, size * 0.5f, type) {
            boneIndex = 0;
        }
        void BoneAnimation::setBoneIndex(unsigned int index) {
            boneIndex = index;
        }
        unsigned int BoneAnimation::getBoneIndex() {
            return boneIndex;
        }
    }
}
