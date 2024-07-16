#include "CGCube.h"
#include <GL/glew.h>
#include "CGFigure.h"

///
/// FUNCION: CGCube::CGCube(GLfloat width, GLfloat height, GLfloat depth, GLfloat textureRepeat)
///
/// PROPÓSITO: Construye un rectángulo con dimensiones especificadas y repeticiones de textura
///
CGCube::CGCube(GLfloat width, GLfloat height, GLfloat depth, GLfloat textureRepeatX, GLfloat textureRepeatY)
{
    numFaces = 12; // Número de caras
    numVertices = 24; // Número de vértices

    


    GLfloat p_normals[24][3] = {
        { 1.0f, 0.0f, 0.0f }, // Positivo X
        { 1.0f, 0.0f, 0.0f },
        { 1.0f, 0.0f, 0.0f },
        { 1.0f, 0.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f }, // Positivo Y
        { 0.0f, 1.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f },
        { -1.0f, 0.0f, 0.0f }, // Negativo X
        { -1.0f, 0.0f, 0.0f },
        { -1.0f, 0.0f, 0.0f },
        { -1.0f, 0.0f, 0.0f },
        { 0.0f, -1.0f, 0.0f }, // Negativo Y
        { 0.0f, -1.0f, 0.0f },
        { 0.0f, -1.0f, 0.0f },
        { 0.0f, -1.0f, 0.0f },
        { 0.0f, 0.0f, 1.0f }, // Positivo Z
        { 0.0f, 0.0f, 1.0f },
        { 0.0f, 0.0f, 1.0f },
        { 0.0f, 0.0f, 1.0f },
        { 0.0f, 0.0f, -1.0f }, // Negativo Z
        { 0.0f, 0.0f, -1.0f },
        { 0.0f, 0.0f, -1.0f },
        { 0.0f, 0.0f, -1.0f }
    };

    GLfloat p_textures[24][2] = { // Array de coordenadas de textura
           { 0.0f, 0.0f }, // Positivo X
           { textureRepeatX, 0.0f },
           { textureRepeatX, textureRepeatY },
           { 0.0f, textureRepeatY },
           { 0.0f, 0.0f }, // Positivo Y
           { textureRepeatX, 0.0f },
           { textureRepeatX, textureRepeatY },
           { 0.0f, textureRepeatY },
           { 0.0f, 0.0f }, // Negativo X
           { textureRepeatX, 0.0f },
           { textureRepeatX, textureRepeatY },
           { 0.0f, textureRepeatY },
           { 0.0f, 0.0f }, // Negativo Y
           { textureRepeatX, 0.0f },
           { textureRepeatX, textureRepeatY },
           { 0.0f, textureRepeatY },
           { 0.0f, 0.0f }, // Positivo Z
           { textureRepeatX, 0.0f },
           { textureRepeatX, textureRepeatY },
           { 0.0f, textureRepeatY },
           { 0.0f, 0.0f }, // Negativo Z
           { textureRepeatX, 0.0f },
           { textureRepeatX, textureRepeatY },
           { 0.0f, textureRepeatY }
    };

    GLfloat p_vertices[24][3] = {
        { +width, +height, +depth }, // A0 // Positivo X
        { +width, -height, +depth }, // D0 
        { +width, -height, -depth }, // D1 
        { +width, +height, -depth }, // A1 

        { -width, +height, +depth }, // B0 // Positivo Y
        { +width, +height, +depth }, // A0 
        { +width, +height, -depth }, // A1 
        { -width, +height, -depth }, // B1 

        { -width, -height, +depth }, // C0 // Negativo X
        { -width, +height, +depth }, // B0 
        { -width, +height, -depth }, // B1 
        { -width, -height, -depth }, // C1

        { +width, -height, +depth }, // D0 // Negativo Y
        { -width, -height, +depth }, // C0 
        { -width, -height, -depth }, // C1
        { +width, -height, -depth }, // D1 

        { +width, +height, +depth }, // A0 // Positivo Z
        { -width, +height, +depth }, // B0 
        { -width, -height, +depth }, // C0 
        { +width, -height, +depth }, // D0 

        { +width, +height, -depth }, // A1 // Negativo Z
        { +width, -height, -depth }, // D1 
        { -width, -height, -depth }, // C1 
        { -width, +height, -depth } // B1 
    };

    GLushort p_indexes[12][3] = { // Array de índices
        { 0, 1, 2 },
        { 0, 2, 3 },
        { 4, 5, 6 },
        { 4, 6, 7 },
        { 8, 9, 10 },
        { 8, 10, 11 },
        { 12, 13, 14 },
        { 12, 14, 15 },
        { 16, 17, 18 },
        { 16, 18, 19 },
        { 20, 21, 22 },
        { 20, 22, 23 }
    };

    normals = new GLfloat[numVertices * 3];
    for (int i = 0; i < numVertices; i++)
        for (int j = 0; j < 3; j++) normals[3 * i + j] = p_normals[i][j];

    vertices = new GLfloat[numVertices * 3];
    for (int i = 0; i < numVertices; i++)
        for (int j = 0; j < 3; j++) vertices[3 * i + j] = p_vertices[i][j];

    textures = new GLfloat[numVertices * 2];
    for (int i = 0; i < numVertices; i++)
        for (int j = 0; j < 2; j++) textures[2 * i + j] = p_textures[i][j];

    indexes = new GLushort[numFaces * 3];
    for (int i = 0; i < numFaces; i++)
        for (int j = 0; j < 3; j++) indexes[3 * i + j] = p_indexes[i][j];

    InitBuffers();
}
