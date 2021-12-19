#include "gui_last.h"

#include "asset_manager.h"
// using namespace GuiLast;

// Camera camera;

// Batch batch;
void GuiLast::Gui::init(Window* win, glm::vec2 resolution, glm::vec2 windowSize)
{
    // camera = Camera{window.GetSize()};
    res = resolution;
    winSize = windowSize;
    window = win;
    shader = Shader("src/graphics/shaders/gui_batch/a.vert", "src/graphics/shaders/gui_batch/a.frag", "", "");

    core.guiBatchInit();
    // batch.Init();
}

void GuiLast::Gui::reloadShader()
{
    shader = Shader("src/graphics/shaders/gui_batch/a.vert", "src/graphics/shaders/gui_batch/a.frag", "", "");
}

GuiLast::GuiEv GuiLast::Gui::game_topBar(const TopBarData& td, int mx, int my)
{
    GuiLast::GuiEv ct{ClickEventType::MISS};
    glm::vec2 offset{5.0f, 2.5f};
    glm::vec2 wSize{res.x * 0.5f, 100.0f};
    glm::vec3 wPos{res.x * 0.5f - wSize.x * 0.5f, res.y - wSize.y, 0.0f};
    core.drawRect(wPos, wSize, darkBrown);

    glm::vec2 ctrSize{200.0f, wSize.y};
    glm::vec3 ctrPos{wPos.x + wSize.x * 0.5f - ctrSize.x * 0.5f, wPos.y, 0.1f};
    core.drawText(ctrPos, ctrSize, greenCol, td.ctrName, TEXT_CENTER, AM::FontSize::PX64);

    glm::vec2 cashSize{100.0f, 40.0f};
    glm::vec3 cashPos{wPos.x + 10.0f, wPos.y + wSize.y * 0.5f, 0.05f};
    core.drawText(cashPos, cashSize, weirdBrown, std::string{"Gold: "} + std::to_string(td.gold), TEXT_LEFT);

    cashPos.y -= cashSize.y;
    core.drawText(cashPos, cashSize, brown, std::string{"Income: "} + std::to_string(td.income), TEXT_LEFT);

    {  // date group

        glm::vec2 dateGrpSize{150.0f, wSize.y - 2.0f * offset.y};
        glm::vec3 dateGrpPos{wPos.x + wSize.x - dateGrpSize.x - offset.x - 30, wPos.y + offset.y, 0.1f};

        glm::vec2 dateSize{dateGrpSize.x - 2 * offset.x, 25.0f};
        glm::vec3 datePos{dateGrpPos.x + offset.x, dateGrpPos.y + offset.y, dateGrpPos.z + 0.05f};
        core.drawText(datePos, dateSize, brown, td.date, TEXT_CENTER);
        datePos.y += dateSize.y + offset.y;
        core.drawText(datePos, dateSize, brown, std::string{"speed: "} + std::to_string(td.dateSpeed),
                      TEXT_CENTER);

        glm::vec2 arrowSize{25.0f, dateSize.y};
        glm::vec3 arrowPos{datePos.x - arrowSize.x, datePos.y, datePos.z};
        core.drawText(arrowPos, arrowSize, brown, "<<", TEXT_CENTER);
        if (core.isInRect(arrowPos, arrowSize, mx, my) && td.dateSpeed - 1 > 0)
            ct = GuiLast::GuiEv{ClickEventType::SET_SPEED, td.dateSpeed - 1};

        arrowPos.x = datePos.x + dateSize.x;
        core.drawText(arrowPos, arrowSize, brown, ">>", TEXT_CENTER);
        if (core.isInRect(arrowPos, arrowSize, mx, my) && td.dateSpeed + 1 <= 5)
            ct = GuiLast::GuiEv{ClickEventType::SET_SPEED, td.dateSpeed + 1};
    }

    if (ct.ct != ClickEventType::MISS)
        return ct;
    if (core.isInRect(ctrPos, ctrSize, mx, my))
        return GuiLast::GuiEv{ClickEventType::OPEN_MY_COUNTRY};
    if (core.isInRect(wPos, wSize, mx, my))
        return GuiLast::GuiEv{ClickEventType::NONE};
    return GuiLast::GuiEv{ClickEventType::MISS};
}

