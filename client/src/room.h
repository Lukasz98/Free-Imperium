#pragma once
#include <SFML/Network.hpp>
#include "scene.h"
#include "settings.h"
//#include "country.h"
#include "game.h"

class Room : public Scene
{
    sf::TcpSocket socket;
    sf::Socket::Status status;
    Settings settings;
    std::vector<std::shared_ptr<Country>> & countries;
    
    void loop(bool & play, std::string & country);
    void processGuiEvent( std::vector<sf::Packet> & toSend, bool & play);
    
public:
    Room(Window & window, Settings & s, std::vector<std::shared_ptr<Country>> & countries);
    ~Room();

    void Play(bool localhost);
};
