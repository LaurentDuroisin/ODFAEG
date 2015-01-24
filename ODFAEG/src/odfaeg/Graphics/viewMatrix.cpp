

#include "../../../include/odfaeg/Graphics/viewMatrix.h"
#include "../../../include/odfaeg/Physics/boundingBox.h"
#include "../../../include/odfaeg/Math/computer.h"
namespace odfaeg {
    namespace graphic {
        ViewMatrix::ViewMatrix() {
            s3d = math::Vec3f (1, 1, 1);
            o3d = math::Vec3f (0, 0, 0);
            pitch = yaw = roll = 0;
            needToUpdate3D = false;
            inverseNeedToUpdate3D = false;
        }
        void ViewMatrix::setScale(const math::Vec3f scale) {
            s3d = scale;
            needToUpdate3D = true;
            inverseNeedToUpdate3D = true;
        }
        void ViewMatrix::setOrigin(math::Vec3f origin) {
            o3d = origin;
            needToUpdate3D = true;
            inverseNeedToUpdate3D = true;
        }
        void ViewMatrix::reset3D () {
            matrix4f.identity();
            invMat4f.identity();
            needToUpdate3D = false;
        }
        math::Vec3f ViewMatrix::transform (const math::Vec3f vec3) {
            if (needToUpdate3D) {
                float x = math::Math::toRadians(pitch);
                float y = math::Math::toRadians(yaw);
                float z = math::Math::toRadians(roll);
                float cx = math::Math::cosinus(x);
                float sx = math::Math::sinus(x);
                float cy = math::Math::cosinus(y);
                float sy = math::Math::sinus(y);
                float cz = math::Math::cosinus(z);
                float sz = math::Math::sinus(z);
                math::Matrix4f transform;
                transform.m11 = cy*cz;
                transform.m12 = sx*sy*cz + cx*sz;
                transform.m13 = -cx*sy*cz + sx*sz;
                transform.m14 = 0;
                transform.m21 = -cy*sz;
                transform.m22 = -sx*sy*sz + cx*cz;
                transform.m23 = cx*sy*sz + sx*cz;
                transform.m24 = 0;
                transform.m31 = sy;
                transform.m32 = -sx*cy;
                transform.m33 = cx*cy;
                transform.m34 = 0;
                transform.m41 = 0;
                transform.m42 = 0;
                transform.m43 = 0;
                transform.m44 = 1;
                matrix4f.m11 = xAxis.x;
                matrix4f.m12 = xAxis.y;
                matrix4f.m13 = xAxis.z;
                matrix4f.m14 = -xAxis.dot2(o3d) / s3d.x;
                matrix4f.m21 = yAxis.x;
                matrix4f.m22 = yAxis.y;
                matrix4f.m23 = yAxis.z;
                matrix4f.m24 = -yAxis.dot2(o3d) / s3d.y;
                matrix4f.m31 = zAxis.x;
                matrix4f.m32 = zAxis.y;
                matrix4f.m33 = zAxis.z;
                matrix4f.m34 = -zAxis.dot2(o3d) / s3d.z;
                matrix4f.m41 = 0;
                matrix4f.m42 = 0;
                matrix4f.m43 = 0;
                matrix4f.m44 = 1;
                matrix4f = transform * matrix4f;
                needToUpdate3D = false;
            }
            return matrix4f * vec3;
        }
        float* ViewMatrix::getGlMatrix () {
            float* matrix = new float[16];
            if (needToUpdate3D) {
                float x = math::Math::toRadians(pitch);
                float y = math::Math::toRadians(yaw);
                float z = math::Math::toRadians(roll);
                float cx = math::Math::cosinus(x);
                float sx = math::Math::sinus(x);
                float cy = math::Math::cosinus(y);
                float sy = math::Math::sinus(y);
                float cz = math::Math::cosinus(z);
                float sz = math::Math::sinus(z);
                math::Matrix4f transform;
                transform.m11 = cy*cz;
                transform.m12 = sx*sy*cz + cx*sz;
                transform.m13 = -cx*sy*cz + sx*sz;
                transform.m14 = 0;
                transform.m21 = -cy*sz;
                transform.m22 = -sx*sy*sz + cx*cz;
                transform.m23 = cx*sy*sz + sx*cz;
                transform.m24 = 0;
                transform.m31 = sy;
                transform.m32 = -sx*cy;
                transform.m33 = cx*cy;
                transform.m34 = 0;
                transform.m41 = 0;
                transform.m42 = 0;
                transform.m43 = 0;
                transform.m44 = 1;
                matrix4f.m11 = xAxis.x;
                matrix4f.m12 = xAxis.y;
                matrix4f.m13 = xAxis.z;
                matrix4f.m14 = -xAxis.dot2(o3d) / s3d.x;
                matrix4f.m21 = yAxis.x;
                matrix4f.m22 = yAxis.y;
                matrix4f.m23 = yAxis.z;
                matrix4f.m24 = -yAxis.dot2(o3d) / s3d.y;
                matrix4f.m31 = zAxis.x;
                matrix4f.m32 = zAxis.y;
                matrix4f.m33 = zAxis.z;
                matrix4f.m34 = -zAxis.dot2(o3d) / s3d.z;
                matrix4f.m41 = 0;
                matrix4f.m42 = 0;
                matrix4f.m43 = 0;
                matrix4f.m44 = 1;
                matrix4f = transform * matrix4f;
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
        void ViewMatrix::update() {
            float x = math::Math::toRadians(pitch);
            float y = math::Math::toRadians(yaw);
            float z = math::Math::toRadians(roll);
            float cx = math::Math::cosinus(x);
            float sx = math::Math::sinus(x);
            float cy = math::Math::cosinus(y);
            float sy = math::Math::sinus(y);
            float cz = math::Math::cosinus(z);
            float sz = math::Math::sinus(z);
            math::Matrix4f transform;
            transform.m11 = cy*cz;
            transform.m12 = sx*sy*cz + cx*sz;
            transform.m13 = -cx*sy*cz + sx*sz;
            transform.m14 = 0;
            transform.m21 = -cy*sz;
            transform.m22 = -sx*sy*sz + cx*cz;
            transform.m23 = cx*sy*sz + sx*cz;
            transform.m24 = 0;
            transform.m31 = sy;
            transform.m32 = -sx*cy;
            transform.m33 = cx*cy;
            transform.m34 = 0;
            transform.m41 = 0;
            transform.m42 = 0;
            transform.m43 = 0;
            transform.m44 = 1;
            matrix4f.m11 = xAxis.x;
            matrix4f.m12 = xAxis.y;
            matrix4f.m13 = xAxis.z;
            matrix4f.m14 = -xAxis.dot2(o3d) / s3d.x;
            matrix4f.m21 = yAxis.x;
            matrix4f.m22 = yAxis.y;
            matrix4f.m23 = yAxis.z;
            matrix4f.m24 = -yAxis.dot2(o3d) / s3d.y;
            matrix4f.m31 = zAxis.x;
            matrix4f.m32 = zAxis.y;
            matrix4f.m33 = zAxis.z;
            matrix4f.m34 = -zAxis.dot2(o3d) / s3d.z;
            matrix4f.m41 = 0;
            matrix4f.m42 = 0;
            matrix4f.m43 = 0;
            matrix4f.m44 = 1;
            matrix4f = transform * matrix4f;
            needToUpdate3D = false;
        }
        math::Matrix4f ViewMatrix::getMatrix () {
            return matrix4f;
        }
        math::Vec3f ViewMatrix::inverseTransform (const math::Vec3f vec3) {

            if (inverseNeedToUpdate3D) {
                if (needToUpdate3D) {
                    float x = math::Math::toRadians(pitch);
                    float y = math::Math::toRadians(yaw);
                    float z = math::Math::toRadians(roll);
                    float cx = math::Math::cosinus(x);
                    float sx = math::Math::sinus(x);
                    float cy = math::Math::cosinus(y);
                    float sy = math::Math::sinus(y);
                    float cz = math::Math::cosinus(z);
                    float sz = math::Math::sinus(z);
                    math::Matrix4f transform;
                    transform.m11 = cy*cz;
                    transform.m12 = sx*sy*cz + cx*sz;
                    transform.m13 = -cx*sy*cz + sx*sz;
                    transform.m14 = 0;
                    transform.m21 = -cy*sz;
                    transform.m22 = -sx*sy*sz + cx*cz;
                    transform.m23 = cx*sy*sz + sx*cz;
                    transform.m24 = 0;
                    transform.m31 = sy;
                    transform.m32 = -sx*cy;
                    transform.m33 = cx*cy;
                    transform.m34 = 0;
                    transform.m41 = 0;
                    transform.m42 = 0;
                    transform.m43 = 0;
                    transform.m44 = 1;
                    matrix4f.m11 = xAxis.x;
                    matrix4f.m12 = xAxis.y;
                    matrix4f.m13 = xAxis.z;
                    matrix4f.m14 = -xAxis.dot2(o3d) / s3d.x;
                    matrix4f.m21 = yAxis.x;
                    matrix4f.m22 = yAxis.y;
                    matrix4f.m23 = yAxis.z;
                    matrix4f.m24 = -yAxis.dot2(o3d) / s3d.y;
                    matrix4f.m31 = zAxis.x;
                    matrix4f.m32 = zAxis.y;
                    matrix4f.m33 = zAxis.z;
                    matrix4f.m34 = -zAxis.dot2(o3d) / s3d.z;
                    matrix4f.m41 = 0;
                    matrix4f.m42 = 0;
                    matrix4f.m43 = 0;
                    matrix4f.m44 = 1;
                    matrix4f = transform * matrix4f;
                    needToUpdate3D = false;
                }
                invMat4f = matrix4f.inverse();
                inverseNeedToUpdate3D = false;
            }
            return invMat4f * vec3;
        }
        void ViewMatrix::setRotation(float pitch, float yaw, float roll) {
            this->pitch = pitch;
            this->yaw = yaw;
            this->roll = roll;
        }
        void ViewMatrix::setAxis(math::Vec3f left, math::Vec3f up, math::Vec3f forward) {
            xAxis = left;
            yAxis = left.cross(forward);
            zAxis = forward;
        }
    }
}

