#pragma once
#include <vector>

#include "battle.h"
#include "camera.h"
#include "country.h"
#include "font_batch.h"
#include "graphics/window.h"
#include "load_data.h"
#include "map2.h"
#include "province.h"
#include "settings.h"
#include "unit.h"
#include "war.h"

struct GameData {
    Window *window;
    std::vector<Country> countries;
    std::vector<Province> provinces;
    std::vector<Battle> battles;
    std::vector<War> wars;
    std::vector<Unit> units;
    std::map<unsigned int, int> colorToId;
    Map2 *map;
    std::vector<Color3> provsCols;
    std::vector<std::vector<int>> ctrProvs;
    std::vector<FontVertex> fontVerts;
    GLuint fontCtrVao, fontCtrVbo;
    Settings settings;
    Camera camera;
    float waterTime = 0.0f;
    float ctrNamesFade = 0.0f, ctrNamesFadeIn = 0.0f;
    GLuint fontTexID[32];
    GLint tex[32];
    Shader fontShader;
    const float zPoint = 1500.0f;

    void updateTime(float dt);

    void initMap();

    void resetMarkedCountry();

    void f(int ctrId);

    void makeCountryNames();

    void editCountryMap();

    void loadProvinceTexture();
};
void checkCountryNamesFade(GameData *gd);
void updateCountryNamesFade(GameData *gd, float dt);
void drawCountryNames(GameData *gd);

void cameraMovement(GameData *gd, float dt);

int clickedProvId(GameData *gd);

