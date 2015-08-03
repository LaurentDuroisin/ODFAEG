#ifndef AXIS_ALIGNED_BOUNDING_BOX
#define AXIS_ALIGNED_BOUNDING_BOX
#include "../Math/vec4.h"
namespace odfaeg {
class AABB {
public :
    AABB() {
        position = Vec3f (0, 0, 0);
        width = 1;
        height = 1;
        depth = 1;
        center = Vec3f (0.5f, 0.5f, 0.5f);
    }
    AABB(Vec3f pos, float w, float h, float d) {
        position = pos;
        width = w;
        height = h;
        depth = d;
        center = Vec3f (width * 0.5f, height * 0.5f, depth * 0.5f);
    }
    void setSize(float width, float height, float depth) {
        this->width = width;
        this->height = height;
        this->depth = depth;
    }
    bool operator== (const AABB &other) const {
        return position == other.position && width == other.width && height == other.height && depth == other.depth;
    }
    bool intersects (AABB &other) {
        int hx1 = width * 0.5f;
        int hy1 = height * 0.5f;
        int hz1 = depth * 0.5f;
        int hx2 = other.width * 0.5f;
        int hy2 = other.height * 0.5f;
        int hz2 = other.depth * 0.5f;
        float minX1 = center.x - hx1, minX2 = other.center.x - hx2;
        float minY1 = center.y - hy1, minY2 = other.center.y - hy2;
        float minZ1 = center.z - hz1, minZ2 = other.center.z - hz2;
        float maxX1 = center.x + hx1, maxX2 = other.center.x + hx2;
        float maxY1 = center.y + hy1, maxY2 = other.center.y + hy2;
        float maxZ1 = center.z + hz1, maxZ2 = other.center.z + hz2;
        for (int i = 0; i < 3; i++) {
            if (i == 0) {
                if (minX1 > maxX2 || maxX1 < minX2)
                    return false;
            } else if (i == 2) {
                if (minY1 > maxY2 || maxY1 < minY2)
                    return false;
            } else {
                if (minZ1 > maxZ2 || maxZ1 < minZ2)
                    return false;
            }
        }
        return true;
    }
    Vec3f position;
    Vec3f center;
    float width, height, depth;
};
}
#endif // AXIS_ALIGNED_BOUNDING_BOX
