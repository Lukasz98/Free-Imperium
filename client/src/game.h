#pragma once
#include <cassert>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <map>
#include <memory>
#include <unordered_set>

#include "font_batch.h"
#include "game_data.h"
#include "gui_last.h"
#include "gui_structs.h"
#include "map2.h"
#include "peace_offer.h"
#include "process_packet.h"
#include "settings.h"

struct UnitsAtPoint {  // for drawing unit labels
    std::vector<int> uind;
    std::unordered_set<int> ctrIds;
    long point;      // from unitfakepos.x & .y
    int status = 0;  // 0 - own, 1 - enemy, 2 - enemy of enemy
    int size = 0;
    int perm = 0;
};


class Game {
    sf::TcpSocket& socket;
    Shader pickModelShader;
    Shader labelShader;
    Model3D model3d;
    std::vector<glm::mat4> uMat;
    std::vector<int> pids;
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
    bool drawingMoves = false;
    bool openUnitsList = false;
    int openUnitsListProvId = 0;
    std::vector<int> clickedUnits;
        std::vector<std::size_t> uinds;
            std::vector<UnitsAtPoint> unitsAtPoint;
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
