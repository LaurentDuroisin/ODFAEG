void RayTracingRenderComponent::drawNextFrame () {
    /*For all lights, we need to cast rays from the light source.
    (Because of alpha filtering, we need to cast rays from the light source and not from the camera)
    So we need to modify the whitted and light transport raytracing algorithms a bit. */
    for (unsigned int l = 0; l < m_lights.size(); l++) {
        //We need to set the camera at the light center, the size of the camera is the light radius.
        View lightView = View(m_lights[l]->getRadius);
        lightView.setCenter(centerLight);
        //We need to cast a ray for each pixel which are in the light's camera.
        for (unsigned int x = 0; x < lightView.getSize().x; x++) {
            for (unsigned int y = 0; y < lightView.getSize().y; y++) {
                Ray ray (m_lights[l]->getCenter(), math::Vec3f(x, y, m_lights[l]->getCenter().z + m_lights[i]->getRadius()));
                castRay(ray, m_lights[l], 1);
            }
        }
    }
}
bool RayTracingRenderComponent::trace (math::Ray ray, float& tnear, Vertex& vert1, Vertex& vert2, Vertex& vert3, Material& mat) {
     //Here we check the nearest triangle.
     bool hitSurface = false;
     for (unsigned int i = 0; i < m_newInstances.size(); i++) {
        for (unsigned int j = 0; j < m_newInstances.getVertexArrays().size(); j++) {
            TransformMatrix& tm = m_newInstances[i]->getTransforms()[j];
            if (m_newInstances[i]->getVertexArrays()[j]->getPrimitiveType() == sf::Quads) {
                for (unsigned int q = 0; q < m_newInstances[i]->getVertexArrays()[j]->getVertexCount() / 4; q++) {
                    for (unsigned int t = 0; t < 2; t++) {
                        Vertex v1;
                        Vertex v2;
                        Vertex v3;
                        if (t == 0) {
                            v1 = m_newInstances[i]->getVertexArrays()[j][q*4];
                            v2 = m_newInstances[i]->getVertexArrays()[j][q*4+1];
                            v3 = m_newInstances[i]->getVertexArrays()[j][q*4+3];
                        } else {
                            v1 = m_newInstances[i]->getVertexArrays()[j][q*4+1];
                            v2 = m_newInstances[i]->getVertexArrays()[j][q*4+2];
                            v3 = m_newInstances[i]->getVertexArrays()[j][q*4+3];
                        }
                        math::Vec3f wv1 = tm.transform(math::Vec3f(v1.position.x, v1.position.y, v1.position.z));
                        wv1 = getWindow().mapCoordsToPixel(math::Vec3f(v1.position.x, v1.position.y, v1.position.z));
                        math::Vec3f wv2 = tm.transform(math::Vec3f(v2.position.x, v2.position.y, v2.position.z));
                        wv2 = getWindow().mapCoordsToPixel(math::Vec3f(v2.position.x, v2.position.y, v2.position.z));
                        math::Vec3f wv3 = tm.transform(math::Vec3f(v3.position.x, v3.position.y, v3.position.z));
                        wv3 = getWindow().mapCoordsToPixel(math::Vec3f(v3.position.x, v3.position.y, v3.position.z));
                        physic::BoundingPolygon bp(wv1, wv2, wv3);
                        float tNearK = std::numeric_limits<float>::max();
                        math::Vec3f i1, i2;
                        if (bp.intersectsWhere(ray, i1, i2) && tNearK < tNear) {
                            vert1 = v1;
                            vert2 = v2;
                            vert3 = v3;
                            vert1.position.x = wv1.x;
                            vert1.position.y = wv1.y;
                            vert1.position.z = wv1.z;
                            vert2.position.x = wv2.x;
                            vert2.position.y = wv2.y;
                            vert2.position.z = wv2.z;
                            vert3.position.x = wv3.x;
                            vert3.position.y = wv3.y;
                            vert3.position.z = wv3.z;
                            mat = m_newInstances[i]->getMaterial();
                            tNearK = (i1 - ray.getOrig()).magnitude() / (ray.getExt() - ray.getOrig()).magnitude();
                            tNear = tNearK;
                            hitSurface  = true;
                        }
                    }
                }
            }
        }
    }
    return hitSurface;
}
math::Vec3f RayTracingRenderComponent::computeColor(math::Ray ray, Vertex v1, Vertex v2, Vertex v3, Vec3f u, Vec3f v, Vec3f n) {
    //Compute the color of a pixel into the given triangle with the interpolation algorithm.
    sf::Color c1 = v1.color;
    sf::Color c2 = v2.color;
    sf::Color c3 = v3.color;
    math::Vec3f ct1 = texM * math::Vec3f((v1.texCoords.x,v1.texCoords.y, 0.f);
    math::Vec3f ct2 = texM * math::Vec3f((v2.texCoords.x,v2.texCoords.y, 0.f);
    math::Vec3f ct3 = texM * math::Vec3f((v3.texCoords.x,v3.texCoords.y, 0.f);
    math::Vec3f r = math::Vec3f (c1.r / 255.f, c2.r / 255.f, c3.r / 255.f);
    math::Vec3f g = math::Vec3f (c1.g / 255.f, c2.g / 255.f, c3.g / 255.f);
    math::Vec3f b = math::Vec3f (c1.b / 255.f, c2.b / 255.f, c3.b / 255.f);
    math::Vec3f a = math::Vec3f (c1.a / 255.f, c2.a / 255.f, c3.a / 255.f);
    math::Vec3f texColor(1.f, 1.f, 1.f, 1.f);
    if (mat.getTexture() != nullptr) {
        const sf::Image& texImg = m_instances[i]->getMaterial().getTexture()->getImage();
        tc.x = math::Math::clamp(tc.x, 0, texImg.getSize().x);
        tc.y = math::Math::clamp(tc.y, 0, texImg.getSize().y);
        sf::Color color = texImg.getPixel(tc.x, tc.y);
        texColor = math::Vec3f (color.r / 255.f, color.g / 255.f, color.b / 255.f, color.a / 255.f);
    }
    color = math::Vec3f(r.x * point.x + r.y * point.y + r.z * point.z,
                            g.x * point.x + g.y * point.y + g.z * point.z,
                            b.x * point.x + b.y * point.y + b.z * point.z,
                            a.x * point.x + a.y * point.y + a.z * point.z) * texColor;
    return color;
}
float RayTracingRenderComponent::computeZ (math::Ray ray, math::Vec3f v1, math::Vec3f v2, math::Vec3f v3, math::Vec3f u, math::Vec3f v, math::Vec3f n) {
   math::Vec3f r = ray.getOrig() - ray.getExt();
   math::Vec3f otr = ray.getOrig() - v1;
   point.x = n.dot2(otr) / n.dot2(r);
   point.y = -otr.cross(u).dot2(r) / n.dot2(r);
   point.z = -v.cross(otr).dot2(r) / n.dot2(r);
   math::Vec3f z (v1.position.z, v2.position.z, v3.position.z);
   return z.x * point.x + z.y * point.y + z.z * point.z;
}
math::Vec3f RayTracingRenderComponent::castRay (math::Ray ray, LightView lview, float alphaFilter, math::Vec3f* newHitPoint, math::Vec3f reflRefHitPoint, bool writeToFrameBuffer, bool addToFrameBuffer) {
    std::array<std::array<sf::Color, lightView.getSize().y>, lightView.getSize().x>> lightDepthBuffer;
    m_normals.clear();
    m_diffuseColors.clear();
    math::Vec3f hitColor = backgroundColor;
    float tNear = std::numeric_limits<float>::max;
    math::Matrix4f texM, invTexM;
    Vertex v1, v2, v3;
    Material mat;
    math::Vec3f e1 = math::Vec3f(v2.position.x, v2.position.y, v2.position.z) - math::Vec3f(v1.position.x, v1.position.y, v1.position.z);
    math::Vec3f e2 = math::Vec3f(v3.position.x, v3.position.y, v3.position.z) - math::Vec3f(v1.position.x, v1.position.y, v1.position.z);
    math::Vec3f n = e1.cross(e2);
    if (trace(ray, tNear,v1, v2, v3, mat)) {
        math::Vec3f hitPoint = ray.getOrig() + ray.getDir() * tNear;
        newHitPoint = hitPoint;
        if (mat.getType() == Material::REFLEXION_AND_REFRACTION) {
                /*When the material is reflectable and refractable we need to generate two new rays :
                *for the reflection ray, he simply rebounds on the surface
                *for the refraction ray, he rebounds on the surface with a deviation angle.
                */
                pixelWorldPos = getWindow().mapPixelToCoords(*newHitPoint, view);
                pixelCamPos = getWindow().mapCoordsToPixel(*newHitPoint, view);
                float z = computeZ(ray, v1, v2, v3, e1, e2, n);
                if (pixelCamPos.x >= 0 && pixelCamPos.y >= 0
                    && pixelCamPos < view.getSize().x && pixelCamPos < view.getSize().y
                    && z >= getView().getViewport().getPosition().z && z < getView().getViewport().getSize().z) {
                    //Compute the interpolated color.
                    math::Vec3f color = computeColor(ray, v1, v2, v3, e1, e2, n);
                    m_refractionReflectionPoints[hitPoint.x * view.getSize().x + hitPoint.y * view.getSize().y + hitPoint.z] = color;
                    math::Vec3f reflectionDirection = reflect (ray.getDir(), n).normalize();
                    math::Vec3f refractionDirection = refract(ray.getDir(), n, mat.getIOR());
                    Vec3f reflectionRayOrig = hitPoint;
                    Vec3f refractionRayOrig = hitPoint;
                    Ray reflectionRay (reflectionRayOrig, reflectionDirection);
                    Ray refractionRay(refractionRayOrig, refractionDirection);
                    float kr;
                    fresnel(ray.getDir(), n, mat.getIOR(), kr);
                    math::Vec3f reflexionColor = castRay(refractionRay, light, alphaFilter, newHitPoint, true, false, hitPoint);
                    pixelWorldPos = getWindow().mapPixelToCoords(*newHitPoint, view);
                    pixelCamPos = getWindow().mapCoordsToPixel(*newHitPoint, view);
                    float z = computeZ(ray, v1, v2, v3, e1, e2, n);
                    if (pixelCamPos.x >= 0 && pixelCamPos.y >= 0
                        && pixelCamPos < view.getSize().x && pixelCamPos < view.getSize().y
                        && z >= getView().getViewport().getPosition().z && z < getView().getViewport().getSize().z) {
                        math::Vec3f objectColor = reflectionColor * kr;
                        m_frameBuffer[pixelCamPos.x][pixelCamPos.y].r = math::Math::camp(0, 255, (reflectionColor.r) * 255);
                        m_frameBuffer[pixelCamPos.x][pixelCamPos.y].g = math::Math::camp(0, 255, (reflectionColor.g) * 255);
                        m_frameBuffer[pixelCamPos.x][pixelCamPos.y].b = math::Math::camp(0, 255, (reflectionColor.b) * 255);
                        m_frameBuffer[pixelCamPos.x][pixelCamPos.y].a = math::Math::camp(0, 255, (reflectionColor.a) * 255);
                    }
                    math::Vec3f refractionColor = castRay(refractionRay, light, alphaFilter, newHitPoint, true, false, hitPoint);
                    pixelWorldPos = getWindow().mapPixelToCoords(*newHitPoint, view);
                    pixelCamPos = getWindow().mapCoordsToPixel(*newHitPoint, view);
                    float z = computeZ(ray, v1, v2, v3, e1, e2, n);
                    hitColor = reflectionColor * kr + refractionColor * (1 - kr);
                    if (pixelCamPos.x >= 0 && pixelCamPos.y >= 0
                        && pixelCamPos < view.getSize().x && pixelCamPos < view.getSize().y
                        && z >= getView().getViewport().getPosition().z && z < getView().getViewport().getSize().z) {
                        refractionColor = refractionColor * (1 - kr);
                        m_frameBuffer[pixelCamPos.x][pixelCamPos.y].r = math::Math::camp(0, 255, (m_frameBuffer[pixelCamPos.x][pixelCamPos.y].r + hitColor.x) * 255);
                        m_frameBuffer[pixelCamPos.x][pixelCamPos.y].g = math::Math::camp(0, 255, (m_frameBuffer[pixelCamPos.x][pixelCamPos.y].g + hitColor.y) * 255);
                        m_frameBuffer[pixelCamPos.x][pixelCamPos.y].b = math::Math::camp(0, 255, (m_frameBuffer[pixelCamPos.x][pixelCamPos.y].b + hitColor.z) * 255);
                        m_frameBuffer[pixelCamPos.x][pixelCamPos.y].a = math::Math::camp(0, 255, (m_frameBuffer[pixelCamPos.x][pixelCamPos.y].a + hitColor.w) * 255);
                    }
                }
                //The color becomes the color of the refracted object + the color of the reflected object.

            } else if (mat.getType() == REFLEXION) {
                /*Here we only need to compute, the reflection color.*/
                pixelWorldPos = getWindow().mapPixelToCoords(*newHitPoint, view);
                pixelCamPos = getWindow().mapCoordsToPixel(*newHitPoint, view);
                float z = computeZ(ray, v1, v2, v3, e1, e2, n);
                if (pixelCamPos.x >= 0 && pixelCamPos.y >= 0
                    && pixelCamPos < view.getSize().x && pixelCamPos < view.getSize().y
                    && z >= getView().getViewport().getPosition().z && z < getView().getViewport().getSize().z) {
                    m_refractionReflectionPoints[hitPoint.x * view.getSize().x + hitPoint.y * view.getSize().y + hitPoint.z] = color;
                    float kr;
                    fresnel(ray.getDir(), n, mat.getIOR(), kr);
                    Vec3f reflectionDirection = reflect(dir, N);
                    Vec3f reflectionRayOrig = hitPoint;
                    Ray reflectionRay (reflectionRayOrig, reflexionRayDirection) * kr;
                    pixelWorldPos = getWindow().mapPixelToCoords(*newHitPoint, view);
                    pixelCamPos = getWindow().mapCoordsToPixel(*newHitPoint, view);
                    float z = computeZ(ray, v1, v2, v3, e1, e2, n);
                    hitColor = castRay(reflectionRay, light, newHitPoint, true, false, hitPoint) * kr;
                    if (pixelCamPos.x >= 0 && pixelCamPos.y >= 0
                        && pixelCamPos < view.getSize().x && pixelCamPos < view.getSize().y
                        && z >= getView().getViewport().getPosition().z && z < getView().getViewport().getSize().z) {
                        math::Vec3f objectColor = reflectionColor * kr;
                        m_frameBuffer[pixelCamPos.x][pixelCamPos.y].r = math::Math::camp(0, 255, (hitColor.x) * 255);
                        m_frameBuffer[pixelCamPos.x][pixelCamPos.y].g = math::Math::camp(0, 255, (hitColor.y) * 255);
                        m_frameBuffer[pixelCamPos.x][pixelCamPos.y].b = math::Math::camp(0, 255, (hitColor.z) * 255);
                        m_frameBuffer[pixelCamPos.x][pixelCamPos.y].a = math::Math::camp(0, 255, (hitColor.w) * 255);
                    }
                }
            } else {
                //Compute the interpolated color.
                math::Vec3f color = computeColor(ray, v1, v2, v3, e1, e2, n, color, z);
                //Light direction.
                math::Vec3f lightDir = light->getCenter() - hitPoint;
                //Light attenuation with the light distance.
                float attenuation = 1.f - lightDir.magnitude() / light->getRadius();
                //Light attenuation depending on the orientation of the surface in front of light.
                float nDotL = lightDir.normalize().dot(it2->second);
                atttenuation *= nDotL;
                //Light reflection.
                math::Vec3f reflectionDirection = reflect(-lightDir, it2->second);
                //Compute the light color with attenuation and intensity.
                math::Vec3f lightAmt = light->getColor() * light->getIntensity() * std::max(0.f, attenuation);
                //Brightness of the objet (specular color), more the brightness if high, more light is reflected.
                math::Vec3f specularColor = math::Math::pow(std::max(0.f, -reflectionDirection.dot(ray.getDir())), mat.getSpecularExponent()) * light->intensity;
                /*LightAmpt is the light attenuation color, specularColor is the britghtness of the reflected surface, alphaFilter is the light which pass through the previous surface
                and it->second * it->second.w is the color of the light reflected by the surface.*/
                hitColor = (lightAmt) * alphaFilter * it->second * it->second.w + specularColor;
                /*If it's the first pixel intersecting the light we simply write it to the framebuffer.*/
                if (writeToFrameBuffer) {
                    math::Vec3f pixelWorldPos = getWindow().mapPixelToCoords(hitPoint, view);
                    math::Vec3f pixelCamPos = getWindow().mapCoordsToPixel(hitPoint, view);
                    float z = computeZ(ray, v1, v2, v3, e1, e2, n);
                    //If the hitPoint is in the camera, we add the resulting color to the framebuffer.
                    if (addToFrameBuffer) {
                        if (pixelCamPos.x >= 0 && pixelCamPos.y >= 0
                            && pixelCamPos < view.getSize().x && pixelCamPos < view.getSize().y
                            && z >= getView().getViewport().getPosition().z && z < getView().getViewport().getSize().z) {
                            m_frameBuffer[pixelCamPos.x][pixelCamPos.y].r = math::Math::camp(0, 255, (hitColor.r) * 255);
                            m_frameBuffer[pixelCamPos.x][pixelCamPos.y].g = math::Math::camp(0, 255, (hitColor.g) * 255);
                            m_frameBuffer[pixelCamPos.x][pixelCamPos.y].b = math::Math::camp(0, 255, (hitColor.b) * 255);
                            m_frameBuffer[pixelCamPos.x][pixelCamPos.y].a = math::Math::camp(0, 255, (hitColor.a) * 255);
                        }
                    } else {
                        if (hitPoint.x == reflRefHitPoint.x && hitPoint.y == reflRefHitPoint.y) {
                            m_reflectionRefractionColors[hitPoint.x * getView().getSize().x + hitPoint.y * getView().getSize().y + hitPoint.z] += hitColor;
                        }
                        hitColor = m_reflectionRefractionColors[hitPoint.x * getView().getSize().x + hitPoint.y * getView().getSize().y + hitPoint.z];
                    }
                }
                //We need to set the alpha filter for the next pixel.
                alphaFilter = 1-it->second.w;
                if (alphaFilter != 0) {
                    float kr;
                    fresnel(ray.getDir(), n, mat.getIOR(), kr);
                    math::Vec3f reflectionDirection = relfect(ray.getDir(), -n);
                    math::Vec3f refractionDirection = refract(ray.getDir(), -n);
                    Vec3f reflectionRayOrig = hitPoint;
                    Vec3f refractionRayOrig = hitPoint;
                    Ray reflectionRay (reflectionRayOrig, reflectionDirection);
                    Ray refractionRay(refractionRayOrig, refractionDirection);
                    math::Vec3f reflectionColor = castRay(reflectionRay, light, alphaFilter, newHitPoint, false, addToFrameBuffer);
                    /*If the ray is not issued from a refraction or reflection pixel and intersects another
                    * transparent pixel, we add the color to the framebuffer
                    */
                    pixelWorldPos = getWindow().mapPixelToCoords(*newHitPoint, view);
                    pixelCamPos = getWindow().mapCoordsToPixel(*newHitPoint, view);
                    if (addToFrameBuffer) {
                        float z = computeZ(ray, v1, v2, v3, e1, e2, n);
                        if (pixelCamPos.x >= 0 && pixelCamPos.y >= 0
                            && pixelCamPos < view.getSize().x && pixelCamPos < view.getSize().y
                            && z >= getView().getViewport().getPosition().z && z < getView().getViewport().getSize().z) {
                            math::Vec3f objectColor = reflectionColor * kr;
                            m_frameBuffer[pixelCamPos.x][pixelCamPos.y].r = math::Math::camp(0, 255, (m_frameBuffer[pixelCamPos.x][pixelCamPos.y].r + reflectionColor.r) * 255);
                            m_frameBuffer[pixelCamPos.x][pixelCamPos.y].g = math::Math::camp(0, 255, (m_frameBuffer[pixelCamPos.x][pixelCamPos.y].g + reflectionColor.g) * 255);
                            m_frameBuffer[pixelCamPos.x][pixelCamPos.y].b = math::Math::camp(0, 255, (m_frameBuffer[pixelCamPos.x][pixelCamPos.y].b + reflectionColor.b) * 255);
                            m_frameBuffer[pixelCamPos.x][pixelCamPos.y].a = math::Math::camp(0, 255, (m_frameBuffer[pixelCamPos.x][pixelCamPos.y].a + reflectionColor.a) * 255);
                            hitColor = reflectionColor * kr + refractionColor * (1 - kr);
                        }
                    } else {
                        if (hitPoint.x == reflRefHitPoint.x && hitPoint.y == reflRefHitPoint.y) {
                            m_reflectionRefractionColors[hitPoint.x * getView().getSize().x + hitPoint.y * getView().getSize().y + hitPoint.z] = reflectionColor * kr;
                        }
                        hitColor = m_reflectionRefractionColors[hitPoint.x * getView().getSize().x + hitPoint.y * getView().getSize().y + hitPoint.z];
                    }
                    math::Vec3f refractionColor = castRay(refractionRay, light, alphaFilter, newHitPoint, false, addToFrameBuffer);
                    pixelWorldPos = getWindow().mapPixelToCoords(*newHitPoint, view);
                    pixelCamPos = getWindow().mapCoordsToPixel(*newHitPoint, view);
                    if (addToFrameBuffer) {
                        float z = computeZ(ray, v1, v2, v3, e1, e2, n);
                        if (pixelCamPos.x >= 0 && pixelCamPos.y >= 0
                            && pixelCamPos < view.getSize().x && pixelCamPos < view.getSize().y
                            && z >= getView().getViewport().getPosition().z && z < getView().getViewport().getSize().z) {
                            refractionColor = refractionColor * (1 - kr);
                            m_frameBuffer[pixelCamPos.x][pixelCamPos.y].r = math::Math::camp(0, 255, (m_frameBuffer[pixelCamPos.x][pixelCamPos.y].r + reflectionColor.r) * 255);
                            m_frameBuffer[pixelCamPos.x][pixelCamPos.y].g = math::Math::camp(0, 255, (m_frameBuffer[pixelCamPos.x][pixelCamPos.y].g + reflectionColor.g) * 255);
                            m_frameBuffer[pixelCamPos.x][pixelCamPos.y].b = math::Math::camp(0, 255, (m_frameBuffer[pixelCamPos.x][pixelCamPos.y].b + reflectionColor.b) * 255);
                            m_frameBuffer[pixelCamPos.x][pixelCamPos.y].a = math::Math::camp(0, 255, (m_frameBuffer[pixelCamPos.x][pixelCamPos.y].a + reflectionColor.a) * 255);
                        }
                    } else {
                        if (hitPoint.x == reflRefHitPoint.x && hitPoint.y == reflRefHitPoint.y) {
                            m_reflectionRefractionColors[hitPoint.x * getView().getSize().x + hitPoint.y * getView().getSize().y + hitPoint.z] = reflectionColor * kr;
                        }
                        hitColor = m_reflectionRefractionColors[hitPoint.x * getView().getSize().x + hitPoint.y * getView().getSize().y + hitPoint.z];
                    }
                } else {
                    /*If the pixel is opaque, we need to check if there's another object behind it,
                    if yes, it means that this object is in the shadow.
                    */
                    pixelWorldPos = getWindow().mapPixelToCoords(*newHitPoint, view);
                    pixelCamPos = getWindow().mapCoordsToPixel(*newHitPoint, view);
                    float z = computeZ(ray, v1, v2, v3, e1, e2, n);
                    if (addToFrameBuffer) {
                        if (pixelCamPos.x >= 0 && pixelCamPos.y >= 0
                            && pixelCamPos < view.getSize().x && pixelCamPos < view.getSize().y
                            && z >= getView().getViewport().getPosition().z && z < getView().getViewport().getSize().z) {
                            refractionColor = refractionColor * (1 - kr);
                            m_frameBuffer[pixelCamPos.x][pixelCamPos.y].r = math::Math::camp(0, 255, (m_frameBuffer[pixelCamPos.x][pixelCamPos.y].r + hitColor.r) * 255);
                            m_frameBuffer[pixelCamPos.x][pixelCamPos.y].g = math::Math::camp(0, 255, (m_frameBuffer[pixelCamPos.x][pixelCamPos.y].g + hitColor.g) * 255);
                            m_frameBuffer[pixelCamPos.x][pixelCamPos.y].b = math::Math::camp(0, 255, (m_frameBuffer[pixelCamPos.x][pixelCamPos.y].b + hitColor.b) * 255);
                            m_frameBuffer[pixelCamPos.x][pixelCamPos.y].a = math::Math::camp(0, 255, (m_frameBuffer[pixelCamPos.x][pixelCamPos.y].a + hitColor.a) * 255);
                            hitColor = math::Vec3f (m_frameBuffer[pixelCamPos.x][pixelCamPos.y].r / 255.f, m_frameBuffer[pixelCamPos.x][pixelCamPos.y].g / 255.f, m_frameBuffer[pixelCamPos.x][pixelCamPos.y].b / 255.f, m_frameBuffer[pixelCamPos.x][pixelCamPos.y].r / 255.f);
                        }
                    } else {
                        if (hitPoint.x == reflRefHitPoint.x && hitPoint.y == reflRefHitPoint.y) {
                            m_reflectionRefractionColors[hitPoint.x * getView().getSize().x + hitPoint.y * getView().getSize().y + hitPoint.z] += hitColor;
                        }
                        hitColor = m_reflectionRefractionColors[hitPoint.x * getView().getSize().x + hitPoint.y * getView().getSize().y + hitPoint.z] += hitColor;
                    }
                    float tNearShadow = std::numeric_limits<float>::max();
                    math::Vec3f shadowRayOrig = hitPoint;
                    math::Vec3f shadowRayDir = lightDir;
                    math::Ray shadowRay (shadowRayOrig, shadowRayDir);
                    bool inShadow = trace(shadowRay, tNearShadow, v1, v2, v3, mat);
                    math::Vec3f shadowHitPoint = ray.getOrig() + ray.getDir() * tNear;
                    if (inShadow) {
                        pixelWorldPos = getWindow().mapPixelToCoords(shadowHitPoint, view);
                        pixelCamPos = getWindow().mapCoordsToPixel(shadowHitPoint, view);
                        if (addToFrameBuffer) {
                            float z = computeZ(ray, v1, v2, v3, e1, e2, n);
                            if (pixelCamPos.x >= 0 && pixelCamPos.y >= 0
                                && pixelCamPos < view.getSize().x && pixelCamPos < view.getSize().y
                                && z >= getView().getViewport().getPosition().z && z < getView().getViewport().getSize().z) {
                                refractionColor = refractionColor * (1 - kr);
                                m_frameBuffer[pixelCamPos.x][pixelCamPos.y].r = 0;
                                m_frameBuffer[pixelCamPos.x][pixelCamPos.y].g = 0;
                                m_frameBuffer[pixelCamPos.x][pixelCamPos.y].b = 0;
                                m_frameBuffer[pixelCamPos.x][pixelCamPos.y].a = 1;
                            }
                        } else {
                            if (shadowHitPoint.x == reflRefHitPoint.x && shadowHitPoint.y == reflRefHitPoint.y) {
                                m_reflectionRefractionColors[hitPoint.x * getView().getSize().x + hitPoint.y * getView().getSize().y + hitPoint.z] = math::Vec3f(0, 0, 0);
                            }
                        }
                    }
                }
            }
        }
    }
    return hitColor;
}
void RayTracingRenderComponent::fresnel(const Vec3f &i, const Vec3f &n, const float &ior, float &kr)
{
    float cosi = clamp(-1, 1, dotProduct(i, n));
    float etai = 1, etat = ior;
    if (cosi > 0) { std::swap(etai, etat); }
    // Compute sini using Snell's law
    float sint = etai / etat * sqrtf(std::max(0.f, 1 - cosi * cosi));
    // Total internal reflection
    if (sint >= 1) {
    kr = 1;
    }
    else {
    float cost = sqrtf(std::max(0.f, 1 - sint * sint));
    cosi = fabsf(cosi);
    float Rs = ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost));
    float Rp = ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost));
    kr = (Rs * Rs + Rp * Rp) / 2;
}
// As a consequence of the conservation of energy, transmittance is given by:
// kt = 1 - kr;
}
math::Vec3f RayTracingRenderComponent::reflect(const Vec3f &i, const Vec3f &n)
{
    return i - 2 * i.dot(n) * n;
}
math::Vec3f RayTracingRenderComponent::refract(const Vec3f &i, const Vec3f &n, const float &ior)
{
    float cosi = math::Math::clamp(-1, 1, i.dot(n));
    float etai = 1, etat = ior;
    Vec3f n = N;
    if (cosi < 0) { cosi = -cosi; } else { std::swap(etai, etat); n= -n; }
    float eta = etai / etat;
    float k = 1 - eta * eta * (1 - cosi * cosi);
    return k < 0 ? 0 : eta * I + (eta * cosi - sqrtf(k)) * n;
}
