#pragma once
#include <cassert>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <map>
#include <memory>

#include "battle.h"
#include "camera.h"
#include "country.h"
#include "country_loader.h"
#include "font_batch.h"
#include "gui_structs.h"
#include "gui_last.h"
#include "map2.h"
#include "peace_offer.h"
#include "process_packet.h"
#include "province.h"
#include "province_loader.h"
#include "scene.h"
#include "unit.h"
#include "war.h"

class Game : public Scene {
    sf::TcpSocket& socket;
    Shader pickModelShader;
    Shader labelShader;
    Model3D model3d;
    std::vector<glm::mat4> uMat;
    std::vector<int> pids;  // chyba trzyma id prowincji na ktorych sa unity
    std::map<unsigned int, int> colorToId;
    std::unique_ptr<Map2> map2;
    GuiLast::Gui guiLast;
    glm::vec3 clickedProvColor;
    unsigned int clickedProviPhash;
    bool game_paused = false;
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
    int openWarId = -1;
    int openPeaceOfferId = -1;
    bool openUnitsList = false;
    int openUnitsListProvId = 0;
    std::vector<int> clickedUnits;
    SideBarData sideBarData;
    void resetGuiWindows();
    glm::vec2 windowSize, resolution;
    float dt = 0.0f, drawDt = 0.0f;

    std::vector<ProvData> provsData;
    std::vector<sf::Packet> toSend;
    std::shared_ptr<Country> myCountry;
    std::vector<std::shared_ptr<Country>> countries;
    std::vector<std::unique_ptr<Province>> provinces;
    std::vector<Unit> units;
    std::vector<War> wars;
    std::vector<Battle> battles;
    std::vector<PeaceOffer> peaceOffers;
    std::vector<FontVertex> fontVerts;
    GLuint fontCtrVao, fontCtrVbo;
    std::vector<std::vector<int>> ctrProvs;

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
    void makeCountryNames(const unsigned char* h);

    void guiDraw();

   public:
    Game(Window& win, sf::TcpSocket& sock, std::string countryName, glm::vec2 res,
         std::vector<std::shared_ptr<Country>>& countries);
    ~Game();

    void Play();
};
