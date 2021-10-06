#pragma once
#include <GL/glew.h>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

#include "log.h"

struct MapTexture
{
    GLuint id = 0;
    unsigned char * pix = nullptr;

    ~MapTexture();

    void Update(int w, int h);
};