GuiLast::GuiEv GuiLast::Gui::game_myCountry(const Country& ctr, int mx, int my)
{
    glm::vec2 offset{5.0f, 2.5f};
    glm::vec2 wSize{res.x * 0.3f, res.y * 0.65f};
    glm::vec3 wPos{offset.x, offset.y, 0.1f};

    core.drawRect(wPos, wSize, darkBrown);

    glm::vec2 closeSize{100.0f, 40.0f};
    glm::vec3 closePos{wPos.x + wSize.x - offset.x - closeSize.x, wPos.y + offset.y, 0.2f};
    core.drawText(closePos, closeSize, greenCol, "Close", TEXT_LEFT, AM::FontSize::PX16);

    glm::vec2 nameSize{100.0f, 40.0f};
    glm::vec3 namePos{wPos.x + offset.x, wPos.y + wSize.y - offset.y - nameSize.y, 0.2f};
    core.drawText(namePos, nameSize, greenCol, "Country: ", TEXT_LEFT, AM::FontSize::PX16);

    glm::vec2 valueSize{100.0f, 40.0f};
    glm::vec3 valuePos{namePos.x + nameSize.x + offset.x, namePos.y, namePos.z};
    core.drawText(valuePos, valueSize, greenCol, ctr.GetName(), TEXT_LEFT, AM::FontSize::PX16);

    if (core.isInRect(closePos, closeSize, mx, my))
        return GuiLast::GuiEv{ClickEventType::CLOSE_WINDOW};
    if (core.isInRect(wPos, wSize, mx, my))
        return GuiLast::GuiEv{ClickEventType::NONE};
    return GuiLast::GuiEv{ClickEventType::MISS};
}

GuiLast::GuiEv GuiLast::Gui::game_country(const Country& ctr, int mx, int my)
{
    GuiLast::GuiEv ct{ClickEventType::MISS};
    glm::vec2 offset{5.0f, 2.5f};
    glm::vec2 wSize{res.x * 0.3f, res.y * 0.65f};
    glm::vec3 wPos{offset.x, offset.y, 0.1f};

    core.drawRect(wPos, wSize, darkBrown);

    glm::vec2 closeSize{100.0f, 40.0f};
    glm::vec3 closePos{wPos.x + wSize.x - offset.x - closeSize.x, wPos.y + offset.y, 0.2f};
    core.drawText(closePos, closeSize, greenCol, "Close", TEXT_LEFT, AM::FontSize::PX16);

    glm::vec2 nameSize{100.0f, 40.0f};
    glm::vec3 namePos{wPos.x + offset.x, wPos.y + wSize.y - offset.y - nameSize.y, 0.2f};
    core.drawText(namePos, nameSize, greenCol, "Country: ", TEXT_LEFT, AM::FontSize::PX16);

    glm::vec2 valueSize{100.0f, 40.0f};
    glm::vec3 valuePos{namePos.x + nameSize.x + offset.x, namePos.y, namePos.z};
    core.drawText(valuePos, valueSize, greenCol, ctr.GetName(), TEXT_LEFT, AM::FontSize::PX16);

    namePos.y -= nameSize.y - offset.y;
    valuePos.y -= valueSize.y - offset.y;
    core.drawRect(namePos, nameSize, weirdBrown);
    namePos.z += 0.1;
    core.drawText(namePos, nameSize, greenCol, "Declare war", TEXT_LEFT, AM::FontSize::PX16);
    namePos.z -= 0.1;
    if (core.isInRect(namePos, nameSize, mx, my))
        ct = GuiLast::GuiEv{ClickEventType::DECLARE_WAR, ctr.GetId()};

    if (ct.ct != ClickEventType::MISS)
        return ct;
    if (core.isInRect(closePos, closeSize, mx, my))
        return GuiLast::GuiEv{ClickEventType::CLOSE_WINDOW};
    if (core.isInRect(wPos, wSize, mx, my))
        return GuiLast::GuiEv{ClickEventType::NONE};
    return GuiLast::GuiEv{ClickEventType::MISS};
}

