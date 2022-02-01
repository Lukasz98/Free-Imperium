//#include <memory>
#include <time.h>

#include <SFML/Network.hpp>

#include "asset_manager.h"
#include "camera.h"
#include "game.h"
#include "game_data.h"
#include "graphics/shader.h"
#include "graphics/window.h"
#include "gui_last.h"
#include "log.h"
#include "menu.h"
//#include "new_map.h"
#include "room.h"
#include "settings.h"

#include "country_loader.h"
#include "province_loader.h"

int main()
{
    // glm::vec2 windowSize = {800, 600};
    // glm::vec2 windowSize = {1280, 720};
    // glm::vec2 windowSize = {1366, 768};
    glm::vec2 windowSize = {1600, 900};
    // glm::vec2 windowSize = {1920, 1080};

    glm::vec2 resolution = windowSize;
    // glm::vec2 resolution = {windowSize.x * 2, windowSize.y * 2};
    // glm::vec2 resolution = {1280, 720};
    // glm::vec2 resolution = {1920, 1080};
    std::string title = "Free_Imperium";

    Window window(windowSize, title);
    if (glewInit() != GLEW_OK) {
        std::cout << "glew failed\n";
        return -1;
    }

    AM::Init();

    GameData gd;
    gd.settings.resolution = resolution;
    gd.countries = CountryLoader::Load();
    gd.provinces = ProvinceLoader::Load(gd.colorToId);
    gd.initMap();
    gd.loadProvinceTexture();
    gd.makeCountryNames();

    Menu menu(&window, &gd);
    menu.Play();

    AM::Dispose();
}

