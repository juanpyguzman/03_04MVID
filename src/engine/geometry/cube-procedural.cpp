#include "engine/geometry/cube-procedural.hpp"
#include <glm/glm.hpp>

CubeProcedural::CubeProcedural(float radius) : _radius(radius) {
    _nVertices = 6 * 2 * 3;   //6 faces * 2 triangles * 3 vertices;
    _nElements = _nVertices;

    float positions[108];
    float uvs[72];
    float normals[108];
               for (int i = 0; i < 6; i++)
               {
                   //Caras laterales (front, right, back, left)
                   if (i < 4) {
                        //Vértice (0) --> Inferior izquierdo de la cara
                        positions[18 * i + 0] = radius * cos(glm::radians(-45.0f)) * sin(glm::radians(-45.0f + i * 90));
                        positions[18 * i + 1] = radius * sin(glm::radians(-30.0f));
                        positions[18 * i + 2] = radius * cos(glm::radians(-45.0f)) * cos(glm::radians(-45.0f + i * 90));

                        uvs[12 * i + 0] = 0.0; uvs[12 * i + 1] = 0.0;

                        //Vértice (1) --> Inferior derecho de la cara
                        positions[18 * i + 3] = radius * cos(glm::radians(-45.0f)) * sin(glm::radians(45.0f + i * 90));
                        positions[18 * i + 4] = radius * sin(glm::radians(-30.0f));
                        positions[18 * i + 5] = radius * cos(glm::radians(-45.0f)) * cos(glm::radians(45.0f + i * 90));

                        uvs[12 * i + 2] = 1.0; uvs[12 * i + 3] = 0.0;

                        //Vértice (2) --> Superior derecho de la cara
                        positions[18 * i + 6] = radius * cos(glm::radians(45.0f)) * sin(glm::radians(45.0f + i * 90));
                        positions[18 * i + 7] = radius * sin(glm::radians(30.0f));
                        positions[18 * i + 8] = radius * cos(glm::radians(45.0f)) * cos(glm::radians(45.0f + i * 90));

                        uvs[12 * i + 4] = 1.0; uvs[12 * i + 5] = 1.0;

                        //Vértice (3) --> (0)
                        positions[18 * i + 9] = positions[18 * i + 0];
                        positions[18 * i + 10] = positions[18 * i + 1];
                        positions[18 * i + 11] = positions[18 * i + 2];

                        uvs[12 * i + 6] = 0.0; uvs[12 * i + 7] = 0.0;

                        //Vértice (4) --> (2)
                        positions[18 * i + 12] = positions[18 * i + 6];
                        positions[18 * i + 13] = positions[18 * i + 7];
                        positions[18 * i + 14] = positions[18 * i + 8];

                        uvs[12 * i + 8] = 1.0; uvs[12 * i + 9] = 1.0;

                        //Vértice (5) --> Superior izquierdo de la cara
                        positions[18 * i + 15] = radius * cos(glm::radians(45.0f)) * sin(glm::radians(-45.0f + i * 90));
                        positions[18 * i + 16] = radius * sin(glm::radians(30.0f));
                        positions[18 * i + 17] = radius * cos(glm::radians(45.0f)) * cos(glm::radians(-45.0f + i * 90));

                        uvs[12 * i + 10] = 0.0; uvs[12 * i + 11] = 1.0;

                        //Generamos las normales
                        if (i == 0) {
                            for (int j = 0; j < 6; j++) {
                                normals[18 * i + j + 0] = 0.0; normals[18 * i + j + 1] = 0.0; normals[18 * i + j + 2] = 1.0;
                            }
                        }

                        if (i == 1) {
                            for (int j = 0; j < 6; j++) {
                                normals[18 * i + j + 0] = 1.0; normals[18 * i + j + 1] = 0.0; normals[18 * i + j + 2] = 0.0;
                            }
                        }

                        if (i == 2) {
                            for (int j = 0; j < 6; j++) {
                                normals[18 * i + j + 0] = 0.0; normals[18 * i + j + 1] = 0.0; normals[18 * i + j + 2] = -1.0;
                            }
                        }

                        if (i == 3) {
                            for (int j = 0; j < 6; j++) {
                                normals[18 * i + j + 0] = -1.0; normals[18 * i + j + 1] = 0.0; normals[18 * i + j + 2] = 0.0;
                            }
                        }

                   }
                   //Cara inferior (BOTTOM)
                   else if (i==4) {
                       //Vértice inferior izquierdo de la cara
                       positions[18 * i + 0] = positions[39];
                       positions[18 * i + 1] = positions[40];
                       positions[18 * i + 2] = positions[41];

                       uvs[12 * i + 0] = 0.0; uvs[12 * i + 1] = 0.0;

                       //Vértice inferior derecho de la cara
                       positions[18 * i + 3] = positions[36];
                       positions[18 * i + 4] = positions[37];
                       positions[18 * i + 5] = positions[38];

                       uvs[12 * i + 2] = 1.0; uvs[12 * i + 3] = 0.0;

                       //Vértice superior derecho de la cara
                       positions[18 * i + 6] = positions[3];
                       positions[18 * i + 7] = positions[4];
                       positions[18 * i + 8] = positions[5];

                       uvs[12 * i + 4] = 1.0; uvs[12 * i + 5] = 1.0;

                       //Vértice (3) --> (0)
                       positions[18 * i + 9] = positions[18 * i + 0];
                       positions[18 * i + 10] = positions[18 * i + 1];
                       positions[18 * i + 11] = positions[18 * i + 2];

                       uvs[12 * i + 6] = 0.0; uvs[12 * i + 7] = 0.0;

                       //Vértice (4) --> (2)
                       positions[18 * i + 12] = positions[18 * i + 6];
                       positions[18 * i + 13] = positions[18 * i + 7];
                       positions[18 * i + 14] = positions[18 * i + 8];

                       uvs[12 * i + 8] = 1.0; uvs[12 * i + 9] = 1.0;

                       //Vértice superior izquierdo de la cara
                       positions[18 * i + 15] = positions[0];
                       positions[18 * i + 16] = positions[1];
                       positions[18 * i + 17] = positions[2];

                       uvs[12 * i + 10] = 0.0; uvs[12 * i + 11] = 1.0;

                       //Generamos las normales
                       for (int j = 0; j < 6; j++) {
                           normals[18*i + j + 0] = 0.0; normals[18 * i + j + 1] = -1.0; normals[18 * i + j + 2] = 0.0;
                       }
                   }
                   
                   //Cara superior (TOP)
                   else if (i == 5) {
                       //Vértice inferior izquierdo de la cara
                       positions[18 * i + 0] = positions[15];
                       positions[18 * i + 1] = positions[16];
                       positions[18 * i + 2] = positions[17];

                       uvs[12 * i + 0] = 0.0; uvs[12 * i + 1] = 0.0;

                       //Vértice inferior derecho de la cara
                       positions[18 * i + 3] = positions[5];
                       positions[18 * i + 4] = positions[6];
                       positions[18 * i + 5] = positions[7];

                       uvs[12 * i + 2] = 1.0; uvs[12 * i + 3] = 0.0;

                       //Vértice superior derecho de la cara
                       positions[18 * i + 6] = positions[24];
                       positions[18 * i + 7] = positions[25];
                       positions[18 * i + 8] = positions[26];

                       uvs[12 * i + 4] = 1.0; uvs[12 * i + 5] = 1.0;

                       //Vértice (3) --> (0)
                       positions[18 * i + 9] = positions[18 * i + 0];
                       positions[18 * i + 10] = positions[18 * i + 1];
                       positions[18 * i + 11] = positions[18 * i + 2];

                       uvs[12 * i + 6] = 0.0; uvs[12 * i + 7] = 0.0;

                       //Vértice (4) --> (2)
                       positions[18 * i + 12] = positions[18 * i + 6];
                       positions[18 * i + 13] = positions[18 * i + 7];
                       positions[18 * i + 14] = positions[18 * i + 8];

                       uvs[12 * i + 8] = 1.0; uvs[12 * i + 9] = 1.0;

                       //Vértice superior izquierdo de la cara
                       positions[18 * i + 15] = positions[42];
                       positions[18 * i + 16] = positions[43];
                       positions[18 * i + 17] = positions[44];

                       uvs[12 * i + 10] = 0.0; uvs[12 * i + 11] = 1.0;

                       //Generamos las normales
                       for (int j = 0; j < 6; j++) {
                           normals[18 * i + j + 0] = 0.0; normals[18 * i + j + 1] = 1.0; normals[18 * i + j + 2] = 0.0;
                       }
                   }


               }

    //Generamos los índices           
    uint32_t indices[36]; 
    for (uint32_t n = 0; n < 36; n++) {
        indices[n] = n;
        }
                            
    uploadData(positions, uvs, normals, indices);
}