GuiLast::GuiEv GuiLast::Gui::game_prov(const Province& prov, int mx, int my, bool clicked, bool myProv)
{
    GuiLast::GuiEv ct{ClickEventType::MISS};
    glm::vec2 offset{5.0f, 2.5f};
    glm::vec2 wSize{res.x * 0.3f, res.y * 0.65f};
    glm::vec3 wPos{offset.x, offset.y, 0.1f};

    core.drawRect(wPos, wSize, darkBrown);

    glm::vec2 closeSize{100.0f, 40.0f};
    glm::vec3 closePos{wPos.x + wSize.x - offset.x - closeSize.x, wPos.y + offset.y, 0.2f};
    core.drawText(closePos, closeSize, greenCol, "Close", TEXT_LEFT, AM::FontSize::PX16);

    glm::vec2 nameSize{100.0f, 40.0f};
    glm::vec3 namePos{wPos.x + offset.x, wPos.y + wSize.y - offset.y - nameSize.y, 0.2f};
    core.drawText(namePos, nameSize, greenCol, "Province: ", TEXT_LEFT, AM::FontSize::PX16);
    glm::vec2 valueSize{100.0f, 40.0f};
    glm::vec3 valuePos{namePos.x + wSize.x * 0.5f - offset.x, namePos.y, namePos.z};
    core.drawText(valuePos, valueSize, greenCol, prov.GetName(), TEXT_LEFT, AM::FontSize::PX16);

    namePos.y -= nameSize.y - offset.y;
    core.drawText(namePos, nameSize, greenCol, "Country: ", TEXT_LEFT, AM::FontSize::PX16);
    valuePos.y -= valueSize.y - offset.y;
    core.drawText(valuePos, valueSize, greenCol, prov.GetCountry(), TEXT_LEFT, AM::FontSize::PX16);
    if (core.isInRect(valuePos, valueSize, mx, my))
        ct = GuiLast::GuiEv{ClickEventType::OPEN_COUNTRY, prov.GetCountryId()};

    if (myProv) {
        static int unitSize = 1000;
        namePos.y -= nameSize.y - offset.y;
        valuePos.y -= valueSize.y - offset.y;
        core.drawRect(namePos, nameSize, weirdBrown);
        namePos.z += 0.1;
        core.drawText(namePos, nameSize, greenCol, "Create unit of size: ", TEXT_LEFT, AM::FontSize::PX16);
        namePos.z -= 0.1;
        core.drawText(valuePos, valueSize, greenCol, std::to_string(unitSize), TEXT_LEFT, AM::FontSize::PX16);
        if (core.isInRect(namePos, nameSize, mx, my))
            ct = GuiLast::GuiEv{ClickEventType::CREATE_UNIT, unitSize};

        namePos.y -= nameSize.y - offset.y;
        valuePos.y -= valueSize.y - offset.y;
        core.drawText(namePos, nameSize, greenCol, "Size +1000", TEXT_LEFT, AM::FontSize::PX16);
        core.drawText(valuePos, valueSize, greenCol, "Size -1000", TEXT_LEFT, AM::FontSize::PX16);
        if (clicked) {
            if (core.isInRect(namePos, nameSize, mx, my))
                unitSize += 1000;
            if (core.isInRect(valuePos, valueSize, mx, my))
                unitSize -= 1000;
        }
    }

    if (ct.ct != ClickEventType::MISS)
        return ct;
    if (core.isInRect(closePos, closeSize, mx, my))
        return GuiLast::GuiEv{ClickEventType::CLOSE_WINDOW};
    if (core.isInRect(wPos, wSize, mx, my))
        return GuiLast::GuiEv{ClickEventType::NONE};
    return GuiLast::GuiEv{ClickEventType::MISS};
}

