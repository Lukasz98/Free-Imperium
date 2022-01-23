#pragma once
#include <SFML/Network.hpp>
#include "scene.h"
#include "settings.h"
#include "room.h"
#include "gui_last.h"

class Menu : public Scene
{
    Settings settings;
    std::vector<std::shared_ptr<Country>> & countries;
    GuiLast::Gui guiLast;

    bool startGame = false, quit = false;
    void loop();
    
public:
    Menu(Window & window, Settings & s, std::vector<std::shared_ptr<Country>> & countries);
    ~Menu();

    void Play();
};
