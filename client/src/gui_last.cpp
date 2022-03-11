#include "gui_last.h"

#include "asset_manager.h"
#include "int_to_string.h"
static glm::vec4 brownCol{56.0f / 255.0f, 31.0f / 255.0f, 19.0f / 255.0f, 1.0f};
static glm::vec4 greenCol{0.0f / 255.0f, 255.0f / 255.0f, 0.0f / 255.0f, 1.0f};
static glm::vec4 darkBrown{0.294, 0.227, 0.223, 1.0};
static glm::vec4 brown{0.388, 0.298, 0.290, 1.0};
static glm::vec4 lightBrown{0.568, 0.435, 0.427, 1.0};
static glm::vec4 weirdBrown{.333, .258, 0.258, 1.0};
static glm::vec4 whiteCol{1.0, 1.0, 1.0, 1.0};

void GuiLast::Gui::init(Window* win, glm::vec2 resolution, glm::vec2 windowSize)
{
    res = resolution;
    winSize = windowSize;
    window = win;
    shader = Shader("src/graphics/shaders/gui_batch/a.vert", "src/graphics/shaders/gui_batch/a.frag", "", "");

    core.guiBatchInit();
}

static void listDraw(GuiLast::Gui& gui, glm::vec3 listPos, glm::vec2 listSize, glm::vec2 offset,
                     glm::vec3& itemPos, glm::vec2 itemSize, float& sc, int scr, int itemCount,
                     bool dontRestrictX = false, float resx = 0.0f)
{
    gui.flush();
    if (dontRestrictX)
        glScissor(0.0f, listPos.y, resx, listSize.y);
    else
        glScissor(listPos.x, listPos.y, listSize.x, listSize.y);
    gui.start();
    {
        itemPos.y = listPos.y + listSize.y - offset.y - itemSize.y;
        float lasty = itemPos.y;
        for (int i = 0; i < itemCount - 1; ++i) {
            lasty = lasty - itemSize.y - offset.y;
        }
        if (scr != 0) {
            sc += scr * 30;
            if (listSize.y > (itemSize.y + offset.y) * (itemCount - 0.0f)) {  // wSize.y > listlen
                sc = 0.0f;
            }
            else if (itemPos.y + sc < itemPos.y) {
                sc = 0.0f;
            }
            else if (lasty + sc > listPos.y + offset.y) {
                sc = listPos.y + offset.y - (lasty);
            }
        }
        itemPos.y += sc;
        lasty += sc;
        {  // draw bar
            glm::vec4 barCol = {200.0f / 255.0f, 200.0f / 255.0f, 200.0f / 255.0f, 1.0f};
            glm::vec4 barBgCol{86.0f / 255.0f, 69.0f / 255.0f, 55.0f / 255.0f, 1.0f};
            float firsty = itemPos.y;
            float totalh = firsty - lasty;
            glm::vec2 rsize = glm::vec2{listSize.x * 0.05f, (listSize.y / totalh) * listSize.y};
            if (rsize.y > listSize.y - offset.y * 2)
                rsize.y = listSize.y - offset.y * 2;

            float bottomYOffset = listPos.y + offset.y - lasty;
            float tmp = bottomYOffset / totalh;
            tmp *= listSize.y;
            glm::vec3 rpos{listPos.x + listSize.x - rsize.x, listPos.y + offset.y + tmp, listPos.z + 0.55};
            if (rpos.y < listPos.y + offset.y)
                rpos.y = listPos.y + offset.y;
            else if (rpos.y + rsize.y > listPos.y + listSize.y - offset.y)
                rpos.y = listPos.y + offset.y + listSize.y - rsize.y - offset.y;
            glm::vec4 rcol{1.0f, 0.0f, 0.0f, 1.0f};
            gui.core.drawRect(glm::vec3{rpos.x, listPos.y, listPos.z + 0.5f}, glm::vec2{rsize.x, listSize.y},
                              barBgCol);
            gui.core.drawRect(rpos, rsize, barCol);
        }
    }
}

void GuiLast::Gui::reloadShader()
{
    shader = Shader("src/graphics/shaders/gui_batch/a.vert", "src/graphics/shaders/gui_batch/a.frag", "", "");
}

GuiLast::GuiEv GuiLast::Gui::start_menu(int mx, int my)
{
    GuiLast::GuiEv ct{ClickEventType::MISS};
    glm::vec2 offset{5.0f, 5.5f};
    glm::vec2 itemSize{res.x * 0.5f, 80.0f};
    glm::vec3 itemPos{50.0f, res.y * 0.7f, 0.0f};

    core.drawRect(itemPos, itemSize, weirdBrown);
    core.drawText(itemPos, itemSize, whiteCol, "Free Imperium", TEXT_LEFT, AM::FontSize::PX64);
    itemSize.y = 40.0f;
    itemSize.x = res.x * 0.3f;
    itemPos.y -= itemSize.y + offset.y * 4.0f;

    core.drawRect(itemPos, itemSize, weirdBrown);
    core.drawText(itemPos, itemSize, whiteCol, "Play", TEXT_LEFT, AM::FontSize::PX32);
    if (core.isInRect(itemPos, itemSize, mx, my))
        ct = GuiLast::GuiEv{ClickEventType::START_ROOM};
    itemPos.y -= itemSize.y + offset.y;

    core.drawRect(itemPos, itemSize, weirdBrown);
    core.drawText(itemPos, itemSize, whiteCol, "Quit", TEXT_LEFT, AM::FontSize::PX32);
    if (core.isInRect(itemPos, itemSize, mx, my))
        ct = GuiLast::GuiEv{ClickEventType::QUIT_GAME};

    return ct;
}