GuiLast::GuiEv GuiLast::Gui::game_battle(const Battle& battle, const Province& prov, int mx, int my, bool clicked,
                                         const std::vector<std::shared_ptr<Country>>& countries,
                                         const std::vector<Unit>& units)
{
    GuiLast::GuiEv ct{ClickEventType::MISS};
    glm::vec2 offset{5.0f, 2.5f};
    glm::vec2 wSize{res.x * 0.3f, res.y * 0.65f};
    glm::vec3 wPos{offset.x, offset.y, 0.1f};

    core.drawRect(wPos, wSize, darkBrown);

    glm::vec2 closeSize{100.0f, 40.0f};
    glm::vec3 closePos{wPos.x + wSize.x - offset.x - closeSize.x, wPos.y + offset.y, 0.2f};
    core.drawText(closePos, closeSize, greenCol, "Close", TEXT_LEFT, AM::FontSize::PX16);

    glm::vec2 nameSize{100.0f, 40.0f};
    glm::vec3 namePos{wPos.x + offset.x, wPos.y + wSize.y - offset.y - nameSize.y, 0.2f};
    core.drawText(namePos, nameSize, greenCol, "Province: ", TEXT_LEFT, AM::FontSize::PX16);
    glm::vec2 valueSize{100.0f, 40.0f};
    glm::vec3 valuePos{namePos.x + wSize.x * 0.5f - offset.x, namePos.y, namePos.z};
    core.drawText(valuePos, valueSize, greenCol, prov.GetName(), TEXT_LEFT, AM::FontSize::PX16);

    namePos.y -= nameSize.y - offset.y;
    core.drawText(namePos, nameSize, greenCol, "Attacker: ", TEXT_LEFT, AM::FontSize::PX16);
    valuePos.y -= valueSize.y - offset.y;
    if (battle.attackers.size()) {
        for (std::size_t i = 0; i < units.size(); ++i) {
            if (units[i].id == battle.attackers[0]) {
                assert(units[i].countryId >= 0 && units[i].countryId < countries.size());
                core.drawText(valuePos, valueSize, greenCol, countries[units[i].countryId]->GetName(), TEXT_LEFT,
                              AM::FontSize::PX16);
                break;
            }
        }
    }
    namePos.y -= nameSize.y - offset.y;
    core.drawText(namePos, nameSize, greenCol, "Defenders count: ", TEXT_LEFT, AM::FontSize::PX16);
    valuePos.y -= valueSize.y - offset.y;
    if (battle.defenders.size()) {
        for (std::size_t i = 0; i < units.size(); ++i) {
            if (units[i].id == battle.defenders[0]) {
                assert(units[i].countryId >= 0 && units[i].countryId < countries.size());
                core.drawText(valuePos, valueSize, greenCol, countries[units[i].countryId]->GetName(), TEXT_LEFT,
                              AM::FontSize::PX16);
                break;
            }
        }
    }

    namePos.y -= nameSize.y - offset.y;
    core.drawText(namePos, nameSize, greenCol, "Attackers count: ", TEXT_LEFT, AM::FontSize::PX16);
    valuePos.y -= valueSize.y - offset.y;
    core.drawText(valuePos, valueSize, greenCol, std::to_string(battle.attackersSize), TEXT_LEFT,
                  AM::FontSize::PX16);

    namePos.y -= nameSize.y - offset.y;
    core.drawText(namePos, nameSize, greenCol, "Defenders count: ", TEXT_LEFT, AM::FontSize::PX16);
    valuePos.y -= valueSize.y - offset.y;
    core.drawText(valuePos, valueSize, greenCol, std::to_string(battle.defendersSize), TEXT_LEFT,
                  AM::FontSize::PX16);

    if (core.isInRect(valuePos, valueSize, mx, my))
        ct = GuiLast::GuiEv{ClickEventType::OPEN_COUNTRY_FROM_PROV};

    if (ct.ct != ClickEventType::MISS)
        return ct;
    if (core.isInRect(closePos, closeSize, mx, my))
        return GuiLast::GuiEv{ClickEventType::CLOSE_WINDOW};
    if (core.isInRect(wPos, wSize, mx, my))
        return GuiLast::GuiEv{ClickEventType::NONE};
    return GuiLast::GuiEv{ClickEventType::MISS};
}
GuiLast::GuiEv GuiLast::Gui::game_unit(const Unit& unit, int mx, int my, bool clicked)
{
    GuiLast::GuiEv ct{ClickEventType::MISS};
    glm::vec2 offset{5.0f, 2.5f};
    glm::vec2 wSize{res.x * 0.3f, res.y * 0.65f};
    glm::vec3 wPos{offset.x, offset.y, 0.1f};

    core.drawRect(wPos, wSize, darkBrown);

    glm::vec2 closeSize{100.0f, 40.0f};
    glm::vec3 closePos{wPos.x + wSize.x - offset.x - closeSize.x, wPos.y + offset.y, 0.2f};
    core.drawText(closePos, closeSize, greenCol, "Close", TEXT_LEFT, AM::FontSize::PX16);

    glm::vec2 nameSize{100.0f, 40.0f};
    glm::vec3 namePos{wPos.x + offset.x, wPos.y + wSize.y - offset.y - nameSize.y, 0.2f};
    core.drawText(namePos, nameSize, greenCol, "name: ", TEXT_LEFT, AM::FontSize::PX16);
    glm::vec2 valueSize{100.0f, 40.0f};
    glm::vec3 valuePos{namePos.x + wSize.x * 0.5f - offset.x, namePos.y, namePos.z};
    core.drawText(valuePos, valueSize, greenCol, unit.GetName(), TEXT_LEFT, AM::FontSize::PX16);

    namePos.y -= nameSize.y - offset.y;
    core.drawText(namePos, nameSize, greenCol, "Country: ", TEXT_LEFT, AM::FontSize::PX16);
    valuePos.y -= valueSize.y - offset.y;
    core.drawText(valuePos, valueSize, greenCol, unit.GetCountry(), TEXT_LEFT, AM::FontSize::PX16);
    if (core.isInRect(valuePos, valueSize, mx, my))
        ct = GuiLast::GuiEv{ClickEventType::OPEN_COUNTRY, unit.GetCountryId()};

    namePos.y -= nameSize.y - offset.y;
    core.drawText(namePos, nameSize, greenCol, "Unit size: ", TEXT_LEFT, AM::FontSize::PX16);
    valuePos.y -= valueSize.y - offset.y;
    core.drawText(valuePos, valueSize, greenCol, std::to_string(unit.GetSoldiers()), TEXT_LEFT,
                  AM::FontSize::PX16);

    if (ct.ct != ClickEventType::MISS)
        return ct;
    if (core.isInRect(closePos, closeSize, mx, my))
        return GuiLast::GuiEv{ClickEventType::CLOSE_WINDOW};
    if (core.isInRect(wPos, wSize, mx, my))
        return GuiLast::GuiEv{ClickEventType::NONE};
    return GuiLast::GuiEv{ClickEventType::MISS};
}

