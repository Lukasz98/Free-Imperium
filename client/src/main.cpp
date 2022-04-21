//#include <memory>
#include <time.h>

#include <SFML/Network.hpp>

#include "../shared/load_countries_data.h"
#include "asset_manager.h"
#include "camera.h"
#include "country_loader.h"
#include "game.h"
#include "game_data.h"
#include "graphics/shader.h"
#include "graphics/window.h"
#include "gui_last.h"
#include "log.h"
#include "menu.h"
#include "province_loader.h"
#include "room.h"
#include "save_borders.h"
#include "save_borders_triangles.h"
#include "save_data.h"
#include "settings.h"

int main(int argc, char** argv)
{
    if (argc == 2) {
        if (0 == strncmp(argv[1], "generate_data", strlen(argv[1]))) {
            Settings settings;
            const int mapWidth = 5632;
            const int mapHeight = 2048;
            int scale = 4;
            std::map<unsigned int, int> colorToId;
            std::map<int, Node> nodes, nodes2;
            Texture provTexture{"src/img/Provinces_org.png", mapWidth,
                                mapHeight};
            Texture waterTexture{"src/img/Blank_map.png"};
            Texture heightMap{"src/img/Heightmap.png", mapWidth, mapHeight};
            saveProvinceFromImg(provTexture.GetPixels(),
                                waterTexture.GetPixels(), mapWidth, mapHeight);

            std::vector<ProvData> provData = LoadProvincesData(colorToId);
            std::sort(provData.begin(), provData.end(),
                      [](const ProvData& p1, const ProvData& p2) {
                          return p1.id < p2.id;
                      });

            createSaveProvPoints(provTexture.GetPixels(), mapWidth, mapHeight,
                                 provData, colorToId);

            saveBorders(provTexture.GetPixels(), mapWidth, mapHeight, provData,
                        nodes);
            saveSeaBorders(provTexture.GetPixels(), mapWidth, mapHeight,
                           provData, nodes2, colorToId);

            saveBordersTriangles(mapWidth, mapHeight, settings.scale,
                                 heightMap.GetPixels());
        }
        return 0;
    }
    glm::vec2 windowSize = {1600, 900};

    glm::vec2 resolution = windowSize;
    std::string title = "Free_Imperium";

    Window window(windowSize, title);
    if (glewInit() != GLEW_OK) {
        std::cout << "glew failed\n";
        return -1;
    }

    AM::Init();

    glfwSetCursor(window.window, AM::am.def_cursor);

    GameData gd;
    gd.window = &window;
    gd.settings.resolution = resolution;
    gd.countries = CountryLoader::Load();
    gd.provinces = ProvinceLoader::Load(gd.colorToId);
    gd.camera = Camera(gd.settings.resolution);
    gd.initMap();
    gd.loadProvinceTexture();
    gd.makeCountryNames();

    Menu menu(&gd);
    menu.Play();
    AM::Dispose();
}

