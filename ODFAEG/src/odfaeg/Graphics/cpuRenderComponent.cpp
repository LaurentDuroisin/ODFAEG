#include "../../../include/odfaeg/Graphics/cpuRenderComponent.hpp"
namespace odfaeg {
    namespace graphic {
        inline void checkErr(cl_int err, const char * name) {
            if (err != CL_SUCCESS) {
               std::cerr << "ERROR: " << name  << " (" << err << ")" << std::endl;
               exit(EXIT_FAILURE);
            }
        }
        inline void checkErr(cl_program prog, void* name) {
            std::cerr << "ERROR: " << name << std::endl;
        }
        CPURenderComponent::CPURenderComponent(math::Vec3f position,math::Vec3f size, math::Vec3f origin, bool useThread, RenderWindow& rw)
        : Component(position,size, origin, useThread), window(rw), view (rw.getView()) {
            redBuffer.resize(size.x * size.y);
            blueBuffer.resize(size.x * size.y);
            greenBuffer.resize(size.x * size.y);
            alphaBuffer.resize(size.x * size.y);
            depthBuffer.resize(size.x * size.y);
            this->size.x = rw.getSize().x;
            this->size.y = rw.getSize().y;
            view = window.getView();
            fbTile = new Tile(&fbTexture, position, size, sf::IntRect(0, 0, size.x, size.y));
            fbTile->setCenter(view.getPosition());
            cl_int err;
            std::vector< cl::Platform > platformList;
            cl::Platform::get(&platformList);
            checkErr(platformList.size()!=0 ? CL_SUCCESS : -1, "cl::Platform::get");
            std::cerr << "Platform number is: " << platformList.size() << std::endl;std::string platformVendor;
            platformList[0].getInfo((cl_platform_info)CL_PLATFORM_VENDOR, &platformVendor);
            std::cerr << "Platform is by: " << platformVendor << "\n";
            cl_context_properties cprops[3] = {CL_CONTEXT_PLATFORM, (cl_context_properties)(platformList[0])(), 0};
            clcontext = cl::Context (
              CL_DEVICE_TYPE_ALL,
              cprops,
              NULL,
              NULL,
              &err);
            /*checkErr(err, "Context::Context()");
            cl::Buffer credBuffer = cl::Buffer (clcontext, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, size.x * size.y,&redBuffer, &err);
            checkErr(err, "Buffer::Buffer()");
            cl::Buffer cblueBuffer = cl::Buffer (clcontext, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, size.x * size.y,&blueBuffer, &err);
            checkErr(err, "Buffer::Buffer()");
            cl::Buffer cgreenBuffer = cl::Buffer (clcontext, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, size.x * size.y,&greenBuffer, &err);
            checkErr(err, "Buffer::Buffer()");
            cl::Buffer calphaBuffer = cl::Buffer (clcontext, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, size.x * size.y,&alphaBuffer, &err);
            checkErr(err, "Buffer::Buffer()");
            cl::Buffer cdepthBuffer = cl::Buffer (clcontext, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, size.x * size.y,&depthBuffer, &err);
            checkErr(err, "Buffer::Buffer()");*/
            devices = clcontext.getInfo<CL_CONTEXT_DEVICES>();
            checkErr(devices.size() > 0 ? CL_SUCCESS : -1, "devices.size() > 0");
            std::string prog = "#pragma OPENCL EXTENSION cl_khr_byte_addressable_store : enable\n"
                               "const int stepXSize = 4;\n"
                               "const int stepYSize = 4;\n"
                               "void multMat (float* matA, float* matB, float* matC, const int n) {\n"
                                    "for (int x = 0; x < n; ++x) {\n"
                                        "for (int y = 0; y < n; ++y) {\n"
                                            "float value = 0;\n"
                                            "for (int k = 0; k < n; ++k) {\n"
                                                "float elementA = matA[y * n + k];\n"
                                                "float elementB = matB[k * n + x];\n"
                                                "value += elementA * elementB;\n"
                                            "}\n"
                                            "matC[y * n + x] = value;\n"
                                        "}\n"
                                    "}\n"
                                "}\n"
                                "void addVec (float* vecA, float* vecB, float* vecC, const int n) {\n"
                                    "for (int i = 0; i < n; i++) {\n"
                                        "vecC[i] = vecA[i] + vecB[i];\n"
                                    "}\n"
                                "}\n"
                                "void multVec (float* vecA, float* vecB, float* vecC, const int n) {\n"
                                    "for (int i = 0; i < n; i++) {\n"
                                        "vecC[i] = vecA[i] * vecB[i];\n"
                                    "}\n"
                                "}\n"
                                "void multMatVec (float* matA, float* vecB, float* vecC, const int n) {\n"
                                   "for (int i = 0; i < n; ++i) {\n"
                                        "float value = 0;\n"
                                        "for (int j = 0; j < n; ++j) {\n"
                                            "value += vecB[j] * matA[i*4+j];\n"
                                        "}\n"
                                        "vecC[i] = value;\n"
                                   "}\n"
                                "}\n"
                                "void transpose(float* matA, float* matB, const int n) {\n"
                                    "for (int i = 0; i < n; ++i) {\n"
                                        "for (int j = 0; j < n; ++j) {\n"
                                            "matB[i*n+j] = matA[j*n+i];\n"
                                        "}\n"
                                    "}\n"
                                "}\n"
                                "float  det (float* mat, const int n) {\n"
                                    "int d=0, p, h, k, i, j;\n"
                                    "const int m = n *n;\n"
                                    "float temp[16];\n"
                                    "if(n==1) {\n"
                                        "return mat[0];\n"
                                    "} else if(n==2) {\n"
                                        "d=(mat[0]*mat[3]-mat[1]*mat[2]);\n"
                                        "return d;\n"
                                    "} else {\n"
                                        "for(p=0;p<n;p++) {\n"
                                            "h = 0;\n"
                                            "k = 0;\n"
                                            "for(i=1;i<n;i++) {\n"
                                                "for( j=0;j<n;j++) {\n"
                                                    "if(j!=p) {\n"
                                                        "temp[h*n+k] = mat[i*n+j];\n"
                                                        "k++;\n"
                                                        "if(k==n-1) {\n"
                                                            "h++;\n"
                                                            "k = 0;\n"
                                                        "}\n"
                                                    "}\n"
                                                "}\n"
                                            "}\n"
                                            "d=d+mat[p]*pow((float) -1,(float) p)*det(temp,n-1);\n"
                                        "}\n"
                                        "return d;\n"
                                    "}\n"
                                "}\n"
                                "/*float min (float* vec, const int n) {\n"
                                    "float cmin = vec[0];\n"
                                    "for (int i = 1; i < n; ++i) {\n"
                                        "if (vec[i] < cmin)\n"
                                            "cmin = vec[i];\n"
                                    "}\n"
                                    "return cmin;\n"
                                "}\n"
                                "float max (float* vec, const int n) {\n"
                                    "float cmax = vec[0];\n"
                                    "for (int i = 1; i < n; ++i) {\n"
                                        "if (vec[i] > cmax)\n"
                                            "cmax = vec[i];\n"
                                    "}\n"
                                    "return cmax;\n"
                                "}*/\n"
                                "int equal(float* v1, float* v2, const int n) {\n"
                                    "for (int i = 0; i < n; i++) {\n"
                                        "if(v1[i] != v2[i]) {\n"
                                            "return 0;\n"
                                        "}\n"
                                    "}\n"
                                    "return 1;\n"
                                "}\n"
                                "void initEdge (const float* v0, const float* v1, const float* origin, float* oneStepX, float* oneStepY, float* e) {\n"
                                       "int a = v0[1] - v1[1], b = v1[0] - v0[0];\n"
                                       "int c = v0[0]*v1[1] - v0[1]*v1[0];\n"
                                       "for (int i = 0; i < 4; i++) {\n"
                                       "    oneStepX[i] = a * stepXSize;\n"
                                       "    oneStepY[i] = b * stepYSize;\n"
                                       "}\n"
                                       "float offsets[] = {0, 1, 2, 3};\n"
                                       "float oxs[] = {origin[0], origin[0], origin[0], origin[0]};\n"
                                       "float oys[] = {origin[1], origin[1], origin[1], origin[1]};\n"
                                       "float xs[4];\n"
                                       "addVec(oxs, offsets, xs, 4);\n"
                                       "float ys[4];\n"
                                       "addVec(oys, offsets, ys, 4);\n"
                                       "float as[] = {a, a, a, a};\n"
                                       "float bs[] = {b, b, b, b};\n"
                                       "float cs[] = {c, c, c, c};\n"
                                       "float axs[4];\n"
                                       "multVec(as, xs, axs, 4);\n"
                                       "float bys[4];\n"
                                       "multVec(bs, ys, bys, 4);\n"
                                       "float axsplusbys[4];\n"
                                       "addVec(axs, bys, axsplusbys, 4);\n"
                                       "addVec(axsplusbys, cs, e, 4);"
                                "}\n"
                                "__kernel void vertexShader(__global float* vPosX, __global float* vPosY, __global float* vPosZ, __global float* vPosW,\n"
                                                           "__global unsigned char* vColRed, __global unsigned char* vColBlue, __global unsigned char* vColGreen, __global unsigned char* vColAlpha,\n"
                                                           "__global unsigned int* vTCU, __global unsigned int* vTCV,\n"
                                                           "__global float* outvPosX, __global float* outvPosY, __global float* outvPosZ, __global float* outvPosW,\n"
                                                           "__global unsigned char* outvColRed, __global unsigned char* outvColBlue, __global unsigned char* outvColGreen, __global unsigned char* outvColAlpha,\n"
                                                           "__global unsigned int* outvTCU, __global unsigned int* outvTCV,\n"
                                                           "__global unsigned int* indices,  __global unsigned int* numIndicies, __global unsigned int* baseIndices,\n"
                                                           "__global unsigned int* baseVertices, __global float* transfMatrices, __global float* projMatrix, __global float* viewMatrix,\n"
                                                           "__global float* viewportMatrix) {\n"
                                    "size_t instanceID = get_global_id(0);\n"
                                    "float transfMatrix[16];\n"
                                    "for (int i = 0; i < 16; i++) {\n"
                                        "transfMatrix[i] = transfMatrices[instanceID*16+i];\n"
                                    "}\n"
                                    "unsigned int currentPos = 0;\n"
                                    "for (unsigned int i = 0; i < instanceID; i++) {\n"
                                        "currentPos+=numIndicies[i];\n"
                                    "}\n"
                                    "for (unsigned int offset = 0; offset < numIndicies[instanceID]; ++offset) {\n"
                                        "float position[] = {vPosX[baseVertices[instanceID]+indices[baseIndices[instanceID]+offset]],\n"
                                                        "vPosY[baseVertices[instanceID]+indices[baseIndices[instanceID]+offset]],\n"
                                                        "vPosZ[baseVertices[instanceID]+indices[baseIndices[instanceID]+offset]],\n"
                                                        "vPosW[baseVertices[instanceID]+indices[baseIndices[instanceID]+offset]]};\n"
                                        "float worldcoords[4];\n"
                                        "multMatVec(transfMatrix, position, worldcoords, 4);\n"
                                        "float viewcoords[4];\n"
                                        "multMatVec(viewMatrix, worldcoords, viewcoords, 4);\n"
                                        "float clipcoords[4];\n"
                                        "multMatVec(projMatrix, viewcoords, clipcoords, 4);\n"
                                        "float ndcCoords[] = {clipcoords[0] / clipcoords[3],\n"
                                        "                     clipcoords[1] / clipcoords[3],\n"
                                        "                     clipcoords[2] / clipcoords[3],\n"
                                        "                     clipcoords[3] / clipcoords[3]};\n"
                                        "float finalPos[4];\n"
                                        "multMatVec(viewportMatrix, ndcCoords, finalPos, 4);\n"
                                        "outvPosX[currentPos+offset] = position[0];\n"
                                        "outvPosY[currentPos+offset] = position[1];\n"
                                        "outvPosZ[currentPos+offset] = position[2];\n"
                                        "outvPosW[currentPos+offset] = position[3];\n"
                                        "outvColRed[currentPos+offset] = vColRed[baseVertices[instanceID]+indices[baseIndices[instanceID]+offset]];\n"
                                        "outvColBlue[currentPos+offset] = vColBlue[baseVertices[instanceID]+indices[baseIndices[instanceID]+offset]];\n"
                                        "outvColGreen[currentPos+offset] = vColGreen[baseVertices[instanceID]+indices[baseIndices[instanceID]+offset]];\n"
                                        "outvColAlpha[currentPos+offset] = vColAlpha[baseVertices[instanceID]+indices[baseIndices[instanceID]+offset]];\n"
                                        "outvTCU[currentPos+offset] = vTCU[baseVertices[instanceID]+indices[baseIndices[instanceID]+offset]];\n"
                                        "outvTCV[currentPos+offset] = vTCV[baseVertices[instanceID]+indices[baseIndices[instanceID]+offset]];\n"
                                    "}\n"
                                "}\n"
                                "__kernel void geometryShader (__global float* vPosX, __global float* vPosY, __global float* vPosZ, __global float* vPosW,\n"
                                "                              __global unsigned int* vColRed, __global unsigned int* vColBlue, __global unsigned int* vColGreen,\n"
                                "                              __global unsigned int* vColAlpha, __global int* vTCU, __global int* vTCV,\n"
                                "                              __global float* outvCentersX, __global float* outvCentersY, __global float* outvCentersZ, __global float* outvCentersW,\n"
                                "                              __global unsigned int* outvAvgColorRed, __global unsigned int* outvAvgColorBlue, __global unsigned int* outvAvgColorGreen, __global unsigned int* outvAvgColorAlpha,\n"
                                "                              __global int* outvAvgCTU, __global int* outvAvgCTV, __global unsigned int* numIndicies) {\n"
                                "   size_t instanceID = get_global_id(0);\n"
                                "   float center[10];\n"
                                "   unsigned int currentPos = 0;\n"
                                "   for (unsigned int i = 0; i < instanceID; i++) {\n"
                                "       currentPos+=numIndicies[i];\n"
                                "   }\n"
                                "   for (unsigned int offset = 0; offset < numIndicies[instanceID]; ++offset) {\n"
                                "       center[0] += vPosX[currentPos+offset];\n"
                                "       center[1] += vPosY[currentPos+offset];\n"
                                "       center[2] += vPosZ[currentPos+offset];\n"
                                "       center[3] += vPosW[currentPos+offset];\n"
                                "       center[4] += vColRed[currentPos+offset];\n"
                                "       center[5] += vColBlue[currentPos+offset];\n"
                                "       center[6] += vColGreen[currentPos+offset];\n"
                                "       center[7] += vColAlpha[currentPos+offset];\n"
                                "       center[8] += vTCU[currentPos+offset];\n"
                                "       center[9] += vTCV[currentPos+offset];\n"
                                "   }\n"
                                "   outvCentersX[instanceID] = center[0] / numIndicies[instanceID];\n"
                                "   outvCentersY[instanceID] = center[1] / numIndicies[instanceID];\n"
                                "   outvCentersZ[instanceID] = center[2] / numIndicies[instanceID];\n"
                                "   outvCentersW[instanceID] = center[3] / numIndicies[instanceID];\n"
                                "   outvAvgColorRed[instanceID] = center[4] / numIndicies[instanceID];\n"
                                "   outvAvgColorBlue[instanceID] = center[5] / numIndicies[instanceID];\n"
                                "   outvAvgColorGreen[instanceID] = center[6] / numIndicies[instanceID];\n"
                                "   outvAvgColorAlpha[instanceID] = center[7] / numIndicies[instanceID];\n"
                                "   outvAvgCTU[instanceID] = center[8] / numIndicies[instanceID];\n"
                                "   outvAvgCTV[instanceID] = center[9] / numIndicies[instanceID];\n"
                                "}\n"
                                "__kernel void tesslationShader(__global float* vPosX, __global float* vPosY, __global float* vPosZ, __global float* vPosW,\n"
                                "                               __global unsigned int* vColRed, __global unsigned int* vColBlue, __global unsigned int* vColGreen, __global unsigned int* vColAlpha,\n"
                                "                               __global int* vTCU, __global int* vTCV, __global float* centersX, __global float* centersY,\n"
                                "                               __global float* centersZ, __global float* centersW, __global unsigned int* centerColRed, __global unsigned int* centerColBlue,\n"
                                "                               __global unsigned int* centerColGreen, __global unsigned int* centerColAlpha, __global int* centerTCU, __global int* centerTCV,\n"
                                "                               __global float* outPosX, __global float* outPosY, __global float* outPosZ, __global float *outPosW, \n"
                                "                               __global unsigned int* outColRed, __global unsigned int* outColBlue, __global unsigned int* outColGreen, __global unsigned int* outColAlpha,\n"
                                "                               __global int* outTCU, __global int* outTCV, __global unsigned int* numIndicies) {\n"
                                "   size_t instanceID = get_global_id(0);\n"
                                "   unsigned int currentPos = 0;\n"
                                "   for (unsigned int i = 0; i < instanceID; i++) {\n"
                                "       currentPos+=numIndicies[i];\n"
                                "   }\n"
                                "   for (unsigned int offset = 0; offset < numIndicies[instanceID]; ++offset) {\n"
                                "        unsigned int offset2 = (offset + 1 >= numIndicies[instanceID]) ? 0 : offset+1;\n"
                                "        outPosX[(currentPos+offset)*3] = centersX[instanceID];\n"
                                "        outPosY[(currentPos+offset)*3] = centersY[instanceID];\n"
                                "        outPosZ[(currentPos+offset)*3] = centersZ[instanceID];\n"
                                "        outPosW[(currentPos+offset)*3] = centersW[instanceID];\n"
                                "        outColRed[(currentPos+offset)*3] = centerColRed[instanceID];\n"
                                "        outColBlue[(currentPos+offset)*3] = centerColBlue[instanceID];\n"
                                "        outColGreen[(currentPos+offset)*3] = centerColGreen[instanceID];\n"
                                "        outColAlpha[(currentPos+offset)*3] = centerColAlpha[instanceID];\n"
                                "        outTCU[(currentPos+offset)*3] = centerTCU[instanceID];\n"
                                "        outTCV[(currentPos+offset)*3] = centerTCV[instanceID];\n"
                                "        outPosX[(currentPos+offset)*3+1] = vPosX[currentPos+offset];\n"
                                "        outPosY[(currentPos+offset)*3+1] = vPosY[currentPos+offset];\n"
                                "        outPosZ[(currentPos+offset)*3+1] = vPosY[currentPos+offset];\n"
                                "        outPosW[(currentPos+offset)*3+1] = vPosZ[currentPos+offset];\n"
                                "        outColRed[(currentPos+offset)*3+1] = vColRed[currentPos+offset];\n"
                                "        outColBlue[(currentPos+offset)*3+1] = vColBlue[currentPos+offset];\n"
                                "        outColGreen[(currentPos+offset)*3+1] = vColGreen[currentPos+offset];\n"
                                "        outColAlpha[(currentPos+offset)*3+1] = vColAlpha[currentPos+offset];\n"
                                "        outTCU[(currentPos+offset)*3+1] = vTCU[currentPos+offset];\n"
                                "        outTCV[(currentPos+offset)*3+1] = vTCV[currentPos+offset];\n"
                                "        outPosX[(currentPos+offset)*3+2] = vPosX[currentPos+offset2];\n"
                                "        outPosY[(currentPos+offset)*3+2] = vPosY[currentPos+offset2];\n"
                                "        outPosZ[(currentPos+offset)*3+2] = vPosY[currentPos+offset2];\n"
                                "        outPosW[(currentPos+offset)*3+2] = vPosZ[currentPos+offset2];\n"
                                "        outColRed[(currentPos+offset)*3+2] = vColRed[currentPos+offset2];\n"
                                "        outColBlue[(currentPos+offset)*3+2] = vColBlue[currentPos+offset2];\n"
                                "        outColGreen[(currentPos+offset)*3+2] = vColGreen[currentPos+offset2];\n"
                                "        outColAlpha[(currentPos+offset)*3+2] = vColAlpha[currentPos+offset2];\n"
                                "        outTCU[(currentPos+offset)*3+2] = vTCU[currentPos+offset2];\n"
                                "        outTCV[(currentPos+offset)*3+2] = vTCV[currentPos+offset2];\n"
                                "   }\n"
                                "}"
                                "__kernel void fragmentShader(__global float* vPosX, __global float* vPosY, __global float* vPosZ, __global float* vPosW,\n"
                                "                             __global unsigned int* vColRed, __global unsigned int* vColBlue, __global unsigned int* vColGreen,\n"
                                "                             __global unsigned int* vColAlpha, __global int* vTCU, __global int* vTCV,\n"
                                "                             __global float* redBuffer, __global float* blueBuffer, __global float* greenBuffer, __global float* alphaBuffer,\n"
                                "                             __global float* depthBuffer, __global float* viewportMatrix, __global float* texMatrix) {\n"
                                "   size_t tid = get_global_id(0);\n"
                                "   float p1[] = {vPosX[tid*3], vPosY[tid*3], vPosZ[tid*3], vPosW[tid*3]};\n"
                                "   float p2[] = {vPosX[tid*3+1],vPosY[tid*3+1],vPosZ[tid*3+1], vPosW[tid*3+1]};\n"
                                "   float p3[] = {vPosX[tid*3+2],vPosY[tid*3+2],vPosZ[tid*3+2], vPosW[tid*3+2]};\n"
                                "   float xs[] = {p1[0], p2[0], p3[0]};\n"
                                "   float ys[] = {p1[1], p2[1], p3[1]};\n"
                                "   float minX = min(xs, 3);\n"
                                "   float minY = min(ys, 3);\n"
                                "   float maxX = max(xs, 3);\n"
                                "   float maxY = max(ys, 3);\n"
                                "   float p[] = {minX, minY};\n"
                                "   float steps_e12X[4], steps_e20X[4], steps_e01X[4],\n"
                                "         steps_e12Y[4], steps_e20Y[4], steps_e01Y[4];\n"
                                "   float w0_row[4];\n"
                                "   initEdge(p2, p3, p, steps_e12X, steps_e12Y, w0_row);"
                                "   float w1_row[4];\n"
                                "   initEdge(p3, p1, p, steps_e20X, steps_e20Y, w1_row);"
                                "   float w2_row[4];\n"
                                "   initEdge(p1, p2, p, steps_e01X, steps_e01Y, w2_row);"
                                "   int viewportWidth = viewportMatrix[0] * 2;\n"
                                "   int viewportHeight = viewportMatrix[5] * 2;\n"
                                "   int viewportDepth = viewportMatrix[10] * 2;\n"
                                "   int viewportX = viewportMatrix[3] - viewportMatrix[0];\n"
                                "   int viewportY = viewportMatrix[7] - viewportMatrix[5];\n"
                                "   int viewportZ = viewportMatrix[11] - viewportMatrix[10];\n"
                                "   for (p[1] = minY; p[1] < maxY; p[1] += stepYSize) {\n"
                                "       float w0[] = {w0_row[0], w0_row[1], w0_row[2], w0_row[3]};\n"
                                "       float w1[] = {w1_row[0], w1_row[1], w1_row[2], w1_row[3]};\n"
                                "       float w2[] = {w2_row[0], w2_row[1], w2_row[2], w2_row[3]};;\n"
                                "       for (p[0] = minX; p[0] < maxX; p[0] += stepXSize) {\n"
                                "            int mask[] = {(int) w0[0] | (int) w1[0] | (int) w2[0],\n"
                                "                          (int) w0[1] | (int) w1[1] | (int) w2[1],\n"
                                "                          (int) w0[2] | (int) w1[2] | (int) w2[2],\n"
                                "                          (int) w0[3] | (int) w1[3] | (int) w2[3]};\n"
                                "              if (mask[0] < 0 || mask[1] < 0 || mask[2] < 0 || mask[3] < 0) {\n"
                                "              for (unsigned int y = p[1]; y < p[1] + stepYSize; y++) {\n"
                                "                   for (unsigned int x = p[0]; x < p[0] + stepXSize; x++) {\n"
                                "                       float m[] =  {p1[0] - p3[0], p2[0] - p3[0],\n"
                                "                                     p1[1] - p3[1], p2[1] - p3[1]};\n"
                                "                       float u = ((p2[1] - p3[1]) * (x - p3[0]) + (p3[0] - p2[0]) * (y - p3[1])) / det(m, 2);\n"
                                "                       float v = ((p3[1] - p1[1]) * (x - p3[0]) + (p1[0] - p3[0]) * (y - p3[1])) / det(m, 2);\n"
                                "                       float w = 1 - u - v;\n"
                                "                   }\n"
                                "              }\n"
                                "           }\n"
                                "           addVec(w0, steps_e12X, w0, 4);\n"
                                "           addVec(w1, steps_e20X, w1, 4);\n"
                                "           addVec(w2, steps_e01X, w2, 4);\n"
                                "       }\n"
                                "       addVec(w0_row, steps_e12Y, w0_row, 4);\n"
                                "       addVec(w1_row, steps_e20Y, w1_row, 4);\n"
                                "       addVec(w2_row, steps_e01Y, w2_row, 4);\n"
                                "   }\n"
                                "}\n";
            cl::Program::Sources source(
                1,
                std::make_pair(prog.c_str(), prog.length()+1));
            cl::Program program(clcontext, source);
            err = program.build(devices,"",&checkErr);
            if (err != CL_SUCCESS) {
                std::string programLog;
                program.getBuildInfo(devices[0],CL_PROGRAM_BUILD_LOG, &programLog);
                std::cerr<<programLog<<std::endl;
            }
            checkErr(err, "Program::build()");
            clkvertexShader = cl::Kernel(program, "vertexShader", &err);
            checkErr(err, "Kernel::Kernel()");
            clkgeometryShader = cl::Kernel(program, "geometryShader", &err);
            checkErr(err, "Kernel::Kernel()");
            /*clkfragmentShader = cl::Kernel(program, "fragmentShader", &err);
            checkErr(err, "Kernel::Kernel()");
            err = clkfragmentShader.setArg(0, credBuffer);
            checkErr(err, "Kernel::setArg()");
            err = clkfragmentShader.setArg(1, cblueBuffer);
            checkErr(err, "Kernel::setArg()");
            err = clkfragmentShader.setArg(2, cgreenBuffer);
            checkErr(err, "Kernel::setArg()");
            err = clkfragmentShader.setArg(3, calphaBuffer);
            checkErr(err, "Kernel::setArg()");
            err = clkfragmentShader.setArg(4, cdepthBuffer);
            checkErr(err, "Kernel::setArg()");*/
            View& view = window.getView();
            ViewportMatrix vpm;
            vpm.setViewport(math::Vec3f(view.getViewport().getPosition().x, view.getViewport().getPosition().y, 0),
            math::Vec3f(view.getViewport().getWidth(), view.getViewport().getHeight(), 1));
            std::array<float, 16> projMatrix = view.getProjMatrix().getMatrix().toGlMatrix();
            std::array<float, 16> viewMatrix = view.getViewMatrix().getMatrix().toGlMatrix();
            std::array<float, 16> viewportMatrix = vpm.getMatrix().toGlMatrix();
            cprojMatrixBuffer = cl::Buffer(clcontext, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, projMatrix.size(), projMatrix.data(), &err);
            checkErr(err, "Buffer::Buffer()");
            cviewMatrixBuffer = cl::Buffer(clcontext, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, viewMatrix.size(), viewMatrix.data(), &err);
            checkErr(err, "Buffer::Buffer()");
            cvpMatrixBuffer = cl::Buffer(clcontext, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, viewportMatrix.size(), viewportMatrix.data(), &err);
            checkErr(err, "Buffer::Buffer()");
            err = clkvertexShader.setArg(25, cprojMatrixBuffer);
            checkErr(err, "Kernel::setArg()");
            err = clkvertexShader.setArg(26, cviewMatrixBuffer);
            checkErr(err, "Kernel::setArg()");
            err = clkvertexShader.setArg(27, cvpMatrixBuffer);
            checkErr(err, "Kernel::setArg()");
        }
        void CPURenderComponent::loadEntitiesOnComponent(std::vector<Entity*> vEntities) {
             if (Shader::isAvailable()) {
                batcher.clear();
                for (unsigned int i = 0; i < vEntities.size(); i++) {
                    if ( vEntities[i]->isLeaf()) {
                        for (unsigned int j = 0; j <  vEntities[i]->getFaces().size(); j++) {
                             batcher.addFace( vEntities[i]->getFaces()[j]);
                        }
                    }
                }
                m_instances = batcher.getInstances();
            }
            this->visibleEntities = vEntities;
        }
        void CPURenderComponent::clear() {
            for (unsigned int x = 0; x < size.x; x++) {
                for (unsigned int y = 0; y < size.y; y++) {
                    redBuffer[(y * size.x + x)] = 0;
                    blueBuffer[(y * size.x + x)] = 0;
                    greenBuffer[(y * size.x + x)] = 0;
                    alphaBuffer[(y * size.x + x)] = 255;
                    depthBuffer[y * size.x + x] = 0;
                }
            }
        }
        void CPURenderComponent::draw(RenderTarget& target, RenderStates states) {
             target.draw(*fbTile, states);
        }
        /*void CPURenderComponent::raytrace() {
             for (unsigned int i = 0; i < m_instances.size(); i++) {
                for (unsigned int j = 0; j < m_instances[i]->getVertexArrays().size(); j++) {
                    TransformMatrix& tm = m_instances[i]->getTransforms()[j].get();
                    for (unsigned int k = 0; k < m_instances[i]->getVertexArrays()[j]->getVertexCount(); k++) {
                        math::Matrix4f texM;
                        math::Matrix4f invTexM;
                        if (m_instances[i]->getMaterial().getTexture() != nullptr) {
                            texM = m_instances[i]->getMaterial().getTexture()->getTextureMatrix();
                            invTexM = texM.inverse();
                        }
                        if (m_instances[i]->getVertexArrays()[j]->getPrimitiveType() == sf::TrianglesFan
                            && m_instances[i]->getVertexArrays()[j]->getVertexCount() >= 3
                            && k < m_instances[i]->getVertexArrays()[j]->getVertexCount() - 2) {
                            math::Vec3f p1 = math::Vec3f((*m_instances[i]->getVertexArrays()[j])[0].position.x,(*m_instances[i]->getVertexArrays()[j])[0].position.y,(*m_instances[i]->getVertexArrays()[j])[0].position.z);
                            math::Vec3f p2 = math::Vec3f((*m_instances[i]->getVertexArrays()[j])[k+1].position.x,(*m_instances[i]->getVertexArrays()[j])[k+1].position.y,(*m_instances[i]->getVertexArrays()[j])[k+1].position.z);
                            math::Vec3f p3 = math::Vec3f((*m_instances[i]->getVertexArrays()[j])[k+2].position.x,(*m_instances[i]->getVertexArrays()[j])[k+2].position.y,(*m_instances[i]->getVertexArrays()[j])[k+2].position.z);
                            p1 = tm.transform(p1);
                            p2 = tm.transform(p2);
                            p3 = tm.transform(p3);
                            math::Vec3f orig = math::Vec3f(view.getPosition().x, view.getPosition().y, view.getViewport().getPosition().z);
                            for (unsigned int x = 0; x < size.x; x++) {
                                for (unsigned int y = 0; y < size.y; y++) {
                                   math::Vec3f ext = window.mapPixelToCoords(math::Vec3f(x, y, view.getViewport().getPosition().z + view.getViewport().getSize().z), view);
                                   math::Ray ray(orig, ext);
                                   math::Vec3f o, point;
                                   math::Vec3f r = ray.getOrig() - ray.getExt();
                                   math::Vec3f u = p2 - p1;
                                   math::Vec3f v = p3 - p1;
                                   math::Vec3f n = u.cross(v);
                                   math::Vec3f otr = ray.getOrig() - p1;
                                   point.x = n.dot2(otr) / n.dot2(r);
                                   point.y = -otr.cross(u).dot2(r) / n.dot2(r);
                                   point.z = -v.cross(otr).dot2(r) / n.dot2(r);
                                   if (0 <= point.x
                                   && 0 <= point.y && point.y <= 1
                                   &&  0 <= point.z && point.z <= 1
                                   && point.y + point.z <= 1) {
                                       //std::cout<<"intersects"<<std::endl;
                                        sf::Color c1 = (*m_instances[i]->getVertexArrays()[j])[0].color;
                                        sf::Color c2 = (*m_instances[i]->getVertexArrays()[j])[k+1].color;
                                        sf::Color c3 = (*m_instances[i]->getVertexArrays()[j])[k+2].color;
                                        math::Vec3f ct1 = texM * math::Vec3f((*m_instances[i]->getVertexArrays()[j])[0].texCoords.x,(*m_instances[i]->getVertexArrays()[j])[0].texCoords.y, 1.f);
                                        math::Vec3f ct2 = texM * math::Vec3f((*m_instances[i]->getVertexArrays()[j])[k+1].texCoords.x,(*m_instances[i]->getVertexArrays()[j])[k+1].texCoords.y, 1.f);
                                        math::Vec3f ct3 = texM * math::Vec3f((*m_instances[i]->getVertexArrays()[j])[k+2].texCoords.x,(*m_instances[i]->getVertexArrays()[j])[k+2].texCoords.y, 1.f);
                                        math::Vec3f z (p1.z, p2.z, p3.z);
                                        float bz = z.x * point.x + z.y * point.y + z.z * point.z;
                                        float actualZ = depthBuffer[y * size.x + x].z;
                                        if (bz >= view.getViewport().getPosition().z && bz <= view.getViewport().getSize().z) {
                                            math::Vec3f r = math::Vec3f (c1.r / 255.f, c2.r / 255.f, c3.r / 255.f);
                                            math::Vec3f g = math::Vec3f (c1.g / 255.f, c2.g / 255.f, c3.g / 255.f);
                                            math::Vec3f b = math::Vec3f (c1.b / 255.f, c2.b / 255.f, c3.b / 255.f);
                                            math::Vec3f a = math::Vec3f (c1.a / 255.f, c2.a / 255.f, c3.a / 255.f);
                                            std::array<math::Vec3f, 2> colors;
                                            colors[0] = math::Vec3f(r.x * point.x + r.y * point.y + r.z * point.z,
                                                                    g.x * point.x + g.y * point.y + g.z * point.z,
                                                                    b.x * point.x + b.y * point.y + b.z * point.z,
                                                                    a.x * point.x + a.y * point.y + a.z * point.z);
                                            math::Vec3f finalColor = colors[0];
                                            depthBuffer[y * size.x + x] = math::Vec3f(0, 0, bz, colors[0].w);
                                            frameBuffer[(y * size.x + x) * 4] = finalColor.x * 255;
                                            frameBuffer[(y * size.x + x) * 4 + 1] = finalColor.y * 255;
                                            frameBuffer[(y * size.x + x) * 4 + 2] = finalColor.z * 255;
                                            frameBuffer[(y * size.x + x) * 4 + 3] = finalColor.w * 255;
                                        }
                                   }
                                }
                            }
                        }
                    }
                }
             }
             winFrameBuffer.create(size.x, size.y, &frameBuffer[0]);
             fbTexture.loadFromImage(winFrameBuffer);
        }*/
        math::Vec3f CPURenderComponent::Edge::init(const math::Vec2f& v0, const math::Vec2f& v1,
                                 const math::Vec2f& origin)
        {
            // Edge setup
            int a = v0.y - v1.y, b = v1.x - v0.x;
            int c = v0.x*v1.y - v0.y*v1.x;

            // Step deltas
            oneStepX = math::Vec3f(a * stepXSize, a * stepXSize, a * stepXSize, a * stepXSize);
            oneStepY = math::Vec3f(b * stepYSize, b * stepYSize, b * stepYSize, b * stepYSize);

            // x/y values for initial pixel block
            math::Vec3f x = math::Vec3f(origin.x, origin.x, origin.x, origin.x) + math::Vec3f(0,1,2,3);
            math::Vec3f y = math::Vec3f(origin.y, origin.y, origin.y, origin.y) + math::Vec3f(0,1,2,3);

            // Edge function values at origin
            return math::Vec3f(a, a, a, a)*x + math::Vec3f(b, b, b, b)*y + math::Vec3f(c, c, c, c);
        }
        void CPURenderComponent::drawNextFrame() {
            for (unsigned int i = 0; i < m_instances.size(); i++) {
                std::vector<float> transformMatrices;
                for (unsigned int j = 0; j < m_instances[i]->getTransforms().size(); j++) {
                    float* tmatrix = m_instances[i]->getTransforms()[j].get().getGlMatrix();
                    for (unsigned int n = 0; n < 16; n++) {
                        transformMatrices.push_back(tmatrix[n]);
                    }
                }
                cl_int err;
                ctransfMatrixBuffer = cl::Buffer(clcontext, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, transformMatrices.size(), transformMatrices.data(), &err);
                checkErr(err, "Buffer::Buffer()");
                math::Matrix4f texM = (m_instances[i]->getMaterial().getTexture() != nullptr) ?
                    m_instances[i]->getMaterial().getTexture()->getTextureMatrix() :
                    math::Matrix4f();
                std::vector<float> vPosX = m_instances[i]->getVertexArray().m_vPosX;
                std::vector<float> vPosY = m_instances[i]->getVertexArray().m_vPosY;
                std::vector<float> vPosZ = m_instances[i]->getVertexArray().m_vPosZ;
                std::vector<float> vPosW = m_instances[i]->getVertexArray().m_vPosW;
                std::vector<unsigned char> vcRed = m_instances[i]->getVertexArray().m_vcRed;
                std::vector<unsigned char> vcBlue = m_instances[i]->getVertexArray().m_vcBlue;
                std::vector<unsigned char> vcGreen = m_instances[i]->getVertexArray().m_vcGreen;
                std::vector<unsigned char> vcAlpha = m_instances[i]->getVertexArray().m_vcAlpha;
                std::vector<unsigned int> ctx = m_instances[i]->getVertexArray().m_ctX;
                std::vector<unsigned int> cty = m_instances[i]->getVertexArray().m_ctY;
                std::vector<unsigned int> indexes = m_instances[i]->getVertexArray().m_indexes;
                std::vector<unsigned int> numIndexes = m_instances[i]->getVertexArray().m_numIndexes;
                std::vector<unsigned int> baseIndexes = m_instances[i]->getVertexArray().m_baseIndexes;
                std::vector<unsigned int> baseVertices = m_instances[i]->getVertexArray().m_baseVertices;
                std::array<float, 16> texMatrix = texM.toGlMatrix();
                cvposXBuffer = cl::Buffer(clcontext, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, vPosX.size(), vPosX.data(), &err);
                checkErr(err, "Buffer::Buffer()");
                cvposYBuffer = cl::Buffer(clcontext, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, vPosY.size(), vPosY.data(), &err);
                checkErr(err, "Buffer::Buffer()");
                cvposZBuffer = cl::Buffer(clcontext, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, vPosZ.size(), vPosZ.data(), &err);
                checkErr(err, "Buffer::Buffer()");
                cvposWBuffer = cl::Buffer(clcontext, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, vPosW.size(), vPosW.data(), &err);
                checkErr(err, "Buffer::Buffer()");
                cvcolRedBuffer = cl::Buffer(clcontext, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, vcRed.size(), vcRed.data(), &err);
                checkErr(err, "Buffer::Buffer()");
                cvcolGreenBuffer = cl::Buffer(clcontext, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, vcBlue.size(), vcBlue.data(), &err);
                checkErr(err, "Buffer::Buffer()");
                cvcolBlueBuffer = cl::Buffer(clcontext, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, vcGreen.size(), vcGreen.data(), &err);
                checkErr(err, "Buffer::Buffer()");
                cvcolAlphaBuffer = cl::Buffer(clcontext, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, vcAlpha.size(), vcAlpha.data(), &err);
                checkErr(err, "Buffer::Buffer()");
                cvtexcoorduBuffer = cl::Buffer(clcontext, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, ctx.size(), ctx.data(), &err);
                checkErr(err, "Buffer::Buffer()");
                cvtexcoordvBuffer = cl::Buffer(clcontext, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, cty.size(), cty.data(), &err);
                checkErr(err, "Buffer::Buffer()");
                cvindexesBuffer = cl::Buffer(clcontext, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, indexes.size(), indexes.data(), &err);
                checkErr(err, "Buffer::Buffer()");
                cnumIndexesBuffer = cl::Buffer(clcontext, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, numIndexes.size(), numIndexes.data(), &err);
                checkErr(err, "Buffer::Buffer()");
                cbaseIndexesBuffer = cl::Buffer(clcontext, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, baseIndexes.size(), baseIndexes.data(), &err);
                checkErr(err, "Buffer::Buffer()");
                cbaseVerticesBuffer = cl::Buffer(clcontext, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, baseVertices.size(), baseVertices.data(), &err);
                checkErr(err, "Buffer::Buffer()");
                ctexMatrixBuffer = cl::Buffer(clcontext, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, texMatrix.size(), texMatrix.data(), &err);
                checkErr(err, "Buffer::Buffer()");
                cl_int workgroupSize = numIndexes.size();
                std::vector<float> vvPosX (indexes.size());
                std::vector<float> vvPosY (indexes.size());
                std::vector<float> vvPosZ (indexes.size());
                std::vector<float> vvPosW (indexes.size());
                std::vector<unsigned char> vvColorRed (indexes.size());
                std::vector<unsigned char> vvColorBlue (indexes.size());
                std::vector<unsigned char> vvColorGreen (indexes.size());
                std::vector<unsigned char> vvColorAlpha (indexes.size());
                std::vector<unsigned int> vvTCU (indexes.size());
                std::vector<unsigned int> vvTCV (indexes.size());
                vcvposXBuffer = cl::Buffer(clcontext, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, vvPosX.size(), vvPosX.data(), &err);
                checkErr(err, "Buffer::Buffer()");
                vcvposYBuffer = cl::Buffer(clcontext, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, vvPosY.size(), vvPosY.data(), &err);
                checkErr(err, "Buffer::Buffer()");
                vcvposZBuffer = cl::Buffer(clcontext, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, vvPosZ.size(), vvPosZ.data(), &err);
                checkErr(err, "Buffer::Buffer()");
                vcvposWBuffer = cl::Buffer(clcontext, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, vvPosW.size(), vvPosW.data(), &err);
                checkErr(err, "Buffer::Buffer()");
                vcvcolorRedBuffer = cl::Buffer(clcontext, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, vvColorRed.size(), vvColorRed.data(), &err);
                checkErr(err, "Buffer::Buffer()");
                vcvcolorBlueBuffer = cl::Buffer(clcontext, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, vvColorBlue.size(), vvColorBlue.data(), &err);
                checkErr(err, "Buffer::Buffer()");
                vcvcolorGreenBuffer = cl::Buffer(clcontext, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, vvColorGreen.size(), vvColorGreen.data(), &err);
                checkErr(err, "Buffer::Buffer()");
                vcvcolorAlphaBuffer = cl::Buffer(clcontext, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, vvColorAlpha.size(), vvColorAlpha.data(), &err);
                checkErr(err, "Buffer::Buffer()");
                vcvTCUBuffer = cl::Buffer(clcontext, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, vvTCU.size(), vvTCU.data(), &err);
                checkErr(err, "Buffer::Buffer()");
                vcvTCVBuffer = cl::Buffer(clcontext, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, vvTCV.size(), vvTCV.data(), &err);
                checkErr(err, "Buffer::Buffer()");
                err = clkvertexShader.setArg(0, cvposXBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clkvertexShader.setArg(1, cvposYBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clkvertexShader.setArg(2, cvposZBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clkvertexShader.setArg(3, cvposWBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clkvertexShader.setArg(4, cvcolRedBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clkvertexShader.setArg(5, cvcolBlueBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clkvertexShader.setArg(6, cvcolGreenBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clkvertexShader.setArg(7, cvcolAlphaBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clkvertexShader.setArg(8, cvtexcoorduBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clkvertexShader.setArg(9, cvtexcoordvBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clkvertexShader.setArg(10, vcvposXBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clkvertexShader.setArg(11, vcvposYBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clkvertexShader.setArg(12, vcvposZBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clkvertexShader.setArg(13, vcvposWBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clkvertexShader.setArg(14, vcvcolorRedBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clkvertexShader.setArg(15, vcvcolorBlueBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clkvertexShader.setArg(16, vcvcolorGreenBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clkvertexShader.setArg(17, vcvcolorAlphaBuffer);
                checkErr(err, "Kernel::setArg()");
                 err = clkvertexShader.setArg(18, vcvTCUBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clkvertexShader.setArg(19, vcvTCVBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clkvertexShader.setArg(20, cvindexesBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clkvertexShader.setArg(21, cnumIndexesBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clkvertexShader.setArg(22, cbaseIndexesBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clkvertexShader.setArg(23, cbaseVerticesBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clkvertexShader.setArg(24, ctransfMatrixBuffer);
                checkErr(err, "Kernel::setArg()");
                cl::CommandQueue clqueue (clcontext, devices[0], 0, &err);
                checkErr(err, "CommandQueue::CommandQueue()");
                cl::Event event;
                err = clqueue.enqueueNDRangeKernel(clkvertexShader,
                                                    1, workgroupSize,
                                                    1, nullptr, &event);
                checkErr(err, "CommandQueue::enqueueNDRangeKernel()");
                event.wait();
                err = clqueue.enqueueReadBuffer(vcvposXBuffer,CL_TRUE,0,vvPosX.size(),vvPosX.data());
                checkErr(err, "CommandQueue::enqueueReadBuffer()");
                err = clqueue.enqueueReadBuffer(vcvposYBuffer,CL_TRUE,0,vvPosY.size(),vvPosY.data());
                checkErr(err, "CommandQueue::enqueueReadBuffer()");
                err = clqueue.enqueueReadBuffer(vcvposZBuffer,CL_TRUE,0,vvPosZ.size(),vvPosZ.data());
                checkErr(err, "CommandQueue::enqueueReadBuffer()");
                err = clqueue.enqueueReadBuffer(vcvposWBuffer,CL_TRUE,0,vvPosW.size(),vvPosW.data());
                checkErr(err, "CommandQueue::enqueueReadBuffer()");
                err = clqueue.enqueueReadBuffer(vcvcolorRedBuffer,CL_TRUE,0,vvColorRed.size(),vvColorRed.data());
                checkErr(err, "CommandQueue::enqueueReadBuffer()");
                err = clqueue.enqueueReadBuffer(vcvcolorBlueBuffer,CL_TRUE,0,vvColorBlue.size(),vvColorBlue.data());
                checkErr(err, "CommandQueue::enqueueReadBuffer()");
                err = clqueue.enqueueReadBuffer(vcvcolorGreenBuffer,CL_TRUE,0,vvColorGreen.size(),vvColorGreen.data());
                checkErr(err, "CommandQueue::enqueueReadBuffer()");
                err = clqueue.enqueueReadBuffer(vcvcolorAlphaBuffer,CL_TRUE,0,vvColorAlpha.size(),vvColorAlpha.data());
                checkErr(err, "CommandQueue::enqueueReadBuffer()");
                err = clqueue.enqueueReadBuffer(vcvTCUBuffer,CL_TRUE,0,vvTCU.size(),vvTCU.data());
                checkErr(err, "CommandQueue::enqueueReadBuffer()");
                err = clqueue.enqueueReadBuffer(vcvTCVBuffer,CL_TRUE,0,vvTCV.size(),vvTCV.data());
                checkErr(err, "CommandQueue::enqueueReadBuffer()");
                for (unsigned int i = 0; i < numIndexes.size(); i++) {
                    for (unsigned int j = 0; j < numIndexes[i]; j++) {
                        std::cout<<vPosX[baseVertices[i]+indexes[baseIndexes[i]+j]]<<std::endl;
                    }
                }
                for (unsigned int i = 0; i < vvPosX.size(); i++) {
                    std::cout<<vvPosX[i]<<std::endl;
                }
                /*std::vector<float> vCentersX(workgroupSize);
                std::vector<float> vCentersY(workgroupSize);
                std::vector<float> vCentersZ(workgroupSize);
                std::vector<float> vCentersW(workgroupSize);
                std::vector<unsigned int> vAvgColorRed(workgroupSize);
                std::vector<unsigned int> vAvgColorBlue(workgroupSize);
                std::vector<unsigned int> vAvgColorGreen(workgroupSize);
                std::vector<unsigned int> vAvgColorAlpha(workgroupSize);
                std::vector<int> vAvgTCU(workgroupSize);
                std::vector<int> vAvgTCV(workgroupSize);
                cvCentersXBuffer = cl::Buffer(clcontext, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, vCentersX.size(), vCentersX.data(), &err);
                checkErr(err, "Buffer::Buffer()");
                cvCentersYBuffer = cl::Buffer(clcontext, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, vCentersY.size(), vCentersY.data(), &err);
                checkErr(err, "Buffer::Buffer()");
                cvCentersZBuffer = cl::Buffer(clcontext, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, vCentersZ.size(), vCentersZ.data(), &err);
                checkErr(err, "Buffer::Buffer()");
                cvCentersWBuffer = cl::Buffer(clcontext, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, vCentersW.size(), vCentersW.data(), &err);
                checkErr(err, "Buffer::Buffer()");
                cvAvgColorRedBuffer = cl::Buffer(clcontext, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, vAvgColorRed.size(), vAvgColorRed.data(), &err);
                checkErr(err, "Buffer::Buffer()");
                cvAvgColorBlueBuffer = cl::Buffer(clcontext, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, vAvgColorBlue.size(), vAvgColorBlue.data(), &err);
                checkErr(err, "Buffer::Buffer()");
                cvAvgColorGreenBuffer = cl::Buffer(clcontext, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, vAvgColorGreen.size(), vAvgColorGreen.data(), &err);
                checkErr(err, "Buffer::Buffer()");
                cvAvgColorAlphaBuffer = cl::Buffer(clcontext, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, vAvgColorAlpha.size(), vAvgColorAlpha.data(), &err);
                checkErr(err, "Buffer::Buffer()");
                cvAvgTCUBuffer = cl::Buffer(clcontext, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, vAvgTCU.size(), vAvgTCU.data(), &err);
                checkErr(err, "Buffer::Buffer()");
                cvAvgTCVBuffer = cl::Buffer(clcontext, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, vAvgTCV.size(), vAvgTCV.data(), &err);
                checkErr(err, "Buffer::Buffer()");
                err = clkgeometryShader.setArg(0, vcvposXBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clkgeometryShader.setArg(1, vcvposYBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clkgeometryShader.setArg(2, vcvposZBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clkgeometryShader.setArg(3, vcvposWBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clkgeometryShader.setArg(4, vcvcolorRedBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clkgeometryShader.setArg(5, vcvcolorBlueBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clkgeometryShader.setArg(6, vcvcolorGreenBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clkgeometryShader.setArg(7, vcvcolorAlphaBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clkgeometryShader.setArg(8, vcvTCUBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clkgeometryShader.setArg(9, vcvTCVBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clkgeometryShader.setArg(10, cvCentersXBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clkgeometryShader.setArg(11, cvCentersYBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clkgeometryShader.setArg(12, cvCentersZBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clkgeometryShader.setArg(13, cvCentersWBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clkgeometryShader.setArg(14, cvAvgColorRedBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clkgeometryShader.setArg(15, cvAvgColorBlueBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clkgeometryShader.setArg(16, cvAvgColorGreenBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clkgeometryShader.setArg(17, cvAvgColorAlphaBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clkgeometryShader.setArg(18, cvAvgTCUBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clkgeometryShader.setArg(19, cvAvgTCVBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clkgeometryShader.setArg(20, cnumIndexesBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clqueue.enqueueNDRangeKernel(clkgeometryShader,
                                                    1, workgroupSize,
                                                    1, NULL, &event);
                checkErr(err, "CommandQueue::enqueueNDRangeKernel()");
                event.wait();
                err = clqueue.enqueueReadBuffer(cvCentersXBuffer,CL_TRUE,0,workgroupSize,vCentersX.data());
                checkErr(err, "CommandQueue::enqueueReadBuffer()");
                err = clqueue.enqueueReadBuffer(cvCentersYBuffer,CL_TRUE,0,workgroupSize,vCentersY.data());
                checkErr(err, "CommandQueue::enqueueReadBuffer()");
                err = clqueue.enqueueReadBuffer(cvCentersZBuffer,CL_TRUE,0,workgroupSize,vCentersZ.data());
                checkErr(err, "CommandQueue::enqueueReadBuffer()");
                err = clqueue.enqueueReadBuffer(cvCentersWBuffer,CL_TRUE,0,workgroupSize,vCentersW.data());
                checkErr(err, "CommandQueue::enqueueReadBuffer()");
                err = clqueue.enqueueReadBuffer(cvAvgColorRedBuffer,CL_TRUE,0,workgroupSize,vAvgColorRed.data());
                checkErr(err, "CommandQueue::enqueueReadBuffer()");
                err = clqueue.enqueueReadBuffer(cvAvgColorBlueBuffer,CL_TRUE,0,workgroupSize,vAvgColorBlue.data());
                checkErr(err, "CommandQueue::enqueueReadBuffer()");
                err = clqueue.enqueueReadBuffer(cvAvgColorGreenBuffer,CL_TRUE,0,workgroupSize,vAvgColorGreen.data());
                checkErr(err, "CommandQueue::enqueueReadBuffer()");
                err = clqueue.enqueueReadBuffer(cvAvgColorAlphaBuffer,CL_TRUE,0,workgroupSize,vAvgColorAlpha.data());
                checkErr(err, "CommandQueue::enqueueReadBuffer()");
                err = clqueue.enqueueReadBuffer(cvAvgTCUBuffer,CL_TRUE,0,workgroupSize,vAvgTCU.data());
                checkErr(err, "CommandQueue::enqueueReadBuffer()");
                err = clqueue.enqueueReadBuffer(cvAvgTCVBuffer,CL_TRUE,0,workgroupSize,vAvgTCV.data());
                checkErr(err, "CommandQueue::enqueueReadBuffer()");
                std::vector<float> tPosX (indexes.size()*3);
                std::vector<float> tPosY (indexes.size()*3);
                std::vector<float> tPosZ (indexes.size()*3);
                std::vector<float> tPosW (indexes.size()*3);
                std::vector<unsigned int> tColorRed (indexes.size()*3);
                std::vector<unsigned int> tColorBlue (indexes.size()*3);
                std::vector<unsigned int> tColorGreen (indexes.size()*3);
                std::vector<unsigned int> tColorAlpha (indexes.size()*3);
                std::vector<int> tTCU (indexes.size()*3);
                std::vector<int> tTCV (indexes.size()*3);
                ctposXBuffer = cl::Buffer(clcontext, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, tPosX.size(), tPosX.data(), &err);
                checkErr(err, "Buffer::Buffer()");
                ctposYBuffer = cl::Buffer(clcontext, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, tPosY.size(), tPosY.data(), &err);
                checkErr(err, "Buffer::Buffer()");
                ctposZBuffer = cl::Buffer(clcontext, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, tPosZ.size(), tPosZ.data(), &err);
                checkErr(err, "Buffer::Buffer()");
                ctposWBuffer = cl::Buffer(clcontext, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, tPosW.size(), tPosW.data(), &err);
                checkErr(err, "Buffer::Buffer()");
                ctcolRedBuffer = cl::Buffer(clcontext, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, tColorRed.size(), tColorRed.data(), &err);
                checkErr(err, "Buffer::Buffer()");
                ctcolGreenBuffer = cl::Buffer(clcontext, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, tColorBlue.size(), tColorBlue.data(), &err);
                checkErr(err, "Buffer::Buffer()");
                ctcolBlueBuffer = cl::Buffer(clcontext, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, tColorGreen.size(), tColorGreen.data(), &err);
                checkErr(err, "Buffer::Buffer()");
                ctcolAlphaBuffer = cl::Buffer(clcontext, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, tColorAlpha.size(), tColorAlpha.data(), &err);
                checkErr(err, "Buffer::Buffer()");
                ctTCUBuffer = cl::Buffer(clcontext, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, tTCU.size(), tTCU.data(), &err);
                checkErr(err, "Buffer::Buffer()");
                ctTCVBuffer = cl::Buffer(clcontext, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, tTCV.size(), tTCV.data(), &err);
                checkErr(err, "Buffer::Buffer()");
                err = clktesslationShader.setArg(0, vcvposXBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clktesslationShader.setArg(1, vcvposYBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clktesslationShader.setArg(2, vcvposZBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clktesslationShader.setArg(3, vcvposWBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clktesslationShader.setArg(4, vcvcolorRedBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clktesslationShader.setArg(5, vcvcolorBlueBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clkgeometryShader.setArg(6, vcvcolorGreenBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clktesslationShader.setArg(7, vcvcolorAlphaBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clktesslationShader.setArg(8, vcvTCUBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clktesslationShader.setArg(9, vcvTCVBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clktesslationShader.setArg(10, cvCentersXBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clktesslationShader.setArg(11, cvCentersYBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clktesslationShader.setArg(12, cvCentersZBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clktesslationShader.setArg(13, cvCentersWBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clktesslationShader.setArg(14, cvAvgColorRedBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clktesslationShader.setArg(15, cvAvgColorBlueBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clktesslationShader.setArg(16, cvAvgColorGreenBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clktesslationShader.setArg(17, cvAvgColorAlphaBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clktesslationShader.setArg(18, cvAvgTCUBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clktesslationShader.setArg(19, cvAvgTCVBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clktesslationShader.setArg(20, ctposXBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clktesslationShader.setArg(21, ctposYBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clktesslationShader.setArg(22, ctposZBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clktesslationShader.setArg(23, ctposWBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clktesslationShader.setArg(24, ctcolRedBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clktesslationShader.setArg(25, ctcolBlueBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clktesslationShader.setArg(26, ctcolGreenBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clktesslationShader.setArg(27, ctcolAlphaBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clktesslationShader.setArg(28, ctTCUBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clktesslationShader.setArg(29, ctTCVBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clktesslationShader.setArg(30, cnumIndexesBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clqueue.enqueueNDRangeKernel(clktesslationShader,
                                                    1, workgroupSize,
                                                    1, NULL, &event);
                checkErr(err, "CommandQueue::enqueueNDRangeKernel()");
                event.wait();
                err = clqueue.enqueueReadBuffer(ctposXBuffer,CL_TRUE,0,workgroupSize,tPosX.data());
                checkErr(err, "CommandQueue::enqueueReadBuffer()");
                err = clqueue.enqueueReadBuffer(ctposYBuffer,CL_TRUE,0,workgroupSize,tPosY.data());
                checkErr(err, "CommandQueue::enqueueReadBuffer()");
                err = clqueue.enqueueReadBuffer(ctposZBuffer,CL_TRUE,0,workgroupSize,tPosZ.data());
                checkErr(err, "CommandQueue::enqueueReadBuffer()");
                err = clqueue.enqueueReadBuffer(ctposWBuffer,CL_TRUE,0,workgroupSize,tPosW.data());
                checkErr(err, "CommandQueue::enqueueReadBuffer()");
                err = clqueue.enqueueReadBuffer(ctcolRedBuffer,CL_TRUE,0,workgroupSize,tColorRed.data());
                checkErr(err, "CommandQueue::enqueueReadBuffer()");
                err = clqueue.enqueueReadBuffer(ctcolBlueBuffer,CL_TRUE,0,workgroupSize,tColorBlue.data());
                checkErr(err, "CommandQueue::enqueueReadBuffer()");
                err = clqueue.enqueueReadBuffer(ctcolGreenBuffer,CL_TRUE,0,workgroupSize,tColorGreen.data());
                checkErr(err, "CommandQueue::enqueueReadBuffer()");
                err = clqueue.enqueueReadBuffer(ctcolAlphaBuffer,CL_TRUE,0,workgroupSize,tColorAlpha.data());
                checkErr(err, "CommandQueue::enqueueReadBuffer()");
                err = clqueue.enqueueReadBuffer(ctTCUBuffer,CL_TRUE,0,workgroupSize,tTCU.data());
                checkErr(err, "CommandQueue::enqueueReadBuffer()");
                err = clqueue.enqueueReadBuffer(ctTCVBuffer,CL_TRUE,0,workgroupSize,tTCV.data());
                checkErr(err, "CommandQueue::enqueueReadBuffer()");*/

            }
        }
        /**
        void CPURenderComponent::drawNextFrame() {
            for (unsigned int i = 0; i < m_instances.size(); i++) {
                for (unsigned int j = 0; j < m_instances[i]->getVertexArrays().size(); j++) {
                    TransformMatrix& tm = m_instances[i]->getTransforms()[j].get();
                    for (unsigned int k = 0; k < m_instances[i]->getVertexArrays()[j]->getVertexCount(); k++) {
                        math::Matrix4f texM;
                        math::Matrix4f invTexM;
                        if (m_instances[i]->getMaterial().getTexture() != nullptr) {
                            texM = m_instances[i]->getMaterial().getTexture()->getTextureMatrix();
                            invTexM = texM.inverse();
                        }
                        if (m_instances[i]->getVertexArrays()[j]->getPrimitiveType() == sf::TrianglesFan
                            && m_instances[i]->getVertexArrays()[j]->getVertexCount() >= 3
                            && k < m_instances[i]->getVertexArrays()[j]->getVertexCount() - 2) {
                            math::Vec3f p1 = math::Vec3f((*m_instances[i]->getVertexArrays()[j])[0].position.x,(*m_instances[i]->getVertexArrays()[j])[0].position.y,(*m_instances[i]->getVertexArrays()[j])[0].position.z);
                            math::Vec3f p2 = math::Vec3f((*m_instances[i]->getVertexArrays()[j])[k+1].position.x,(*m_instances[i]->getVertexArrays()[j])[k+1].position.y,(*m_instances[i]->getVertexArrays()[j])[k+1].position.z);
                            math::Vec3f p3 = math::Vec3f((*m_instances[i]->getVertexArrays()[j])[k+2].position.x,(*m_instances[i]->getVertexArrays()[j])[k+2].position.y,(*m_instances[i]->getVertexArrays()[j])[k+2].position.z);
                            p1 = tm.transform(p1);
                            p2 = tm.transform(p2);
                            p3 = tm.transform(p3);
                            p1 = window.mapCoordsToPixel(p1, view);
                            p2 = window.mapCoordsToPixel(p2, view);
                            p3 = window.mapCoordsToPixel(p3, view);
                            sf::Color c1 = (*m_instances[i]->getVertexArrays()[j])[0].color;
                            sf::Color c2 = (*m_instances[i]->getVertexArrays()[j])[k+1].color;
                            sf::Color c3 = (*m_instances[i]->getVertexArrays()[j])[k+2].color;
                            math::Vec3f ct1 = texM * math::Vec3f((*m_instances[i]->getVertexArrays()[j])[0].texCoords.x,(*m_instances[i]->getVertexArrays()[j])[0].texCoords.y, 1.f);
                            math::Vec3f ct2 = texM * math::Vec3f((*m_instances[i]->getVertexArrays()[j])[k+1].texCoords.x,(*m_instances[i]->getVertexArrays()[j])[k+1].texCoords.y, 1.f);
                            math::Vec3f ct3 = texM * math::Vec3f((*m_instances[i]->getVertexArrays()[j])[k+2].texCoords.x,(*m_instances[i]->getVertexArrays()[j])[k+2].texCoords.y, 1.f);
                            std::array<math::Vec3f, 3> vertices = {math::Vec3f(p1.x, p1.y, 0),math::Vec3f(p2.x, p2.y, 0), math::Vec3f(p3.x, p3.y, 0)};
                            std::array<std::array<float, 2>, 3> extends = math::Computer::getExtends(vertices);
                            int minX = (extends[0][0] < 0) ? 0 : extends[0][0];
                            int minY = (extends[1][0] < 0) ? 0 : extends[1][0];
                            int maxX = (extends[0][1] >= size.x) ? size.x-1 : extends[0][1];
                            int maxY = (extends[1][0] >= size.y) ? size.y-1 : extends[1][1];
                            int a01 = p1.y - p2.y, b01 = p2.x - p1.x;
                            int a12 = p2.y - p3.y, b12 = p3.x - p2.x;
                            int a20 = p3.y - p1.y, b20 = p1.x - p3.x;
                            math::Vec2f p(minX, minY);
                            Edge e01, e12, e20;
                            math::Vec3f w0_row = e12.init(p2, p3, p);
                            math::Vec3f w1_row = e20.init(p3, p1, p);
                            math::Vec3f w2_row = e01.init(p1, p2, p);
                            for (p.y = minY; p.y < maxY; p.y += Edge::stepYSize) {
                                math::Vec3f w0 = w0_row;
                                math::Vec3f w1 = w1_row;
                                math::Vec3f w2 = w2_row;
                                for (p.x = minX; p.x <= maxX; p.x += Edge::stepXSize) {
                                    math::Vec3f mask ((int) w0.x | (int) w1.x | (int) w2.x,
                                                      (int) w0.y | (int) w1.y | (int) w2.y,
                                                      (int) w0.z | (int) w1.z | (int) w2.z,
                                                      (int) w0.w | (int) w1.w | (int) w2.w);
                                    if (mask.x < 0 || mask.y < 0 || mask.z < 0 || mask.w < 0) {
                                        for (unsigned int y = p.y; y < p.y + Edge::stepYSize; y++) {
                                            for (unsigned int x = p.x; x < p.x + Edge::stepXSize; x++) {
                                                math::Matrix2f m1(p1.x - p3.x, p2.x - p3.x,
                                                                  p1.y - p3.y, p2.y - p3.y);
                                                float u = ((p2.y - p3.y) * (x - p3.x) + (p3.x - p2.x) * (y - p3.y)) / m1.getDet();
                                                float v = ((p3.y - p1.y) * (x - p3.x) + (p1.x - p3.x) * (y - p3.y)) / m1.getDet();
                                                float w = 1 - u - v;
                                                math::Vec3f z (p1.z, p2.z, p3.z);
                                                float bz = z.x * u + z.y * v + z.z * w;
                                                float actualZ = depthBuffer[y * size.x + x].z;

                                                if (bz >= view.getViewport().getPosition().z && bz <= view.getViewport().getSize().z) {
                                                    math::Vec3f tcx = math::Vec3f(ct1.x, ct2.x, ct3.x);
                                                    math::Vec3f tcy = math::Vec3f(ct1.y, ct2.y, ct3.y);
                                                    math::Vec3f tc = invTexM * math::Vec3f(tcx.x * u + tcx.y * v + tcx.z * w,
                                                                                 tcy.x * u + tcy.y * v + tcy.z * w, 1.f);
                                                    math::Vec3f texColor(1.f, 1.f, 1.f, 1.f);
                                                    if (m_instances[i]->getMaterial().getTexture() != nullptr) {
                                                        const sf::Image& texImg = m_instances[i]->getMaterial().getTexture()->getImage();
                                                        tc.x = math::Math::clamp(tc.x, 0, texImg.getSize().x);
                                                        tc.y = math::Math::clamp(tc.y, 0, texImg.getSize().y);
                                                        sf::Color color = texImg.getPixel(tc.x, tc.y);
                                                        texColor = math::Vec3f (color.r / 255.f, color.g / 255.f, color.b / 255.f, color.a / 255.f);
                                                    }
                                                    math::Vec3f r = math::Vec3f (c1.r / 255.f, c2.r / 255.f, c3.r / 255.f);
                                                    math::Vec3f g = math::Vec3f (c1.g / 255.f, c2.g / 255.f, c3.g / 255.f);
                                                    math::Vec3f b = math::Vec3f (c1.b / 255.f, c2.b / 255.f, c3.b / 255.f);
                                                    math::Vec3f a = math::Vec3f (c1.a / 255.f, c2.a / 255.f, c3.a / 255.f);
                                                    std::array<math::Vec3f, 2> colors;
                                                    colors[0] = math::Vec3f(frameBuffer[(y * size.x + x)*4] / 255.f,
                                                                                       frameBuffer[(y * size.x + x)*4+1] / 255.f,
                                                                                       frameBuffer[(y * size.x + x)*4+2] / 255.f,
                                                                                       depthBuffer[y * size.x + x].w);
                                                    colors[1] = math::Vec3f(r.x * u + r.y * v + r.z * w,
                                                                            g.x * u + g.y * v + g.z * w,
                                                                            b.x * u + b.y * v + b.z * w,
                                                                            a.x * u + a.y * v + a.z * w) * texColor;
                                                    bool src=(bz >= actualZ);
                                                    float z[2];
                                                    z[0] = actualZ;
                                                    z[1] = bz;
                                                    math::Vec3f finalColor = colors[src] * colors[src].w + colors[!src] * (1 - colors[src].w);
                                                    depthBuffer[y * size.x + x] = math::Vec3f(0, 0, z[src], colors[src].w);
                                                    frameBuffer[(y * size.x + x) * 4] = finalColor.x * 255;
                                                    frameBuffer[(y * size.x + x) * 4 + 1] = finalColor.y * 255;
                                                    frameBuffer[(y * size.x + x) * 4 + 2] = finalColor.z * 255;
                                                    frameBuffer[(y * size.x + x) * 4 + 3] = finalColor.w * 255;
                                                }
                                            }
                                        }
                                    }
                                    w0 += e12.oneStepX;
                                    w1 += e20.oneStepX;
                                    w2 += e01.oneStepX;
                                }
                                w0_row += e12.oneStepY;
                                w1_row += e20.oneStepY;
                                w2_row += e01.oneStepY;
                            }
                        }
                    }
                }
            }
            winFrameBuffer.create(size.x, size.y, &frameBuffer[0]);
            fbTexture.loadFromImage(winFrameBuffer);
        }*/
    }
}