GuiLast::GuiEv GuiLast::Gui::game_unitsList(const std::vector<Unit*>& units, int mx, int my, bool clicked)
{
    GuiLast::GuiEv ct{ClickEventType::MISS};
    glm::vec2 offset{5.0f, 2.5f};
    glm::vec2 wSize{res.x * 0.3f, res.y * 0.65f};
    glm::vec3 wPos{offset.x, offset.y, 0.1f};

    core.drawRect(wPos, wSize, darkBrown);

    glm::vec2 closeSize{100.0f, 40.0f};
    glm::vec3 closePos{wPos.x + wSize.x - offset.x - closeSize.x, wPos.y + offset.y, 0.2f};
    core.drawText(closePos, closeSize, greenCol, "Close", TEXT_LEFT, AM::FontSize::PX16);

    glm::vec2 nameSize{100.0f, 40.0f};
    glm::vec3 namePos{wPos.x + offset.x, wPos.y + wSize.y - offset.y - nameSize.y, 0.2f};
    glm::vec2 valueSize{100.0f, 40.0f};
    glm::vec3 valuePos{namePos.x + wSize.x * 0.5f - offset.x, namePos.y, namePos.z};
    for (auto& unit : units) {
        core.drawText(namePos, nameSize, greenCol, "name: ", TEXT_LEFT, AM::FontSize::PX16);
        core.drawText(valuePos, valueSize, greenCol, unit->GetName(), TEXT_LEFT, AM::FontSize::PX16);

        if (core.isInRect(valuePos, valueSize, mx, my))
            ct = GuiLast::GuiEv{ClickEventType::OPEN_UNIT, unit->GetId()};
        namePos.y -= nameSize.y - offset.y;
        valuePos.y -= valueSize.y - offset.y;
    }

    if (ct.ct != ClickEventType::MISS)
        return ct;
    if (core.isInRect(closePos, closeSize, mx, my))
        return GuiLast::GuiEv{ClickEventType::CLOSE_WINDOW};
    if (core.isInRect(wPos, wSize, mx, my))
        return GuiLast::GuiEv{ClickEventType::NONE};
    return GuiLast::GuiEv{ClickEventType::MISS};
}

