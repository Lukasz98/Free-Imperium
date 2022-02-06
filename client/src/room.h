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
    void loop();
    void receivePackets();
    void guiDraw();
    void input();
    bool play = false;
    std::string myCountryName;
    std::vector<std::string> plarr;
    std::vector<sf::Packet> toSend;
    std::vector<std::string> ctrarr;

    GuiLast::Gui guiLast;
    GameData* gd;
    float dt;
    int markedCountryId = -1;
    GuiLast::GuiEv ctype;
    
public:
    Room(GameData* gd);
    ~Room();

    void Play(bool localhost = true);
};
