#ifndef OrientedBoundingBox_H
#define OrientedBoundingBox_H
#include "../Math/vec4.h"
#include "../Math/computer.h"
#include "axisAlignedBoundingBox.h"
namespace odfaeg {
class OBB {
public :
    OBB () {
        x = y = z = 0;
        bx = Vec3f::xAxis;
        by = Vec3f::yAxis;
        bz = Vec3f::zAxis;
        width = 1;
        height = 1;
        depth = 1;
    }
    OBB (Vec3f p1, Vec3f p2, Vec3f p3, Vec3f p4, Vec3f p5, Vec3f p6, Vec3f p7, Vec3f p8) {
        std::array<Vec3f, 8> corners;
        std::array<Vec3f, 4> frontCorners;
        std::array<Vec3f, 4> backCorners;
        corners[0] = p1;
        corners[1] = p2;
        corners[2] = p3;
        corners[3] = p4;
        corners[4] = p5;
        corners[5] = p6;
        corners[6] = p7;
        corners[7] = p8;
        for (unsigned int i = 0; i < corners.size() - 1; i++) {
            for (unsigned int j = 1; j < corners.size(); j++) {
                if (corners[j].z < corners[i].z) {
                    float tmp = corners[i].z;
                    corners[i].z = corners[j].z;
                    corners[j].z = tmp;
                }
            }
        }
        frontCorners[0] = corners[0];
        frontCorners[1] = corners[1];
        frontCorners[2] = corners[2];
        frontCorners[3] = corners[3];
        frontCorners[0] = corners[4];
        frontCorners[1] = corners[5];
        frontCorners[2] = corners[6];
        frontCorners[3] = corners[7];
        std::array<std::array<float, 3>, 2> store;
        store = Computer::getExtends(corners);
        width = store[0][1] - store[0][0];
        height = store[1][1] - store[1][0];
        depth = store[2][1] - store[2][0];
        x = store[0][0];
        y = store[1][0];
        z = store[2][0];
        store = Computer::getExtends(frontCorners);
        Vec3f position (store[0][0], store[1][0], store[2][0]);
        int p1Index, p2Index,  p3Index, p4Index, p5Index, p6Index, p7Index, p8Index;
        //Check the right order of the points to find the rays of the oriented bounding rectangle.
        for (unsigned int i = 0; i < 4; i++) {
            if (corners[i].x == position.x)
                p1Index = i;
            else if (corners[i].y <= position.y && corners[i].x != store[0][1])
                p2Index = i;
            else if (corners[i].y == store[1][1])
                p4Index = i;
            else
                p3Index = i;
        }
        store = Computer::getExtends(backCorners);
        position = Vec3f (store[0][0], store[1][0], store[2][0]);
        //Check the right order of the points to find the rays of the oriented bounding rectangle.
        for (unsigned int i = 4; i < 8; i++) {
            if (corners[i].x == position.x)
                p5Index = i;
            else if (corners[i].y <= position.y && corners[i].x != store[0][1])
                p6Index = i;
            else if (corners[i].y == store[1][1])
                p8Index = i;
            else
                p7Index = i;
        }
        bx = Vec3f (corners[p2Index] - corners[p1Index]);
        bx = bx.normalize();
        by = Vec3f (corners[p3Index] - corners[p2Index]);
        by = by.normalize();
        bz = Vec3f (corners[p5Index] - corners[p1Index]);
        center = Vec3f (x + width * 0.5f, y + height * 0.5f, z + depth * 0.5f);
    }
    bool operator== (const OBB &other) const {
        return x == other.x && y == other.y && z == other.z && width == other.width && height == other.height && depth == other.depth;
    }
    bool intersects(AABB &br) {
        //Direction between the 2 centers.
        Vec3f d = br.center - center;
        //Distance between the two center.
        float m = center.computeDist(br.center);
        //Half dimensions.
        int hi[3];
        hi[0] = br.width * 0.5f;
        hi[1] = br.height * 0.5f;
        hi[2] = br.depth * 0.5f;
        /*Vectors between the center of our oriented bounding rectangle and the middle of their edges.
         * this is the rays in x and y of our oriented bounding rectangle.
        */
        Vec3f bi[3];
        bi[0] = bx * width * 0.5f;
        bi[1] = by * height * 0.5f;
        bi[2] = bz * depth * 0.5f;
        for (unsigned int i = 0; i < 3; i++) {
            //Projections of the rays of our bounding rectangle from d to x and y axis.
            float r1 = d.projOnAxis(bi[i]);
            for (unsigned int j = 0; j < 3; j++) {
                //The projection of hi from d to x and y axis is equal to hi because the rect is aligned with x and y axis.
                float r2 = hi[j];
                //Calculate the sum of the rays of our bounding volumes.
                float rSum = r1 + r2;
                /*If the distance between the two centers is shorter than the sum of the rays,
                 * it means that our two bouding volumes are in collision.
                 *(Separator axis theorem)
                 */
                if (m - rSum <= 0)
                    return true;
            }
        }
        return false;
    }
    int width, height, depth;
    int x, y, z;
    Vec3f bx, by, bz, center;
};
}
#endif // OrientedBoundingBox
