#pragma once
#include <cassert>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <map>
#include <memory>

#include "font_batch.h"
#include "game_data.h"
#include "gui_last.h"
#include "gui_structs.h"
#include "map2.h"
#include "peace_offer.h"
#include "process_packet.h"
#include "settings.h"


class Game {
    sf::TcpSocket& socket;
    Shader pickModelShader;
    Shader labelShader;
    Model3D model3d;
    std::vector<glm::mat4> uMat;
    std::vector<int> pids;  // chyba trzyma id prowincji na ktorych sa unity
    GuiLast::Gui guiLast;
    bool game_paused = false;
    float markedProvId = -1.0f;
    GuiLast::GuiEv ctype = ClickEventType::MISS;
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
    float dt = 0.0f, drawDt = 0.0f;

    std::vector<sf::Packet> toSend;
    int myCountryId;
    std::vector<PeaceOffer> peaceOffers;
    GameData* gd;

    std::vector<CountryData> ctrsData;
    void setCountryMap();
    void receivePackets();
    void processPacket(sf::Packet packet);
    void input();
    int provClick();
    bool unitClick();
    void processGuiEvent();
    void sendPackets();
    void updateBattles();

    void guiDraw();
    bool isAtWar(int ctr1, int ctr2);
   public:
    Game(sf::TcpSocket& sock, std::string countryName, GameData* gd);
    ~Game();

    void Play();
};
