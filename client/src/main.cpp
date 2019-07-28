//#include <memory>
#include <SFML/Network.hpp>
#include "log.h"
#include "graphics/window.h"
#include "graphics/shader.h"

#include "menu.h"
#include "room.h"
#include "game.h"

#include "asset_manager.h"
#include "settings.h"

int main()
{
    //glm::vec2 windowSize = {800, 600};
    glm::vec2 windowSize = {1280, 720};
    //glm::vec2 windowSize = {1366, 768};
    //glm::vec2 windowSize = {1600, 900};
    //glm::vec2 windowSize = {1920, 1080};

    glm::vec2 resolution = windowSize;
    //glm::vec2 resolution = {windowSize.x * 2, windowSize.y * 2};
    //glm::vec2 resolution = {1280, 720};
    //glm::vec2 resolution = {1920, 1080};
    std::string title = "Word";

    Window window(windowSize, title);
    if (glewInit() != GLEW_OK) {
        std::cout << "glew failed\n";
		return -1;
    }

    AM::Init();

    Settings settings;
    auto countries = CountryLoader::Load();

    Menu menu(window, settings, countries);
    //Room room(window, settings, countries);
    //room.Play(true);
    menu.Play();
    
    AM::Dispose();
    //system("./../server/build/Word_and_Sword");
    //socket.disconnect();
    
}

