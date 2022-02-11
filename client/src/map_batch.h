#pragma once

#include <GL/glew.h>

#include "log.h"
#include "vec.h"

struct MapVertex {
    Vec3 pos;
    Vec2 tc;
};

struct MapBatch {
    GLuint vao, vbo, ibo;
    int maxSprites = 5000, vertCount = 0, indicesCount = 0, spriteCount = 0;
    MapVertex* vertexData;

    void Init();
    void Begin();
    void Push(const MapVertex* verts);  // takes array of 4
    void Flush();
};
