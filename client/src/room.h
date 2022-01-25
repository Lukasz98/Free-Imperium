#pragma once
#include <SFML/Network.hpp>
#include "settings.h"
//#include "country.h"
#include "game.h"
#include "gui_last.h"
#include "game_data.h"

class Room
{
    sf::TcpSocket socket;
    sf::Socket::Status status;
    void loop(bool & play, std::string & country);
    GuiLast::Gui guiLast;
    Window* window;
    GameData* gd;
    
public:
    Room(Window* window, GameData* gd);
    ~Room();

    void Play(bool localhost = true);
};
