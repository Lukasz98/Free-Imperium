#pragma once
#include <cassert>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <map>

#include "map.h"
//#include "graphics/window.h"
//#include "graphics/shader.h"
#include "battle.h"
#include "camera.h"
#include "country.h"
#include "country_loader.h"
//#include "gui/gui_bridge.h"
//#include "gui_aid.h"
#include "peace_offer.h"
#include "process_packet.h"
#include "province.h"
#include "province_loader.h"
#include "scene.h"
#include "unit.h"
#include "war.h"
#include "map2.h"
#include "gui_last.h"
#include "gui/gui_structs.h"

class Game : public Scene {
    sf::TcpSocket& socket;
    Shader pickModelShader;
    Model3D model3d;
    std::vector<glm::mat4> uMat;
    std::vector<int> pids; // chyba trzyma id prowincji na ktorych sa unity
    std::map<unsigned int, int> colorToId;
    // Shader shader;
    // Window & window;
    Map map;
    std::unique_ptr<Map2> map2;
    GuiLast::Gui guiLast;
    glm::vec3 clickedProvColor;
    unsigned int clickedProviPhash;
    float markedProvId = -1.0f;
    glm::vec3 unitPos;
    float scale = 4.0f;
    std::unique_ptr<Texture> heightMap;
    int mapWidth = 5632, mapHeight = 2048;
    GuiLast::GuiEv ctype;
bool openMyCountry = false;
int openCountryId = -1;
int openProvId = -1;
int openBattleId = -1;
int openUnitId = -1;
int openWarInd = -1;
int openPeaceOffer = -1;
bool openUnitsList = false;
std::vector<int> clickedUnits;
SideBarData sideBarData;
void resetGuiWindows();
    // Camera camera;
    // Gui gui;
    glm::vec2 windowSize, resolution;
    float dt = 0.0f, drawDt = 0.0f;

    std::vector<sf::Packet> toSend;
    std::shared_ptr<Country> myCountry;
    std::vector<std::shared_ptr<Country>> countries;
    std::vector<std::unique_ptr<Province>> provinces;
    //std::vector<std::shared_ptr<Unit>> units;
    std::vector<Unit> units;
    std::vector<War> wars;
    std::vector<Battle> battles;
    std::vector<PeaceOffer> peaceOffers;

    std::vector<CountryData> ctrsData;
    void editCountryMap2(MapTexture* mt);
    void setCountryMap();
    void receivePackets();
    void processPacket(sf::Packet packet);
    void input();
    int provClick(glm::vec2 mouseInWorld);
    bool unitClick(glm::vec2 mouseInWorld);
    void unitMove(std::unordered_map<std::string, std::string>& values, glm::vec2 mouseInWorld);
    void processGuiEvent();
    void sendPackets();
    void updateBattles();
    void updateGui();
    
    void guiDraw();

   public:
    Game(Window& win, sf::TcpSocket& sock, std::string countryName, glm::vec2 res,
         std::vector<std::shared_ptr<Country>>& countries);
    ~Game();

    void Play();
};
