#include "CGCube.h"
#include <GL/glew.h>

//
// FUNCIÓN: CGCube::CGCube(GLfloat width, GLfloat height, GLfloat depth, GLfloat textureRepeatX, GLfloat textureRepeatY)
//
// PROPÓSITO: Construye un paralelepípedo con dimensiones y repeticiones de textura
//
CGCube::CGCube(GLfloat width, GLfloat height, GLfloat depth, GLfloat textureRepeatX, GLfloat textureRepeatY)
{
    numFaces = 12;
    numVertices = 24;

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

    GLfloat p_textures[24][2] = {
        { 0.0f, 0.0f }, { textureRepeatX, 0.0f }, { textureRepeatX, textureRepeatY }, { 0.0f, textureRepeatY }, // Pos X
        { 0.0f, 0.0f }, { textureRepeatX, 0.0f }, { textureRepeatX, textureRepeatY }, { 0.0f, textureRepeatY }, // Pos Y
        { 0.0f, 0.0f }, { textureRepeatX, 0.0f }, { textureRepeatX, textureRepeatY }, { 0.0f, textureRepeatY }, // Neg X
        { 0.0f, 0.0f }, { textureRepeatX, 0.0f }, { textureRepeatX, textureRepeatY }, { 0.0f, textureRepeatY }, // Neg Y
        { 0.0f, 0.0f }, { textureRepeatX, 0.0f }, { textureRepeatX, textureRepeatY }, { 0.0f, textureRepeatY }, // Pos Z
        { 0.0f, 0.0f }, { textureRepeatX, 0.0f }, { textureRepeatX, textureRepeatY }, { 0.0f, textureRepeatY }  // Neg Z
    };

    GLfloat p_vertices[24][3] = {
        { +width, +height, +depth }, // Positivo X
        { +width, -height, +depth },
        { +width, -height, -depth },
        { +width, +height, -depth },

        { -width, +height, +depth }, // Positivo Y
        { +width, +height, +depth },
        { +width, +height, -depth },
        { -width, +height, -depth },

        { -width, -height, +depth }, // Negativo X
        { -width, +height, +depth },
        { -width, +height, -depth },
        { -width, -height, -depth },

        { +width, -height, +depth }, // Negativo Y
        { -width, -height, +depth },
        { -width, -height, -depth },
        { +width, -height, -depth },

        { +width, +height, +depth }, // Positivo Z
        { -width, +height, +depth },
        { -width, -height, +depth },
        { +width, -height, +depth },

        { +width, +height, -depth }, // Negativo Z
        { +width, -height, -depth },
        { -width, -height, -depth },
        { -width, +height, -depth }
    };

    GLushort p_indexes[12][3] = {
        { 0, 1, 2 }, { 0, 2, 3 },
        { 4, 5, 6 }, { 4, 6, 7 },
        { 8, 9, 10 }, { 8, 10, 11 },
        { 12, 13, 14 }, { 12, 14, 15 },
        { 16, 17, 18 }, { 16, 18, 19 },
        { 20, 21, 22 }, { 20, 22, 23 }
    };

    normals = new GLfloat[numVertices * 3];
    for (int i = 0; i < (int)numVertices; i++)
        for (int j = 0; j < 3; j++) normals[3 * i + j] = p_normals[i][j];

    vertices = new GLfloat[numVertices * 3];
    for (int i = 0; i < (int)numVertices; i++)
        for (int j = 0; j < 3; j++) vertices[3 * i + j] = p_vertices[i][j];

    textures = new GLfloat[numVertices * 2];
    for (int i = 0; i < (int)numVertices; i++)
        for (int j = 0; j < 2; j++) textures[2 * i + j] = p_textures[i][j];

    indexes = new GLushort[numFaces * 3];
    for (int i = 0; i < (int)numFaces; i++)
        for (int j = 0; j < 3; j++) indexes[3 * i + j] = p_indexes[i][j];

    InitBuffers();
}
