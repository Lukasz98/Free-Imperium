#pragma once

#include <GL/glew.h>
#include "vec.h"
#include "log.h"

struct BorderVertex {
    Vec3 pos;
    Vec4 color{1.0f, 0.0f, 0.0f, 1.0f};
    Vec2 tc;
};

struct BorderBatch {
    GLuint vao, vbo, ibo;
    int maxSprites = 5000, vertCount = 0, indicesCount = 0, spriteCount = 0;
    BorderVertex* vertexData;

    void Init();
    void Begin();
    void Push(const BorderVertex* verts);  // takes array of 2
    void Flush();
};
