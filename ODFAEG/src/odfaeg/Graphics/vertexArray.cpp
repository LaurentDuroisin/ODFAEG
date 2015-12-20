#include "../../../include/odfaeg/Graphics/vertexArray.h"
#include "../../../include/odfaeg/Graphics/renderTarget.h"

#include "glCheck.h"
#include <string.h>
namespace odfaeg {
    namespace graphic {
        using namespace sf;
        ////////////////////////////////////////////////////////////
        VertexArray::VertexArray() :
        m_vertices     (),
        m_primitiveType(Points)
        {

            vboVertexBuffer = 0;
            vboNormalBuffer = 0;
            vboIndexBuffer = 0;
            needToUpdateVBOBuffer = false;
            nbVerticesPerFace = 4;
            loop = true;
        }
        VertexArray::VertexArray(const VertexArray& va) {
            vboVertexBuffer = 0;
            vboNormalBuffer = 0;
            vboIndexBuffer = 0;
            m_normals = va.m_normals;
            m_locals = va.m_locals;
            m_vertices = va.m_vertices;
            m_primitiveType = va.m_primitiveType;
            oldVerticesSize = va.oldVerticesSize;
            m_numIndexes = va.m_numIndexes;
            m_baseIndexes = va.m_baseIndexes;
            m_baseVertices = va.m_baseVertices;
            m_indexes = va.m_indexes;
            loop = va.loop;
            m_vPosX = va.m_vPosX;
            m_vPosY = va.m_vPosY;
            m_vPosZ = va.m_vPosZ;
            m_vPosW = va.m_vPosW;
            m_vcRed = va.m_vcRed;
            m_vcGreen = va.m_vcGreen;
            m_vcBlue = va.m_vcBlue;
            m_vcAlpha = va.m_vcAlpha;
            m_ctX = va.m_ctX;
            m_ctY = va.m_ctY;
            nbVerticesPerFace = va.nbVerticesPerFace;
            needToUpdateVBOBuffer = true;
        }
        VertexArray::VertexArray(const VertexArray&& va) {
            vboVertexBuffer = 0;
            vboNormalBuffer = 0;
            vboIndexBuffer = 0;
            m_normals = va.m_normals;
            m_locals = va.m_locals;
            m_vertices = va.m_vertices;
            m_primitiveType = va.m_primitiveType;
            oldVerticesSize = va.oldVerticesSize;
            m_numIndexes = va.m_numIndexes;
            m_baseIndexes = va.m_baseIndexes;
            m_baseVertices = va.m_baseVertices;
            m_indexes = va.m_indexes;
            loop = va.loop;
            m_vPosX = va.m_vPosX;
            m_vPosY = va.m_vPosY;
            m_vPosZ = va.m_vPosZ;
            m_vPosW = va.m_vPosW;
            m_vcRed = va.m_vcRed;
            m_vcGreen = va.m_vcGreen;
            m_vcBlue = va.m_vcBlue;
            m_vcAlpha = va.m_vcAlpha;
            m_ctX = va.m_ctX;
            m_ctY = va.m_ctY;
            nbVerticesPerFace = va.nbVerticesPerFace;
            needToUpdateVBOBuffer = true;
        }
        VertexArray& VertexArray::operator= (const VertexArray& va) {
            vboVertexBuffer = 0;
            vboNormalBuffer = 0;
            vboIndexBuffer = 0;
            m_normals = va.m_normals;
            m_locals = va.m_locals;
            m_vertices = va.m_vertices;
            m_primitiveType = va.m_primitiveType;
            oldVerticesSize = va.oldVerticesSize;
            m_numIndexes = va.m_numIndexes;
            m_baseIndexes = va.m_baseIndexes;
            m_baseVertices = va.m_baseVertices;
            m_indexes = va.m_indexes;
            loop = va.loop;
            m_vPosX = va.m_vPosX;
            m_vPosY = va.m_vPosY;
            m_vPosZ = va.m_vPosZ;
            m_vPosW = va.m_vPosW;
            m_vcRed = va.m_vcRed;
            m_vcGreen = va.m_vcGreen;
            m_vcBlue = va.m_vcBlue;
            m_vcAlpha = va.m_vcAlpha;
            m_ctX = va.m_ctX;
            m_ctY = va.m_ctY;
            nbVerticesPerFace = va.nbVerticesPerFace;
            needToUpdateVBOBuffer = true;
            return *this;
        }
        VertexArray& VertexArray::operator= (const VertexArray&& va) {
            vboVertexBuffer = 0;
            vboNormalBuffer = 0;
            vboIndexBuffer = 0;
            m_normals = va.m_normals;
            m_locals = va.m_locals;
            m_vertices = va.m_vertices;
            m_primitiveType = va.m_primitiveType;
            oldVerticesSize = va.oldVerticesSize;
            m_numIndexes = va.m_numIndexes;
            m_baseIndexes = va.m_baseIndexes;
            m_baseVertices = va.m_baseVertices;
            m_indexes = va.m_indexes;
            loop = va.loop;
            m_vPosX = va.m_vPosX;
            m_vPosY = va.m_vPosY;
            m_vPosZ = va.m_vPosZ;
            m_vPosW = va.m_vPosW;
            m_vcRed = va.m_vcRed;
            m_vcGreen = va.m_vcGreen;
            m_vcBlue = va.m_vcBlue;
            m_vcAlpha = va.m_vcAlpha;
            m_ctX = va.m_ctX;
            m_ctY = va.m_ctY;
            nbVerticesPerFace = va.nbVerticesPerFace;
            needToUpdateVBOBuffer = true;
            return *this;
        }
        bool VertexArray::isLoop() {
            return loop;
        }
        void VertexArray::transform(TransformMatrix tm) {
            for (unsigned int i = 0; i < m_vertices.size(); i++) {
                if (m_locals.size() < m_vertices.size())
                    m_locals.push_back(math::Vec3f(m_vertices[i].position.x, m_vertices[i].position.y,m_vertices[i].position.z));
                m_vertices[i].position = tm.transform(m_locals[i]);
            }
            needToUpdateVBOBuffer = true;
        }
        ////////////////////////////////////////////////////////////
        VertexArray::VertexArray(PrimitiveType type, unsigned int vertexCount) :
        m_vertices     (vertexCount),
        m_primitiveType(type)
        {
            vboVertexBuffer = 0;
            vboNormalBuffer = 0;
            vboIndexBuffer = 0;
            needToUpdateVBOBuffer = true;
            loop = true;
            nbVerticesPerFace = 4;
        }
        void VertexArray::addInstancedRenderingInfos(unsigned int numIndexes, unsigned int baseVertex, unsigned int baseIndex) {
            m_numIndexes.push_back(numIndexes);
            m_baseVertices.push_back(baseVertex);
            m_baseIndexes.push_back(baseIndex);
        }
        void VertexArray::addIndex(unsigned int index) {
            m_indexes.push_back(index);
            if (!needToUpdateVBOBuffer)
                needToUpdateVBOBuffer = true;
        }
        std::vector<unsigned int> VertexArray::getBaseIndexes() {
            return m_baseIndexes;
        }
        std::vector<unsigned int> VertexArray::getIndexes() {
            return m_indexes;
        }
        void VertexArray::computeNormals() {
            int size = 0;
            if (m_primitiveType == sf::PrimitiveType::Quads)
                size = m_vertices.size() / 4;
            else
                size = m_vertices.size() / 3;
            for (int i = 0; i < size; i++) {
                if (m_primitiveType == sf::PrimitiveType::Quads) {
                    Vector3f v1 = m_vertices[i*4+1].position - m_vertices[i*4].position;
                    Vector3f v2 = m_vertices[i*4+2].position - m_vertices[i*4+1].position;
                    math::Vec3f v1f (v1.x, v1.y, v1.z);
                    math::Vec3f v2f (v2.x, v2.y, v2.z);
                    math::Vec3f n = v1f.cross(v2f).normalize();
                    m_normals.push_back(Vector3f(n.x, n.y, n.z));
                } else if (m_primitiveType == sf::PrimitiveType::Triangles) {
                    Vector3f v1 = m_vertices[i*3+1].position - m_vertices[i*3].position;
                    Vector3f v2 = m_vertices[i*3+2].position - m_vertices[i*3+1].position;
                    math::Vec3f v1f (v1.x, v1.y, v1.z);
                    math::Vec3f v2f (v2.x, v2.y, v2.z);
                    math::Vec3f n = v1f.cross(v2f).normalize();
                    m_normals.push_back(Vector3f(n.x, n.y, n.z));
                } else if (m_primitiveType == sf::PrimitiveType::TrianglesStrip) {
                    Vector3f v1 = m_vertices[i*1+1].position - m_vertices[i*1].position;
                    Vector3f v2 = m_vertices[i*1+2].position - m_vertices[i*1+1].position;
                    math::Vec3f v1f (v1.x, v1.y, v1.z);
                    math::Vec3f v2f (v2.x, v2.y, v2.z);
                    math::Vec3f n = v1f.cross(v2f).normalize();
                    m_normals.push_back(Vector3f(n.x, n.y, n.z));
                } else if (m_primitiveType == sf::PrimitiveType::TrianglesFan) {
                    Vector3f v1 = m_vertices[i+1].position - m_vertices[0].position;
                    Vector3f v2 = m_vertices[i+2].position - m_vertices[1].position;
                    math::Vec3f v1f (v1.x, v1.y, v1.z);
                    math::Vec3f v2f (v2.x, v2.y, v2.z);
                    math::Vec3f n = v1f.cross(v2f).normalize();
                    m_normals.push_back(Vector3f(n.x, n.y, n.z));
                }
            }
        }
        ///////////////////////////////////////////////////////////
        unsigned int VertexArray::getVertexCount() const
        {
            return static_cast<unsigned int>(m_vertices.size());
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
            m_indexes.clear();
            m_normals.clear();
            m_locals.clear();
        }


