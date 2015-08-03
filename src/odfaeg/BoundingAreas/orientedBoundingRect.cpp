#include "../../../include/odfaeg/BoundingAreas/boundingRectangle.h"
#include "../../../include/odfaeg/BoundingAreas/boundingCircle.h"
#include "../../../include/odfaeg/BoundingAreas/boundingEllipse.h"
#include "../../../include/odfaeg/BoundingAreas/orientedBoundingRect.h"
#include "../../../include/odfaeg/BoundingAreas/boundingPolygon.h"

namespace odfaeg {
OrientedBoundingBox::OrientedBoundingBox (Vec3f p1, Vec3f p2, Vec3f p3, Vec3f p4, Vec3f p5, Vec3f p6, Vec3f p7, Vec3f p8) {
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
    backCorners[0] = corners[4];
    backCorners[1] = corners[5];
    backCorners[2] = corners[6];
    backCorners[3] = corners[7];
    std::array<std::array<float, 2>, 3> store;
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
bool OrientedBoundingBox::intersects(BoundingSphere &bs) {
    return bs.intersects(*this);
}
bool OrientedBoundingBox::intersects(BoundingEllipsoid &be) {
    return be.intersects(*this);
}
bool OrientedBoundingBox::intersects (BoundingBox &bx) {
    //Direction between the 2 centers.
    Vec3f d = bx.getCenter() - center;
    //Distance between the two center.
    float m = center.computeDist(bx.getCenter());
    //Half dimensions.
    int hi[3];
    hi[0] = bx.getWidth() * 0.5f;
    hi[1] = bx.getHeight() * 0.5f;
    hi[2] = bx.getDepth() * 0.5f;
    /*Vectors between the center of our oriented bounding rectangle and the middle of their edges.
     * this is the rays in x and y of our oriented bounding rectangle.
    */
    Vec3f bi[3];
    bi[0] = this->bx * width * 0.5f;
    bi[1] = this->by * height * 0.5f;
    bi[2] = this->bz * depth * 0.5f;
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
bool OrientedBoundingBox::intersects (OrientedBoundingBox &obx) {
    //Direction between the 2 centers.
    Vec3f d = obx.getCenter() - center;
    //Distance between the two center.
    float m = center.computeDist(obx.getCenter());
    /*Vectors between the center of our oriented bounding rectangles and the middle of their edges.
     * this is the rays in x and y of our oriented bounding rectangles.
    */
    Vec3f bi1[3];
    bi1[0] = bx * width * 0.5f;
    bi1[1] = by * height * 0.5f;
    bi1[2] = bz * depth * 0.5f;
    Vec3f bi2[3];
    bi2[0] = obx.getBX() * obx.getWidth() * 0.5f;
    bi2[1] = obx.getBY() * obx.getHeight() * 0.5f;
    bi2[2] = obx.getBZ() * obx.getDepth() * 0.5f;
    for (unsigned int i = 0; i < 3; i++) {
        //Projections of the rays of our bounding rectangle from d to the x and y axis.
        float r1 = d.projOnAxis(bi1[i]);
        for (unsigned int j = 0; j < 3; j++) {
            //Projections of the rays of the other bounding rectangle from d to the x and y axis.
            float r2 = d.projOnAxis(bi2[j]);
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
bool OrientedBoundingBox::intersects (BoundingPolyhedron &bp) {
    //Direction between the 2 centers.
    Vec3f d = bp.getCenter() - center;
    //Distance between the two center.
    float m = center.computeDist(bp.getCenter());
    /*Vectors between the center of our oriented bounding rectangles and the middle of their edges.
     * this is the rays in x and y of our oriented bounding rectangles.
    */
    Vec3f bi1[3];
    bi1[0] = bx * width * 0.5f;
    bi1[1] = by * height * 0.5f;
    bi1[2] = bz * depth * 0.5f;

    for (unsigned int i = 0; i < 3; i++) {
        //Projections of the rays of our bounding rectangle from d to the x and y axis.
        float ray1 = d.projOnAxis(bi1[i]);
        Vec3f a = bp.getPoint(0);
        Vec3f b = bp.getPoint(1);
        Vec3f mid = a + (b - a) * 0.5f;
        Vec3f bi = mid - bp.getCenter();
        float proj = d.projOnAxis(bi);
        int min = proj;
        int max = proj;
        int ray2;
        for (unsigned int j = 1; j < bp.nbPoints(); j++) {
           a = bp.getPoint(j);
           if (j != bp.nbPoints() - 1)
               b = bp.getPoint(j+1);
           else
               b = bp.getPoint(0);
           mid = a + (b - a) * 0.5f;
           bi = (mid - bp.getCenter()) * 0.5f;
           proj = d.projOnAxis(bi);
           if (proj < min)
               min = proj;
           if (proj > max)
               max = proj;
        }
        ray2 = max - min;
        int rSum = ray1 + ray2;
        if (m - (ray1 + ray2))
            return false;
    }
    return true;
}

Vec3f OrientedBoundingBox::getCenter() {
    return center;
}
int OrientedBoundingBox::getWidth() {
    return width;
}
int OrientedBoundingBox::getHeight() {
    return height;
}
int OrientedBoundingBox::getDepth() {
    return depth;
}
Vec3f OrientedBoundingBox::getPosition() {
    return Vec3f(x, y, z);
}
void OrientedBoundingBox::setCorners (Vec3f p1, Vec3f p2, Vec3f p3, Vec3f p4, Vec3f p5, Vec3f p6, Vec3f p7, Vec3f p8) {
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
    backCorners[0] = corners[4];
    backCorners[1] = corners[5];
    backCorners[2] = corners[6];
    backCorners[3] = corners[7];
    std::array<std::array<float, 2>, 3> store;
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
Vec3f OrientedBoundingBox::getBX() {
    return bx;
}
Vec3f OrientedBoundingBox::getBY() {
    return by;
}
Vec3f OrientedBoundingBox::getBZ() {
    return bz;
}
}
