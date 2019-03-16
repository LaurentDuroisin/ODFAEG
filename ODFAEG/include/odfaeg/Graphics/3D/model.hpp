#ifndef ODFAEG_3D_MODEL_HPP
#define ODFAEG_3D_MODEL_HPP
#include "../mesh.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
namespace odfaeg {
    namespace graphic {
        namespace g3d {
            class Model : public Entity {
                public :
                    Model (std::string path, math::Vec3f position);
                    bool operator==(Entity& other);
                    bool isAnimated() const;
                    bool isModel() const;
                    bool selectable() const;
                    bool isLight() const;
                    bool isShadow() const;
                    bool isLeaf() const;
                private :
                    void loadModel(std::string path);
                    void processNode(aiNode *node, const aiScene *scene);
                    Entity* processMesh(aiMesh *mesh, const aiScene *scene);
                    std::vector<const Texture*> loadMaterialTextures(aiMaterial *mat, aiTextureType type,
                                             std::string typeName);
                    core::TextureManager<> tm;
                    math::Vec3f max, min;
                    std::string directory;
            };
        }
    }
}
#endif
