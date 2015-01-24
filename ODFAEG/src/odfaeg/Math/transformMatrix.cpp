

#include "../../../include/odfaeg/Math/transformMatrix.h"
#include "../../../include/odfaeg/Math/computer.h"
namespace odfaeg {
TransformMatrix::TransformMatrix() {
    t3d = Vec3f(0, 0, 0);
    t2d = Vec2f(0, 0);
    r3d = 0;
    r3dAxis = Vec3f(0, 0, 0);
    r2d = 0;
    s3d = Vec3f (1, 1, 1);
    s2d = Vec2f (1, 1);
    o2d = Vec2f (0, 0);
    o3d = Vec3f (0, 0, 0);
    needToUpdate2D = false;
    inverseNeedToUpdate2D = false;
    needToUpdate3D = false;
    inverseNeedToUpdate3D = false;
}
sf::Transform TransformMatrix::getSFMLTransform()  {
    if (needToUpdate2D) {
        float angle = Math::toRadians(r2d);
        float ssx, scx, ssy, scy, tx, ty;
        ssx = Math::sinus(angle) * s2d.x;
        scx = Math::cosinus(angle) * s2d.x;
        ssy = Math::sinus(angle) * s2d.y;
        scy = Math::cosinus(angle) * s2d.y;

        tx = -o2d.x * scx - o2d.y * ssy + t2d.x + o2d.x;
        ty = o2d.x * ssx - o2d.y * scy + t2d.y + o2d.y;
        matrix3f.m11 = scx;
        matrix3f.m12 = ssy;
        matrix3f.m13 = tx;
        matrix3f.m21 = -ssx;
        matrix3f.m22 = scy;
        matrix3f.m23 = ty;
        matrix3f.m31 = 0;
        matrix3f.m32 = 0;
        matrix3f.m33 = 1;
        needToUpdate2D = false;
    }
    return sf::Transform(matrix3f.m11, matrix3f.m12, matrix3f.m13,
                     matrix3f.m21, matrix3f.m22, matrix3f.m23,
                     matrix3f.m31, matrix3f.m32, matrix3f.m33);
}
void TransformMatrix::setOrigin(float x, float y) {
    o2d = Vec2f (x, y);
}
void TransformMatrix::setOrigin(Vec3f vec3) {
    o3d = vec3;
}
void TransformMatrix::setTranslation (const Vec3f trans) {
    t3d = trans;
    needToUpdate3D = true;
    inverseNeedToUpdate3D = true;
}
void TransformMatrix::setScale(const Vec3f scale) {
    s3d = scale;
    needToUpdate3D = true;
    inverseNeedToUpdate3D = true;
}
void TransformMatrix::setRotation(const Vec3f axis, float teta) {
    r3d = teta;
    r3dAxis = axis.normalize();
    needToUpdate3D = true;
    inverseNeedToUpdate3D = true;
}
void TransformMatrix::setRotation (int angle) {
    r2d = angle;
    needToUpdate2D = true;
    inverseNeedToUpdate2D = true;
}

void TransformMatrix::setScale (float sx, float sy) {
    s2d = Vec2f (sx, sy);
    needToUpdate2D = true;
    inverseNeedToUpdate2D = true;
}
void TransformMatrix::setTranslation (int dx, int dy) {
    t2d = Vec2f (dx, dy);
    needToUpdate2D = true;
    inverseNeedToUpdate2D = true;
}
void TransformMatrix::reset2D () {
     matrix3f.identity ();
     t2d = Vec2f(0, 0);
     s2d = Vec2f(1, 1);
     r2d = 0;
     needToUpdate2D = false;
}

void TransformMatrix::reset3D () {
    matrix4f.identity();
    needToUpdate3D = false;
}
Vec3f TransformMatrix::transform (const Vec3f vec3) {
    if (needToUpdate3D) {
        float angle;
        float sr, sp, sy, cr, cp, cy, tx, ty, tz;
        angle = Math::toRadians(r3d);
        matrix4f.m11 = (r3dAxis.x * r3dAxis.x * (1 - Math::cosinus(angle)) + Math::cosinus(angle)) * s3d.x;
        matrix4f.m12 = (r3dAxis.x * r3dAxis.y * (1 - Math::cosinus(angle)) - r3dAxis.z * Math::sinus(angle)) * s3d.y;
        matrix4f.m13 = (r3dAxis.x * r3dAxis.z * (1 - Math::cosinus(angle)) + r3dAxis.y * Math::sinus(angle)) * s3d.z;
        matrix4f.m14 = -o3d.x * matrix4f.m11 - o3d.y * matrix4f.m12 - o3d.z * matrix4f.m13 + t3d.x + o3d.x;
        matrix4f.m21 = (r3dAxis.y * r3dAxis.x * (1 - Math::cosinus(angle)) + r3dAxis.z * Math::sinus(angle)) * s3d.x;
        matrix4f.m22 = (r3dAxis.y * r3dAxis.y * (1 - Math::cosinus(angle)) + Math::cosinus(angle)) * s3d.y;
        matrix4f.m23 = (r3dAxis.y * r3dAxis.z * (1 - Math::cosinus(angle)) - r3dAxis.x * Math::sinus(angle)) * s3d.z;
        matrix4f.m24 = -o3d.x * matrix4f.m21 - o3d.y * matrix4f.m22 - o3d.z * matrix4f.m23 + t3d.y + o3d.y;
        matrix4f.m31 = (r3dAxis.z * r3dAxis.x * (1 - Math::cosinus(angle)) - r3dAxis.y * Math::sinus(angle)) * s3d.x;
        matrix4f.m32 = (r3dAxis.z * r3dAxis.y * (1 - Math::cosinus(angle)) + r3dAxis.x * Math::cosinus(angle)) * s3d.y;
        matrix4f.m33 = (r3dAxis.z * r3dAxis.z * (1 - Math::cosinus(angle)) + Math::cosinus(angle)) * s3d.z;
        matrix4f.m34 = -o3d.x * matrix4f.m31 - o3d.y * matrix4f.m32 - o3d.z * matrix4f.m33 + t3d.z + o3d.z;
        matrix4f.m41 = 0;
        matrix4f.m42 = 0;
        matrix4f.m43 = 0;
        matrix4f.m44 = 1;
        needToUpdate3D = false;
    }
    return matrix4f * vec3;
}
Vec2f TransformMatrix::transform (const Vec2f vec2) {
    if (needToUpdate2D) {
        float angle = Math::toRadians(r2d);
        float ssx, scx, ssy, scy, tx, ty;
        ssx = Math::sinus(angle) * s2d.x;
        scx = Math::cosinus(angle) * s2d.x;
        ssy = Math::sinus(angle) * s2d.y;
        scy = Math::cosinus(angle) * s2d.y;

        tx = -o2d.x * scx - o2d.y * ssy + t2d.x + o2d.x;
        ty = o2d.x * ssx - o2d.y * scy + t2d.y + o2d.y;
        matrix3f.m11 = scx;
        matrix3f.m12 = ssy;
        matrix3f.m13 = tx;
        matrix3f.m21 = -ssx;
        matrix3f.m22 = scy;
        matrix3f.m23 = ty;
        matrix3f.m31 = 0;
        matrix3f.m32 = 0;
        matrix3f.m33 = 1;
        needToUpdate2D = false;
    }
    return matrix3f * vec2;
}
float* TransformMatrix::getGlMatrix () {
    float* matrix = new float[16];

    if (needToUpdate3D) {
        float angle;
        float sr, sp, sy, cr, cp, cy, tx, ty, tz;
        angle = Math::toRadians(r3d);
        matrix4f.m11 = (r3dAxis.x * r3dAxis.x * (1 - Math::cosinus(angle)) + Math::cosinus(angle)) * s3d.x;
        matrix4f.m12 = (r3dAxis.x * r3dAxis.y * (1 - Math::cosinus(angle)) - r3dAxis.z * Math::sinus(angle)) * s3d.y;
        matrix4f.m13 = (r3dAxis.x * r3dAxis.z * (1 - Math::cosinus(angle)) + r3dAxis.y * Math::sinus(angle)) * s3d.z;
        matrix4f.m14 = -o3d.x * matrix4f.m11 - o3d.y * matrix4f.m12 - o3d.z * matrix4f.m13 + t3d.x + o3d.x;
        matrix4f.m21 = (r3dAxis.y * r3dAxis.x * (1 - Math::cosinus(angle)) + r3dAxis.z * Math::sinus(angle)) * s3d.x;
        matrix4f.m22 = (r3dAxis.y * r3dAxis.y * (1 - Math::cosinus(angle)) + Math::cosinus(angle)) * s3d.y;
        matrix4f.m23 = (r3dAxis.y * r3dAxis.z * (1 - Math::cosinus(angle)) - r3dAxis.x * Math::sinus(angle)) * s3d.z;
        matrix4f.m24 = -o3d.x * matrix4f.m21 - o3d.y * matrix4f.m22 - o3d.z * matrix4f.m23 + t3d.y + o3d.y;
        matrix4f.m31 = (r3dAxis.z * r3dAxis.x * (1 - Math::cosinus(angle)) - r3dAxis.y * Math::sinus(angle)) * s3d.x;
        matrix4f.m32 = (r3dAxis.z * r3dAxis.y * (1 - Math::cosinus(angle)) + r3dAxis.x * Math::cosinus(angle)) * s3d.y;
        matrix4f.m33 = (r3dAxis.z * r3dAxis.z * (1 - Math::cosinus(angle)) + Math::cosinus(angle)) * s3d.z;
        matrix4f.m34 = -o3d.x * matrix4f.m31 - o3d.y * matrix4f.m32 - o3d.z * matrix4f.m33 + t3d.z + o3d.z;
        matrix4f.m41 = 0;
        matrix4f.m42 = 0;
        matrix4f.m43 = 0;
        matrix4f.m44 = 1;
        needToUpdate3D = false;
    }
    matrix[0] = matrix4f.m11;
    matrix[1] = matrix4f.m21;
    matrix[2] = matrix4f.m31;
    matrix[3] = matrix4f.m41;
    matrix[4] = matrix4f.m12;
    matrix[5] = matrix4f.m22;
    matrix[6] = matrix4f.m32;
    matrix[7] = matrix4f.m42;
    matrix[8] = matrix4f.m13;
    matrix[9] = matrix4f.m23;
    matrix[10] = matrix4f.m33;
    matrix[11] = matrix4f.m43;
    matrix[12] = matrix4f.m14;
    matrix[13] = matrix4f.m24;
    matrix[14] = matrix4f.m34;
    matrix[15] = matrix4f.m44;
    return matrix;
}
Matrix3f TransformMatrix::get2DMatrix() const {
    return matrix3f;
}
Matrix4f TransformMatrix::get3DMatrix () {
    return matrix4f;
}
Vec3f TransformMatrix::inverseTransform (const Vec3f vec3) {
    if (inverseNeedToUpdate3D) {
        try {
            invMat4f = matrix4f.inverse();
        } catch (std::exception &e) {
            invMat4f.identity();
        }
        inverseNeedToUpdate3D = false;
    }
    return invMat4f * vec3;
}
Vec2f TransformMatrix::inverseTransform (const Vec2f vec2) {
    if (inverseNeedToUpdate2D) {
        try {
            invMat3f = matrix3f.inverse();
        } catch (std::exception &e) {
            invMat3f.identity();
        }
        inverseNeedToUpdate2D = false;
    }
    return invMat3f * vec2;
}
void TransformMatrix::combine(Matrix3f other) {
    matrix3f =  matrix3f * other;
}
BoundingRectangle TransformMatrix::transformRect (BoundingRectangle br) {
    std::array<Vec2f, 4> points;
    points[0] = br.getPosition();
    points[1] = Vec2f (br.getPosition().x + br.getWidth(),br.getPosition().y);
    points[2] = Vec2f (br.getPosition().x + br.getWidth(), br.getPosition().y + br.getHeight());
    points[3] = Vec2f (br.getPosition().x, br.getPosition().y + br.getHeight());
    for (unsigned int i = 0; i < points.size(); i++) {
        points[i] = transform(points[i]);
    }
    std::array<std::array<float, 2>, 2> store;
    store = Computer::getExtends(points);

    return BoundingRectangle(store[0][0], store[1][0],store[0][1] - store[0][0],store[1][1] - store[1][0]);
}
AABB TransformMatrix::transformAABB (AABB aabb) {
    std::array<Vec3f, 24> points;
    //Devant
    points[0] = aabb.position;
    points[1] = Vec3f (aabb.position.x + aabb.width, aabb.position.y, aabb.position.z);
    points[2] = Vec3f (aabb.position.x + aabb.width, aabb.position.y + aabb.height, aabb.position.z);
    points[3] = Vec3f (aabb.position.x, aabb.position.y + aabb.height, aabb.position.z);
    //Derrière
    points[4] = Vec3f (aabb.position.x, aabb.position.y, aabb.position.z - aabb.depth);
    points[5] = Vec3f (aabb.position.x, aabb.position.y + aabb.height, aabb.position.z - aabb.depth);
    points[6] = Vec3f (aabb.position.x + aabb.width, aabb.position.y + aabb.height, aabb.position.z - aabb.depth);
    points[7] = Vec3f (aabb.position.x + aabb.width, aabb.position.y, aabb.position.z - aabb.depth);
    //Dessus
    points[8] = Vec3f (aabb.position.x, aabb.position.y + aabb.height, aabb.position.z);
    points[9] = Vec3f (aabb.position.x + aabb.width, aabb.position.y + aabb.height, aabb.position.z);
    points[10] = Vec3f (aabb.position.x + aabb.width, aabb.position.y + aabb.height, aabb.position.z - aabb.depth);
    points[11] = Vec3f (aabb.position.x, aabb.position.y + aabb.height, aabb.position.z - aabb.depth);
    //Dessous
    points[12] = aabb.position;
    points[13] = Vec3f(aabb.position.x + aabb.width, aabb.position.y, aabb.position.z);
    points[14] = Vec3f(aabb.position.x + aabb.width, aabb.position.y, aabb.position.z - aabb.depth);
    points[15] = Vec3f(aabb.position.x, aabb.position.y, aabb.position.z - aabb.depth);
    //Gauche
    points[16] = aabb.position;
    points[17] = Vec3f(aabb.position.x, aabb.position.y + aabb.height, aabb.position.z);
    points[18] = Vec3f(aabb.position.x, aabb.position.y + aabb.height, aabb.position.z - aabb.depth);
    points[19] = Vec3f (aabb.position.x, aabb.position.y, aabb.position.z - aabb.depth);
    //Droite
    points[20] = Vec3f(aabb.position.x + aabb.width, aabb.position.y, aabb.position.z);
    points[21] = Vec3f(aabb.position.x + aabb.width, aabb.position.y + aabb.height, aabb.position.z);
    points[22] = Vec3f(aabb.position.x + aabb.width, aabb.position.y + aabb.height, aabb.position.z - aabb.depth);
    points[23] = Vec3f (aabb.position.x + aabb.width, aabb.position.y, aabb.position.z - aabb.depth);
    for (unsigned int i = 0; i < points.size(); i++) {

        points[i] = transform(points[i]);
    }
    std::array<std::array<float, 3>, 2> store;
    store = Computer::getExtends(points);
    return AABB(Vec3f(store[0][0], store[1][0],store[2][0]), store[0][1] - store[0][0],store[1][1] - store[1][0], store[2][1] - store[2][0]);
}
}

