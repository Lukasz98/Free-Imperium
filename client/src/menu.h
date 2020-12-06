#pragma once
#include <SFML/Network.hpp>
#include "scene.h"
#include "settings.h"
#include "room.h"

class Menu : public Scene
{
    Settings settings;
    std::vector<std::shared_ptr<Country>> & countries;

    bool localGame = false, guestGame = false, quit = false;
    void loop();
    void processGuiEvent();
    
public:
    Menu(Window & window, Settings & s, std::vector<std::shared_ptr<Country>> & countries);
    ~Menu();

    void Play();
};
