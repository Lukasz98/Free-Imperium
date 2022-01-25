#pragma once
#include <SFML/Network.hpp>
#include "settings.h"
#include "room.h"
#include "gui_last.h"
#include "game_data.h"
#include "graphics/window.h"

class Menu
{
    GuiLast::Gui guiLast;
    GameData* gd;
    Window* window;
        
    bool startGame = false, quit = false;
    void loop();
    
public:
    Menu(Window* window, GameData* gd);
    ~Menu();

    void Play();
};
