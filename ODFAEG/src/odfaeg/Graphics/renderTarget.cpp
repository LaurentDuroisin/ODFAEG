#include "../../../include/odfaeg/Graphics/renderTarget.h"
#include "../../../extlibs/headers/GL/glew.h"
#include <SFML/OpenGL.hpp>
#include "glCheck.h"
#include "../../../include/odfaeg/Graphics/drawable.h"
#include "GLExtensions.hpp"
namespace
{
    // Convert an sf::BlendMode::Factor constant to the corresponding OpenGL constant.
    sf::Uint32 factorToGlConstant(sf::BlendMode::Factor blendFactor)
    {
        switch (blendFactor)
        {
            default:
            case sf::BlendMode::Zero:             return GL_ZERO;
            case sf::BlendMode::One:              return GL_ONE;
            case sf::BlendMode::SrcColor:         return GL_SRC_COLOR;
            case sf::BlendMode::OneMinusSrcColor: return GL_ONE_MINUS_SRC_COLOR;
            case sf::BlendMode::DstColor:         return GL_DST_COLOR;
            case sf::BlendMode::OneMinusDstColor: return GL_ONE_MINUS_DST_COLOR;
            case sf::BlendMode::SrcAlpha:         return GL_SRC_ALPHA;
            case sf::BlendMode::OneMinusSrcAlpha: return GL_ONE_MINUS_SRC_ALPHA;
            case sf::BlendMode::DstAlpha:         return GL_DST_ALPHA;
            case sf::BlendMode::OneMinusDstAlpha: return GL_ONE_MINUS_DST_ALPHA;
        }
    }


    // Convert an sf::BlendMode::BlendEquation constant to the corresponding OpenGL constant.
    sf::Uint32 equationToGlConstant(sf::BlendMode::Equation blendEquation)
    {
        switch (blendEquation)
        {
            default:
            case sf::BlendMode::Add:             return GLEXT_GL_FUNC_ADD;
            case sf::BlendMode::Subtract:        return GLEXT_GL_FUNC_SUBTRACT;
        }
    }
}
namespace odfaeg {
    namespace graphic {
        unsigned int RenderTarget::majorVersion = getMajorVersion();
        unsigned int RenderTarget::minorVersion = getMinorVersion();
        using namespace sf;
        ////////////////////////////////////////////////////////////
        RenderTarget::RenderTarget() :
        m_defaultView(), m_view(), m_cache()
        {
            m_cache.glStatesSet = false;
        }


        ////////////////////////////////////////////////////////////
        RenderTarget::~RenderTarget()
        {
        }


