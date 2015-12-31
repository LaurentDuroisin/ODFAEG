#include "../../../include/odfaeg/Graphics/tile.h"
namespace odfaeg {
    namespace graphic {

        /*Gère les différents tiles, cette classe hérite de Sprite.
        */
        //Crée un tile, avec une image passée en paramètre.

        Tile::Tile() : Entity (math::Vec3f(0, 0, 0), math::Vec3f(0, 0, 0),math::Vec3f(0, 0, 0),"E_TILE") {

        }
        Tile::Tile (const Tile& tile) : Entity (tile.getPosition(), tile.getSize(), tile.getSize() * 0.5f, "E_TILE"){
            VertexArray va(sf::Quads);
            Vertex v1(sf::Vector3f(0, 0, 0));
            Vertex v2(sf::Vector3f(tile.getSize().x, 0, 0));
            Vertex v3(sf::Vector3f(tile.getSize().x, tile.getSize().y, 0));
            Vertex v4(sf::Vector3f(0, tile.getSize().y, 0));
            sf::IntRect subRect = tile.getFaces()[0]->getMaterial().getTexRect();
            v1.texCoords = sf::Vector2f(subRect.left, subRect.top);
            v2.texCoords = sf::Vector2f(subRect.left + subRect.width, subRect.top);
            v3.texCoords = sf::Vector2f(subRect.left + subRect.width, subRect.top + subRect.height);
            v4.texCoords = sf::Vector2f(subRect.left, subRect.top + subRect.height);
            va.append(v1);
            va.append(v2);
            va.append(v3);
            va.append(v4);
            Material material;
            material.addTexture(tile.getFaces()[0]->getMaterial().getTexture(), subRect);
            material.setTexId(tile.getFaces()[0]->getMaterial().getTexId());
            Face* face = new Face(va, material,getTransform());
            addFace(face);
            parent = tile.parent;
        }
        Tile* Tile::clone() {
            return new Tile(getFaces()[0]->getMaterial().getTexture(), getPosition(), getSize(), getFaces()[0]->getMaterial().getTexRect(),getFaces()[0]->getVertexArray()[0].color,getParent());
        }
        Tile::Tile (const Texture *image, math::Vec3f position, math::Vec3f size, sf::IntRect subRect, sf::Color color, Entity *parent)
        : Entity (position, size, size * 0.5f, "E_TILE") {
            VertexArray va(sf::Quads);
            Vertex v1(sf::Vector3f(0, 0, 0));
            Vertex v2(sf::Vector3f(size.x, 0, 0));
            Vertex v3(sf::Vector3f(size.x, size.y, 0));
            Vertex v4(sf::Vector3f(0, size.y, 0));
            v1.texCoords = sf::Vector2f(subRect.left, subRect.top);
            v2.texCoords = sf::Vector2f(subRect.left + subRect.width, subRect.top);
            v3.texCoords = sf::Vector2f(subRect.left + subRect.width, subRect.top + subRect.height);
            v4.texCoords = sf::Vector2f(subRect.left, subRect.top + subRect.height);
            v1.color = v2.color = v3.color = v4.color = color;
            va.append(v1);
            va.append(v2);
            va.append(v3);
            va.append(v4);
            Material material;
            material.addTexture(image, subRect);
            Face* face = new Face(va, material,getTransform());
            addFace(face);
        }
        void Tile::changeVerticesHeights(float h1, float h2, float h3, float h4) {
            getFaces()[0]->getVertexArray()[0].position.z = h1;
            getFaces()[0]->getVertexArray()[1].position.z = h2;
            getFaces()[0]->getVertexArray()[2].position.z = h3;
            getFaces()[0]->getVertexArray()[3].position.z = h4;
            float min, max;
            if (h1 < h2 && h1 < h3 && h1 < h4)
                min = h1;
            else if (h2 < h1 && h2 < h3 && h2 < h4)
                min = h2;
            else if (h3 < h1 && h3 < h2 && h3 < h4)
                min = h3;
            else
                min = h4;
            if (h1 > h2 && h1 > h3 && h1 > h4)
                max = h1;
            else if (h2 > h1 && h2 > h3 && h2 > h4)
                max = h2;
            else if (h3 > h1 && h3 > h2 && h3 > h4)
                max = h3;
            else
                max = h4;
            setPosition(math::Vec3f(getPosition().x, getPosition().y, 0));
            setSize(math::Vec3f(getSize().x, getSize().y, max - min));
        }
        bool Tile::operator== (Entity &other) {
            if (other.getType() != "E_TILE")
                return false;
            Tile &tile = static_cast<Tile&> (other);
            return getFaces()[0]->getMaterial() == other.getFaces()[0]->getMaterial() &&
                   getPosition().x == tile.getPosition().x &&
                   getPosition().y == tile.getPosition().y &&
                   getPosition().z == tile.getPosition().z &&
                   getSize().x == tile.getSize().x &&
                   getSize().y == tile.getSize().y &&
                   getFaces()[0]->getMaterial().getTexRect().left == tile.getFaces()[0]->getMaterial().getTexRect().left &&
                   getFaces()[0]->getMaterial().getTexRect().top == tile.getFaces()[0]->getMaterial().getTexRect().top &&
                   getFaces()[0]->getMaterial().getTexRect().height == tile.getFaces()[0]->getMaterial().getTexRect().height &&
                   getFaces()[0]->getMaterial().getTexRect().width == tile.getFaces()[0]->getMaterial().getTexRect().width;
        }

        bool Tile::operator!= (Tile &tile) {
            return !(*this == tile);
        }

        void Tile::onDraw(RenderTarget &target, RenderStates states) {
            states.texture = const_cast<Tile*>(this)->getFaces()[0]->getMaterial().getTexture();
            target.draw(const_cast<Tile*>(this)->getFaces()[0]->getVertexArray(), states);
        }

        void Tile::setColor (sf::Color color) {
            for (unsigned int j = 0; j < getFaces()[0]->getVertexArray().getVertexCount(); j++)
                getFaces()[0]->getVertexArray()[j].color = color;
        }
    }
}

