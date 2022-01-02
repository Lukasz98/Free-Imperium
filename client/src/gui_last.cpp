#include "gui_last.h"

#include "asset_manager.h"
// using namespace GuiLast;
static glm::vec4 brownCol{56.0f / 255.0f, 31.0f / 255.0f, 19.0f / 255.0f, 1.0f};
static glm::vec4 greenCol{0.0f / 255.0f, 255.0f / 255.0f, 0.0f / 255.0f, 1.0f};
static glm::vec4 darkBrown{0.294, 0.227, 0.223, 1.0};
static glm::vec4 brown{0.388, 0.298, 0.290, 1.0};
static glm::vec4 lightBrown{0.568, 0.435, 0.427, 1.0};
static glm::vec4 weirdBrown{.333, .258, 0.258, 1.0};

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

static void listDraw(GuiLast::Gui& gui, glm::vec3 listPos, glm::vec2 listSize, glm::vec2 offset,
                     glm::vec3& itemPos, glm::vec2 itemSize, float& sc, int scr, int itemCount)
{
    gui.flush();
    glScissor(listPos.x, listPos.y, listSize.x, listSize.y);
    gui.start();
    {
        itemPos.y = listPos.y + listSize.y - offset.y - itemSize.y;
        float lasty = itemPos.y;
        for (int i = 0; i < itemCount - 1; ++i) {
            lasty -= itemSize.y - offset.y;
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
                              greenCol);
            gui.core.drawRect(rpos, rsize, rcol);
        }
    }
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

