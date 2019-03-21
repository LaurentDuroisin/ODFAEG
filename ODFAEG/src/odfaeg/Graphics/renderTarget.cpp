#include "../../../include/odfaeg/Graphics/renderTarget.h"
//#include "../../../include/odfaeg/Graphics/glExtensions.hpp"
#include <GL/glew.h>
#include <SFML/OpenGL.hpp>
#include "glCheck.h"
#include "../../../include/odfaeg/Graphics/drawable.h"

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
            case sf::BlendMode::Add:             return GL_FUNC_ADD;
            case sf::BlendMode::Subtract:        return GL_FUNC_SUBTRACT;
        }
    }
}
namespace odfaeg {
    namespace graphic {
        using namespace sf;
        ////////////////////////////////////////////////////////////
        RenderTarget::RenderTarget() :
        m_defaultView(), m_view(), m_cache()
        {
            m_cache.glStatesSet = false;
            m_vao = m_versionMajor = m_versionMinor = 0;
        }
        void RenderTarget::setVersionMajor (unsigned int versionMajor) {
            m_versionMajor = versionMajor;
        }
        void RenderTarget::setVersionMinor (unsigned int versionMinor) {
            m_versionMinor = versionMinor;
        }
        unsigned int RenderTarget::getVersionMajor() {
            return m_versionMajor;
        }
        unsigned int RenderTarget::getVersionMinor() {
            return m_versionMinor;
        }
        ////////////////////////////////////////////////////////////
        RenderTarget::~RenderTarget()
        {
            if (m_versionMajor >= 3 && m_versionMinor >= 3 && m_vao) {
                //glCheck(glDeleteVertexArrays(1, &m_vao));
            }
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
            vpm.setViewport(math::Vec3f(view.getViewport().getPosition().x, view.getViewport().getPosition().y, 0)
                                        ,math::Vec3f(view.getViewport().getWidth(), view.getViewport().getHeight(), 1));
            math::Vec3f coords = vpm.toNormalizedCoordinates(point);
            coords = view.getProjMatrix().unProject(coords);
            coords = coords.normalizeToVec3();
            coords = view.getViewMatrix().inverseTransform(coords);
            return coords;
        }

        math::Vec3f RenderTarget::mapCoordsToPixel(const math::Vec3f& point)
        {
            return mapCoordsToPixel(point, getView());
        }


        math::Vec3f RenderTarget::mapCoordsToPixel(const math::Vec3f& point, View& view) {
            ViewportMatrix vpm;
            vpm.setViewport(math::Vec3f(view.getViewport().getPosition().x, view.getViewport().getPosition().y, 0),
            math::Vec3f(view.getViewport().getWidth(), view.getViewport().getHeight(), 1));
            math::Vec3f coords = view.getViewMatrix().transform(point);
            coords = view.getProjMatrix().project(coords);
            coords = coords.normalizeToVec3();
            coords = vpm.toViewportCoordinates(coords);
            return coords;
        }
        ////////////////////////////////////////////////////////////
        void RenderTarget::draw(Drawable& drawable, RenderStates states)
        {
            drawable.draw(*this, states);
        }


