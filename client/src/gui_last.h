#pragma once

#include <cassert>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <vector>

#include "battle.h"
#include "camera.h"
#include "click_event_enum.h"
#include "country.h"
#include "graphics/shader.h"
#include "graphics/window.h"
#include "gui_last/gui_core_last.h"
#include "gui_structs.h"
#include "peace_offer.h"
#include "province.h"
#include "unit.h"
#include "war.h"

namespace GuiLast {

struct GuiEv {
    ClickEventType ct;
    int val;
    GuiEv() : ct(ClickEventType::MISS) {}
    GuiEv(ClickEventType ct) : ct(ct) {}
    GuiEv(ClickEventType ct, int val) : ct(ct), val(val) {}
};

struct Gui {
    GuiCore core;
    GLuint iconsTid;

    Window* window;
    Shader shader;
    glm::vec2 res, winSize;

    void init(Window* window, glm::vec2 resolution, glm::vec2 windowSize);
    void start();
    void start(const glm::mat4& matrix);
    void flush();

    void reloadShader();

    GuiEv start_menu(int mx, int my);
    GuiEv game_topBar(const TopBarData& td, const Country* ctr, int mx, int my);
    GuiEv game_drawPause();
    GuiEv game_myCountry(const Country* ctr, int mx, int my);
    GuiEv game_country(const Country* ctr, const Country* myCountry, const std::vector<War>& wars, int mx, int my);
    GuiEv game_prov(const Province& prov, int mx, int my, bool clicked, bool myProv);
    GuiEv game_battle(const Battle& battle, const Province& prov, int mx, int my, bool clicked,
                      const std::vector<Country>& countries, const std::vector<Unit>& units);
    GuiEv game_unit(const Unit& unit, int mx, int my, bool clicked, std::vector<Country>& countries);
    GuiEv game_unitsList(const std::vector<Unit*>& units, int mx, int my, bool clicked, int scr,
                         const Province* prov, std::vector<Country>& countries);
    GuiEv game_SideBar(const SideBarData& sbData, int mx, int my, bool clicked, int scr);

    GuiEv room_playerList(const std::string& playerName, const std::vector<std::string>& plarr, int mx, int my,
                          int sc);
    //GuiEv room_countryList(const std::vector<std::string>& ctrarr, int mx, int my, int scr);
    GuiEv room_startButton(int mx, int my);

    GuiEv game_war(const War* war, int mx, int my, bool clicked);
    GuiEv game_peaceOffer(const PeaceOffer* offer, int mx, int my, int myctrid,
                          const std::vector<Province>& provinces, std::vector<Country>& countries, bool clicked,
                          int scr);

    //void game_drawUnitBar(const Unit& u);
};

};  // namespace GuiLast
