#include "map_batch.h"




void MapBatch::Init()
{
    int vertexSize = sizeof(MapVertex);
    int spriteSize = vertexSize * 4;
    int vertexDataSize = spriteSize * maxSprites;
    int indicesSize = maxSprites * 6;
    Log(vertexSize);
    // glGenVertexArrays(1, &vao);
    glCreateVertexArrays(1, &vao);
    glBindVertexArray(vao);
    // glGenBuffers(1, &vbo);
    glCreateBuffers(1, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertexDataSize, NULL, GL_DYNAMIC_DRAW);
    Log("vbo=" << vbo);
    // Vertex vx;
    // Log(&vx);
    // Log(&vx.color);
    // Log(&vx.textureId);
    // Log(&vx.tc);
    // Log(&vx.normal);
    glEnableVertexArrayAttrib(vao, 0);
    glEnableVertexArrayAttrib(vao, 1);
    //glEnableVertexArrayAttrib(vao, 2);
    //glEnableVertexArrayAttrib(vao, 3);
    //glEnableVertexArrayAttrib(vao, 4);
    GLuint err = glGetError();
    if (err)
        Log("Opengl error: " << err);
    // glEnableVertexAttribArray(0);
    // glEnableVertexAttribArray(1);
    // glEnableVertexAttribArray(2);
    // glEnableVertexAttribArray(3);
    // glEnableVertexAttribArray(4);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexSize, NULL);  //(const GLvoid*)0);
    //glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, vertexSize,
    //                      (const GLvoid*)(offsetof(MapVertex, MapVertex::color)));  //(const GLvoid*)(3 * GL_FLOAT));
    //glVertexAttribPointer(
    //    2, 1, GL_FLOAT, GL_FALSE, vertexSize,
    //    (const GLvoid*)(offsetof(MapVertex, MapVertex::textureId)));  //(const GLvoid*)(7 * GL_FLOAT));
    //glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, vertexSize,
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, vertexSize,
                          (const GLvoid*)(offsetof(MapVertex, MapVertex::tc)));  //(const GLvoid*)(7 * GL_FLOAT));
    //glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, vertexSize,
    //                      (const GLvoid*)(offsetof(MapVertex, MapVertex::normal)));  //(const GLvoid*)(7 * GL_FLOAT));

    err = glGetError();
    if (err)
        Log("Opengl error: " << err);
    // glBindBuffer(GL_ARRAY_BUFFER, 0);

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
    err = glGetError();
    if (err)
        Log("Opengl error: " << err);
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
        // if (spriteCount >= maxSprites) {
        Flush();
        Begin();
    }

    spriteCount++;
    vertCount += 4;
    indicesCount += 6;
    for (int i = 0; i < 4; ++i, vertexData++) {
        *vertexData = verts[i];
        vertexData->pos = verts[i].pos;
        //vertexData->color = verts[i].color;
        vertexData->tc = verts[i].tc;
        //vertexData->textureId = verts[i].textureId;
        //vertexData->normal = verts[i].normal;
    }
}
//float tFlush = 0.0f;
void MapBatch::Flush()
{
    //float tt2 = glfwGetTime();
    glDrawElements(GL_PATCHES, indicesCount, GL_UNSIGNED_SHORT, NULL);
    // glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_SHORT, NULL);
    // Log(vertCount);
    vertCount = 0;
    spriteCount = 0;
    indicesCount = 0;
    glUnmapBuffer(GL_ARRAY_BUFFER);
    //tFlush += (glfwGetTime() - tt2);
}
