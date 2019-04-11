#include "../../../include/odfaeg/Graphics/mesh.hpp"
namespace odfaeg {
    namespace graphic {
        Mesh::Mesh() : Entity(math::Vec3f(0, 0, 0), math::Vec3f(0, 0, 0),math::Vec3f(0, 0, 0), "E_MESH") {}
        Mesh::Mesh(math::Vec3f position, math::Vec3f size, std::string type) : Entity(position, size, size*0.5f, type) {}
        bool Mesh::operator==(Entity& other) {
            Mesh *mesh = dynamic_cast<Mesh*>(&other);
            if (mesh == nullptr)
                return false;
            if (!(getPosition().x == other.getPosition().x &&
                   getPosition().y == other.getPosition().y &&
                   getPosition().z == other.getPosition().z &&
                   getSize().x == other.getSize().x &&
                   getSize().y == other.getSize().y))
                   return false;
            for (unsigned int i = 0; i < mesh->getNbFaces(); i++) {
               if(!(getFace(i)->getMaterial() == other.getFace(i)->getMaterial() &&
               getFace(i)->getMaterial().getTexRect().left == other.getFace(i)->getMaterial().getTexRect().left &&
               getFace(i)->getMaterial().getTexRect().top == other.getFace(i)->getMaterial().getTexRect().top &&
               getFace(i)->getMaterial().getTexRect().height == other.getFace(i)->getMaterial().getTexRect().height &&
               getFace(i)->getMaterial().getTexRect().width == other.getFace(i)->getMaterial().getTexRect().width))
                   return false;
            }
            return true;
        }
        bool Mesh::isAnimated() const {
            return false;
        }
        bool Mesh::isModel() const {
            return true;
        }
        bool Mesh::selectable() const {
            return true;
        }
        bool Mesh::isLight() const {
            return false;
        }
        bool Mesh::isShadow() const {
            return false;
        }
        bool Mesh::isLeaf() const {
            return true;
        }
        void Mesh::onDraw(RenderTarget& target, RenderStates states) {
            for (unsigned int i = 0; i < getFaces().size(); i++) {
                states.texture = getFaces()[i]->getMaterial().getTexture();
                target.draw(getFaces()[i]->getVertexArray(), states);
            }
        }
    }
}
