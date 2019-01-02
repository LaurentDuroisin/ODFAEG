#include "../../../include/odfaeg/Graphics/vertexArray.h"
#include "../../../include/odfaeg/Graphics/renderTarget.h"
#include "../../../include/odfaeg/Graphics/entity.h"
#include "glCheck.h"
#include <string.h>
namespace odfaeg {
    namespace graphic {
        using namespace sf;
        ////////////////////////////////////////////////////////////
        VertexArray::VertexArray(PrimitiveType type, unsigned int vertexCount, Entity* entity) :
        m_vertices     (vertexCount),
        m_primitiveType(type),
        m_entity(entity)
        {
             std::cout<<"set vertex array entity adr : "<<entity<<std::endl;
        }
        ///////////////////////////////////////////////////////////
        unsigned int VertexArray::getVertexCount() const
        {
            return static_cast<unsigned int>(m_vertices.size());
        }
        Entity* VertexArray::getEntity() {
            return m_entity;
        }
        void VertexArray::setEntity(Entity* entity) {
            m_entity = entity;
            std::cout<<"set vertex array entity adr : "<<entity<<std::endl;
        }
        ////////////////////////////////////////////////////////////
        Vertex& VertexArray::operator [](unsigned int index)
        {
            return m_vertices[index];
        }


        ////////////////////////////////////////////////////////////
        const Vertex& VertexArray::operator [](unsigned int index) const
        {
            return m_vertices[index];
        }


        ////////////////////////////////////////////////////////////
        void VertexArray::clear()
        {
            m_vertices.clear();
        }


        ////////////////////////////////////////////////////////////
        void VertexArray::resize(unsigned int vertexCount)
        {
            m_vertices.resize(vertexCount);
        }


        ////////////////////////////////////////////////////////////
        void VertexArray::append(const Vertex& vertex)
        {
            m_vertices.push_back(vertex);
        }
        ////////////////////////////////////////////////////////////
        void VertexArray::setPrimitiveType(PrimitiveType type)
        {
            m_primitiveType = type;
        }

        ////////////////////////////////////////////////////////////
        PrimitiveType VertexArray::getPrimitiveType() const
        {
            return m_primitiveType;
        }

        bool VertexArray::operator== (VertexArray &other) const {
            if (getVertexCount() != other.getVertexCount())
                return false;
            if (m_primitiveType != other.m_primitiveType)
                return false;
            for (unsigned int i = 0; i < getVertexCount(); i++)
                if (!(m_vertices[i] == other.m_vertices[i]))
                    return false;
            return true;
        }
        ////////////////////////////////////////////////////////////
        void VertexArray::draw(RenderTarget& target, RenderStates states)
        {
            if (!m_vertices.empty()) {
                target.draw(&m_vertices[0], m_vertices.size(), m_primitiveType, states);
            }
        }
        ////////////////////////////////////////////////////////////
        physic::BoundingBox VertexArray::getBounds()
        {
            if (!m_vertices.empty())
            {
                float left   = m_vertices[0].position.x;
                float top    = m_vertices[0].position.y;
                float right  = m_vertices[0].position.x;
                float bottom = m_vertices[0].position.y;
                float near = m_vertices[0].position.z;
                float far = m_vertices[0].position.z;

                for (std::size_t i = 1; i < m_vertices.size(); ++i)
                {
                    Vector3f position = m_vertices[i].position;

                    // Update left and right
                    if (position.x < left)
                        left = position.x;
                    else if (position.x > right)
                        right = position.x;

                    // Update top and bottom
                    if (position.y < top)
                        top = position.y;
                    else if (position.y > bottom)
                        bottom = position.y;

                    //Update the near and the far.

                    if (position.z < far)
                        far = position.z;
                    else if (position.z > near)
                        near = position.z;

                }

                return physic::BoundingBox(left, top, near, right - left, bottom - top, far - near);
            }
            else
            {
                // Array is empty
                return physic::BoundingBox();
            }
        }
    }
} // namespace sf3



