#pragma once
#include "camera.h"
#include "graphics/shader.h"
#include "graphics/window.h"
#include "vec.h"

struct Vertexx {
    Vec3 pos;
    Vec4 color;
    float textureId = 1.0f;
    Vec2 tc;      //{1.0f, 0.0f};
    Vec2 normal;  //{0.5f,1.0f};
};

struct MapBatch {
    GLuint vao, vbo, ibo;
    int maxSprites = 5000, vertCount = 0, indicesCount = 0, spriteCount = 0;
    Vertexx* vertexData;

    void Init();
    void Begin();
    void Push(const Vertexx* verts);  // takes array of 4
    void Flush();
};

void newTesMapTest(Window& window, glm::vec2 resolution, glm::vec2 windowSize);
void newMapTest(Window& window, glm::vec2 resolution, glm::vec2 windowSize);