GuiLast::GuiEv GuiLast::Gui::game_SideBar(const SideBarData& sbData, int mx, int my, bool clicked)
{
    GuiLast::GuiEv ct{ClickEventType::MISS};
    glm::vec2 offset{5.0f, 2.5f};
    glm::vec2 wSize{res.x * 0.05f, res.y * 0.65f};
    glm::vec3 wPos{res.x - offset.x - wSize.x, wSize.y * 0.25f, 0.1f};

    core.drawRect(wPos, wSize, darkBrown);

    glm::vec2 elSize{wSize.x, wSize.x};
    glm::vec3 startPos{wPos.x, wPos.y + wSize.y - elSize.y, 0.2f};
    glm::vec2 elTextSize{200.0f, wSize.x};
    glm::vec3 startTextPos{startPos.x - elTextSize.x, startPos.y, startPos.z};
    for (std::size_t i = 0; i < sbData.elements.size(); ++i) {
        if (core.isInRect(startPos, elSize, mx, my)) {
            core.drawRect(startTextPos, elTextSize, darkBrown);
            core.drawText(startTextPos, elTextSize, greenCol, sbData.elements[i].hoverText, TEXT_LEFT,
                          AM::FontSize::PX16);
        }
        core.drawIcon(startPos, elSize, (int)sbData.elements[i].type);

        if (core.isInRect(startPos, elSize, mx, my))
            ct = GuiLast::GuiEv{ClickEventType::SIDEBAR_LEFTC, i};
        startPos.y -= elSize.y - offset.y;
        startTextPos.y = startPos.y;
    }

    if (ct.ct != ClickEventType::MISS)
        return ct;
    //    if (core.isInRect(closePos, closeSize, mx, my))
    //        return GuiLast::GuiEv{ClickEventType::CLOSE_WINDOW};
    if (core.isInRect(wPos, wSize, mx, my))
        return GuiLast::GuiEv{ClickEventType::NONE};
    return GuiLast::GuiEv{ClickEventType::MISS};
}

GuiLast::GuiEv GuiLast::Gui::room_countryList(const std::vector<std::string>& plarr, int mx, int my)
{
    return GuiLast::GuiEv{ClickEventType::MISS};
}

