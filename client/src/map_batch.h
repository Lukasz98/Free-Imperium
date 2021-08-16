#pragma once

#include <GL/glew.h>
#include "vec.h"
#include "log.h"

struct MapVertex {
    Vec3 pos;
    //Vec4 color;
    //float textureId = 1.0f;
    Vec2 tc;      //{1.0f, 0.0f};
    //Vec2 normal;  //{0.5f,1.0f};
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
