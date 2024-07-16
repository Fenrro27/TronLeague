
#include "Porteria.h"
#include <GL/glew.h>
#include "CGFigure.h"

Porteria::Porteria(GLfloat width, GLfloat height, GLfloat depth)
{
    numFaces = 14; // Número de caras (12 originales menos 2 de la cara frontal + 2 por cada palo)
    numVertices = 12; // Número de vértices (24 originales menos 4 de la cara frontal + 4 de cada palo es decir 12 )

    // Normales de las caras
    GLfloat p_normals[12][3] = {
        { 1.0f, 0.0f, 0.0f },   // Positivo X
        { 1.0f, 0.0f, 0.0f },
        { 1.0f, 0.0f, 0.0f },
        { 1.0f, 0.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f },   // Positivo Y
        { 0.0f, 1.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f },
        { -1.0f, 0.0f, 0.0f },  // Negativo X
        { -1.0f, 0.0f, 0.0f },
        { -1.0f, 0.0f, 0.0f },
        { -1.0f, 0.0f, 0.0f }
    };

    // Coordenadas de textura
    GLfloat p_textures[12][2] = {
        { 1.0f, 1.0f }, 
        { 0.0f, 1.0f },
       
        { 0.0f, 0.0f },
        { 0.05f, 0.0f },

        { 0.05f, 0.95f }, 
        { 0.95f, 0.95f }, 
        
        { 0.95f, 0.0f }, 
        { 1.0f, 0.0f },

        // Interior
        { 0.25f, 0.0f },
        { 0.25f, 0.6f },
        { 0.75f, 0.6f },
        { 0.75f, 0.0f }

    };

    // Coordenadas de vértices
    GLfloat p_vertices[12][3] = {
        { +width + 4, +height + 4, +depth }, //Arriba derecha
        { -width - 4, +height + 4, +depth }, //Arriba Izquierda
        { -width - 4, -height, +depth }, //Abajo Izquierda
        { -width, -height, +depth },
        { -width, +height, +depth },
        { +width, +height, +depth },
        { +width, -height, +depth },
        { +width + 4, -height, +depth },

        // Profundidad
        { +width, -height, -depth },
        { +width, +height, -depth },
        { -width, +height, -depth },
        { -width, -height, -depth }

    };

    // Índices de las caras
    GLushort p_indexes[14][3] = {
        //Caras posters
        {1,2,3},
        {1,3,4},
        {0,1,4},
        {0,4,5},
        {0,5,6},
        {0,6,7},


        //Caras porteria
        {3,11,4},
        {4,11,10},

        {4,10,5},
        {5,10,9},

        {5,9,8},
        {5,8,6},

        {9,10,11},
        {8,9,11}


    };

    // Copiar los datos a los miembros de la clase
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