// void GuiLast::Gui::room_playerListDraw(const std::vector<std::string>& playersList)
GuiLast::GuiEv GuiLast::Gui::room_playerList(const std::vector<std::string>& plarr, int mx, int my, int scr)
{
    GuiLast::GuiEv ct{ClickEventType::MISS};
    glm::vec2 offset{5.0f, 2.5f};
    // glm::vec2 wSize{res.x * 0.3f, res.y * 0.65f};
    // glm::vec3 wPos{offset.x, offset.y, 0.1f};
    glm::vec2 wSize{res.x * 0.3f, 200.0f};
    glm::vec3 wPos{offset.x, 200.0f, 0.1f};

    core.drawRect(wPos, wSize, darkBrown);
    glm::vec2 nameSize{wSize.x, 50.0f};
    glm::vec3 namePos{wPos.x + offset.x, wPos.y + wSize.y - offset.y - nameSize.y, 0.2f};
    {  // list
        flush();
        glScissor(wPos.x, wPos.y, wSize.x, wSize.y);
        start();

        static float sc = 0;
        float pcount = 20;
        if (1 || scr != 0) {
            sc += scr * 30;
            float lasty = namePos.y;
            for (int i = 0; i < pcount - 1; ++i) {
                lasty -= nameSize.y - offset.y;
            }
            if (wSize.y > (nameSize.y + offset.y) * (pcount - 0.0f)) {  // wSize.y > listlen
                sc = 0.0f;
            }
            else if (namePos.y + sc < namePos.y) {
                sc = 0.0f;
            }
            else if (lasty + sc > wPos.y + offset.y) {
                sc = wPos.y + offset.y - (lasty);
                Log(plarr.size());
            }
        }
        namePos.y += sc;

        for (int i = 0; i < pcount; ++i) {
            core.drawText(namePos, nameSize, greenCol, std::to_string(i), TEXT_LEFT, AM::FontSize::PX16);
            namePos.y -= nameSize.y - offset.y;
        }
        // Log(namePos.y + nameSize.y + offset.y);
        //  for (auto& pl : plarr) {
        //      core.drawText(namePos, nameSize, greenCol, pl, TEXT_LEFT, AM::FontSize::PX16);
        //      namePos.y -= nameSize.y - offset.y;
        //  }
        flush();
        glScissor(0.0f, 0.0f, res.x, res.y);
        start();
    }
    if (ct.ct != ClickEventType::MISS)
        return ct;
    if (core.isInRect(wPos, wSize, mx, my))
        return GuiLast::GuiEv{ClickEventType::NONE};
    return GuiLast::GuiEv{ClickEventType::MISS};

    return GuiLast::GuiEv{ClickEventType::MISS};
}

