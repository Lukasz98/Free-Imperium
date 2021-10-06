#include "gui_batch.h"


void Batch::Init()
{
    int vertexSize = sizeof(Vertex);
    int spriteSize = vertexSize * 4;
    int vertexDataSize = spriteSize * maxSprites;
    int indicesSize = maxSprites * 6;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertexDataSize, NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexSize, NULL);//(const GLvoid*)0);
    
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, vertexSize, (const GLvoid*)(offsetof(Vertex, Vertex::color)));//(const GLvoid*)(3 * GL_FLOAT));
                  
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, vertexSize, (const GLvoid*)(offsetof(Vertex, Vertex::tc)));//(const GLvoid*)(7 * GL_FLOAT));
                          
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, vertexSize, (const GLvoid*)(offsetof(Vertex, Vertex::textureId)));//(const GLvoid*)(7 * GL_FLOAT));
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLushort * indices = new GLushort[indicesSize];
    int offset = 0;
    for (int i = 0; i < indicesSize; i += 6) {
       indices[i] = offset + 0;
       indices[i + 1] = offset + 1;
       indices[i + 2] = offset + 2;
       indices[i + 3] = offset + 2;
       indices[i + 4] = offset + 3;
       indices[i + 5] = offset + 0;
        offset += 4;
    }
    
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize * sizeof(GLushort), indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    GLenum err = glGetError();
    if (err)
        Log("Opengl error: " << err);
    delete [] indices;
    
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
}

void Batch::Begin()
{
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    vertexData = (Vertex*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
}

void Batch::Push(const Vertex * verts) // takes array of 4
{
    if (vertCount + 4 >= maxSprites) {
        Flush();
        Begin();
    }

    vertCount += 4;
    indicesCount += 6;
    for (int i = 0; i < 4; ++i, vertexData++) {
        vertexData->pos = verts[i].pos;
        vertexData->color = verts[i].color;
        vertexData->tc = verts[i].tc;
        vertexData->textureId = verts[i].textureId;
    
    }
}

void Batch::Flush() {
    glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_SHORT, NULL);
    vertCount = 0;
    indicesCount = 0;
    glUnmapBuffer(GL_ARRAY_BUFFER);
}

