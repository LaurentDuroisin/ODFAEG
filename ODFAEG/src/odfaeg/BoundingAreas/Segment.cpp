#include "../../../include/odfaeg/BoundingAreas/segment.h"
namespace odfaeg {
Ray::Ray () : orig(0, 0, 0), ext(0, 0, 0), dir(0, 0, 0){


}
Ray::Ray (Vec3f orig, Vec3f ext) : orig(orig), ext(ext), dir(ext - orig) {

}

void Ray::setOrig (Vec3f &orig) {
    this->orig = orig;
}
void Ray::setExt (Vec3f &ext) {
    this->ext = ext;
}
Vec3f& Ray::getOrig () {
    return orig;
}
Vec3f& Ray::getExt () {
    return ext;
}
Vec3f& Ray::getDir () {
    return dir;
}
float Ray::intersects (Ray &other) {
    double denom = dir.y * other.dir.z - dir.z * other.dir.y + dir.z * other.dir.x - dir.x * other.dir.z + dir.x * other.dir.y - dir.y * other.dir.x;

    if (denom == 0)
        return -1;
    float t = -(orig.y * other.dir.z - orig.z * other.dir.y + orig.z * other.dir.x - orig.x * other.dir.z + orig.x * other.dir.y - orig.y * other.dir.x) / denom;
    if (t < 0 || t >= 1)
        return 0;
    float u = -(-dir.y * other.orig.z - dir.z * other.orig.y + dir.z * other.orig.x - dir.x * other.orig.z + dir.x * other.orig.y - dir.y * other.orig.x) / denom;
    if (u < 0 || u >= 1)
        return 0;
    return 1;
}
float Ray::intersectsWhere (Ray &other) {

    Vec3f ap = other.ext - orig;
    Vec3f ao = other.orig - orig;
    double denom = dir.y * other.dir.z - dir.z * other.dir.y + dir.z * other.dir.x - dir.x * other.dir.z + dir.x * other.dir.y - dir.y * other.dir.x;

    if ((dir.y * ap.z - dir.z * ap.y + dir.z * ap.x - dir.x * ap.z + dir.x * ap.y - dir.y * ap.x) * (dir.y * ao.z - dir.z * ao.y + dir.z * ao.x - dir.x * ao.z + dir.x * ao.y - dir.y * ao.x) >= 0) {

        return -1.f;
    }
    float k = -(orig.y * other.dir.z - orig.z * other.dir.y + orig.z * other.dir.x - orig.x * other.dir.z + orig.x * other.dir.y - orig.y * other.dir.x) / denom;

    if (k < 0 || k > 1)
        return -1.f;

    return k;
}
float Ray::intersectWhereOther (Ray &other) {
    Vec3f ap = other.ext - orig;
    Vec3f ao = other.orig - orig;
    double denom = dir.y * other.dir.z - dir.z * other.dir.y + dir.z * other.dir.x - dir.x * other.dir.z + dir.x * other.dir.y - dir.y * other.dir.x;
    if ((dir.y * ap.z - dir.z * ap.y + dir.z * ap.x - dir.x * ap.z + dir.x * ap.y - dir.y * ap.x) * (dir.y * ao.z - dir.z * ao.y + dir.z * ao.x - dir.x * ao.z + dir.x * ao.y - dir.y * ao.x) >= 0) {
        return -1.f;
    }
    float l = -(-dir.y * other.orig.z - dir.z * other.orig.y + dir.z * other.orig.x - dir.x * other.orig.z + dir.x * other.orig.y - dir.y * other.orig.x) / denom;
    if (l < 0 || l > 1)
        return -1.f;

    return l;
}
}



