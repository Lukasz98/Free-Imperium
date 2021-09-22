#pragma once

#include <GL/glew.h>

#include <fstream>
#include <glm/glm.hpp>
#include <vector>

#include "color.h"
#include "vec.h"

struct LandBorders {
    GLuint vao, vbo;
    std::vector<Vec3> verts;

    LandBorders(const unsigned char* hpix, int mapWidth, int mapHeight, int scale);
};

struct BorderVertex {
    Vec3 pos;
    Vec2 tc;
};

struct SeaBorders {
    GLuint vao, vbo;
    std::vector<BorderVertex> verts;

    SeaBorders(const unsigned char* hpix, int mapWidth, int mapHeight, int scale);
};

struct PolyVert {
    glm::vec3 pos;
    glm::vec3 col{1.0f, 0.0f, 0.0f};
    float tx;
    glm::vec3 normal;
};

struct PolyMap {
    GLuint vao, vbo;
    std::vector<PolyVert> verts;

    PolyMap(const unsigned char* hpix, int mapWidth, int mapHeight, const std::vector<Color3>& provsCols,
            int scale);
};

struct SeaProvVert
{
    Vec3 pos;
    Vec2 tc;
};

struct SeaProvColor {
    GLuint vao, vbo;
    std::vector<SeaProvVert> verts;

    SeaProvColor(int mapWidth, int mapHeight, int scale);
};

struct WaterVert {
    Vec3 pos;
    Vec2 tc;
};

struct Water {
    GLuint vao, vbo;
    std::vector<WaterVert> verts;

    Water(int mapWidth, int mapHeight, int scale);
};

struct Map2 {
    LandBorders landBorders;
    SeaBorders seaBorders;
    PolyMap polyMap;
    SeaProvColor seaProvColor;
    Water water;

    Map2(const unsigned char* hpix, int mapWidth, int mapHeight, const std::vector<Color3>& provsCols, int scale);
};

