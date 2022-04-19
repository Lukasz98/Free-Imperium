#pragma once

#include "graphics/window.h"
#include "graphics/shader.h"

#include "log.h"
#include "vec.h"

struct Vertex {
    glm::vec3 pos;
    glm::vec4 color;
    glm::vec2 tc;
    float textureId = 443.0f;
};

struct Batch {
    GLuint vao, vbo, ibo;
    int maxSprites = 10000, vertCount = 0, indicesCount = 0;
    Vertex * vertexData;

    void Init();
    void Begin();
    void Push(const Vertex * verts); // takes array of 4
    void Flush();
};