/*
void doTestList(int scroll, int mx, int my, float dt, bool lpress, bool lclicked)
{
    static int scr = 0;
    int scrspeed = 10;
    static glm::vec3 delta{0.0f, 0.0f, 0.0f};
    static glm::vec3 lpos{300.0f, 250.0f, 0.1f};
    glm::vec2 lsize{100.0f, 200.0f};
    if (delta.x != 0.0f && delta.y != 0.0f) {
        lpos.x = ((float)mx - (delta.x));
        lpos.y = ((float)my - (delta.y));
    }
    bool minside = core.isInRect(lpos, lsize, mx, my);
    if (minside)
        scr -= scroll * scrspeed;
    if (delta.x == 0.0f && delta.y == 0.0f && lpress && minside) {
        delta = glm::vec3{(float)mx - lpos.x, (float)my - lpos.y, 0.0f};
    }
    else if (!lpress || !minside)
        delta = glm::vec3{0.0f, 0.0f, 0.0f};

    std::size_t listid = createList(lpos, lsize, brownCol);
    std::string text = std::to_string(dt);
    float lastY = getListLastY(listid);
    float ypadding = getListYPadding(listid);
    int listElCount = 6;
    std::size_t lrectids[listElCount];
    // ClickEvent ce[listElCount];
    for (int i = 0; i < listElCount; ++i) {
        glm::vec2 elSize{lsize.x, 40.0f};
        glm::vec3 elPos{lpos.x, lastY - ypadding - elSize.y, lpos.z + 0.01f};
        lastY = elPos.y;
        lrectids[i] = core.drawRectToList(elPos, elSize, greenCol, listid);
        glm::vec3 tElPos = elPos;
        tElPos.z += 0.01f;
        // core.drawTextToList(tElPos, elSize, brownCol, "Lukasz Kadracki", TEXT_CENTER, listid);
        core.drawTextToList(tElPos, elSize, brownCol, text, TEXT_CENTER, listid);
    }
    setListLastY(listid, lastY);
    listScroll(listid, scr);

    if (minside && lclicked)
        for (int i = 0; i < listElCount; ++i) {
            if (core.isInRectList(listid, lrectids[i], mx, my))
                Log("clicked " << i);
        }
}
*/
void GuiLast::Gui::start()
{
    // Shader shader("src/graphics/shaders/gui_batch/a.vert", "src/graphics/shaders/gui_batch/a.frag", "", "");
    glUseProgram(shader.GetProgram());
    glm::mat4 matrix = glm::ortho(0.0f, (float)winSize.x, 0.0f, (float)winSize.y);
    glUniformMatrix4fv(glGetUniformLocation(shader.GetProgram(), "pr_matrix"), 1, GL_FALSE,
                       glm::value_ptr(matrix));

    GLint tex[32];
    for (GLint i = 0; i < 32; ++i) {
        tex[i] = i;
    }
    GLuint fontTexID[32];
    for (int i = 0; i <= (int)AM::FontSize::PX160; ++i) {
        fontTexID[i] = AM::atlasTexture[i]->GetId();
    }
    for (int i = 0; i <= (int)AM::FontSize::PX160; ++i) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, (GLuint)fontTexID[i]);
    }
    fontTexID[31] = AM::am.iconsTexture->GetId();
    glActiveTexture(GL_TEXTURE0 + 31);
    glBindTexture(GL_TEXTURE_2D, (GLuint)fontTexID[31]);
    glUniform1iv(glGetUniformLocation(shader.GetProgram(), "tex"), 32, tex);
    core.guiBatchBegin();
    /*
        float dt = 0.0f, time = 0.0f;
        while (!window->ShouldClose()) {
            glEnable(GL_DEPTH_TEST);  // Enable depth testing for z-culling
            glDepthFunc(GL_LESS);
            glEnable(GL_SCISSOR_TEST);
            glScissor(0, 0, winSize.x, winSize.y);
            window->Refresh();
            glm::vec2 mousePos{(float)window->xMouse * res.x / winSize.x,
                               (float)(winSize.y - window->yMouse) * (res.y / winSize.y)};

            if (window->keys['M']) {
                shader =
                    Shader("src/graphics/shaders/gui_batch/a.vert", "src/graphics/shaders/gui_batch/a.frag", "",
       "");
            }

            core.guiBatchBegin();
            // batch.Begin();
            glm::vec3 pos{10.0f, 135.0f, 0.0f};
            glm::vec2 size{100.0f, 200.0f};

            core.drawRect(pos, size, brownCol);
            glm::vec3 tpos{10.0f, 10.0f, 0.1f};
            glm::vec2 tsize{100.0f, 200.0f};

            core.drawText(tpos, tsize, greenCol, "Lukasz Kadracki", TEXT_CENTER);

            doTestList(window->scrollOffset, mousePos.x, mousePos.y, dt, window->mouseL, window->mouseLClicked);
            window->scrollOffset = 0;
            window->mouseLClicked = false;
            drawLists();

            core.guiBatchFlush();
            // batch.Flush();

            window->Update();
            dt = glfwGetTime() - time;
            time = glfwGetTime();
        }
        */
}

void GuiLast::Gui::flush()
{
    // drawLists();
    core.guiBatchFlush();
}