        ////////////////////////////////////////////////////////////
        void VertexArray::resize(unsigned int vertexCount)
        {
            m_vertices.resize(vertexCount);
        }


        ////////////////////////////////////////////////////////////
        void VertexArray::append(const Vertex& vertex)
        {
            /*bool contains = false;
            for (unsigned int i = 0; i < m_vertices.size(); i++) {
                if (m_vertices[i] == vertex)
                    contains = true;
            }*/
            //if (!contains)  {
                m_vertices.push_back(vertex);
                m_vPosX.push_back(vertex.position.x);
                m_vPosY.push_back(vertex.position.y);
                m_vPosZ.push_back(vertex.position.z);
                m_vPosW.push_back(1);
                m_vcRed.push_back(vertex.color.r);
                m_vcBlue.push_back(vertex.color.g);
                m_vcGreen.push_back(vertex.color.b);
                m_vcAlpha.push_back(vertex.color.a);
                m_ctX.push_back(vertex.texCoords.x);
                m_ctY.push_back(vertex.texCoords.y);
                m_locals.push_back(math::Vec3f(vertex.position.x, vertex.position.y, vertex.position.z));
                if (!needToUpdateVBOBuffer)
                    needToUpdateVBOBuffer = true;            //
        }
        math::Vec3f VertexArray::getLocal(unsigned int index) const {
            return m_locals[index];
        }
        void VertexArray::setLocal(unsigned int index, math::Vec3f v) {
            m_locals[index] = v;
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

                if (GLEW_ARB_vertex_buffer_object) {
                    if (needToUpdateVBOBuffer) {
                        if (vboVertexBuffer == 0) {
                            GLuint vbo;
                            glCheck(glGenBuffers(1, &vbo));
                            vboVertexBuffer = static_cast<unsigned int>(vbo);
                        }
                        if (oldVerticesSize != m_vertices.size()) {
                            glCheck(glBindBuffer(GL_ARRAY_BUFFER, vboVertexBuffer));
                            glCheck(glBufferData(GL_ARRAY_BUFFER_ARB, m_vertices.size() * sizeof(Vertex), &m_vertices[0], GL_DYNAMIC_DRAW));
                            glCheck(glBindBuffer(GL_ARRAY_BUFFER, 0));
                        } else {
                            GLvoid *pos_vbo = nullptr;
                            glCheck(glBindBuffer(GL_ARRAY_BUFFER, vboVertexBuffer));
                            pos_vbo = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
                            if (pos_vbo != nullptr) {
                                memcpy(pos_vbo,&m_vertices[0],  m_vertices.size() * sizeof(Vertex));
                                glCheck(glUnmapBuffer(GL_ARRAY_BUFFER));
                                pos_vbo = nullptr;
                            }
                            glCheck(glBindBuffer(GL_ARRAY_BUFFER, 0));
                        }
                        if (vboNormalBuffer == 0) {
                            GLuint vbo;
                            glCheck(glGenBuffers(1, &vbo));
                            vboNormalBuffer = static_cast<unsigned int>(vbo);
                        }
                        computeNormals();
                        if (oldVerticesSize != m_vertices.size()) {
                            glCheck(glBindBuffer(GL_ARRAY_BUFFER, vboNormalBuffer));
                            glCheck(glBufferData(GL_ARRAY_BUFFER_ARB, m_normals.size() * sizeof(Vector3f), &m_normals[0], GL_DYNAMIC_DRAW));
                            glCheck(glBindBuffer(GL_ARRAY_BUFFER, 0));
                        } else {
                            GLvoid *pos_vbo = nullptr;
                            glCheck(glBindBuffer(GL_ARRAY_BUFFER, vboNormalBuffer));
                            pos_vbo = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
                            if (pos_vbo != nullptr) {
                                memcpy(pos_vbo,&m_normals[0],  m_normals.size() * sizeof(Vector3f));
                                glCheck(glUnmapBuffer(GL_ARRAY_BUFFER));
                                pos_vbo = nullptr;
                            }
                            glCheck(glBindBuffer(GL_ARRAY_BUFFER, 0));
                        }
                        if (vboIndexBuffer == 0) {
                            GLuint vbo;
                            glCheck(glGenBuffers(1, &vbo));
                            vboIndexBuffer = static_cast<unsigned int>(vbo);
                        }
                        if (oldVerticesSize != m_vertices.size()) {
                            glCheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndexBuffer));
                            glCheck(glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexes.size() * sizeof(unsigned int),&m_indexes[0], GL_DYNAMIC_DRAW));
                            glCheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
                        } else {
                            GLvoid *pos_vbo = nullptr;
                            glCheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndexBuffer));
                            pos_vbo = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
                            if (pos_vbo != nullptr) {
                                memcpy(pos_vbo,&m_indexes[0],  m_indexes.size() * sizeof(unsigned int));
                                glCheck(glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER));
                                pos_vbo = nullptr;
                            }
                            glCheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
                        }
                        needToUpdateVBOBuffer = false;
                    }
                    states.vertexBufferId = vboVertexBuffer;
                    states.normalBufferId = vboNormalBuffer;
                    states.indexBufferId = vboIndexBuffer;
                }
                oldVerticesSize = m_vertices.size();
                target.draw(&m_vertices[0], m_vertices.size(), m_primitiveType, states, &m_normals[0], m_normals.size(),&m_indexes[0], m_indexes.size(),
                            &m_numIndexes[0], m_numIndexes.size(), &m_baseVertices[0], m_baseVertices.size(),&m_baseIndexes[0],m_baseIndexes.size());
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
        void VertexArray::updateVBOBuffer() {
            needToUpdateVBOBuffer = true;
        }
        void VertexArray::remove (unsigned int index) {
            std::vector<Vertex>::iterator it = m_vertices.begin();
            for (unsigned int i = 0; i < m_vertices.size(); i++) {
                if (i == index) {
                    it = m_vertices.erase(it);
                } else {
                    it++;
                }
                std::vector<unsigned int>::iterator it2 = m_indexes.begin();
                for (unsigned int j = 0; j < m_indexes.size(); j++) {
                    if (i == m_indexes[j]) {
                        it2 = m_indexes.erase(it2);
                    } else {
                        it2++;
                    }
                }
            }
        }
        VertexArray::~VertexArray() {
            if (VBO::isAvailable() && vboVertexBuffer != 0) {
                GLuint vbo = static_cast<GLuint>(vboVertexBuffer);
                glCheck(glDeleteBuffers(1, &vbo));
            }
            if (VBO::isAvailable() && vboNormalBuffer != 0) {
                GLuint vbo = static_cast<GLuint>(vboNormalBuffer);
                glCheck(glDeleteBuffers(1, &vbo));
            }
            if (VBO::isAvailable() && vboIndexBuffer != 0) {
                GLuint vbo = static_cast<GLuint>(vboIndexBuffer);
                glCheck(glDeleteBuffers(1, &vbo));
            }
        }
    }
} // namespace sf3



