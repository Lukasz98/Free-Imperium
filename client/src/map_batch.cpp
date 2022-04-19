#include "map_batch.h"

void MapBatch::Init()
{
    int vertexSize = sizeof(MapVertex);
    int spriteSize = vertexSize * 4;
    int vertexDataSize = spriteSize * maxSprites;
    int indicesSize = maxSprites * 6;
    Log(vertexSize);
    glCreateVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glCreateBuffers(1, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertexDataSize, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexArrayAttrib(vao, 0);
    glEnableVertexArrayAttrib(vao, 1);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexSize, NULL);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, vertexSize,
                          (const GLvoid*)(offsetof(MapVertex, tc)));

    GLushort* indices = new GLushort[indicesSize];
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
    delete[] indices;

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
}

void MapBatch::Begin()
{
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    vertexData = (MapVertex*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
}

void MapBatch::Push(const MapVertex* verts)  // takes array of 4
{
    if (vertCount + 4 >= maxSprites) {
        Flush();
        Begin();
    }

    spriteCount++;
    vertCount += 4;
    indicesCount += 6;
    for (int i = 0; i < 4; ++i, vertexData++) {
        *vertexData = verts[i];
        vertexData->pos = verts[i].pos;
        vertexData->tc = verts[i].tc;
    }
}

void MapBatch::Flush()
{
    glDrawElements(GL_PATCHES, indicesCount, GL_UNSIGNED_SHORT, NULL);
    vertCount = 0;
    spriteCount = 0;
    indicesCount = 0;
    glUnmapBuffer(GL_ARRAY_BUFFER);
}
