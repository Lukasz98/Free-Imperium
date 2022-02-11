#pragma once

#include <GL/glew.h>

#include <fstream>
#include <glm/glm.hpp>
#include <vector>

#include "color.h"
#include "graphics/map_texture.h"
#include "graphics/shader.h"
#include "graphics/texture.h"
#include "vec.h"

struct LandBorders {
    GLuint vao, vbo;
    std::vector<Vec3> verts;

    LandBorders() {}
    LandBorders(const unsigned char* hpix, int mapWidth, int mapHeight, int scale);
};

struct BorderVertex {
    Vec3 pos;
    Vec2 tc;
};

struct SeaBorders {
    GLuint vao, vbo;
    std::vector<BorderVertex> verts;

    SeaBorders() {}
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

    PolyMap() {}
    PolyMap(const unsigned char* hpix, int mapWidth, int mapHeight, const std::vector<Color3>& provsCols,
            int scale);
};

struct SeaProvVert {
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

struct MapTextures {
    MapTexture country, province;
};

struct Map2 {
    const float MAPID_PROV_COLOR = 20.0f;
    const float MAPID_COUNTRY = 21.0f;
    const float MAPID_PEACE_OFFER = 22.0f;

    const int mapWidth = 5632;
    const int mapHeight = 2048;

    LandBorders landBorders;
    SeaBorders seaBorders;
    PolyMap polyMap;
    SeaProvColor seaProvColor;
    Water water;

    //
    Shader borderShader;
    Shader seaBorderShader;
    Shader waterShader;
    Shader landShader;
    Shader waterColorShader;
    Shader landTesShader;
    //

    GLint tex[32];
    GLuint texID[32];
    MapTextures mapTextures;
    Texture provTexture;
    Texture grassT;
    Texture* occupiedText = nullptr;
    std::vector<unsigned char> occupiedPix;
    Texture* occupyingText = nullptr;
    std::vector<unsigned char> occupyingPix;
    Texture* markedCtrText = nullptr;
    std::vector<unsigned char> markedCtrPix;
    Texture heightMap;

    Map2(const std::vector<Color3>& provsCols, int scale);
    ~Map2();

    void DrawForColorPick(glm::mat4 proj, float provCount);
    void DrawWater(glm::mat4 proj, glm::vec3 eye);
    void DrawLand(glm::mat4 proj, glm::vec3 eye, float provId, float provCount, int MAPID, float time);
    void DrawBorders(glm::mat4 proj);

    void ReloadShaders();
    void ActivateTextures();
};