        ////////////////////////////////////////////////////////////
        void RenderTarget::clear(const Color& color)
        {

            if (activate(true))
            {
                glCheck(glClearColor(color.r / 255.f, color.g / 255.f, color.b / 255.f, color.a / 255.f));
                glCheck(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
            }
        }


        ////////////////////////////////////////////////////////////
        void RenderTarget::setView(View view)
        {
            m_view = view;
            m_cache.viewChanged = true;
        }


        ////////////////////////////////////////////////////////////
        View& RenderTarget::getView()
        {
            return m_view;
        }


        ////////////////////////////////////////////////////////////
        View RenderTarget::getDefaultView() const
        {
            return m_defaultView;
        }
        math::Vec3f RenderTarget::mapPixelToCoords(const math::Vec3f& point)
        {
            return mapPixelToCoords(point, getView());
        }


        math::Vec3f RenderTarget::mapPixelToCoords(const math::Vec3f& point, View& view)
        {
            ViewportMatrix vpm;
            vpm.setViewport(math::Vec3f(view.getViewport().getPosition().x, view.getViewport().getPosition().y, -view.getViewport().getDepth()), math::Vec3f(view.getViewport().getWidth(), view.getViewport().getHeight(), view.getViewport().getDepth()));
            math::Vec3f coords = vpm.toNormalizedCoordinates(point);
            coords = view.getProjMatrix().unProject(coords);
            coords /= math::Math::abs(coords.w);
            coords = view.getViewMatrix().inverseTransform(coords);
            return coords;
        }

        math::Vec3f RenderTarget::mapCoordsToPixel(const math::Vec3f& point)
        {
            return mapCoordsToPixel(point, getView());
        }


        math::Vec3f RenderTarget::mapCoordsToPixel(const math::Vec3f& point, View& view)
        {
            ViewportMatrix vpm;
            vpm.setViewport(math::Vec3f(view.getViewport().getPosition().x, view.getViewport().getPosition().y, -view.getDepth()),
            math::Vec3f(view.getViewport().getWidth(), view.getViewport().getHeight(), view.getDepth()));
            math::Vec3f coords = view.getViewMatrix().transform(point);
            coords = view.getProjMatrix().project(coords);
            float tmp = coords.y;
            coords.y = coords.z;
            coords.z = -tmp;
            coords = vpm.toViewportCoordinates(coords);
            coords /= math::Math::abs(coords.w);
            /*if (view.isXFlipped())
                coords.x = view.getViewport().getSize().x - coords.x;
            if (view.isYFlipped())
                coords.y = view.getViewport().getSize().y - coords.y;*/
            return coords;
           /*physic::BoundingBox frustum = view.getFrustum();
           math::Vec3f fCoords = view.getViewMatrix().transform(point);
           fCoords = fCoords - frustum.getPosition();
            std::cout<<"f coords : "<<fCoords<<std::endl;
           return fCoords;*/
        }

        ////////////////////////////////////////////////////////////
        void RenderTarget::draw(Drawable& drawable, RenderStates states)
        {
            drawable.draw(*this, states);
        }


        ////////////////////////////////////////////////////////////
        void RenderTarget::draw(const Vertex* vertices, unsigned int vertexCount,
                                PrimitiveType type, RenderStates states, const Vector3f* normals, unsigned int normalCount,
                                const unsigned int* indexes, const unsigned int indexesCount,
                                const unsigned int* numIndexes, unsigned int numIndexesCount, const unsigned int* baseVertices,
                                unsigned int baseVerticesCount, const unsigned int* baseIndexes, unsigned int baseIndexesCount)
        {

            // Nothing to draw?
            if (!vertices || (vertexCount == 0))
                return;

            if (activate(true))
            {

                // First set the persistent OpenGL states if it's the very first call
                if (!m_cache.glStatesSet)
                    resetGLStates();
                // Apply the view
                if (m_cache.viewChanged || m_view.viewUpdated)
                    applyCurrentView();
                // Check if the vertex count is low enough so that we can pre-transform them
                bool useVertexCache = (vertexCount <= StatesCache::VertexCacheSize);
                if (useVertexCache && !GLEW_ARB_vertex_buffer_object)
                {

                    // Pre-transform the vertices and store them into the vertex cache
                    for (unsigned int i = 0; i < vertexCount; ++i)
                    {

                        Vertex& vertex = m_cache.vertexCache[i];
                        math::Vec3f pos (vertices[i].position.x, vertices[i].position.y, vertices[i].position.z);
                        math::Vec3f finalpos = states.transform.transform(pos);

                        vertex.position = Vector3f(finalpos.x, finalpos.y, finalpos.z);
                        vertex.color = vertices[i].color;
                        vertex.texCoords = vertices[i].texCoords;
                    }
                    // Since vertices are transformed, we must use an identity transform to render them
                    states.transform.reset3D();
                    applyTransform(states.transform);
                }
                else
                {
                    applyTransform(states.transform);
                }
                // Apply the blend mode
                if (states.blendMode != m_cache.lastBlendMode)
                    applyBlendMode(states.blendMode);

                // Apply the texture
                sf::Uint64 textureId = states.texture ? states.texture->m_cacheId : 0;

                if (textureId != m_cache.lastTextureId)
                    applyTexture(states.texture);

                // Apply the shader
                if (states.shader)
                    applyShader(states.shader);

                // If we pre-transform the vertices, we must use our internal vertex cache
                if (useVertexCache && !GLEW_ARB_vertex_buffer_object)
                {
                    // ... and if we already used it previously, we don't need to set the pointers again
                    if (!m_cache.useVertexCache)
                        vertices = m_cache.vertexCache;
                    else
                        vertices = nullptr;
                }
                // Setup the pointers to the vertices' components
                if (vertices && GLEW_ARB_vertex_buffer_object) {
                    //In moddern opengl we need to use glVertexAttribPointer functions. (gl*Pointer is deprecated)
                    if (majorVersion >= 3 && minorVersion >= 3) {
                        glCheck(glBindBuffer(GL_ARRAY_BUFFER, states.vertexBufferId));
                        glCheck(glEnableVertexAttribArray(0));
                        glCheck(glVertexAttribPointer(0, 3,GL_FLOAT,GL_FALSE,sizeof(Vertex), (GLvoid*) 0));
                        glCheck(glEnableVertexAttribArray(1));
                        glCheck(glVertexAttribPointer(1, 4,GL_UNSIGNED_BYTE,GL_FALSE,sizeof(Vertex),(GLvoid*) 12));
                        glCheck(glEnableVertexAttribArray(2));
                        glCheck(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*) 16));
                        glCheck(glBindBuffer(GL_ARRAY_BUFFER, states.normalBufferId));
                        glCheck(glEnableVertexAttribArray(3));
                        glCheck(glVertexAttribPointer(3, 3,GL_FLOAT,GL_FALSE,sizeof(Vector3f), (GLvoid*) 0));
                        static const GLenum modes[] = {GL_POINTS, GL_LINES, GL_LINE_STRIP, GL_TRIANGLES,
                                                       GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, GL_QUADS};
                        GLenum mode = modes[type];
                        if (indexesCount == 0) {
                            glCheck(glDrawArrays(mode, 0, vertexCount));
                        } else if (indexesCount > 0 && (numIndexesCount == 0 || baseVerticesCount == 0 || baseIndexesCount == 0)) {

                            glCheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, states.indexBufferId));
                            glCheck(glDrawElements(mode, indexesCount,GL_UNSIGNED_INT,0));
                        } else {
                            // Find the OpenGL primitive type
                            glCheck(glBindBuffer(GL_ARRAY_BUFFER, states.mvpBufferId));
                            for (unsigned int i = 0; i < 4 ; i++) {
                                glCheck(glEnableVertexAttribArray(MVP_LOCATION + i));
                                glCheck(glVertexAttribPointer(MVP_LOCATION + i, 4, GL_FLOAT, GL_FALSE, sizeof(math::Matrix4f),
                                                    (const GLvoid*)(sizeof(GLfloat) * i * 4)));
                                glCheck(glVertexAttribDivisor(MVP_LOCATION+ i, 1));
                            }
                            // Draw the primitives
                            for (unsigned int i = 0; i < states.numInstances; i++) {

                                glCheck(glDrawElementsInstancedBaseVertex(mode,
                                                                          numIndexes[i],
                                                                          GL_UNSIGNED_INT,
                                                                          (void*)(sizeof(unsigned int) * baseIndexes[i]),
                                                                          states.numInstances,
                                                                          baseVertices[i]));
                            }
                            for (unsigned int i = 0; i < 4 ; i++) {
                                glCheck(glDisableVertexAttribArray(MVP_LOCATION + i));
                            }
                        }
                        glCheck(glDisableVertexAttribArray(3));
                        glCheck(glDisableVertexAttribArray(2));
                        glCheck(glDisableVertexAttribArray(1));
                        glCheck(glDisableVertexAttribArray(0));
                        glCheck(glBindBuffer(GL_ARRAY_BUFFER, 0));
                        states.vertexBufferId = 0;
                        states.normalBufferId = 0;
                        states.indexBufferId = 0;
                    } else {
                        glCheck(glBindBuffer(GL_ARRAY_BUFFER, states.vertexBufferId));
                        glCheck(glEnableClientState(GL_COLOR_ARRAY));
                        glCheck(glEnableClientState(GL_NORMAL_ARRAY));
                        glCheck(glEnableClientState(GL_TEXTURE_COORD_ARRAY));
                        glCheck(glEnableClientState(GL_VERTEX_ARRAY));
                        glCheck(glVertexPointer(3, GL_FLOAT, sizeof(Vertex), (GLvoid*) 0));
                        glCheck(glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex), (GLvoid*) 12));
                        glCheck(glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), (GLvoid*) 16));
                        glCheck(glBindBuffer(GL_ARRAY_BUFFER, states.normalBufferId));
                        glCheck(glNormalPointer(GL_FLOAT,sizeof(Vector3f), (GLvoid*) 0));
                        // Find the OpenGL primitive type
                        static const GLenum modes[] = {GL_POINTS, GL_LINES, GL_LINE_STRIP, GL_TRIANGLES,
                                                       GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, GL_QUADS};
                        GLenum mode = modes[type];
                        // Draw the primitives
                        if (indexes != nullptr) {
                            glCheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, states.indexBufferId));
                            glCheck(glDrawElements(mode,indexesCount,GL_UNSIGNED_INT,0));
                        } else {
                            glCheck(glDrawArrays(mode, 0, vertexCount));
                        }
                        glCheck(glDisableClientState(GL_COLOR_ARRAY));
                        glCheck(glDisableClientState(GL_NORMAL_ARRAY));
                        glCheck(glDisableClientState(GL_TEXTURE_COORD_ARRAY));
                        glCheck(glDisableClientState(GL_VERTEX_ARRAY));
                        glCheck(glBindBuffer(GL_ARRAY_BUFFER, 0));
                        states.vertexBufferId = 0;
                        states.normalBufferId = 0;
                    }
                } else if (vertices) {
                    glCheck(glEnableClientState(GL_COLOR_ARRAY));
                    glCheck(glEnableClientState(GL_NORMAL_ARRAY));
                    glCheck(glEnableClientState(GL_TEXTURE_COORD_ARRAY));
                    glCheck(glEnableClientState(GL_VERTEX_ARRAY));
                    const char* data = reinterpret_cast<const char*>(vertices);
                    const char* data2 = reinterpret_cast<const char*>(normals);
                    glCheck(glVertexPointer(3, GL_FLOAT, sizeof(Vertex), data + 0));
                    glCheck(glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex), data + 12));
                    glCheck(glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), data + 16));
                    glCheck(glNormalPointer(GL_FLOAT,sizeof(Vector3f), data2 + 0));
                    // Find the OpenGL primitive type
                    static const GLenum modes[] = {GL_POINTS, GL_LINES, GL_LINE_STRIP, GL_TRIANGLES,
                                                   GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, GL_QUADS};
                    GLenum mode = modes[type];
                    // Draw the primitives
                    glCheck(glDrawArrays(mode, 0, vertexCount));
                    glCheck(glDisableClientState(GL_COLOR_ARRAY));
                    glCheck(glDisableClientState(GL_NORMAL_ARRAY));
                    glCheck(glDisableClientState(GL_TEXTURE_COORD_ARRAY));
                    glCheck(glDisableClientState(GL_VERTEX_ARRAY));
                }
                // Unbind the shader, if any
                if (states.shader)
                    applyShader(nullptr);

                // Update the cache
                m_cache.useVertexCache = useVertexCache;
            }
        }


        ////////////////////////////////////////////////////////////
        void RenderTarget::pushGLStates()
        {
            if (activate(true))
            {
                glCheck(glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS));
                glCheck(glPushAttrib(GL_ALL_ATTRIB_BITS));
                glCheck(glMatrixMode(GL_MODELVIEW));
                glCheck(glPushMatrix());
                glCheck(glMatrixMode(GL_PROJECTION));
                glCheck(glPushMatrix());
                glCheck(glMatrixMode(GL_TEXTURE));
                glCheck(glPushMatrix());
            }

            resetGLStates();
        }


        ////////////////////////////////////////////////////////////
        void RenderTarget::popGLStates()
        {
            if (activate(true))
            {
                glCheck(glMatrixMode(GL_PROJECTION));
                glCheck(glPopMatrix());
                glCheck(glMatrixMode(GL_MODELVIEW));
                glCheck(glPopMatrix());
                glCheck(glMatrixMode(GL_TEXTURE));
                glCheck(glPopMatrix());
                glCheck(glPopClientAttrib());
                glCheck(glPopAttrib());
            }
        }


        ////////////////////////////////////////////////////////////
        void RenderTarget::resetGLStates()
        {
            if (activate(true))
            {

                #ifdef SFML_DEBUG
                // make sure that the user didn't leave an unchecked OpenGL error
                GLenum error = glGetError();
                if (error != GL_NO_ERROR)
                {
                    err() << "OpenGL error (" << error << ") detected in user code, "
                          << "you should check for errors with glGetError()"
                          << std::endl;        }

                #endif
                // Define the default OpenGL states
                glCheck(glDisable(GL_CULL_FACE));
                glCheck(glDisable(GL_LIGHTING));
                if (isUsingDepthTest()) {
                    glCheck(glEnable(GL_DEPTH_TEST));
                    glCheck(glEnable(GL_ALPHA_TEST));
                    glCheck(glAlphaFunc(GL_GREATER, 0.f));
                    glCheck(glDepthFunc(GL_LESS));
                } else {
                    glCheck(glDisable(GL_DEPTH_TEST));
                    glCheck(glDisable(GL_ALPHA_TEST));
                }
                glCheck(glEnable(GL_TEXTURE_2D));
                glCheck(glEnable(GL_BLEND));
                glCheck(glClearDepth(1));
                glCheck(glDepthMask(GL_TRUE));
                glCheck(glDisable(GL_SCISSOR_TEST));

                m_cache.glStatesSet = true;

                // Apply the default SFML states
                applyBlendMode(BlendAlpha);
                TransformMatrix tm;
                applyTransform(tm);
                applyTexture(nullptr);
                if (Shader::isAvailable())
                    applyShader(nullptr);
                m_cache.useVertexCache = false;

                // Set the default view
                setView(getView());
            }
        }


        ////////////////////////////////////////////////////////////
        void RenderTarget::initialize()
        {

            // Setup the default and current views
            m_defaultView = View (static_cast<float>(getSize().x), static_cast<float>(getSize().y),0, static_cast<float>(getSize().y)+100);
            m_defaultView.reset(physic::BoundingBox(0, 0, 0,static_cast<float>(getSize().x), static_cast<float>(getSize().y),static_cast<float>(getSize().y)+100));
            m_view = m_defaultView;

            // Set GL states only on first draw, so that we don't pollute user's states
            m_cache.glStatesSet = false;
        }


        ////////////////////////////////////////////////////////////
        void RenderTarget::applyCurrentView()
        {
            // Set the viewport
            physic::BoundingBox viewport = getView().getViewport();
            glCheck(glViewport(viewport.getPosition().x, viewport.getPosition().y, viewport.getWidth(), viewport.getHeight()));
            // Set the projection matrix
            glCheck(glMatrixMode(GL_PROJECTION));
            float* projMatrix = getView().getProjMatrix().getGlMatrix();
            glCheck(glLoadMatrixf(projMatrix));
            delete projMatrix;
            float* viewMatrix = getView().getViewMatrix().getGlMatrix();
            glCheck(glMultMatrixf(viewMatrix));
            delete viewMatrix;

            // Go back to model-view mode
            glCheck(glMatrixMode(GL_MODELVIEW));

        }
        ////////////////////////////////////////////////////////////
        void RenderTarget::applyBlendMode(const BlendMode& mode)
        {
            // Apply the blend mode, falling back to the non-separate versions if necessary
            if (GLEXT_blend_func_separate)
            {
                glCheck(GLEXT_glBlendFuncSeparate(
                    factorToGlConstant(mode.colorSrcFactor), factorToGlConstant(mode.colorDstFactor),
                    factorToGlConstant(mode.alphaSrcFactor), factorToGlConstant(mode.alphaDstFactor)));
            }
            else
            {
                glCheck(glBlendFunc(
                    factorToGlConstant(mode.colorSrcFactor),
                    factorToGlConstant(mode.colorDstFactor)));
            }

            if (GLEXT_blend_equation_separate)
            {
                glCheck(GLEXT_glBlendEquationSeparate(
                    equationToGlConstant(mode.colorEquation),
                    equationToGlConstant(mode.alphaEquation)));
            }
            else
            {
                glCheck(GLEXT_glBlendEquation(equationToGlConstant(mode.colorEquation)));
            }

            m_cache.lastBlendMode = mode;
        }


        ////////////////////////////////////////////////////////////
        void RenderTarget::applyTransform(TransformMatrix& transform)
        {
            // No need to call glMatrixMode(GL_MODELVIEW), it is always the
            // current mode (for optimization purpose, since it's the most used)
            glCheck(glLoadIdentity());
            glCheck(glMultMatrixf(transform.getGlMatrix()));
        }


        ////////////////////////////////////////////////////////////
        void RenderTarget::applyTexture(const Texture* texture)
        {

            Texture::bind(texture, Texture::Pixels);

            m_cache.lastTextureId = texture ? texture->m_cacheId : 0;

        }
        void RenderTarget::setMajorVersion(unsigned int version) {
             majorVersion = version;
        }
        void RenderTarget::setMinorVersion(unsigned int version) {
             minorVersion = version;
        }
        unsigned int RenderTarget::getMajorVersion() {
            const GLubyte* version = glGetString(GL_VERSION);
            if (version)
                return version[0] - '0';
            return 2;
        }
        unsigned int RenderTarget::getMinorVersion() {
            const GLubyte* version = glGetString(GL_VERSION);

            if (version) {
                return version[2] - '0';
            }
            return 0;
        }
        ////////////////////////////////////////////////////////////
        void RenderTarget::applyShader(const Shader* shader)
        {
            Shader::bind(shader);
        }
    }
}

