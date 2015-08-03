#include "../../../include/odfaeg/Graphics/model.h"
namespace odfaeg {
    namespace graphic {
        Model::Model (math::Vec3f position, math::Vec3f size, math::Vec3f origin, std::string type, Entity *parent)
            : Entity(position, size, origin, type, parent) {
        }
        bool Model::isModel() const {
            return true;
        }
        bool Model::isAnimated() const {
            return false;
        }
        bool Model::isLeaf() const {
            return false;
        }
        void Model::addSegment (math::Ray *segment) {
            segments.push_back(segment);
        }
        void Model::removeSegment (math::Ray *segment) {
            std::vector<math::Ray*>::iterator it;
            for (it = segments.begin(); it != segments.end();) {
                if(*it == segment)
                    it = segments.erase(it);
                else
                    it++;
            }
        }
        void Model::addShadowPoint (math::Vec3f *point) {
            points.push_back(point);
        }
        void Model::removeShadowPoint (math::Vec3f *point) {
            std::vector<math::Vec3f*>::iterator it;
            for (it = points.begin(); it != points.end();) {
                if(*it == point)
                    it = points.erase(it);
                else
                    it++;
            }
        }
        std::vector<math::Ray*> Model::getSegments() const {
            return segments;
        }
        std::vector<math::Vec3f*> Model::getPoints() const {
            return points;
        }
        Model::~Model () {
            for (unsigned int i = 0; i < segments.size(); i++)
                delete segments[i];
            segments.clear();
            for (unsigned int i = 0; i < points.size(); i++)
                delete points[i];
            points.clear();
        }
    }
}

