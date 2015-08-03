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
                                "}\n*/"
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
                                                           "__global float* outvPosX, __global float* outvPosY, __global float* outvPosZ, __global float* outvPosW,"
                                                           "__global unsigned int* indices,  __global unsigned int* numIndices, __global unsigned int* baseIndices,\n"
                                                           "__global unsigned int* baseVertices,  __global unsigned int* nbVerticesPerFace,\n"
                                                           "__global float* transfMatrices, __global float* projMatrix, __global float* viewMatrix,\n"
                                                           "__global float* viewportMatrix) {\n"
                                    "size_t tid = get_global_id(0);\n"
                                    "int instanceID = tid / nbVerticesPerFace[0];\n"
                                    "int offset = tid % nbVerticesPerFace[0];\n"
                                    "float transfMatrix[16];\n"
                                    "float position[4] = {vPosX[baseVertices[instanceID]+indices[baseIndices[instanceID]+offset]], vPosY[baseVertices[instanceID]+indices[baseIndices[instanceID]+offset]],\n"
                                    "vPosZ[baseVertices[instanceID]+indices[baseIndices[instanceID]+offset]], vPosW[baseVertices[instanceID]+indices[baseIndices[instanceID]+offset]]};\n"
                                    "for (int i = 0; i < 16; i++) {\n"
                                        "transfMatrix[i] = transfMatrices[instanceID*16+i];\n"
                                    "}\n"
                                    "float worldcoords[4];\n;"
                                    "multMatVec(transfMatrix, position, worldcoords, 4);\n"
                                    "float viewcoords[4];\n"
                                    "multMatVec(viewMatrix, worldcoords, viewcoords, 4);\n"
                                    "float clipcoords[4];\n"
                                    "multMatVec(projMatrix, viewcoords, clipcoords, 4);\n"
                                    "float ndcCoords[] = {clipcoords[0] / clipcoords[3], clipcoords[1] / clipcoords[3], clipcoords[2] / clipcoords[3], 1 / clipcoords[3]};\n"
                                    "float finalPos[4];"
                                    "multMatVec(viewportMatrix, ndcCoords, finalPos, 4);\n"
                                    "outvPosX[baseVertices[instanceID]+indices[baseIndices[instanceID]+offset]] = finalPos[0];\n"
                                    "outvPosY[baseVertices[instanceID]+indices[baseIndices[instanceID]+offset]] = finalPos[1];\n"
                                    "outvPosZ[baseVertices[instanceID]+indices[baseIndices[instanceID]+offset]] = finalPos[2];\n"
                                    "outvPosW[baseVertices[instanceID]+indices[baseIndices[instanceID]+offset]] = finalPos[3];\n"
                                "}\n"
                                "__kernel void geometryShader (__global float* vPosX, __global float* vPosY, __global float* vPosZ, __global float* vPosW,\n"
                                "                              __global float* outvPosX, __global float* outvPosY, __global float* outvPosZ, __global float* outvPosW,\n"
                                "                              __global unsigned int* indices,  __global unsigned int* numIndices, __global unsigned int* baseIndices,\n"
                                "                              __global unsigned int* baseVertices, __global unsigned int* nbVerticesPerFace) {\n"
                                "   size_t tid = get_global_id(0);\n"
                                "   int instanceID = tid / nbVerticesPerFace[0];\n"
                                "   int offset = tid % nbVerticesPerFace[0];\n"
                                "   if (offset == nbVerticesPerFace[0] - 1) {\n"
                                "       if (nbVerticesPerFace[1] == 1) {\n"
                                "           float centerLineX=0, centerLineY=0, centerLineZ=0, centerLineW=0;\n"
                                "           float v1[] = {vPosX[baseVertices[instanceID]+indices[baseIndices[instanceID]+offset]], vPosY[baseVertices[instanceID]+indices[baseIndices[instanceID]+offset]],\n"
                                "           vPosZ[baseVertices[instanceID]+indices[baseIndices[instanceID]+offset]],vPosW[baseVertices[instanceID]+indices[baseIndices[instanceID]+offset]]};\n"
                                "           float v2[] = {vPosX[baseVertices[0]+indices[baseIndices[0]]], vPosY[baseVertices[0]+indices[baseIndices[0]]],\n"
                                "           vPosZ[baseVertices[0]+indices[baseIndices[0]]],vPosW[baseVertices[0]+indices[baseIndices[0]]]};\n"
                                "           centerLineX = (v1[0] + v2[0]) * 0.5;\n"
                                "           centerLineY = (v1[1] + v2[1]) * 0.5;\n"
                                "           centerLineZ = (v1[2] + v2[2]) * 0.5;\n"
                                "           centerLineW = (v1[3] + v2[3]) * 0.5;\n"
                                "           outvPosX[tid*2+1] = centerLineX;\n"
                                "           outvPosY[tid*2+1] = centerLineY;\n"
                                "           outvPosZ[tid*2+1] = centerLineZ;\n"
                                "           outvPosW[tid*2+1] = centerLineW;\n"
                                "       } else {\n"
                                "           float v[] = {vPosX[baseVertices[instanceID]+indices[baseIndices[instanceID]+offset]], vPosY[baseVertices[instanceID]+indices[baseIndices[instanceID]+offset]],\n"
                                "           vPosZ[baseVertices[instanceID]+indices[baseIndices[instanceID]+offset]],vPosW[baseVertices[instanceID]+indices[baseIndices[instanceID]+offset]]};\n"
                                "           outvPosX[tid*2] = v[0];\n"
                                "           outvPosY[tid*2] = v[1];\n"
                                "           outvPosZ[tid*2] = v[2];\n"
                                "           outvPosW[tid*2] = v[3];\n"
                                "       }\n"
                                "   } else {\n"
                                "       size_t tid2 = tid + 1;\n"
                                "       int instanceID2 = tid2 / nbVerticesPerFace[0];\n"
                                "       int offset2 = tid2 % nbVerticesPerFace[0];\n"
                                "       float centerLineX=0, centerLineY=0, centerLineZ=0, centerLineW=0;\n"
                                "       float v1[] = {vPosX[baseVertices[instanceID]+indices[baseIndices[instanceID]+offset]], vPosY[baseVertices[instanceID]+indices[baseIndices[instanceID]+offset]],\n"
                                "       vPosZ[baseVertices[instanceID]+indices[baseIndices[instanceID]+offset]],vPosW[baseVertices[instanceID]+indices[baseIndices[instanceID]+offset]]};\n"
                                "       float v2[] = {vPosX[baseVertices[instanceID2]+indices[baseIndices[instanceID2]+offset2]], vPosY[baseVertices[instanceID2]+indices[baseIndices[instanceID2]+offset2]],\n"
                                "       vPosZ[baseVertices[instanceID2]	+ indices[baseIndices[instanceID2]+offset2]],vPosW[baseVertices[instanceID2]+indices[baseIndices[instanceID2]+offset2]]};\n"
                                "       centerLineX = (v1[0] + v2[0]) * 0.5;\n"
                                "       centerLineY = (v1[1] + v2[1]) * 0.5;\n"
                                "       centerLineZ = (v1[2] + v2[2]) * 0.5;\n"
                                "       centerLineW = (v1[3] + v2[3]) * 0.5;\n"
                                "       outvPosX[tid*2] = v1[0];\n"
                                "       outvPosY[tid*2] = v1[1];\n"
                                "       outvPosZ[tid*2] = v1[2];\n"
                                "       outvPosW[tid*2] = v1[3];\n"
                                "       outvPosX[tid*2+1] = centerLineX;\n"
                                "       outvPosY[tid*2+1] = centerLineY;\n"
                                "       outvPosZ[tid*2+1] = centerLineZ;\n"
                                "       outvPosW[tid*2+1] = centerLineW;\n"
                                "    }"
                                "}\n"
                                "__kernel void tesslationShader(__global float* vPosX, __global float* vPosY, __global float* vPosZ, __global float* vPosW,\n"
                                "                               __global unsigned int* nbVerticesPerFace, __global float* centersX, __global float* centersY,\n"
                                "                               __global float* centersZ, __global float* centersW) {\n"
                                "   size_t tid = get_global_id(0);\n"
                                "   float centerX=0, centerY=0, centerZ=0, centerW = 0;\n"
                                "   for (int i = 0; i < nbVerticesPerFace[2]; i++) {\n"
                                "        centerX += vPosX[tid*nbVerticesPerFace[2]+i];\n"
                                "        centerY += vPosY[tid*nbVerticesPerFace[2]+i];\n"
                                "        centerZ += vPosZ[tid*nbVerticesPerFace[2]+i];\n"
                                "        centerW += vPosW[tid*nbVerticesPerFace[2]+i];\n"
                                "   }\n"
                                "   centerX = centerX / nbVerticesPerFace[2];\n"
                                "   centerY = centerY / nbVerticesPerFace[2];\n"
                                "   centerZ = centerZ / nbVerticesPerFace[2];\n"
                                "   centerW = centerW / nbVerticesPerFace[2];\n"
                                "   centersX[tid] = centerX;\n"
                                "   centersY[tid] = centerY;\n"
                                "   centersZ[tid] = centerZ;\n"
                                "   centersW[tid] = centerW;\n"
                                "}";
                                /*__kernel void fragmentShader(__global float* vPosX, __global float* vPosY, __global float* vPosZ, __global float* vPosW,
                                                 __global unsigned int* vColRed, __global unsigned int* vColBlue, __global unsigned int* vColGreen,
                                                 __global unsigned int* vColAlpha, __global int* vTCU, __global int* vTCV,
                                                 __global float* redBuffer, __global float* blueBuffer, __global float* greenBuffer, __global float* alphaBuffer,
                                                 __global float* depthBuffer, __global unsigned float* centersX, __global unsigned float* centersY, __global float* centersZ,
                                                 __global unsigned float* centersW, __global float16 viewportMatrix, __global float16 texMatrix) {
                                    size_t tid = get_global_id(0);
                                    float4 p1, p2, p3;
                                    p1 = (float4) (centersX[tid], centersY[tid], centersZ[tid], centersW[tid]);
                                    int max = (nbVerticesPerFace[1]) ? nbVerticesPerFace[2] - 1 : nbVerticesPerFace[2] - 2;
                                    for (unsigned int i = 0; i < max; i++) {
                                        int i2 = (i + 1 >= nbVerticesPerFace[2]) ? 0 : i+1;
                                        p2 = (float4) (vPosX[tid*nbVerticesPerFaces[2]+i],vPosY[tid*nbVerticesPerFace[2]+i],
                                                       vPosZ[tid*nbVerticesPerFaces[2]+i],vPosW[tid*nbVerticesPerFaces[2]+i]);
                                        p3 = (float4) (vPosX[tid*nbVerticesPerFaces[2]+i2],vPosY[tid*nbVerticesPerFace[2]+i2],
                                                       vPosZ[tid*nbVerticesPerFaces[2]+i2],vPosW[tid*nbVerticesPerFaces[2]+i2]);
                                        if (!equal(p1, p2) && !equal(p1, p3) && !equal(p2, p3)) {
                                            float3 xs (p1.x, p2.x, p3.x);
                                            float3 ys (p1.y, p2.y, p3.y);
                                            float minX = min(xs);
                                            float minY = min(ys);
                                                float maxX = max(xs);
                                            float maxY = max(ys);
                                            int a01 = p1.y - p2.y, b01 = p2.x - p1.x;
                                                int a12 = p2.y - p3.y, b12 = p3.x - p2.x;
                                                int a20 = p3.y - p1.y, b20 = p1.x - p3.x;
                                                float2 p(minX, minY);
                                            float4 steps_e12X, steps_e20X, steps_e01X,
                                                   steps_e12Y, steps_e20Y, steps_e01Y;
                                            float4 w0_row = initEdge(p2, p3, p, steps_e12X, steps_e12Y);
                                            float4 w1_row = initEdge(p3, p1, p, steps_e20X, steps_e20Y);
                                            float4 w2_row = initEdge(p1, p2, p, steps_e01X, steps_e01Y);
                                            int viewportWidth = viewportMatrix[0] * 2;
                                            int viewportHeight = viewportMatrix[5] * 2;
                                            int viewportDepth = viewportMatrix[10] * 2;
                                            int viewportX = viewportMatrix[3] - viewportMatrix[0];
                                            int viewportY = viewportMatrix[7] - viewportMatrix[5];
                                            int viewportZ = viewportMatrix[11] - viewportMatrix[10];
                                            for (p.y = minY; p.y < maxY; p += stepYSize) {
                                                float4 w0 = w0_row;
                                                float4 w1 = w1_row;
                                                float4 w2 = w2_row;
                                                for (p.x = minX; p.x < maxX; p.x += stepXSize) {
                                                    int4 mask ((int) w0.x | (int) w1.x | (int) w2.x,
                                                           (int) w0.y | (int) w1.y | (int) w2.y,
                                                           (int) w0.z | (int) w1.z | (int) w2.z,
                                                           (int) w0.w | (int) w1.w | (int) w2.w);
                                                    if (mask.x < 0 || mask.y < 0 || mask.z < 0 || mask.w < 0) {
                                                        for (unsigned int y = p.y; y < p.y + Edge::stepYSize; y++) {
                                                            for (unsigned int x = p.x; x < p.x + Edge::stepXSize; x++) {
                                                                float4 m  (p1.x - p3.x, p2.x - p3.x,
                                                                          p1.y - p3.y, p2.y - p3.y);
                                                                float u = ((p2.y - p3.y) * (x - p3.x) + (p3.x - p2.x) * (y - p3.y)) / det(m);
                                                                float v = ((p3.y - p1.y) * (x - p3.x) + (p1.x - p3.x) * (y - p3.y)) / det(m);
                                                                float w = 1 - u - v;
                                                            }
                                                        }
                                                    }
                                                    w0 += steps_e12X;
                                                    w1 += steps_e20X;
                                                    w2 += steps_e01X;
                                                }
                                                w0_row += steps_e12Y;
                                                w1_row += steps_e20Y;
                                                w2_row += steps_e01Y;
                                            }
                                        }
                                    }
                                }*/
            cl::Program::Sources source(
                1,
                std::make_pair(prog.c_str(), prog.length()+1));
            cl::Program program(clcontext, source);
            err = program.build(devices,"",&checkErr);
            if (err != CL_SUCCESS) {
                std::string programLog;
                program.getBuildInfo(devices[0],CL_PROGRAM_BUILD_LOG, &programLog);
                std::ofstream file("buildLog.log");
                file<<programLog;
                file.close();
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
            err = clkvertexShader.setArg(14, cprojMatrixBuffer);
            checkErr(err, "Kernel::setArg()");
            err = clkvertexShader.setArg(15, cviewMatrixBuffer);
            checkErr(err, "Kernel::setArg()");
            err = clkvertexShader.setArg(16, cvpMatrixBuffer);
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
                std::vector<unsigned int> nbVerticesPerFace(3);
                std::array<float, 16> texMatrix = texM.toGlMatrix();
                nbVerticesPerFace[0] = m_instances[i]->getVertexArray().nbVerticesPerFace;
                nbVerticesPerFace[1] = m_instances[i]->getVertexArray().isLoop();
                nbVerticesPerFace[2] = (m_instances[i]->getVertexArray().isLoop()) ? m_instances[i]->getVertexArray().nbVerticesPerFace * 2 : m_instances[i]->getVertexArray().nbVerticesPerFace * 2 - 1;
                VertexArray va = m_instances[i]->getVertexArray();
                for (unsigned int j = 0; j < m_instances[i]->getVertexArray().getVertexCount(); j++) {
                    int instanceID = j / nbVerticesPerFace[0];
                    int offset = j % nbVerticesPerFace[0];
                    std::cout<<"index : "<<baseVertices[instanceID]+indexes[baseIndexes[instanceID]+offset]<<std::endl;
                    std::cout<<m_instances[i]->getVertexArray()[baseVertices[instanceID]+indexes[baseIndexes[instanceID]+offset]].position.x<<std::endl;
                }
                cvposXBuffer = cl::Buffer(clcontext, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, vPosX.size(), vPosX.data(), &err);
                checkErr(err, "Buffer::Buffer()");
                cvposYBuffer = cl::Buffer(clcontext, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, vPosY.size(), vPosY.data(), &err);
                checkErr(err, "Buffer::Buffer()");
                cvposZBuffer = cl::Buffer(clcontext, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, vPosZ.size(), vPosZ.data(), &err);
                checkErr(err, "Buffer::Buffer()");
                cvposWBuffer = cl::Buffer(clcontext, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, vPosW.size(), vPosW.data(), &err);
                checkErr(err, "Buffer::Buffer()");
                cvcolredBuffer = cl::Buffer(clcontext, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, vcRed.size(), vcRed.data(), &err);
                checkErr(err, "Buffer::Buffer()");
                cvcolgreenBuffer = cl::Buffer(clcontext, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, vcBlue.size(), vcBlue.data(), &err);
                checkErr(err, "Buffer::Buffer()");
                cvcolblueBuffer = cl::Buffer(clcontext, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, vcGreen.size(), vcGreen.data(), &err);
                checkErr(err, "Buffer::Buffer()");
                cvcolalphaBuffer = cl::Buffer(clcontext, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, vcAlpha.size(), vcAlpha.data(), &err);
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
                cNbVerticesPerFaces = cl::Buffer(clcontext, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, nbVerticesPerFace.size(), nbVerticesPerFace.data(), &err);
                checkErr(err, "Buffer::Buffer()");
                ctexMatrixBuffer = cl::Buffer(clcontext, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, texMatrix.size(), texMatrix.data(), &err);
                checkErr(err, "Buffer::Buffer()");
                cl_int workgroupSize = m_instances[i]->getVertexArray().getVertexCount();
                std::vector<float> vvPosX (workgroupSize);
                std::vector<float> vvPosY (workgroupSize);
                std::vector<float> vvPosZ (workgroupSize);
                std::vector<float> vvPosW (workgroupSize);
                vcvposXBuffer = cl::Buffer(clcontext, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, vvPosX.size(), vvPosX.data(), &err);
                checkErr(err, "Buffer::Buffer()");
                vcvposYBuffer = cl::Buffer(clcontext, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, vvPosY.size(), vvPosY.data(), &err);
                checkErr(err, "Buffer::Buffer()");
                vcvposZBuffer = cl::Buffer(clcontext, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, vvPosZ.size(), vvPosZ.data(), &err);
                checkErr(err, "Buffer::Buffer()");
                vcvposWBuffer = cl::Buffer(clcontext, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, vvPosW.size(), vvPosW.data(), &err);
                checkErr(err, "Buffer::Buffer()");
                err = clkvertexShader.setArg(0, cvposXBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clkvertexShader.setArg(1, cvposYBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clkvertexShader.setArg(2, cvposZBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clkvertexShader.setArg(3, cvposWBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clkvertexShader.setArg(4, vcvposXBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clkvertexShader.setArg(5, vcvposYBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clkvertexShader.setArg(6, vcvposZBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clkvertexShader.setArg(7, vcvposWBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clkvertexShader.setArg(8, cvindexesBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clkvertexShader.setArg(9, cnumIndexesBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clkvertexShader.setArg(10, cbaseIndexesBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clkvertexShader.setArg(11, cbaseVerticesBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clkvertexShader.setArg(12, cNbVerticesPerFaces);
                checkErr(err, "Kernel::setArg()");
                err = clkvertexShader.setArg(13, ctransfMatrixBuffer);
                checkErr(err, "Kernel::setArg()");
                cl::CommandQueue clqueue (clcontext, devices[0], 0, &err);
                checkErr(err, "CommandQueue::CommandQueue()");
                cl::Event event;
                err = clqueue.enqueueNDRangeKernel(clkvertexShader,
                                                    1, workgroupSize,
                                                    1, nullptr, &event);
                checkErr(err, "ComamndQueue::enqueueNDRangeKernel()");
                event.wait();
                err = clqueue.enqueueReadBuffer(cvposXBuffer,CL_TRUE,0,workgroupSize,vvPosX.data());
                checkErr(err, "ComamndQueue::enqueueReadBuffer()");
                err = clqueue.enqueueReadBuffer(cvposYBuffer,CL_TRUE,0,workgroupSize,vvPosY.data());
                checkErr(err, "ComamndQueue::enqueueReadBuffer()");
                err = clqueue.enqueueReadBuffer(cvposZBuffer,CL_TRUE,0,workgroupSize,vvPosZ.data());
                checkErr(err, "ComamndQueue::enqueueReadBuffer()");
                err = clqueue.enqueueReadBuffer(cvposWBuffer,CL_TRUE,0,workgroupSize,vvPosW.data());
                checkErr(err, "ComamndQueue::enqueueReadBuffer()");
                for (unsigned int i = 0; i < workgroupSize; i++)
                    std::cout<<vPosX[i]<<" "<<vPosY[i]<<" "<<vPosZ[i]<<" "<<vPosW[i]<<std::endl;
                unsigned int newSize = (nbVerticesPerFace[1]) ? workgroupSize * 2 : workgroupSize * 2 - workgroupSize / nbVerticesPerFace[0];
                std::vector<float> gvPosX(newSize);
                std::vector<float> gvPosY(newSize);
                std::vector<float> gvPosZ(newSize);
                std::vector<float> gvPosW(newSize);
                gcvposXBuffer = cl::Buffer(clcontext, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, gvPosX.size(), gvPosX.data(), &err);
                checkErr(err, "Buffer::Buffer()");
                gcvposYBuffer = cl::Buffer(clcontext, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, gvPosY.size(), gvPosY.data(), &err);
                checkErr(err, "Buffer::Buffer()");
                gcvposZBuffer = cl::Buffer(clcontext, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, gvPosZ.size(), gvPosZ.data(), &err);
                checkErr(err, "Buffer::Buffer()");
                gcvposWBuffer = cl::Buffer(clcontext, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, gvPosW.size(), gvPosW.data(), &err);
                err = clkgeometryShader.setArg(0, vcvposXBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clkgeometryShader.setArg(1, vcvposYBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clkgeometryShader.setArg(2, vcvposZBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clkgeometryShader.setArg(3, vcvposWBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clkgeometryShader.setArg(4, gcvposXBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clkgeometryShader.setArg(5, gcvposYBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clkgeometryShader.setArg(6, gcvposZBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clkgeometryShader.setArg(7, gcvposWBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clkgeometryShader.setArg(8, cvindexesBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clkgeometryShader.setArg(9, cnumIndexesBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clkgeometryShader.setArg(10, cbaseIndexesBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clkgeometryShader.setArg(11, cbaseVerticesBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clkgeometryShader.setArg(12, cNbVerticesPerFaces);
                checkErr(err, "Kernel::setArg()");
                err = clqueue.enqueueNDRangeKernel(clkgeometryShader,
                                                    1, workgroupSize,
                                                    1, NULL, &event);
                checkErr(err, "ComamndQueue::enqueueNDRangeKernel()");
                event.wait();
                err = clqueue.enqueueReadBuffer(gcvposXBuffer,CL_TRUE,0,newSize,gvPosX.data());
                checkErr(err, "ComamndQueue::enqueueReadBuffer()");
                err = clqueue.enqueueReadBuffer(gcvposYBuffer,CL_TRUE,0,newSize,gvPosY.data());
                checkErr(err, "ComamndQueue::enqueueReadBuffer()");
                err = clqueue.enqueueReadBuffer(gcvposZBuffer,CL_TRUE,0,newSize,gvPosZ.data());
                checkErr(err, "ComamndQueue::enqueueReadBuffer()");
                err = clqueue.enqueueReadBuffer(gcvposWBuffer,CL_TRUE,0,newSize,gvPosW.data());
                checkErr(err, "ComamndQueue::enqueueReadBuffer()");
                for (unsigned int i = 0; i < newSize; i++)
                    std::cout<<gvPosX[i]<<" "<<gvPosY[i]<<" "<<gvPosZ[i]<<" "<<gvPosW[i]<<std::endl;
                unsigned int newWorkGroupSize = newSize / nbVerticesPerFace[2];
                std::vector<float> vCentersX(newWorkGroupSize);
                std::vector<float> vCentersY(newWorkGroupSize);
                std::vector<float> vCentersZ(newWorkGroupSize);
                std::vector<float> vCentersW(newWorkGroupSize);
                cvCentersXBuffer = cl::Buffer(clcontext, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, vCentersX.size(), vCentersX.data(), &err);
                checkErr(err, "Buffer::Buffer()");
                cvCentersYBuffer = cl::Buffer(clcontext, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, vCentersY.size(), vCentersY.data(), &err);
                checkErr(err, "Buffer::Buffer()");
                cvCentersZBuffer = cl::Buffer(clcontext, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, vCentersZ.size(), vCentersZ.data(), &err);
                checkErr(err, "Buffer::Buffer()");
                cvCentersWBuffer = cl::Buffer(clcontext, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, vCentersW.size(), vCentersW.data(), &err);
                checkErr(err, "Buffer::Buffer()");
                err = clktesslationShader.setArg(0, gcvposXBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clktesslationShader.setArg(1, gcvposYBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clktesslationShader.setArg(2, gcvposZBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clktesslationShader.setArg(3, gcvposWBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clktesslationShader.setArg(4, cvCentersXBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clktesslationShader.setArg(5, cvCentersYBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clktesslationShader.setArg(6, cvCentersZBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clktesslationShader.setArg(7, cvCentersWBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clqueue.enqueueNDRangeKernel(clktesslationShader,
                                                    1, newWorkGroupSize,
                                                    1, NULL, &event);
                checkErr(err, "ComamndQueue::enqueueNDRangeKernel()");
                event.wait();
                err = clqueue.enqueueReadBuffer(cvCentersXBuffer,CL_TRUE,0,newWorkGroupSize,vCentersX.data());
                checkErr(err, "ComamndQueue::enqueueReadBuffer()");
                err = clqueue.enqueueReadBuffer(cvCentersYBuffer,CL_TRUE,0,newWorkGroupSize,vCentersY.data());
                checkErr(err, "ComamndQueue::enqueueReadBuffer()");
                err = clqueue.enqueueReadBuffer(cvCentersZBuffer,CL_TRUE,0,newWorkGroupSize,vCentersZ.data());
                checkErr(err, "ComamndQueue::enqueueReadBuffer()");
                err = clqueue.enqueueReadBuffer(cvCentersWBuffer,CL_TRUE,0,newWorkGroupSize,vCentersW.data());
                checkErr(err, "ComamndQueue::enqueueReadBuffer()");
                for (unsigned int i = 0; i < newWorkGroupSize; i++)
                    std::cout<<vCentersX[i]<<" "<<vCentersY[i]<<" "<<vCentersZ[i]<<" "<<vCentersW[i]<<std::endl;
                /*err = clkvertexShader.setArg(4, cvcolredBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clkvertexShader.setArg(5, cvcolblueBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clkvertexShader.setArg(6, cvcolgreenBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clkvertexShader.setArg(7, cvcolalphaBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clkvertexShader.setArg(8, cvtexcoorduBuffer);
                checkErr(err, "Kernel::setArg()");
                err = clkvertexShader.setArg(9, cvtexcoordvBuffer);
                checkErr(err, "Kernel::setArg()");*/
                /*checkErr(err, "Kernel::setArg()");
                err = clkvertexShader.setArg(15, ctexMatrixBuffer);*/
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