GuiLast::GuiEv GuiLast::Gui::game_war(const War* war, int mx, int my, bool clicked)
{
    GuiLast::GuiEv ct{ClickEventType::MISS};
    glm::vec2 offset{5.0f, 2.5f};
    glm::vec2 wSize{res.x * 0.35f, res.y * 0.35f};
    glm::vec3 wPos{res.x * 0.5f - wSize.x * 0.5f, res.y * 0.5f - wSize.y * 0.5f, 0.1f};

    core.drawRect(wPos, wSize, darkBrown);

    glm::vec2 closeSize{100.0f, 40.0f};
    glm::vec3 closePos{wPos.x + wSize.x - offset.x - closeSize.x, wPos.y + offset.y, 0.2f};
    core.drawText(closePos, closeSize, greenCol, "Close", TEXT_LEFT, AM::FontSize::PX16);

    glm::vec2 nameSize{wSize.x - offset.x * 2.0f, 40.0f};
    glm::vec3 namePos{wPos.x + offset.x, wPos.y + wSize.y - offset.y - nameSize.y, 0.2f};
    core.drawText(namePos, nameSize, greenCol, "War", TEXT_CENTER, AM::FontSize::PX16);

    glm::vec2 leftSize{wSize.x * 0.5f - offset.x * 2.0f, 40.0f};
    glm::vec3 leftPos{wPos.x + offset.x, namePos.y - leftSize.y - offset.y, 0.2f};
    core.drawText(leftPos, leftSize, greenCol, "Attacker:", TEXT_CENTER, AM::FontSize::PX16);

    glm::vec2 rightSize = leftSize;
    glm::vec3 rightPos{leftPos.x + leftSize.x + offset.x * 2.0f, leftPos.y, 0.2f};
    core.drawText(rightPos, rightSize, greenCol, "Defender:", TEXT_CENTER, AM::FontSize::PX16);

    leftPos.y -= (offset.y + leftSize.y);
    core.drawText(leftPos, leftSize, greenCol, war->attackers[0], TEXT_CENTER, AM::FontSize::PX16);
    if (core.isInRect(leftPos, leftSize, mx, my))
        ct = GuiLast::GuiEv{ClickEventType::OPEN_COUNTRY, war->attackersIds[0]};

    rightPos.y -= (offset.y + rightSize.y);
    core.drawText(rightPos, rightSize, greenCol, war->defenders[0], TEXT_CENTER, AM::FontSize::PX16);
    if (core.isInRect(rightPos, rightSize, mx, my))
        ct = GuiLast::GuiEv{ClickEventType::OPEN_COUNTRY, war->defendersIds[0]};

    namePos.y = leftPos.y - offset.y - nameSize.y;
    core.drawText(namePos, nameSize, greenCol,
                  std::string{"Attacker war score: "} + std::to_string(war->attackerWarScore), TEXT_CENTER,
                  AM::FontSize::PX16);

    namePos.y = closePos.y;
    nameSize.x *= 0.5f;
    core.drawText(namePos, nameSize, greenCol, "Offer peace: ", TEXT_LEFT, AM::FontSize::PX16);
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

GuiLast::GuiEv GuiLast::Gui::game_peaceOffer(const PeaceOffer* offer, int mx, int my, int myCtrId,
                                             const std::vector<std::unique_ptr<Province>>& provinces,
                                             std::vector<std::shared_ptr<Country>>& countries, bool clicked,
                                             int scr)
{
    GuiLast::GuiEv ct{ClickEventType::MISS};
    glm::vec2 offset{5.0f, 2.5f};
    glm::vec2 wSize{res.x * 0.3f, res.y * 0.85f};
    glm::vec3 wPos{offset.x, offset.y, 0.1f};

    core.drawRect(wPos, wSize, darkBrown);

    glm::vec2 closeSize{100.0f, 40.0f};
    glm::vec3 closePos{wPos.x + wSize.x - offset.x - closeSize.x, wPos.y + offset.y, 0.2f};
    core.drawText(closePos, closeSize, greenCol, "Close", TEXT_LEFT, AM::FontSize::PX16);

    glm::vec2 nameSize{wSize.x, wSize.y * 0.1f};
    glm::vec3 namePos{wPos.x + offset.x, wPos.y + wSize.y * 0.9f, 0.2f};
    core.drawText(namePos, nameSize, greenCol, std::string("Peace Offer"), TEXT_CENTER,
                  AM::FontSize::PX16);
    namePos.y = namePos.y - offset.y - nameSize.y;
    core.drawText(namePos, nameSize, greenCol, countries[offer->recipant]->GetName() + std::string(" vs ") + countries[offer->offeredBy]->GetName(), TEXT_CENTER,
                  AM::FontSize::PX16);

    auto* lostTuples = &offer->lostProv;
    auto* gainTuples = &offer->gainProv;
    if (offer->offeredBy != myCtrId) {
        lostTuples = &offer->gainProv;
        gainTuples = &offer->lostProv;
    }
    {  // list1
        glm::vec2 listSize{wSize.x, wSize.y * 0.3f};
        glm::vec3 listPos{wPos.x, wPos.y + wSize.y * 0.8 - listSize.y, 0.2f};
        core.drawRect(listPos, listSize, lightBrown);
        glm::vec2 elSize{listSize.x, 40.0f};
        glm::vec3 startPos{listPos.x, listPos.y + listSize.y - elSize.y, listPos.z + 0.2f};
        {  // list
            static float sc = 0;
            if (!core.isInRect(listPos, listSize, mx, my))
                scr = 0;
            listDraw(*this, listPos, listSize, offset, startPos, elSize, sc, scr, lostTuples->size());
            for (std::size_t i = 0; i < lostTuples->size(); ++i) {
                core.drawRect(glm::vec3{startPos.x, startPos.y, startPos.z - 0.05f}, elSize, darkBrown);
                core.drawText(startPos, elSize, greenCol,
                              provinces[std::get<0>((*lostTuples)[i])]->GetName() + std::string{" to "} +
                                  countries[std::get<1>((*lostTuples)[i])]->GetName(),
                              TEXT_LEFT, AM::FontSize::PX16);

                startPos.y -= elSize.y - offset.y;
            }
            flush();
            glScissor(0.0f, 0.0f, res.x, res.y);
            start();
        }
    }
    {  // list2
        glm::vec2 listSize{wSize.x, wSize.y * 0.3f};
        glm::vec3 listPos{wPos.x, wPos.y + offset.y, 0.2f};
        core.drawRect(listPos, listSize, lightBrown);
        glm::vec2 elSize{listSize.x, 40.0f};
        glm::vec3 startPos{listPos.x, listPos.y + listSize.y - elSize.y, listPos.z + 0.2f};
        {  // list
            static float sc = 0;
            if (!core.isInRect(listPos, listSize, mx, my))
                scr = 0;
            listDraw(*this, listPos, listSize, offset, startPos, elSize, sc, scr, gainTuples->size());
            for (std::size_t i = 0; i < gainTuples->size(); ++i) {
                core.drawRect(glm::vec3{startPos.x, startPos.y, startPos.z - 0.05f}, elSize, darkBrown);
                core.drawText(startPos, elSize, greenCol,
                              provinces[std::get<0>((*gainTuples)[i])]->GetName() + std::string{" to "} +
                                  countries[std::get<1>((*gainTuples)[i])]->GetName(),
                              TEXT_LEFT, AM::FontSize::PX16);

                startPos.y -= elSize.y - offset.y;
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
            core.drawRect(glm::vec3{startTextPos.x, startTextPos.y, startTextPos.z - 0.01f}, elTextSize,
                          darkBrown);
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

GuiLast::GuiEv GuiLast::Gui::room_startButton(int mx, int my)
{
    GuiLast::GuiEv ct{ClickEventType::MISS};
    glm::vec2 offset{5.0f, 2.5f};
    glm::vec2 wSize{150, 100.0f};
    glm::vec3 wPos{res.x - wSize.x - offset.x, 50.0f, 0.1f};

    core.drawRect(wPos, wSize, darkBrown);
    core.drawText(glm::vec3{wPos.x, wPos.y, wPos.z + 0.1f}, glm::vec2{wSize.x, wSize.y}, greenCol, "Start",
                  TEXT_CENTER, AM::FontSize::PX16);
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

    core.drawRect(wPos, wSize, darkBrown);
    core.drawText(glm::vec3{wPos.x, wPos.y + wSize.y * 0.9f, wPos.z + 0.2f}, glm::vec2{wSize.x, wSize.y * 0.1f},
                  greenCol, "Countries", TEXT_CENTER, AM::FontSize::PX16);

    glm::vec2 listSize{wSize.x, wSize.y * 0.9f};
    glm::vec3 listPos{wPos.x, wPos.y, wPos.z + 0.1f};
    core.drawRect(listPos, listSize, brown);
    glm::vec2 nameSize{listSize.x, 50.0f};
    glm::vec3 namePos{listPos.x + offset.x, listPos.y + listSize.y - offset.y - nameSize.y, listPos.z + 0.1f};
    {  // list
        static float sc = 0;
        if (!core.isInRect(listPos, listSize, mx, my))
            scr = 0;
        listDraw(*this, listPos, listSize, offset, namePos, nameSize, sc, scr, ctrarr.size());
        for (std::size_t i = 0; i < ctrarr.size(); ++i) {
            core.drawText(namePos, nameSize, greenCol, ctrarr[i], TEXT_LEFT, AM::FontSize::PX16);
            if (core.isInRect(namePos, nameSize, mx, my))
                ct = GuiLast::GuiEv{ClickEventType::ROOM_PICK_COUNTRY, i};
            namePos.y -= nameSize.y - offset.y;
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

GuiLast::GuiEv GuiLast::Gui::room_playerList(const std::vector<std::string>& plarr, int mx, int my, int scr)
{
    GuiLast::GuiEv ct{ClickEventType::MISS};
    glm::vec2 offset{5.0f, 2.5f};
    glm::vec2 wSize{res.x * 0.3f, res.y * 0.7f};
    glm::vec3 wPos{offset.x, 50.0f, 0.1f};

    core.drawRect(wPos, wSize, darkBrown);
    core.drawText(glm::vec3{wPos.x, wPos.y + wSize.y * 0.9f, wPos.z + 0.2f}, glm::vec2{wSize.x, wSize.y * 0.1f},
                  greenCol, "Players list", TEXT_CENTER, AM::FontSize::PX16);

    glm::vec2 listSize{wSize.x, wSize.y * 0.9f};
    glm::vec3 listPos{wPos.x, wPos.y, wPos.z + 0.1f};
    core.drawRect(listPos, listSize, brown);
    glm::vec2 nameSize{listSize.x, 50.0f};
    glm::vec3 namePos{listPos.x + offset.x, listPos.y + listSize.y - offset.y - nameSize.y, 0.2f};
    {  // list
        static float sc = 0;
        if (!core.isInRect(listPos, listSize, mx, my))
            scr = 0;
        listDraw(*this, listPos, listSize, offset, namePos, nameSize, sc, scr, plarr.size());
        for (auto& pl : plarr) {
            core.drawText(namePos, nameSize, greenCol, pl, TEXT_LEFT, AM::FontSize::PX16);
            namePos.y -= nameSize.y - offset.y;
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
    // Shader shader("src/graphics/shaders/gui_batch/a.vert", "src/graphics/shaders/gui_batch/a.frag", "", "");
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
                    Shader("src/graphics/shaders/gui_batch/a.vert", "src/graphics/shaders/gui_batch/a.frag",
       "",
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

            doTestList(window->scrollOffset, mousePos.x, mousePos.y, dt, window->mouseL,
       window->mouseLClicked); window->scrollOffset = 0; window->mouseLClicked = false; drawLists();

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
