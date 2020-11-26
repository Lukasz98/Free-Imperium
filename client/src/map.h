#pragma once
#include <vector>
#include <memory>
#include "graphics/rectangle.h"
#include "graphics/shader.h"
#include "camera.h"
#include "graphics/shader.h"
#include "log.h"

class Map : public Rectangle
{
    const float chunkScale = 2.0f; // same as map scale
    const int originW = 1920, originH = 1088;

    struct Chunk {
        Rectangle rect;
        Chunk(glm::vec3 pos, glm::vec2 size, 
              glm::vec2 texCoordLeftBottom, glm::vec2 texCoordLen)
              : rect(pos, size, texCoordLeftBottom, texCoordLen)
        {

        }
    };

    Texture provsMap, heightMap;
    std::vector<unsigned char> countryMap;
    std::unique_ptr<Texture> countryBorders, provsBorders;
    std::vector<std::unique_ptr<Chunk>> chunks;

    Texture grassT;
    Texture stoneT;
    Texture water;

    // color for brightening provinces
    Color brightCol{0, 0, 255, 55};
    
    bool brightenedProv = false;
    float waterTime = 0.0f;

    Shader shader;

    // setting multiple chunks of a map
    // its texture cordinates etc.
    void setChunks();
    
    void setProvsBorders();

public:
    Map();
    ~Map();

    // detects which province was clicked and makes it bright
    // if other province was clicked before: unbrights it
    Color ClickOnProvince(int x, int y);

    // draws borders countries of countries to countryBordersTexture
    void DrawCountries(std::vector<std::pair<int, Color>> cColor, std::vector<std::pair<Color, int>> pColor);
    
    void Draw(Camera & camera, float dt);
    void DrawSieged(const Color & prov, const Color & siege);

    // lights province of given color by drawing on provsBorders texture
    // (when clicked on given prov for ex.)
    void BrightenProv(Color c);
    
    // undo what BrightenProv() does
    void Unbright();

    const unsigned char * GetProvsPixels() { return provsMap.GetPixels(); } 
    const unsigned char * GetHeightTerrain() { return heightMap.GetPixels(); }
};
