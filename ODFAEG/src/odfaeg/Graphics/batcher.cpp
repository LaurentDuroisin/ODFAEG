#include "../../../include/odfaeg/Graphics/batcher.h"
/**
  *\namespace odfaeg
  * the namespace of the Opensource Development Framework Adapted for Every Games.
  */
namespace odfaeg {
    namespace graphic {
            float Material::maxSpecularIntensity = 0.f;
            float Material::maxSpecularPower = 0.f;
            Material::TextureInfo::TextureInfo() {
                texture = nullptr;
                texId = "";
                rect = sf::IntRect(0, 0, 0, 0);
            }
            Material::TextureInfo::TextureInfo (const Texture* texture, sf::IntRect rect, std::string texId) {
                this->texture = texture;
                this->rect = rect;
                this->texId = texId;
            }
            void Material::TextureInfo::setTexId(std::string texId) {
                this->texId = texId;
            }
            std::string Material::TextureInfo::getTexId() const {
                return texId;
            }
            bool Material::TextureInfo::operator== (TextureInfo& info) {
                return texture == info.texture;
            }
            bool Material::TextureInfo::operator!= (TextureInfo& info) {
                return texture != info.texture;
            }
            const Texture* Material::TextureInfo::getTexture() const {
                return texture;
            }
            sf::IntRect Material::TextureInfo::getTexRect() const {
                return rect;
            }
            Material::Material() {
                color = sf::Color::White;
                specularIntensity = 0;
                specularPower = 0;
                refractionFactor = 0;
                bumpTexture = nullptr;
            }
            float Material::getMaxSpecularIntensity() {
                return maxSpecularIntensity;
            }
            float Material::getMaxSpecularPower() {
                return maxSpecularPower;
            }
            void Material::setMaxSpecularIntensity(float maxSpecularIntensity) {
                Material::maxSpecularIntensity = maxSpecularIntensity;
            }
            void Material::setMaxSpecularPower(float maxSpecularPower) {
                Material::maxSpecularPower = maxSpecularPower;
            }
            float Material::getSpecularIntensity() {
                return specularIntensity;
            }
            float Material::getSpecularPower() {
                return specularPower;
            }
            void Material::setSpecularIntensity(float specularIntensity) {
                if (specularIntensity > maxSpecularIntensity)
                    maxSpecularIntensity = specularIntensity;
                this->specularIntensity = specularIntensity;
            }
            void Material::setSpecularPower(float specularPower) {
                if (specularPower > maxSpecularPower)
                    maxSpecularPower = specularPower;
                this->specularPower = specularPower;
            }
            void Material::setBumpTexture(const Texture* bumpTexture) {
                this->bumpTexture = bumpTexture;
            }
            const Texture* Material::getBumpTexture() {
                return bumpTexture;
            }
            void Material::setRefractionFactor(float refractionFactor) {
                this->refractionFactor = refractionFactor;
            }
            float Material::getRefractionFactor() {
                return refractionFactor;
            }
            int Material::getNbTextures () {
                return texInfos.size();
            }
            void Material::addTexture (const Texture* texture, sf::IntRect rect) {
                texInfos.push_back(new TextureInfo(texture, rect));
            }
            sf::IntRect Material::getTexRect(int texUnit) const {
                return (texInfos.size() > 0) ? texInfos[texUnit]->getTexRect() : sf::IntRect(0, 0, 0, 0);
            }
            void Material::clearTextures() {
                texInfos.clear();
            }
            const Texture* Material::getTexture(int texUnit) {
                return (texInfos.size() > 0) ? texInfos[texUnit]->getTexture() : nullptr;
            }
            std::string Material::getTexId(int texUnit) {
                return (texInfos.size() > 0) ? texInfos[texUnit]->getTexId() : "";
            }
            void Material::setTexId(std::string texId, int texUnit) {
                if (texInfos.size() > 0) {
                    texInfos[texUnit]->setTexId(texId);
                }
            }
            bool Material::useSameTextures (const Material& material) {
                if (texInfos.size() != material.texInfos.size())
                    return false;
                for (unsigned int i = 0; i < texInfos.size(); i++) {
                    if (*texInfos[i] != *material.texInfos[i])
                        return false;
                }
                return true;

            }
            bool Material::hasSameColor (const Material& material) {
                return color == material.color;
            }
            bool Material::operator== (const Material& material) {
                return useSameTextures(material) && hasSameColor(material)
                       && specularIntensity == material.specularIntensity
                       && specularPower == material.specularPower
                       && bumpTexture == material.bumpTexture
                       && refractionFactor == material.refractionFactor;
            }
            bool Material::operator!= (Material& material) {
                return !useSameTextures(material) || !hasSameColor(material);
            }

