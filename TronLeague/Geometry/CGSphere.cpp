#include "CGSphere.h"
#include <GL/glew.h>
#include <cmath>
#include <glm/glm.hpp>

//
// FUNCIÓN: CGSphere::CGSphere(GLint p, GLint m, GLfloat r)
//
// PROPÓSITO: Construye una esfera de radio 'r' con 'p' paralelos y 'm' meridianos
//
CGSphere::CGSphere(GLint p, GLint m, GLfloat r)
{
    numFaces = 2 * m * (p - 1);
    numVertices = (m + 1) * (p + 1);

    normals = new GLfloat[numVertices * 3];
    textures = new GLfloat[numVertices * 2];
    vertices = new GLfloat[numVertices * 3];
    indexes = new GLushort[numFaces * 3];

    int texturesIndex = 0;
    int normalsIndex = 0;
    int verticesIndex = 0;
    int indexesIndex = 0;

    // Casquete polar norte
    for (int j = 0; j <= m; j++)
    {
        textures[texturesIndex] = (GLfloat)(j + 0.5f) / m;
        textures[texturesIndex + 1] = 1.0f;
        texturesIndex += 2;
        normals[normalsIndex] = 0.0f;
        normals[normalsIndex + 1] = 0.0f;
        normals[normalsIndex + 2] = 1.0f;
        normalsIndex += 3;
        vertices[verticesIndex] = 0.0f;
        vertices[verticesIndex + 1] = 0.0f;
        vertices[verticesIndex + 2] = r;
        verticesIndex += 3;
    }

    for (int i = 1; i < p; i++)
    {
        for (int j = 0; j <= m; j++)
        {
            GLfloat pCos = (GLfloat)std::cos(glm::radians(180.0f * i / p));
            GLfloat pSin = (GLfloat)std::sin(glm::radians(180.0f * i / p));
            GLfloat mCos = (GLfloat)std::cos(glm::radians(360.0f * j / m));
            GLfloat mSin = (GLfloat)std::sin(glm::radians(360.0f * j / m));

            textures[texturesIndex] = ((GLfloat)j) / m;
            textures[texturesIndex + 1] = 1.0f - ((GLfloat)i / p);
            texturesIndex += 2;
            normals[normalsIndex] = pSin * mCos;
            normals[normalsIndex + 1] = pSin * mSin;
            normals[normalsIndex + 2] = pCos;
            normalsIndex += 3;
            vertices[verticesIndex] = pSin * mCos * r;
            vertices[verticesIndex + 1] = pSin * mSin * r;
            vertices[verticesIndex + 2] = pCos * r;
            verticesIndex += 3;
        }
    }

    // Casquete polar sur
    for (int j = 0; j <= m; j++)
    {
        textures[texturesIndex] = (GLfloat)(j + 0.5f) / m;
        textures[texturesIndex + 1] = 0.0f;
        texturesIndex += 2;
        normals[normalsIndex] = 0.0f;
        normals[normalsIndex + 1] = 0.0f;
        normals[normalsIndex + 2] = -1.0f;
        normalsIndex += 3;
        vertices[verticesIndex] = 0.0f;
        vertices[verticesIndex + 1] = 0.0f;
        vertices[verticesIndex + 2] = -r;
        verticesIndex += 3;
    }

    for (int j = 0; j < m; j++)
    {
        indexes[indexesIndex] = (GLushort)j;
        indexes[indexesIndex + 1] = (GLushort)(m + j + 1);
        indexes[indexesIndex + 2] = (GLushort)(m + j + 2);
        indexesIndex += 3;
    }
    for (int i = 1; i < p - 1; i++)
    {
        for (int j = 0; j < m; j++)
        {
            indexes[indexesIndex] = (GLushort)(i * (m + 1) + j);
            indexes[indexesIndex + 1] = (GLushort)((i + 1) * (m + 1) + j);
            indexes[indexesIndex + 2] = (GLushort)(i * (m + 1) + j + 1);
            indexes[indexesIndex + 3] = (GLushort)((i + 1) * (m + 1) + j);
            indexes[indexesIndex + 4] = (GLushort)((i + 1) * (m + 1) + j + 1);
            indexes[indexesIndex + 5] = (GLushort)(i * (m + 1) + j + 1);
            indexesIndex += 6;
        }
    }
    for (int j = 0; j < m; j++)
    {
        indexes[indexesIndex] = (GLushort)((p - 1) * (m + 1) + j);
        indexes[indexesIndex + 1] = (GLushort)(p * (m + 1) + j);
        indexes[indexesIndex + 2] = (GLushort)((p - 1) * (m + 1) + j + 1);
        indexesIndex += 3;
    }

    InitBuffers();
}
