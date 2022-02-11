#pragma once
#include <SFML/Network.hpp>

#include "game_data.h"
#include "graphics/window.h"
#include "gui_last.h"
#include "room.h"
#include "settings.h"

class Menu {
    GuiLast::Gui guiLast;
    GameData* gd;
    Window* window;

    bool startGame = false, quit = false;
    void loop();

   public:
    Menu(GameData* gd);
    ~Menu();

    void Play();
};
