#pragma once

#include <cassert>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <vector>

#include "battle.h"
#include "camera.h"
#include "country.h"
#include "graphics/shader.h"
#include "graphics/window.h"
#include "gui/click_event_enum.h"
#include "gui/gui_structs.h"
#include "province.h"
#include "unit.h"
#include "gui_last/gui_core_last.h"

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
    glm::vec4 brownCol{56.0f / 255.0f, 31.0f / 255.0f, 19.0f / 255.0f, 1.0f};
    glm::vec4 greenCol{0.0f / 255.0f, 255.0f / 255.0f, 0.0f / 255.0f, 1.0f};
    glm::vec4 darkBrown{0.294, 0.227, 0.223, 1.0};
    glm::vec4 brown{0.388, 0.298, 0.290, 1.0};
    glm::vec4 lightBrown{0.568, 0.435, 0.427, 1.0};
    glm::vec4 weirdBrown{.333, .258, 0.258, 1.0};

    GLuint iconsTid;

    Window* window;
    Shader shader;
    glm::vec2 res, winSize;

    void init(Window* window, glm::vec2 resolution, glm::vec2 windowSize);
    void start();
    void flush();

    void reloadShader();

    GuiEv game_topBar(const TopBarData& td, int mx, int my);
    GuiEv game_myCountry(const Country& ctr, int mx, int my);
    GuiEv game_country(const Country& ctr, int mx, int my);
    GuiEv game_prov(const Province& prov, int mx, int my, bool clicked, bool myProv);
    GuiEv game_battle(const Battle& battle, const Province& prov, int mx, int my, bool clicked,
                      const std::vector<std::shared_ptr<Country>>& countries, const std::vector<Unit>& units);
    GuiEv game_unit(const Unit& unit, int mx, int my, bool clicked);
    GuiEv game_unitsList(const std::vector<Unit*>& units, int mx, int my, bool clicked);
    GuiEv game_SideBar(const SideBarData& sbData, int mx, int my, bool clicked);

    GuiEv room_playerList(const std::vector<std::string>& plarr, int mx, int my, int sc);
    GuiEv room_countryList(const std::vector<std::string>& ctrarr, int mx, int my);
};

};  // namespace GuiLast
