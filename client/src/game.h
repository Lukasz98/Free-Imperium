#pragma once
#include <memory>
#include <cassert>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "map.h"
//#include "graphics/window.h"
//#include "graphics/shader.h"
#include "camera.h"
#include "gui/gui_bridge.h"

#include "country.h"
#include "country_loader.h"
#include "province.h"
#include "province_loader.h"
#include "gui_aid.h"
#include "unit.h"
#include "war.h"
#include "battle.h"
#include "process_packet.h"
#include "peace_offer.h"
#include "scene.h"

class Game : public Scene
{
    sf::TcpSocket & socket;
    //Shader shader;
    //Window & window;
    Map map;
    //Camera camera;
    //Gui gui;
    glm::vec2 windowSize, resolution;
    float dt = 0.0f, drawDt = 0.0f;
    
    std::vector<sf::Packet> toSend;
    std::shared_ptr<Country> myCountry;
    std::vector<std::shared_ptr<Country>> countries;
    std::vector<std::unique_ptr<Province>> provinces;
    std::vector<std::shared_ptr<Unit>> units;
    std::vector<War> wars;
    std::vector<std::unique_ptr<Battle>> battles;
    std::vector<PeaceOffer> peaceOffers;
    
    void setCountryMap();
    void receivePackets();
    void processPacket(sf::Packet packet);
    void input();
    bool provClick(glm::vec2 mouseInWorld);
    bool unitClick(glm::vec2 mouseInWorld);
    void unitMove(std::unordered_map<std::string,std::string> & values, glm::vec2 mouseInWorld);
    void processGuiEvent();
    void sendPackets();
    void updateBattles();
    void updateGui();
    
public:
    Game(Window & win, sf::TcpSocket & sock, std::string countryName, glm::vec2 res, std::vector<std::shared_ptr<Country>> & countries);
    ~Game();

    void Play();
};