            Face::Face() {
                transform = nullptr;
            }
            Face::Face(sf::PrimitiveType primType, TransformMatrix& tm) : transform(&tm) {
                m_vertices.setPrimitiveType(primType);
            }
            Face::Face(VertexArray va, Material mat, TransformMatrix& tm) : transform(&tm) {
                m_vertices = va;
                m_material = mat;
            }
            TransformMatrix& Face::getTransformMatrix() const {
                return *transform;
            }
            void Face::append(Vertex vertex, unsigned int indice) {
                m_vertices.append(vertex);
                m_vertices.addIndex(indice);
            }
            Material& Face::getMaterial() {
                return m_material;
            }
            VertexArray& Face::getVertexArray() {
                return m_vertices;
            }
            bool Face::useSameMaterial(Face& other) {
                return m_material == other.m_material;

            }
            bool Face::useSamePrimType (Face &other) {
                return m_vertices.getPrimitiveType() == other.m_vertices.getPrimitiveType();
            }
            Instance::Instance (Material& material, sf::PrimitiveType pType) : material(material), m_vertices(pType) {
                primType = pType;
                numInstances = 0;
            }
            void Instance::addVertexArray(VertexArray *va, TransformMatrix& tm, unsigned int baseVertex, unsigned int baseIndex) {
                if (Shader::getShadingLanguageVersionMajor() >= 4 && Shader::getShadingLanguageVersionMinor() >= 2
                    || Shader::isUsingOpenCL()) {
                    for (unsigned int i = 0; i < va->getVertexCount(); i++) {
                        m_vertices.append((*va)[i]);
                    }
                    for (unsigned int i = 0; i < va->getIndexes().size(); i++) {
                        m_vertices.addIndex(va->getIndexes()[i]);
                    }
                    unsigned int numIndexes = va->getIndexes().size();
                    m_vertices.addInstancedRenderingInfos(numIndexes,baseVertex,baseIndex);
                    numInstances++;
                } else {
                    unsigned int numIndexes = m_vertices.getIndexes().size();
                    VertexArray vat = *va;
                    vat.transform(tm);
                    for (unsigned int i = 0; i < vat.getVertexCount(); i++) {
                        m_vertices.append(vat[i]);
                    }
                    for (unsigned int i = 0; i < vat.getIndexes().size(); i++) {
                        m_vertices.addIndex(numIndexes + vat.getIndexes()[i]);
                    }
                }
                m_transforms.push_back(std::ref(tm));
                m_vertexArrays.push_back(va);
            }
            std::vector<VertexArray*> Instance::getVertexArrays() {
                return m_vertexArrays;
            }
            void Instance::clear() {
                m_transforms.clear();
                m_vertices.clear();
                m_vertexArrays.clear();
            }
            std::vector<std::reference_wrapper<TransformMatrix>> Instance::getTransforms() {
                 return m_transforms;
            }
            VertexArray& Instance::getVertexArray() {
                return m_vertices;
            }
            Material& Instance::getMaterial() {
                return material;
            }
            sf::PrimitiveType Instance::getPrimitiveType() {
                return primType;
            }
            unsigned int Instance::getNumInstances() {
                return numInstances;
            }
            Instance::~Instance() {
                m_vertices.clear();
                m_transforms.clear();
                m_vertexArrays.clear();
            }

            Batcher::Batcher() {
                numVertices = 0;
                numIndexes = 0;
            }
            void Batcher::addFace(Face* face) {
                bool added = false;
                for (unsigned int i = 0; i < instances.size() && !added; i++) {
                    if (instances[i]->getMaterial() == face->getMaterial()
                        && instances[i]->getPrimitiveType() == face->getVertexArray().getPrimitiveType()) {
                            added = true;
                            unsigned int baseVertex = instances[i]->getVertexArray().getVertexCount();
                            unsigned int baseIndex = instances[i]->getVertexArray().getIndexes().size();
                            instances[i]->addVertexArray(&face->getVertexArray(),face->getTransformMatrix(), baseVertex, baseIndex);
                    }
                }
                if (!added) {
                    Instance* instance = new Instance(face->getMaterial(), face->getVertexArray().getPrimitiveType());
                    instance->addVertexArray(&face->getVertexArray(),face->getTransformMatrix(), 0, 0);
                    instances.push_back(instance);
                }
                numIndexes += face->getVertexArray().getIndexes().size();
                numVertices += face->getVertexArray().getVertexCount();
            }
            std::vector<Instance*> Batcher::getInstances() {
                return instances;
            }
            unsigned int Batcher::getNumIndexes() {
                return numIndexes;
            }
            unsigned int Batcher::getNumVertices() {
                return numVertices;
            }
            void Batcher::clear() {
                numVertices = 0;
                numIndexes = 0;
                for (unsigned int i = 0; i < instances.size(); i++) {
                    delete instances[i];
                }
                instances.clear();
            }
    }
}