GuiLast::GuiEv GuiLast::Gui::game_topBar(const TopBarData& td, const Country* ctr, int mx, int my)
{
    GuiLast::GuiEv ct{ClickEventType::MISS};
    glm::vec2 offset{5.0f, 2.5f};
    glm::vec2 wSize{res.x * 0.5f, 100.0f};
    glm::vec3 wPos{res.x * 0.5f - wSize.x * 0.5f, res.y - wSize.y, 0.0f};
    core.drawRect(wPos, wSize, darkBrown);

    glm::vec2 ctrSize{200.0f, wSize.y};
    glm::vec3 ctrPos{wPos.x + wSize.x * 0.5f - ctrSize.x * 0.5f, wPos.y, 0.1f};
    core.drawText(ctrPos, ctrSize, whiteCol, td.ctrName, TEXT_CENTER, AM::FontSize::PX64);

    { // ctr bar
        glm::vec2 barSize{10.0f, wSize.y};
        glm::vec3 barPos{ctrPos.x - barSize.x, wPos.y, 0.1f};
        core.drawRect(barPos, barSize, glm::vec4{ctr->color.r / 255.0f, ctr->color.g / 255.0f, ctr->color.b / 255.0f, 1.0f});

        barPos.x = ctrPos.x + ctrSize.x;
        core.drawRect(barPos, barSize, glm::vec4{ctr->color.r / 255.0f, ctr->color.g / 255.0f, ctr->color.b / 255.0f, 1.0f});
    }

    glm::vec2 cashSize{100.0f, 40.0f};
    glm::vec3 cashPos{wPos.x + 10.0f, wPos.y + wSize.y * 0.5f, 0.05f};
    core.drawText(cashPos, cashSize, whiteCol, std::string{"Gold: "} + std::to_string(td.gold), TEXT_LEFT);

    cashPos.y -= cashSize.y;
    core.drawText(cashPos, cashSize, whiteCol, std::string{"Income: "} + std::to_string(td.income), TEXT_LEFT);

    {  // date group

        glm::vec2 dateGrpSize{150.0f, wSize.y - 2.0f * offset.y};
        glm::vec3 dateGrpPos{wPos.x + wSize.x - dateGrpSize.x - offset.x - 30, wPos.y + offset.y, 0.1f};

        glm::vec2 dateSize{dateGrpSize.x - 2 * offset.x, 25.0f};
        glm::vec3 datePos{dateGrpPos.x + offset.x, dateGrpPos.y + offset.y, dateGrpPos.z + 0.05f};
        core.drawText(datePos, dateSize, whiteCol, td.date, TEXT_CENTER);
        datePos.y += dateSize.y + offset.y;
        core.drawText(datePos, dateSize, whiteCol, std::string{"speed: "} + std::to_string(td.dateSpeed),
                      TEXT_CENTER);

        glm::vec2 arrowSize{25.0f, dateSize.y};
        glm::vec3 arrowPos{datePos.x - arrowSize.x, datePos.y, datePos.z};
        core.drawRect(arrowPos, arrowSize, weirdBrown);
        arrowPos.z += 0.05f;
        core.drawText(arrowPos, arrowSize, whiteCol, "<<", TEXT_CENTER);
        if (core.isInRect(arrowPos, arrowSize, mx, my) && td.dateSpeed - 1 > 0)
            ct = GuiLast::GuiEv{ClickEventType::SET_SPEED, td.dateSpeed - 1};

        arrowPos.x = datePos.x + dateSize.x;
        arrowPos.z -= 0.05f;
        core.drawRect(arrowPos, arrowSize, weirdBrown);
        arrowPos.z += 0.05f;
        core.drawText(arrowPos, arrowSize, whiteCol, ">>", TEXT_CENTER);
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

GuiLast::GuiEv GuiLast::Gui::game_drawPause()
{
    glm::vec2 itemSize{res.x * 0.5f, 100.0f};
    glm::vec3 itemPos{res.x * 0.5f - itemSize.x * 0.5f, 0.6f * res.y, 0.3f};
    core.drawRect(itemPos, itemSize, glm::vec4{weirdBrown.x, weirdBrown.y, weirdBrown.z, 0.3f});
    itemPos.z += 0.05f;
    core.drawText(itemPos, itemSize, whiteCol, "Pause", TEXT_CENTER, AM::FontSize::PX64);

    return GuiLast::GuiEv{ClickEventType::MISS};
}

GuiLast::GuiEv GuiLast::Gui::game_myCountry(const Country* ctr, int mx, int my)
{
    glm::vec2 offset{5.0f, 2.5f};
    glm::vec2 wSize{res.x * 0.3f, res.y * 0.65f};
    glm::vec3 wPos{offset.x, offset.y, 0.1f};

    core.drawRect(wPos, wSize, darkBrown);

    glm::vec2 closeSize{100.0f, 40.0f};
    glm::vec3 closePos{wPos.x + wSize.x - offset.x - closeSize.x, wPos.y + offset.y, 0.2f};
    core.drawRect(closePos, closeSize, weirdBrown);
    closePos.z += 0.05f;
    core.drawText(closePos, closeSize, whiteCol, "Close", TEXT_CENTER, AM::FontSize::PX16);

    glm::vec2 nameSize{200.0f, 40.0f};
    glm::vec3 namePos{wPos.x + offset.x, wPos.y + wSize.y - offset.y - nameSize.y, 0.2f};
    core.drawText(namePos, nameSize, whiteCol, "Country: ", TEXT_LEFT, AM::FontSize::PX16);

    glm::vec2 valueSize{100.0f, 40.0f};
    glm::vec3 valuePos{namePos.x + nameSize.x + offset.x, namePos.y, namePos.z};
    core.drawText(valuePos, valueSize, whiteCol, ctr->GetName(), TEXT_LEFT, AM::FontSize::PX16);

    if (core.isInRect(closePos, closeSize, mx, my))
        return GuiLast::GuiEv{ClickEventType::CLOSE_WINDOW};
    if (core.isInRect(wPos, wSize, mx, my))
        return GuiLast::GuiEv{ClickEventType::NONE};
    return GuiLast::GuiEv{ClickEventType::MISS};
}

GuiLast::GuiEv GuiLast::Gui::game_country(const Country* ctr, const Country* myCountry,
                                          const std::vector<War>& wars, int mx, int my)
{
    GuiLast::GuiEv ct{ClickEventType::MISS};
    glm::vec2 offset{5.0f, 2.5f};
    glm::vec2 wSize{res.x * 0.3f, res.y * 0.65f};
    glm::vec3 wPos{offset.x, offset.y, 0.1f};

    core.drawRect(wPos, wSize, darkBrown);

    glm::vec2 closeSize{100.0f, 40.0f};
    glm::vec3 closePos{wPos.x + wSize.x - offset.x - closeSize.x, wPos.y + offset.y, 0.2f};
    core.drawRect(closePos, closeSize, weirdBrown);
    closePos.z += 0.05f;
    core.drawText(closePos, closeSize, whiteCol, "Close", TEXT_CENTER, AM::FontSize::PX16);

    glm::vec2 nameSize{200.0f, 40.0f};
    glm::vec3 namePos{wPos.x + offset.x, wPos.y + wSize.y - offset.y - nameSize.y, 0.2f};
    core.drawText(namePos, nameSize, whiteCol, "Country: ", TEXT_LEFT, AM::FontSize::PX16);

    glm::vec2 valueSize{200.0f, 40.0f};
    glm::vec3 valuePos{namePos.x + nameSize.x + offset.x, namePos.y, namePos.z};
    core.drawText(valuePos, valueSize, whiteCol, ctr->GetName(), TEXT_LEFT, AM::FontSize::PX16);

    bool atWar = false;
    int warid = 0;
    for (auto& war : wars) {
        if (war.ShouldTheyFight(ctr->GetId(), myCountry->GetId())) {
            atWar = true;
            warid = war.id;
            break;
        }
    }

    namePos.y -= nameSize.y + offset.y;
    valuePos.y -= valueSize.y + offset.y;
    core.drawRect(namePos, nameSize, weirdBrown);
    namePos.z += 0.05;
    if (atWar) {
        core.drawText(namePos, nameSize, whiteCol, "Offer peace", TEXT_LEFT, AM::FontSize::PX16);
        if (core.isInRect(namePos, nameSize, mx, my))
            ct = GuiLast::GuiEv{ClickEventType::OPEN_OFFER_PEACE, warid};
    }
    else {
        core.drawText(namePos, nameSize, whiteCol, "Declare war", TEXT_LEFT, AM::FontSize::PX16);
        if (core.isInRect(namePos, nameSize, mx, my))
            ct = GuiLast::GuiEv{ClickEventType::DECLARE_WAR, ctr->GetId()};
    }
    namePos.z -= 0.1;

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
    core.drawRect(closePos, closeSize, weirdBrown);
    closePos.z += 0.05f;
    core.drawText(closePos, closeSize, whiteCol, "Close", TEXT_CENTER, AM::FontSize::PX16);

    glm::vec2 nameSize{200.0f, 40.0f};
    glm::vec3 namePos{wPos.x + offset.x, wPos.y + wSize.y - offset.y - nameSize.y, 0.2f};
    core.drawText(namePos, nameSize, whiteCol, "Province: ", TEXT_LEFT, AM::FontSize::PX16);
    glm::vec2 valueSize{100.0f, 40.0f};
    glm::vec3 valuePos{namePos.x + wSize.x * 0.5f - offset.x, namePos.y, namePos.z};
    core.drawText(valuePos, valueSize, whiteCol, prov.GetName(), TEXT_LEFT, AM::FontSize::PX16);

    namePos.y -= nameSize.y + offset.y;
    core.drawText(namePos, nameSize, whiteCol, "Country: ", TEXT_LEFT, AM::FontSize::PX16);
    valuePos.y -= valueSize.y + offset.y;
    core.drawRect(valuePos, valueSize, weirdBrown);
    valuePos.z += 0.05f;
    core.drawText(valuePos, valueSize, whiteCol, prov.GetCountry(), TEXT_LEFT, AM::FontSize::PX16);
    if (core.isInRect(valuePos, valueSize, mx, my))
        ct = GuiLast::GuiEv{ClickEventType::OPEN_COUNTRY, prov.GetCountryId()};
    valuePos.z -= 0.05f;

    if (myProv) {
        static int unitSize = 1000;
        namePos.y -= nameSize.y + offset.y;
        valuePos.y -= valueSize.y + offset.y;
        core.drawRect(namePos, nameSize, weirdBrown);
        namePos.z += 0.05f;
        core.drawText(namePos, nameSize, whiteCol, "Create unit of size: ", TEXT_LEFT, AM::FontSize::PX16);
        namePos.z -= 0.05;
        core.drawText(valuePos, valueSize, whiteCol, std::to_string(unitSize), TEXT_LEFT, AM::FontSize::PX16);
        if (core.isInRect(namePos, nameSize, mx, my))
            ct = GuiLast::GuiEv{ClickEventType::CREATE_UNIT, unitSize};

        namePos.y -= nameSize.y + offset.y;
        valuePos.y -= valueSize.y + offset.y;
        core.drawRect(namePos, nameSize, weirdBrown);
        namePos.z += 0.05f;
        core.drawText(namePos, nameSize, whiteCol, "Size +1000", TEXT_LEFT, AM::FontSize::PX16);
        core.drawRect(valuePos, valueSize, weirdBrown);
        valuePos.z += 0.05f;
        core.drawText(valuePos, valueSize, whiteCol, "Size -1000", TEXT_LEFT, AM::FontSize::PX16);
        valuePos.z -= 0.05f;
        namePos.z -= 0.05f;
        if (core.isInRect(namePos, nameSize, mx, my)) {
            ct = GuiLast::GuiEv{ClickEventType::UNIT_SIZE, unitSize};
            if (clicked)
                unitSize += 1000;
        }
        if (core.isInRect(valuePos, valueSize, mx, my)) {
            ct = GuiLast::GuiEv{ClickEventType::UNIT_SIZE, unitSize};
            if (clicked)
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
                                         const std::vector<Country>& countries, const std::vector<Unit>& units)
{
    GuiLast::GuiEv ct{ClickEventType::MISS};
    glm::vec2 offset{5.0f, 2.5f};
    glm::vec2 wSize{res.x * 0.3f, res.y * 0.65f};
    glm::vec3 wPos{offset.x, offset.y, 0.1f};

    core.drawRect(wPos, wSize, darkBrown);

    glm::vec2 closeSize{100.0f, 40.0f};
    glm::vec3 closePos{wPos.x + wSize.x - offset.x - closeSize.x, wPos.y + offset.y, 0.2f};
    core.drawRect(closePos, closeSize, weirdBrown);
    closePos.z += 0.05f;
    core.drawText(closePos, closeSize, whiteCol, "Close", TEXT_CENTER, AM::FontSize::PX16);

    glm::vec2 nameSize{100.0f, 40.0f};
    glm::vec3 namePos{wPos.x + offset.x, wPos.y + wSize.y - offset.y - nameSize.y, 0.2f};
    core.drawText(namePos, nameSize, whiteCol, "Province: ", TEXT_LEFT, AM::FontSize::PX16);
    glm::vec2 valueSize{100.0f, 40.0f};
    glm::vec3 valuePos{namePos.x + wSize.x * 0.5f - offset.x, namePos.y, namePos.z};
    core.drawText(valuePos, valueSize, whiteCol, prov.GetName(), TEXT_LEFT, AM::FontSize::PX16);

    namePos.y -= nameSize.y + offset.y;
    core.drawText(namePos, nameSize, whiteCol, "Country: ", TEXT_LEFT, AM::FontSize::PX16);
    valuePos.y -= valueSize.y + offset.y;
    core.drawRect(valuePos, valueSize, weirdBrown);
    valuePos.z += 0.05f;
    core.drawText(valuePos, valueSize, whiteCol, prov.GetCountry(), TEXT_LEFT, AM::FontSize::PX16);
    if (core.isInRect(valuePos, valueSize, mx, my))
        ct = GuiLast::GuiEv{ClickEventType::OPEN_COUNTRY, prov.GetCountryId()};
    valuePos.z -= 0.05f;

    namePos.y -= nameSize.y + offset.y;
    core.drawText(namePos, nameSize, whiteCol, "Attacker: ", TEXT_LEFT, AM::FontSize::PX16);
    valuePos.y -= valueSize.y + offset.y;
    if (battle.attackers.size()) {
        for (std::size_t i = 0; i < units.size(); ++i) {
            if (units[i].id == battle.attackers[0]) {
                assert(units[i].countryId >= 0 && units[i].countryId < countries.size());
                core.drawText(valuePos, valueSize, whiteCol, countries[units[i].countryId].GetName(), TEXT_LEFT,
                              AM::FontSize::PX16);
                break;
            }
        }
    }
    namePos.y -= nameSize.y + offset.y;
    core.drawText(namePos, nameSize, whiteCol, "Defenders count: ", TEXT_LEFT, AM::FontSize::PX16);
    valuePos.y -= valueSize.y + offset.y;
    if (battle.defenders.size()) {
        for (std::size_t i = 0; i < units.size(); ++i) {
            if (units[i].id == battle.defenders[0]) {
                assert(units[i].countryId >= 0 && units[i].countryId < countries.size());
                core.drawText(valuePos, valueSize, whiteCol, countries[units[i].countryId].GetName(), TEXT_LEFT,
                              AM::FontSize::PX16);
                break;
            }
        }
    }

    namePos.y -= nameSize.y + offset.y;
    core.drawText(namePos, nameSize, whiteCol, "Attackers count: ", TEXT_LEFT, AM::FontSize::PX16);
    valuePos.y -= valueSize.y + offset.y;
    core.drawText(valuePos, valueSize, whiteCol, std::to_string(battle.attackersSize), TEXT_LEFT,
                  AM::FontSize::PX16);

    namePos.y -= nameSize.y + offset.y;
    core.drawText(namePos, nameSize, whiteCol, "Defenders count: ", TEXT_LEFT, AM::FontSize::PX16);
    valuePos.y -= valueSize.y + offset.y;
    core.drawText(valuePos, valueSize, whiteCol, std::to_string(battle.defendersSize), TEXT_LEFT,
                  AM::FontSize::PX16);

    if (ct.ct != ClickEventType::MISS)
        return ct;
    if (core.isInRect(closePos, closeSize, mx, my))
        return GuiLast::GuiEv{ClickEventType::CLOSE_WINDOW};
    if (core.isInRect(wPos, wSize, mx, my))
        return GuiLast::GuiEv{ClickEventType::NONE};
    return GuiLast::GuiEv{ClickEventType::MISS};
}

GuiLast::GuiEv GuiLast::Gui::game_unit(const Unit& unit, int mx, int my, bool clicked,
                                       std::vector<Country>& countries)
{
    GuiLast::GuiEv ct{ClickEventType::MISS};
    glm::vec2 offset{5.0f, 2.5f};
    glm::vec2 wSize{res.x * 0.3f, res.y * 0.65f};
    glm::vec3 wPos{offset.x, offset.y, 0.1f};

    core.drawRect(wPos, wSize, darkBrown);

    glm::vec2 closeSize{100.0f, 40.0f};
    glm::vec3 closePos{wPos.x + wSize.x - offset.x - closeSize.x, wPos.y + offset.y, 0.2f};
    core.drawRect(closePos, closeSize, weirdBrown);
    closePos.z += 0.05f;
    core.drawText(closePos, closeSize, whiteCol, "Close", TEXT_CENTER, AM::FontSize::PX16);

    glm::vec2 nameSize{100.0f, 40.0f};
    glm::vec3 namePos{wPos.x + offset.x, wPos.y + wSize.y - offset.y - nameSize.y, 0.2f};
    core.drawText(namePos, nameSize, whiteCol, "name: ", TEXT_LEFT, AM::FontSize::PX16);
    glm::vec2 valueSize{100.0f, 40.0f};
    glm::vec3 valuePos{namePos.x + wSize.x * 0.5f - offset.x, namePos.y, namePos.z};
    core.drawText(valuePos, valueSize, whiteCol, unit.GetName(), TEXT_LEFT, AM::FontSize::PX16);

    namePos.y -= nameSize.y + offset.y;
    core.drawText(namePos, nameSize, whiteCol, "Country: ", TEXT_LEFT, AM::FontSize::PX16);
    valuePos.y -= valueSize.y - offset.y;
    core.drawRect(valuePos, valueSize, weirdBrown);
    valuePos.z += 0.05f;
    core.drawText(valuePos, valueSize, whiteCol, countries[unit.countryId].GetName(), TEXT_LEFT,
                  AM::FontSize::PX16);
    if (core.isInRect(valuePos, valueSize, mx, my))
        ct = GuiLast::GuiEv{ClickEventType::OPEN_COUNTRY, unit.GetCountryId()};
    valuePos.z -= 0.05f;

    namePos.y -= nameSize.y + offset.y;
    core.drawText(namePos, nameSize, whiteCol, "Unit size: ", TEXT_LEFT, AM::FontSize::PX16);
    valuePos.y -= valueSize.y + offset.y;
    core.drawText(valuePos, valueSize, whiteCol, std::to_string(unit.GetSoldiers()), TEXT_LEFT,
                  AM::FontSize::PX16);

    if (ct.ct != ClickEventType::MISS)
        return ct;
    if (core.isInRect(closePos, closeSize, mx, my))
        return GuiLast::GuiEv{ClickEventType::CLOSE_WINDOW};
    if (core.isInRect(wPos, wSize, mx, my))
        return GuiLast::GuiEv{ClickEventType::NONE};
    return GuiLast::GuiEv{ClickEventType::MISS};
}

GuiLast::GuiEv GuiLast::Gui::game_war(const War* war, int mx, int my, bool clicked)
{
    GuiLast::GuiEv ct{ClickEventType::MISS};
    glm::vec2 offset{5.0f, 2.5f};
    glm::vec2 wSize{res.x * 0.35f, res.y * 0.35f};
    glm::vec3 wPos{res.x * 0.5f - wSize.x * 0.5f, res.y * 0.5f - wSize.y * 0.5f, 0.1f};

    core.drawRect(wPos, wSize, darkBrown);

    glm::vec2 closeSize{100.0f, 40.0f};
    glm::vec3 closePos{wPos.x + wSize.x - offset.x - closeSize.x, wPos.y + offset.y, 0.2f};
    core.drawRect(closePos, closeSize, weirdBrown);
    closePos.z += 0.05f;
    core.drawText(closePos, closeSize, whiteCol, "Close", TEXT_CENTER, AM::FontSize::PX16);

    glm::vec2 nameSize{wSize.x - offset.x * 2.0f, 40.0f};
    glm::vec3 namePos{wPos.x + offset.x, wPos.y + wSize.y - offset.y - nameSize.y, 0.2f};
    core.drawText(namePos, nameSize, whiteCol, "War", TEXT_CENTER, AM::FontSize::PX16);

    glm::vec2 leftSize{wSize.x * 0.5f - offset.x * 2.0f, 40.0f};
    glm::vec3 leftPos{wPos.x + offset.x, namePos.y - leftSize.y - offset.y, 0.2f};
    core.drawText(leftPos, leftSize, whiteCol, "Attacker:", TEXT_CENTER, AM::FontSize::PX16);

    glm::vec2 rightSize = leftSize;
    glm::vec3 rightPos{leftPos.x + leftSize.x + offset.x * 2.0f, leftPos.y, 0.2f};
    core.drawText(rightPos, rightSize, whiteCol, "Defender:", TEXT_CENTER, AM::FontSize::PX16);

    leftPos.y -= (offset.y + leftSize.y);
    core.drawRect(leftPos, leftSize, weirdBrown);
    leftPos.z += 0.05f;
    core.drawText(leftPos, leftSize, whiteCol, war->attackers[0], TEXT_CENTER, AM::FontSize::PX16);
    if (core.isInRect(leftPos, leftSize, mx, my))
        ct = GuiLast::GuiEv{ClickEventType::OPEN_COUNTRY, war->attackersIds[0]};

    rightPos.y -= (offset.y + rightSize.y);
    core.drawRect(rightPos, rightSize, weirdBrown);
    rightPos.z += 0.05f;
    core.drawText(rightPos, rightSize, whiteCol, war->defenders[0], TEXT_CENTER, AM::FontSize::PX16);
    if (core.isInRect(rightPos, rightSize, mx, my))
        ct = GuiLast::GuiEv{ClickEventType::OPEN_COUNTRY, war->defendersIds[0]};

    namePos.y = leftPos.y - offset.y - nameSize.y;
    core.drawText(namePos, nameSize, whiteCol,
                  std::string{"Attacker war score: "} + std::to_string(war->attackerWarScore), TEXT_CENTER,
                  AM::FontSize::PX16);

    namePos.y = closePos.y;
    nameSize.x *= 0.5f;
    core.drawRect(namePos, nameSize, weirdBrown);
    namePos.z += 0.05f;
    core.drawText(namePos, nameSize, whiteCol, "Offer peace: ", TEXT_CENTER, AM::FontSize::PX16);
    if (core.isInRect(namePos, nameSize, mx, my))
        ct = GuiLast::GuiEv{ClickEventType::OPEN_OFFER_PEACE, war->id};

    if (ct.ct != ClickEventType::MISS)
        return ct;
    if (core.isInRect(closePos, closeSize, mx, my))
        return GuiLast::GuiEv{ClickEventType::CLOSE_WINDOW};
    if (core.isInRect(wPos, wSize, mx, my))
        return GuiLast::GuiEv{ClickEventType::NONE};
    return GuiLast::GuiEv{ClickEventType::MISS};
}

GuiLast::GuiEv GuiLast::Gui::game_unitsList(const std::vector<Unit*>& units, int mx, int my, bool clicked, int scr,
                                            const Province* prov, std::vector<Country>& countries)
{
    GuiLast::GuiEv ct{ClickEventType::MISS};
    glm::vec2 offset{5.0f, 2.5f};
    glm::vec2 wSize{res.x * 0.3f, res.y * 0.65f};
    glm::vec3 wPos{offset.x, offset.y, 0.1f};

    core.drawRect(wPos, wSize, darkBrown);

    glm::vec2 closeSize{100.0f, 40.0f};
    glm::vec3 closePos{wPos.x + wSize.x - offset.x - closeSize.x, wPos.y + offset.y, 0.2f};
    core.drawRect(closePos, closeSize, weirdBrown);
    closePos.z += 0.05f;
    core.drawText(closePos, closeSize, whiteCol, "Close", TEXT_CENTER, AM::FontSize::PX16);

    glm::vec2 mergeSize = closeSize;
    glm::vec3 mergePos{wPos.x + offset.x, closePos.y, 0.2f};
    core.drawRect(mergePos, mergeSize, weirdBrown);
    mergePos.z += 0.05f;
    core.drawText(mergePos, mergeSize, whiteCol, "Merge", TEXT_CENTER, AM::FontSize::PX16);
    if (core.isInRect(mergePos, mergeSize, mx, my))
        ct = GuiLast::GuiEv{ClickEventType::MERGE_UNITS};

    glm::vec2 nameSize{wSize.x * 0.9f, 50.0f};
    glm::vec3 namePos{wPos.x + offset.x, wPos.y + wSize.y * 0.9f, 0.2f};
    core.drawRect(namePos, nameSize, weirdBrown);
    namePos.z += 0.05f;
    core.drawText(namePos, nameSize, whiteCol, std::string("Units at ") + prov->GetName(), TEXT_CENTER,
                  AM::FontSize::PX16);
    namePos.z -= 0.05f;
    if (core.isInRect(namePos, nameSize, mx, my))
        ct = GuiLast::GuiEv{ClickEventType::OPEN_PROV, prov->GetId()};
    namePos.y = namePos.y - offset.y - nameSize.y;
    {  // list1
        glm::vec2 listSize{wSize.x, wSize.y * 0.6f};
        glm::vec3 listPos{wPos.x, namePos.y - offset.y * 8.0f - listSize.y, 0.2f};
        core.drawRect(listPos, listSize, weirdBrown);
        glm::vec2 elSize{listSize.x * 0.7f, 40.0f};
        glm::vec2 rectSize{20.0f, 40.0f};
        glm::vec3 startPos{listPos.x, listPos.y + listSize.y - elSize.y, listPos.z + 0.2f};
        glm::vec3 valuePos{listPos.x, listPos.y + listSize.y - elSize.y, listPos.z + 0.2f};
        glm::vec3 erasePos{listPos.x, listPos.y + listSize.y - elSize.y, listPos.z + 0.2f};
        glm::vec2 eraseSize{40.0f, 40.0f};
        {  // list
            static float sc = 0;
            if (!core.isInRect(listPos, listSize, mx, my))
                scr = 0;
            listDraw(*this, listPos, listSize, offset, startPos, elSize, sc, scr, units.size());
            valuePos = startPos;
            valuePos.x += offset.x * 1.0f + rectSize.x;
            erasePos = startPos;
            // erasePos.x = listPos.x + listSize.x - offset.x * 4.0f - eraseSize.x;
            erasePos.x = valuePos.x + elSize.x + offset.x;
            for (auto& unit : units) {
                // core.drawText(startPos, elSize, greenCol, "name: ", TEXT_LEFT, AM::FontSize::PX16);
                Color rectcol = countries[unit->countryId].color;
                glm::vec4 r2col{(float)rectcol.r / 255.0f, (float)rectcol.g / 255.0f, (float)rectcol.b / 255.0f,
                                (float)rectcol.a / 255.0f};
                core.drawRect(startPos, rectSize, r2col);
                core.drawRect(valuePos, elSize, brown);
                valuePos.z += 0.05f;
                core.drawText(valuePos, elSize, whiteCol,
                              unit->GetName() + std::string{", size: "} + int_to_string(unit->soldiers), TEXT_LEFT,
                              AM::FontSize::PX16);
                core.drawIcon(erasePos, eraseSize, 4);
                valuePos.z -= 0.05f;

                if (core.isInRect(valuePos, elSize, mx, my))
                    ct = GuiLast::GuiEv{ClickEventType::OPEN_UNIT, unit->GetId()};
                if (core.isInRect(erasePos, eraseSize, mx, my))
                    ct = GuiLast::GuiEv{ClickEventType::DEL_FROM_UNITS_LIST, unit->GetId()};
                startPos.y = startPos.y - elSize.y - offset.y;
                valuePos.y = startPos.y;
                erasePos.y = startPos.y;
            }

            flush();
            glScissor(0.0f, 0.0f, res.x, res.y);
            start();
        }
        namePos.y = listPos.y - nameSize.y;
    }
    if (ct.ct != ClickEventType::MISS)
        return ct;
    if (core.isInRect(closePos, closeSize, mx, my))
        return GuiLast::GuiEv{ClickEventType::CLOSE_WINDOW};
    if (core.isInRect(wPos, wSize, mx, my))
        return GuiLast::GuiEv{ClickEventType::NONE};
    return GuiLast::GuiEv{ClickEventType::MISS};
}

GuiLast::GuiEv GuiLast::Gui::game_peaceOffer(const PeaceOffer* offer, int mx, int my, int myCtrId,
                                             const std::vector<Province>& provinces,
                                             std::vector<Country>& countries, bool clicked, int scr)
{
    GuiLast::GuiEv ct{ClickEventType::MISS};
    glm::vec2 offset{5.0f, 2.5f};
    glm::vec2 wSize{res.x * 0.3f, res.y * 0.85f};
    glm::vec3 wPos{offset.x, offset.y, 0.1f};

    core.drawRect(wPos, wSize, brown);

    glm::vec2 closeSize{100.0f, 40.0f};
    glm::vec3 closePos{wPos.x + wSize.x - offset.x - closeSize.x, wPos.y + offset.y, 0.2f};
    core.drawRect(closePos, closeSize, weirdBrown);
    closePos.z += 0.05f;
    core.drawText(closePos, closeSize, whiteCol, "Close", TEXT_CENTER, AM::FontSize::PX16);
    closePos.z -= 0.05f;

    glm::vec2 sendSize{100.0f, 40.0f};
    glm::vec3 sendPos{wPos.x + offset.x, closePos.y, closePos.z};
    // Log(offer->peaceId);
    core.drawRect(sendPos, sendSize, weirdBrown);
    sendPos.z += 0.05f;
    if (offer->peaceId != -2) {
        core.drawText(sendPos, sendSize, whiteCol, "Accept", TEXT_CENTER, AM::FontSize::PX16);
        glm::vec3 declinePos{sendPos.x + offset.x * 3.0f + sendSize.x, sendPos.y, sendPos.z};
        core.drawRect(declinePos, sendSize, weirdBrown);
        declinePos.z += 0.05f;
        core.drawText(declinePos, sendSize, whiteCol, "Decline", TEXT_CENTER, AM::FontSize::PX16);
        if (core.isInRect(declinePos, sendSize, mx, my))
            ct = GuiLast::GuiEv{ClickEventType::REJECT_PEACE_OFFER, offer->peaceId};
    }
    else {
        core.drawText(sendPos, sendSize, whiteCol, "Send", TEXT_CENTER, AM::FontSize::PX16);
    }
    if (core.isInRect(sendPos, sendSize, mx, my))
        ct = GuiLast::GuiEv{ClickEventType::SEND_PEACE_OFFER, offer->peaceId};
    sendPos.z -= 0.05f;

    glm::vec2 nameSize{wSize.x, 50.0f};
    glm::vec3 namePos{wPos.x + offset.x, wPos.y + wSize.y * 0.9f, 0.2f};
    core.drawText(namePos, nameSize, whiteCol, std::string("Peace Offer"), TEXT_CENTER, AM::FontSize::PX16);
    namePos.y = namePos.y - offset.y - nameSize.y;
    assert(offer->recipant >= 0 && offer->recipant < countries.size());
    assert(offer->offeredBy >= 0 && offer->offeredBy < countries.size());
    core.drawText(
        namePos, nameSize, whiteCol,
        countries[offer->recipant].GetName() + std::string(" vs ") + countries[offer->offeredBy].GetName(),
        TEXT_CENTER, AM::FontSize::PX16);

    namePos.y = namePos.y - offset.y * 7.0f - nameSize.y;
    core.drawText(namePos, nameSize, whiteCol, "Lost provinces:", TEXT_CENTER, AM::FontSize::PX16);
    auto* lostTuples = &offer->lostProv;
    auto* gainTuples = &offer->gainProv;
    // }
    int scr2 = scr;
    {  // list1
        glm::vec2 listSize{wSize.x, wSize.y * 0.2f};
        glm::vec3 listPos{wPos.x, namePos.y - offset.y * 1.0f - listSize.y, 0.2f};
        core.drawRect(listPos, listSize, weirdBrown);
        glm::vec2 elSize{listSize.x, 40.0f};
        glm::vec3 startPos{listPos.x, listPos.y + listSize.y - elSize.y, listPos.z + 0.2f};
        {  // list
            static float sc = 0;
            if (!core.isInRect(listPos, listSize, mx, my))
                scr = 0;
            listDraw(*this, listPos, listSize, offset, startPos, elSize, sc, scr, lostTuples->size());
            for (std::size_t i = 0; i < lostTuples->size(); ++i) {
                core.drawRect(glm::vec3{startPos.x, startPos.y, startPos.z - 0.05f}, elSize, brown);
                core.drawText(startPos, elSize, whiteCol,
                              provinces[std::get<0>((*lostTuples)[i])].GetName() + std::string{" to "} +
                                  countries[std::get<1>((*lostTuples)[i])].GetName(),
                              TEXT_LEFT, AM::FontSize::PX16);

                startPos.y = startPos.y - elSize.y - offset.y;
            }
            flush();
            glScissor(0.0f, 0.0f, res.x, res.y);
            start();
        }
        namePos.y = listPos.y - nameSize.y - offset.y * 7.0f;
    }
    core.drawText(namePos, nameSize, whiteCol, "Gain provinces:", TEXT_CENTER, AM::FontSize::PX16);
    {  // list2
        glm::vec2 listSize{wSize.x, wSize.y * 0.2f};
        glm::vec3 listPos{wPos.x, namePos.y - offset.y * 1.0f - listSize.y, 0.2f};
        core.drawRect(listPos, listSize, weirdBrown);
        glm::vec2 elSize{listSize.x, 40.0f};
        glm::vec3 startPos{listPos.x, listPos.y + listSize.y - elSize.y, listPos.z + 0.2f};
        {  // list
            static float sc = 0;
            if (!core.isInRect(listPos, listSize, mx, my))
                scr2 = 0;
            listDraw(*this, listPos, listSize, offset, startPos, elSize, sc, scr2, gainTuples->size());
            for (std::size_t i = 0; i < gainTuples->size(); ++i) {
                core.drawRect(glm::vec3{startPos.x, startPos.y, startPos.z - 0.05f}, elSize, brown);
                core.drawText(startPos, elSize, whiteCol,
                              provinces[std::get<0>((*gainTuples)[i])].GetName() + std::string{" to "} +
                                  countries[std::get<1>((*gainTuples)[i])].GetName(),
                              TEXT_LEFT, AM::FontSize::PX16);

                startPos.y = startPos.y - elSize.y - offset.y;
            }
            flush();
            glScissor(0.0f, 0.0f, res.x, res.y);
            start();
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

GuiLast::GuiEv GuiLast::Gui::game_SideBar(const SideBarData& sbData, int mx, int my, bool clicked, int scr)
{
    GuiLast::GuiEv ct{ClickEventType::MISS};
    glm::vec2 offset{5.0f, 2.5f};
    glm::vec2 wSize{res.x * 0.05f, res.y * 0.65f};
    glm::vec3 wPos{res.x - offset.x - wSize.x, wSize.y * 0.25f, 0.1f};
    {  // list
        glm::vec2 listSize = wSize;
        glm::vec3 listPos = wPos;
        core.drawRect(listPos, listSize, weirdBrown);
        glm::vec2 elSize{wSize.x, wSize.x};
        glm::vec3 startPos{wPos.x, wPos.y + wSize.y - elSize.y, 0.2f};
        glm::vec2 elTextSize{200.0f, wSize.x};
        glm::vec3 startTextPos{startPos.x - elTextSize.x, startPos.y, startPos.z};
        {  // list
            static float sc = 0;
            if (!core.isInRect(listPos, listSize, mx, my))
                scr = 0;
            listDraw(*this, listPos, listSize, offset, startPos, elSize, sc, scr, sbData.elements.size(), true,
                     res.x);
            for (std::size_t i = 0; i < sbData.elements.size(); ++i) {
                if (core.isInRect(startPos, elSize, mx, my)) {
                    core.drawRect(glm::vec3{startTextPos.x, startTextPos.y, startTextPos.z - 0.01f}, elTextSize,
                                  weirdBrown);
                    core.drawText(startTextPos, elTextSize, whiteCol, sbData.elements[i].hoverText, TEXT_LEFT,
                                  AM::FontSize::PX16);
                }
                core.drawIcon(startPos, elSize, (int)sbData.elements[i].type);

                if (core.isInRect(startPos, elSize, mx, my))
                    ct = GuiLast::GuiEv{ClickEventType::SIDEBAR_LEFTC, (int)i};
                startPos.y = startPos.y - elSize.y - offset.y;
                startTextPos.y = startPos.y;
            }

            flush();
            glScissor(0.0f, 0.0f, res.x, res.y);
            start();
        }
    }

    if (ct.ct != ClickEventType::MISS)
        return ct;
    if (core.isInRect(wPos, wSize, mx, my))
        return GuiLast::GuiEv{ClickEventType::NONE};
    return GuiLast::GuiEv{ClickEventType::MISS};
}

GuiLast::GuiEv GuiLast::Gui::room_startButton(int mx, int my)
{
    GuiLast::GuiEv ct{ClickEventType::MISS};
    glm::vec2 offset{5.0f, 2.5f};
    glm::vec2 wSize{150, 100.0f};
    glm::vec3 wPos{res.x - wSize.x - offset.x * 3.0f, 50.0f, 0.1f};

    core.drawRect(wPos, wSize, weirdBrown);
    core.drawText(glm::vec3{wPos.x, wPos.y, wPos.z + 0.1f}, glm::vec2{wSize.x, wSize.y}, whiteCol, "Start",
                  TEXT_CENTER, AM::FontSize::PX32);
    if (core.isInRect(wPos, wSize, mx, my))
        ct = GuiLast::GuiEv{ClickEventType::ROOM_START_GAME};

    if (ct.ct != ClickEventType::MISS)
        return ct;
    if (core.isInRect(wPos, wSize, mx, my))
        return GuiLast::GuiEv{ClickEventType::NONE};
    return GuiLast::GuiEv{ClickEventType::MISS};
}

GuiLast::GuiEv GuiLast::Gui::room_countryList(const std::vector<std::string>& ctrarr, int mx, int my, int scr)
{
    GuiLast::GuiEv ct{ClickEventType::MISS};
    glm::vec2 offset{5.0f, 2.5f};
    glm::vec2 wSize{res.x * 0.3f, res.y * 0.7f};
    glm::vec3 wPos{offset.x * 2 + wSize.y, 50.0f, 0.1f};

    core.drawRect(wPos, wSize, weirdBrown);
    core.drawText(glm::vec3{wPos.x, wPos.y + wSize.y * 0.9f, wPos.z + 0.2f}, glm::vec2{wSize.x, wSize.y * 0.1f},
                  whiteCol, "Countries", TEXT_CENTER, AM::FontSize::PX32);

    glm::vec2 listSize{wSize.x, wSize.y * 0.9f};
    glm::vec3 listPos{wPos.x, wPos.y, wPos.z + 0.1f};
    core.drawRect(listPos, listSize, weirdBrown);
    glm::vec2 nameSize{listSize.x, 50.0f};
    glm::vec3 namePos{listPos.x + offset.x, listPos.y + listSize.y - offset.y - nameSize.y, listPos.z + 0.1f};
    {  // list
        static float sc = 0;
        if (!core.isInRect(listPos, listSize, mx, my))
            scr = 0;
        listDraw(*this, listPos, listSize, offset, namePos, nameSize, sc, scr, ctrarr.size());
        for (std::size_t i = 0; i < ctrarr.size(); ++i) {
            core.drawRect(namePos, nameSize, brown);
            core.drawText(namePos, nameSize, whiteCol, ctrarr[i], TEXT_LEFT, AM::FontSize::PX16);
            if (core.isInRect(namePos, nameSize, mx, my))
                ct = GuiLast::GuiEv{ClickEventType::ROOM_PICK_COUNTRY, (int)i};
            namePos.y = namePos.y - nameSize.y - offset.y;
        }
        flush();
        glScissor(0.0f, 0.0f, res.x, res.y);
        start();
    }
    if (ct.ct != ClickEventType::MISS)
        return ct;
    if (core.isInRect(wPos, wSize, mx, my))
        return GuiLast::GuiEv{ClickEventType::NONE};
    return GuiLast::GuiEv{ClickEventType::MISS};
}

GuiLast::GuiEv GuiLast::Gui::room_playerList(const std::string& playerName, const std::vector<std::string>& plarr,
                                             int mx, int my, int scr)
{
    GuiLast::GuiEv ct{ClickEventType::MISS};
    glm::vec2 offset{5.0f, 2.5f};
    glm::vec2 wSize{res.x * 0.3f, res.y * 0.7f};
    glm::vec3 wPos{offset.x, 10.0f, 0.1f};
    core.drawRect(wPos, wSize, darkBrown);

    glm::vec2 itemSize{wPos.x, 50.0f};
    core.drawText(glm::vec3{wPos.x, wPos.y + wSize.y - itemSize.y, wPos.z + 0.1f}, glm::vec2{wSize.x, itemSize.y},
                  whiteCol, "Pick your country " + playerName, TEXT_CENTER, AM::FontSize::PX32);

    glm::vec2 buttonSize{wSize.x * 0.35f, 50.0f};
    glm::vec3 buttonPos{wPos.x + wSize.x - buttonSize.x - offset.x * 2.0f, wPos.y + offset.y * 3.0f, 0.1f};
    core.drawRect(buttonPos, buttonSize, weirdBrown);
    core.drawText(buttonPos, buttonSize, whiteCol, "Spectator", TEXT_CENTER, AM::FontSize::PX32);
    if (core.isInRect(buttonPos, buttonSize, mx, my))
        ct = GuiLast::GuiEv{ClickEventType::START_AS_SPECTATOR};

    glm::vec2 listSize{wSize.x, wSize.y * 0.35f};
    glm::vec3 listPos{wPos.x, wPos.y + buttonPos.y + buttonSize.y + offset.y, wPos.z + 0.1f};
    core.drawText(glm::vec3{wPos.x, listPos.y + listSize.y + offset.y, wPos.z + 0.1f}, glm::vec2{wSize.x, 40.0f},
                  whiteCol, "Players list", TEXT_CENTER, AM::FontSize::PX32);
    core.drawRect(listPos, listSize, weirdBrown);
    glm::vec2 nameSize{listSize.x, 50.0f};
    glm::vec3 namePos{listPos.x + offset.x, listPos.y + listSize.y - offset.y - nameSize.y, 0.2f};
    {  // list
        static float sc = 0;
        if (!core.isInRect(listPos, listSize, mx, my))
            scr = 0;
        listDraw(*this, listPos, listSize, offset, namePos, nameSize, sc, scr, plarr.size());
        for (auto& pl : plarr) {
            core.drawRect(namePos, nameSize, brown);
            core.drawText(namePos, nameSize, whiteCol, pl, TEXT_LEFT, AM::FontSize::PX16);
            namePos.y -= nameSize.y + offset.y;
        }
        flush();
        glScissor(0.0f, 0.0f, res.x, res.y);
        start();
    }
    if (ct.ct != ClickEventType::MISS)
        return ct;
    if (core.isInRect(wPos, wSize, mx, my))
        return GuiLast::GuiEv{ClickEventType::NONE};
    return GuiLast::GuiEv{ClickEventType::MISS};
}

void GuiLast::Gui::game_drawUnitBar(const Unit& u)
{
    glm::vec2 wSize{100.0f, 50.0f};
    glm::vec3 wPos{u.fakePos.x, u.fakePos.y, u.fakePos.z + 0.1f};

    core.drawRect(wPos, wSize, darkBrown);
    core.drawText(glm::vec3{wPos.x, wPos.y, wPos.z + 0.2f}, glm::vec2{wSize.x, wSize.y}, greenCol, "ASD",
                  TEXT_CENTER, AM::FontSize::PX32);
}

void GuiLast::Gui::start()
{
    start(glm::ortho(0.0f, (float)winSize.x, 0.0f, (float)winSize.y));
}

void GuiLast::Gui::start(const glm::mat4& matrix)
{
    glUseProgram(shader.GetProgram());
    glUniformMatrix4fv(glGetUniformLocation(shader.GetProgram(), "pr_matrix"), 1, GL_FALSE,
                       glm::value_ptr(matrix));

    GLint tex[32];
    for (GLint i = 0; i < 32; ++i) {
        tex[i] = i;
    }
    GLuint fontTexID[32];
    for (int i = 0; i <= (int)AM::FontSize::PX160; ++i) {
        fontTexID[i] = AM::atlasTexture[i]->GetId();
    //}
    //for (int i = 0; i <= (int)AM::FontSize::PX160; ++i) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, (GLuint)fontTexID[i]);
    }
    fontTexID[31] = AM::am.iconsTexture->GetId();
    glActiveTexture(GL_TEXTURE0 + 31);
    glBindTexture(GL_TEXTURE_2D, (GLuint)fontTexID[31]);
    glUniform1iv(glGetUniformLocation(shader.GetProgram(), "tex"), 32, tex);
    core.guiBatchBegin();
}

void GuiLast::Gui::flush()
{
    core.guiBatchFlush();
}

