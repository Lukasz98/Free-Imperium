#pragma once

#include <GL/glew.h>
#include "vec.h"
#include "log.h"

struct FontVertex {
    Vec3 pos;
    Vec2 tc;
    Vec4 color{1.0f, 0.0f, 0.0f, 1.0f};
    float tid;
};

struct FontBatch {
    GLuint vao, vbo;
    int maxSprites = 10000, vertCount = 0, indicesCount = 0, spriteCount = 0;
    FontVertex* vertexData;

    void Init();
    void Begin();
    //void Push(const FontVertex* verts);  // takes array of 2
    void Push(float x, float y, float z, float w, float h, float tcx, float tcy, float tcxLen, float tcyLen, float tid);
    void Flush();
};