        ////////////////////////////////////////////////////////////
        void RenderTarget::draw(const Vertex* vertices, unsigned int vertexCount,
                                PrimitiveType type, RenderStates states)
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
                if (useVertexCache /*&& !GL_ARB_vertex_buffer_object*/)
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
                    TransformMatrix tm = states.transform;
                    applyTransform(tm);
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
                if (useVertexCache /*&& !GL_ARB_vertex_buffer_object*/)
                {
                    // ... and if we already used it previously, we don't need to set the pointers again
                    if (!m_cache.useVertexCache)
                        vertices = m_cache.vertexCache;
                    else
                        vertices = nullptr;
                }
                // Setup the pointers to the vertices' components
                if (vertices) {
                    if (m_versionMajor >= 3 && m_versionMinor >= 3) {
                        /*glCheck(glBindBuffer(GL_ARRAY_BUFFER, states.vboVertexID));
                        glCheck(glEnableVertexAttribArray(0));
                        glCheck(glEnableVertexAttribArray(1));
                        glCheck(glEnableVertexAttribArray(2));
                        glCheck(glVertexAttribPointer(0, 3,GL_FLOAT,GL_FALSE,sizeof(Vertex), (GLvoid*) 0));
                        glCheck(glVertexAttribPointer(1, 4,GL_UNSIGNED_BYTE,GL_TRUE,sizeof(Vertex),(GLvoid*) 12));
                        glCheck(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*) 16));
                        glCheck(glDisableVertexAttribArray(0));
                        glCheck(glDisableVertexAttribArray(1));
                        glCheck(glDisableVertexAttribArray(2));
                        glCheck(glBindBuffer(GL_ARRAY_BUFFER, 0));*/
                    } /*else if (GL_ARB_vertex_buffer_object) {
                        glCheck(glBindBuffer(GL_ARRAY_BUFFER, states.vboVertexID));
                        glCheck(glEnableClientState(GL_COLOR_ARRAY));
                        glCheck(glEnableClientState(GL_TEXTURE_COORD_ARRAY));
                        glCheck(glEnableClientState(GL_VERTEX_ARRAY));
                        glCheck(glVertexPointer(3, GL_FLOAT, sizeof(Vertex), (GLvoid*) 0 ));
                        glCheck(glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex), (GLvoid*) 12));
                        glCheck(glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex),(GLvoid*) 16));
                        glCheck(glDisableClientState(GL_COLOR_ARRAY));
                        glCheck(glDisableClientState(GL_TEXTURE_COORD_ARRAY));
                        glCheck(glDisableClientState(GL_VERTEX_ARRAY));
                        glCheck(glBindBuffer(GL_ARRAY_BUFFER, 0));
                    }*/ else {
                        const char* data = reinterpret_cast<const char*>(vertices);
                        glCheck(glEnableClientState(GL_COLOR_ARRAY));
                        glCheck(glEnableClientState(GL_TEXTURE_COORD_ARRAY));
                        glCheck(glEnableClientState(GL_VERTEX_ARRAY));
                        glCheck(glVertexPointer(3, GL_FLOAT, sizeof(Vertex), data + 0 ));
                        glCheck(glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex), data + 12));
                        glCheck(glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), data + 16));
                        glCheck(glDisableClientState(GL_COLOR_ARRAY));
                        glCheck(glDisableClientState(GL_TEXTURE_COORD_ARRAY));
                        glCheck(glDisableClientState(GL_VERTEX_ARRAY));
                    }
                }
                if (m_versionMajor >= 3 && m_versionMinor >= 3) {
                    /*glCheck(glEnableVertexAttribArray(0));
                    glCheck(glEnableVertexAttribArray(1));
                    glCheck(glEnableVertexAttribArray(2));*/
                } else {
                    glCheck(glEnableClientState(GL_COLOR_ARRAY));
                    glCheck(glEnableClientState(GL_TEXTURE_COORD_ARRAY));
                    glCheck(glEnableClientState(GL_VERTEX_ARRAY));
                }
                // Find the OpenGL primitive type
                static const GLenum modes[] = {GL_POINTS, GL_LINES, GL_LINE_STRIP, GL_TRIANGLES,
                                                   GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, GL_QUADS};
                GLenum mode = modes[type];
                // Draw the primitives
                //glCheck(glBindFramebuffer(GL_FRAMEBUFFER, m_framebufferId));
                glCheck(glDrawArrays(mode, 0, vertexCount));
                //glCheck(glBindFramebuffer(GL_FRAMEBUFFER, 0));
                if (m_versionMajor >= 3 && m_versionMinor >= 3) {
                    /*glCheck(glDisableVertexAttribArray(0));
                    glCheck(glDisableVertexAttribArray(1));
                    glCheck(glDisableVertexAttribArray(2));*/
                } else {
                    // Draw the primitives
                    glCheck(glDisableClientState(GL_COLOR_ARRAY));
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
        void RenderTarget::drawVertexBuffer(VertexBuffer& vertexBuffer, RenderStates states) {
            if (vertexBuffer.getVertexCount() == 0) {
                return;
            }

            if (activate(true))
            {

                // First set the persistent OpenGL states if it's the very first call
                if (!m_cache.glStatesSet)
                    resetGLStates();
                // Apply the view
                if (m_cache.viewChanged || m_view.viewUpdated)
                    applyCurrentView();
                // Apply the blend mode
                if (states.blendMode != m_cache.lastBlendMode)
                    applyBlendMode(states.blendMode);
                TransformMatrix tm = states.transform;
                applyTransform(tm);
                // Apply the texture
                sf::Uint64 textureId = states.texture ? states.texture->m_cacheId : 0;
                /*if (textureId > 0)
                     std::cout<<"texture id : "<<textureId<<" "<<states.texture->m_texture<<std::endl;*/
                if (textureId != m_cache.lastTextureId)
                    applyTexture(states.texture);

                // Apply the shader
                if (states.shader)
                    applyShader(states.shader);
                if (m_cache.lastVboBuffer != &vertexBuffer) {
                    glCheck(glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer.vboVertexBuffer));
                    glCheck(glEnableClientState(GL_COLOR_ARRAY));
                    glCheck(glEnableClientState(GL_TEXTURE_COORD_ARRAY));
                    glCheck(glEnableClientState(GL_VERTEX_ARRAY));
                    glCheck(glVertexPointer(3, GL_FLOAT, sizeof(Vertex), (GLvoid*) 0 ));
                    glCheck(glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex), (GLvoid*) 12));
                    glCheck(glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex),(GLvoid*) 16));
                    glCheck(glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer.vboNormalBuffer));
                    glCheck(glEnableClientState(GL_NORMAL_ARRAY));
                    glCheck(glNormalPointer(GL_FLOAT,sizeof(sf::Vector3f),(GLvoid*) 0));
                    glCheck(glBindBuffer(GL_ARRAY_BUFFER, 0));
                    m_cache.lastVboBuffer = &vertexBuffer;
                } else {
                    glCheck(glEnableClientState(GL_COLOR_ARRAY));
                    glCheck(glEnableClientState(GL_TEXTURE_COORD_ARRAY));
                    glCheck(glEnableClientState(GL_VERTEX_ARRAY));
                    glCheck(glEnableClientState(GL_NORMAL_ARRAY));
                }
                // Find the OpenGL primitive type
                static const GLenum modes[] = {GL_POINTS, GL_LINES, GL_LINE_STRIP, GL_TRIANGLES,
                                                   GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, GL_QUADS};
                GLenum mode = modes[vertexBuffer.getPrimitiveType()];
                if (vertexBuffer.m_indexes.size() > 0) {
                    glCheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexBuffer.vboIndexBuffer));
                    glCheck(glDrawElements(mode, vertexBuffer.m_indexes.size(), GL_UNSIGNED_INT, (GLvoid*) 0));
                    glCheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
                } else {
                    //std::cout<<"draw arrays"<<std::endl;
                    //glCheck(glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer.vboVertexBuffer));
                    glCheck(glDrawArrays(mode, 0, vertexBuffer.getVertexCount()));
                    //glCheck(glBindBuffer(GL_ARRAY_BUFFER, 0));
                }
                glCheck(glDisableClientState(GL_COLOR_ARRAY));
                glCheck(glDisableClientState(GL_TEXTURE_COORD_ARRAY));
                glCheck(glDisableClientState(GL_VERTEX_ARRAY));
                glCheck(glDisableClientState(GL_NORMAL_ARRAY));
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

                #ifdef ODFAEG_DEBUG
                // make sure that the user didn't leave an unchecked OpenGL error
                GLenum error = glGetError();
                if (error != GL_NO_ERROR)
                {
                    err() << "OpenGL error (" << error << ") detected in user code, "
                          << "you should check for errors with glGetError()"
                          << std::endl;        }

                #endif
                glCheck(glDisable(GL_CULL_FACE));
                glCheck(glDisable(GL_LIGHTING));
                glCheck(glEnable(GL_DEPTH_TEST));
                glCheck(glEnable(GL_ALPHA_TEST));
                glCheck(glAlphaFunc(GL_GREATER, 0.f));
                glCheck(glDepthFunc(GL_GREATER));
                glCheck(glEnable(GL_TEXTURE_2D));
                glCheck(glEnable(GL_BLEND));
                glCheck(glClearDepth(0));
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
        void RenderTarget::initialize(unsigned int framebufferId)
        {
            m_framebufferId = framebufferId;
            if (m_versionMajor >= 3 && m_versionMinor >= 3) {
                /*GLuint vaoID;
                glCheck(glGenVertexArrays(1, &vaoID));
                m_vao = static_cast<unsigned int>(vaoID);
                glCheck(glBindVertexArray(m_vao));*/
            }
            // Setup the default and current views
            m_defaultView = View (static_cast<float>(getSize().x), static_cast<float>(getSize().y), -static_cast<float>(getSize().y) - 100, static_cast<float>(getSize().y)+100);
            m_defaultView.reset(physic::BoundingBox(0, 0, -static_cast<float>(getSize().y) - 100,static_cast<float>(getSize().x), static_cast<float>(getSize().y),static_cast<float>(getSize().y)+100));
            m_view = m_defaultView;

            // Set GL states only on first draw, so that we don't pollute user's states
            m_cache.glStatesSet = false;
            m_cache.lastVboBuffer = nullptr;
        }


        ////////////////////////////////////////////////////////////
        void RenderTarget::applyCurrentView()
        {
            // Set the viewport
            physic::BoundingBox viewport = getView().getViewport();
            glCheck(glViewport(viewport.getPosition().x, viewport.getPosition().y, viewport.getWidth(), viewport.getHeight()));
            // Set the projection matrix
            glCheck(glMatrixMode(GL_PROJECTION));
            //float* projMatrix = getView().getProjMatrix().getGlMatrix();
            glCheck(glLoadMatrixf(getView().getProjMatrix().getMatrix().transpose().toGlMatrix().data()));
            //delete projMatrix;
            //float* viewMatrix = getView().getViewMatrix().getGlMatrix();
            glCheck(glMultMatrixf(getView().getViewMatrix().getMatrix().transpose().toGlMatrix().data()));
            //delete viewMatrix;

            // Go back to model-view mode
            glCheck(glMatrixMode(GL_MODELVIEW));
        }
        ////////////////////////////////////////////////////////////
        void RenderTarget::applyBlendMode(const BlendMode& mode)
        {
            //priv::ensureExtensionsInit();
            // Apply the blend mode, falling back to the non-separate versions if necessary
            /*if (GLEXT_blend_func_separate)
            {
                glCheck(GLEXT_glBlendFuncSeparate(
                    factorToGlConstant(mode.colorSrcFactor), factorToGlConstant(mode.colorDstFactor),
                    factorToGlConstant(mode.alphaSrcFactor), factorToGlConstant(mode.alphaDstFactor)));
            }
            else
            {*/
                glCheck(glBlendFunc(
                    factorToGlConstant(mode.colorSrcFactor),
                    factorToGlConstant(mode.colorDstFactor)));
            //}

            /*if (GLEXT_blend_equation_separate)
            {
                glCheck(GLEXT_glBlendEquationSeparate(
                    equationToGlConstant(mode.colorEquation),
                    equationToGlConstant(mode.alphaEquation)));
            }
            else
            {*/
                //glCheck(glBlendEquation(equationToGlConstant(mode.colorEquation)));
            //}
            m_cache.lastBlendMode = mode;
        }
        ////////////////////////////////////////////////////////////
        void RenderTarget::applyTexture(const Texture* texture)
        {
            Texture::bind(texture, Texture::Pixels);
            m_cache.lastTextureId = texture ? texture->m_cacheId : 0;

        }
        void RenderTarget::applyTransform(TransformMatrix& tm) {
            glCheck(glLoadIdentity());
            //float* matrix = tm.getGlMatrix();
            glCheck(glMultMatrixf(tm.getMatrix().transpose().toGlMatrix().data()));
            //delete matrix;
        }
        ////////////////////////////////////////////////////////////
        void RenderTarget::applyShader(const Shader* shader)
        {
            Shader::bind(shader);
        }
    }
}

