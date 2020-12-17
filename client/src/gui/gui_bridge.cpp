#include "gui_bridge.h"
#include "gui_core.h"

using namespace Gui::Core;

static ClickData clickData;


glm::vec4 darkBrown{0.294, 0.227, 0.223, 1.0};
glm::vec4 brown{0.388, 0.298, 0.290, 1.0};
glm::vec4 lightBrown{0.568, 0.435, 0.427, 1.0};
glm::vec4 weirdBrown{.333, .258, 0.258, 1.0};



namespace Gui::Base {
std::vector<Gui::Core::Window*> windows;
bool dragging = false;

ClickEventType Click(const glm::vec2 & mousePos)
{
    ClickEventType clickType = ClickEventType::MISS;
    if (dragging) {
        dragging = false;
        for (auto w : Base::windows)
            w->dragging = false;
    }
    for (auto w : Base::windows) {
        clickType = w->GetClick(clickData, mousePos);
        if (clickType != ClickEventType::NONE && clickType != ClickEventType::MISS) {
            clickData.window = w;
            return clickType;
        }
    }
    return clickType;
}

void Hover(const glm::vec2 & mousePos)
{
    for (auto w : Base::windows)
        w->Hover(mousePos);
}

bool Scroll(int y, const glm::vec2 & mousePos)
{
    for (auto w : Base::windows) {
        for (auto l : w->lists) {
            if (l->Hover(mousePos)) {
                l->Scroll(y);
                return true;
            }
        }
    }
    return false;
}

bool Drag(const glm::vec2 & mousePos, float dt)
{
    for (auto w : Base::windows) {
        if (w->backgr) {
            if (w->dragable && ((dragging && w->dragging) || !dragging) && w->backgr->Click(mousePos.x, mousePos.y)) {
                w->Drag(mousePos, dt);
                dragging = true;
                w->dragging = true;
                return true;
            }
        }
    }
    return false;
}

void Draw()
{
    for (auto w : Base::windows) {
        w->Draw();
    }
}

void ResetClick()
{
    clickData.val = -1;
    clickData.hiddenValue = -1;
    clickData.window = nullptr;
    clickData.group = nullptr;
    clickData.evType = ClickEventType::MISS;
}

void UpdateVals()
{
    for (auto w : windows)
        w->UpdateValues();
}

int GetHiddenValue()
{
    return clickData.hiddenValue;
}


} // Gui::Base




namespace Gui::Menu {

void Open(const glm::vec2 & resolution)
{
    Window * w = new Window{glm::vec3{0.0f, 0.0f, 0.0f}, resolution, darkBrown, false, WindowType::MENU_SCREEN};
    Gui::Base::windows.push_back(w);
    
    Group * g = new Group{glm::vec3{0.0f, 0.0f, 0.0f}, resolution, darkBrown, false};
    w->groups.push_back(g);

    TextLabel * titleLabel = new TextLabel{glm::vec3{resolution.x * 0.5f, resolution.y * 0.5f, 0.1f}, glm::vec2{500.0f, 200.0f},
                                           weirdBrown, AM::FontSize::PX64, true, glm::vec3{250.0f, 100.0f, 0.21f}};
    g->tLabels.push_back(titleLabel);
    titleLabel->setText("Free Imperium");


    float labelsOffset = 30.0f;
    glm::vec2 labelSize{450.f, 100.0f};
    glm::vec3 labelPos{100.0f, 450.0f, 0.1f};
   
    TextLabel * playLabel = new TextLabel{labelPos, labelSize, weirdBrown, AM::FontSize::PX64, true,
                                          glm::vec3{labelSize.x * 0.5f, labelSize.y * 0.5f, 0.21f}};
    g->tLabels.push_back(playLabel);
    playLabel->evName = ClickEventType::START_ROOM;
    playLabel->setText("Play");

    labelPos.y -= labelsOffset + labelSize.y;

    TextLabel * quitLabel = new TextLabel{labelPos, labelSize, weirdBrown, AM::FontSize::PX16, true,
                                          glm::vec3{labelSize.x * 0.5f, labelSize.y * 0.5f, 0.21f}};
    g->tLabels.push_back(quitLabel);
    quitLabel->evName = ClickEventType::QUIT_GAME;
    quitLabel->setText("Quit game");
}

void Close()
{
    for (auto it = Gui::Base::windows.begin(); it != Gui::Base::windows.end(); ++it) {
        if ((*it)->type == WindowType::MENU_SCREEN) {
            delete *it;
            Gui::Base::windows.erase(it);
            return;
        }
    }
}


} // Gui::Menu



namespace Gui::Room {

void Open(const glm::vec2 & resolution)
{
    Window * w = new Window{glm::vec3{0.0, 0.0, 0.0}, resolution, darkBrown, false, WindowType::ROOM_SCREEN};
    Gui::Base::windows.push_back(w);
   
    float listsXOffset = 100.0f;
    glm::vec3 listPos{100.0f, 50.0f, 0.1f};
    glm::vec2 listSize{500.0f, 600.0f};
    List * playerList = new List{listPos, listSize, 40.0f, brown, 
                                  lightBrown, 5.0f};
    w->lists.push_back(playerList);
    playerList->id = 0;
    playerList->SetTitle("Players", AM::FontSize::PX32);

    listPos.x += listSize.x + listsXOffset;
    List * countryList = new List{listPos, listSize, 40.0f, brown, 
                                  lightBrown, 5.0f};
    w->lists.push_back(countryList);
    countryList->id = 1;
    countryList->SetTitle("Pick country", AM::FontSize::PX32);

    { // group with start game button
        glm::vec3 pos = glm::vec3{listPos.x + listSize.x + listsXOffset, listPos.y, 0.0f};
        Group * grp = new Group{pos, glm::vec3{200.0f, 100.0f, 0.1f}, weirdBrown, false};
        w->groups.push_back(grp);
    
        glm::vec2 labelSize{200.0f,  50.0f};
        TextLabel * startLabel = new TextLabel{pos, labelSize, weirdBrown, AM::FontSize::PX32, true,
                                               glm::vec3{labelSize * 0.5f, 0.2f}};
        grp->tLabels.push_back(startLabel);
        startLabel->evName = ClickEventType::START_GAME;
        startLabel->setText("Start game");
    }

}

void AddCountryToList(const std::string & countryName)
{
    List * countryList = nullptr;
    for (auto win : Gui::Base::windows) {
        if (win->type == WindowType::ROOM_SCREEN) {
            for (auto list : win->lists) {
                if (list->id == 1) {
                    countryList = list;
                    break;
                }
            }
            break;
        }
    }
    assert(countryList);
Log("asdasd");

    Group * grp = new Group{glm::vec3{0.0, 0.0, .1}, glm::vec2{500.0, 20.0}, weirdBrown, false};
    
    TextLabel * title2 = new TextLabel{glm::vec3{0.0, 0.0, .11}, glm::vec2{500.0, 20.0}, weirdBrown, AM::FontSize::PX16,
                                       true, glm::vec3{grp->backgr->GetSize() * 0.5f, .2}};
    grp->tLabels.push_back(title2);
    title2->evName = ClickEventType::PICK_COUNTRY;
    title2->setText(countryName);
   
    countryList->AddGroup(grp);
}

void ClearPlayersList()
{
    List * playersList = nullptr;
    for (auto win : Gui::Base::windows) {
        if (win->type == WindowType::ROOM_SCREEN) {
            for (auto list : win->lists) {
                if (list->id == 0) {
                    playersList = list;
                    break;
                }
            }
            break;
        }
    }
    assert(playersList);

    for (int i = 0; i < playersList->groups.size(); ++i) {
        delete playersList->groups[i];
    }
    playersList->groups.clear();

}

void AddPlayerToList(const std::string & s)
{
    List * playersList = nullptr;
    for (auto win : Gui::Base::windows) {
        if (win->type == WindowType::ROOM_SCREEN) {
            for (auto list : win->lists) {
                if (list->id == 0) {
                    playersList = list;
                    break;
                }
            }
            break;
        }
    }
    assert(playersList);

    Group * grp = new Group{glm::vec3{0.0, 0.0, .1}, glm::vec2{500.0, 20.0}, weirdBrown, false};
    
    TextLabel * title2 = new TextLabel{glm::vec3{0.0, 0.0, .11}, glm::vec2{500.0, 20.0}, weirdBrown,
                                       AM::FontSize::PX16, true, glm::vec3{grp->backgr->GetSize() * 0.5f, .2}};
    grp->tLabels.push_back(title2);
    title2->setText(s);
   
    playersList->AddGroup(grp);
}

std::string GetPickedCountry()
{
    Log((int)clickData.evType);
    //Log(clickData.group << ", "<< clickData.group->tLabels.size());
    assert(clickData.group && clickData.group->tLabels.size());
    return clickData.group->tLabels[0]->text.content;
}

void Close()
{
    for (auto it = Gui::Base::windows.begin(); it != Gui::Base::windows.end(); ++it) {
        if ((*it)->type == WindowType::ROOM_SCREEN) {
            delete *it;
            Gui::Base::windows.erase(it);
            break;
        }
    }
}


} // Gui::Room


namespace Gui::TopBar {

Observer * Open(const std::vector<std::string> & values, const glm::vec2 & resolution)
{
    glm::vec2 offset{5.0f, 2.5f};
    
    glm::vec2 wSize{resolution.x * 0.5f, 100.0f};
    glm::vec3 wPos{resolution.x * 0.5f - wSize.x * 0.5f, resolution.y - wSize.y, 0.0f};

    Window * w = new Window{wPos, wSize, darkBrown, false, WindowType::TOP_BAR};
    Gui::Base::windows.push_back(w);

    Group * grp = new Group{wPos, wSize, darkBrown, false};
    grp->id = 0;
    w->groups.push_back(grp);

    glm::vec2 ctrSize{200.0f, 100.0f};
    glm::vec3 ctrPos{wPos.x + wSize.x * 0.5f - ctrSize.x * 0.5f, wPos.y, 0.1f};
    TextLabel * ctrName = new TextLabel{ctrPos, ctrSize, weirdBrown, AM::FontSize::PX32, true, glm::vec3{ctrSize * 0.5f, 0.11f}};
    grp->tLabels.push_back(ctrName);
    ctrName->setText("Atlantyda");
    ctrName->id = 1;
    ctrName->evName = ClickEventType::OPEN_COUNTRY;

    { // cash group
        float cashNumLen = 60.0f;
        glm::vec2 cashSize{100.0f, 40.0f};
        glm::vec3 cashPos{wPos.x + 10.0f, wPos.y + wSize.y * 0.5f, 0.05f};
        Group * cashG = new Group{cashPos, cashSize, brown, true};
        cashG->id = 1;
        w->groups.push_back(cashG);
        
        TextLabel * cash = new TextLabel{cashPos, cashSize, weirdBrown, AM::FontSize::PX16, true,
                                         glm::vec3{cashSize.x * 0.5f + 20.0f, cashSize.y * 0.5f, 0.06f}};
        cashG->tLabels.push_back(cash);
        cash->setText("321k");
        cash->id = 1;

        IconLabel * icon = new IconLabel{cashPos, glm::vec2{40, 40}};
        cashG->iLabels.push_back(icon);
        icon->setIcon("src/img/gold.png");
     
        cashSize.x *= 2.0f;
        cashSize.y *= 3.0f;
        cashPos.y -= cashSize.y;
        cashPos.z = 0.05f;
        Group * cashGHov = new Group{cashPos, cashSize, brown, false};
        cashGHov->id = 2;
        cashG->groups.push_back(cashGHov);


        glm::vec2 propGrpSize{cashSize.x * 0.65f - offset.x * 2.0f, cashSize.y};
        glm::vec3 propGrpPos{cashPos.x + offset.x, cashPos.y + cashSize.y - offset.y - propGrpSize.y, 0.12f};
        Group * propGrp = new Group{propGrpPos, propGrpSize, brown, false};
        cashGHov->groups.push_back(propGrp);
            
        glm::vec2 valGrpSize = glm::vec2{cashSize.x - propGrpSize.x - offset.x, propGrpSize.y};
        glm::vec3 valGrpPos{cashPos.x + propGrpSize.x, cashPos.y + cashSize.y - offset.y - valGrpSize.y, 0.12f};
        Group * valGrp = new Group{valGrpPos, valGrpSize, brown, false};
        valGrp->id = 3;
        cashGHov->groups.push_back(valGrp);
            
        glm::vec2 propNameSize{propGrpSize.x, 25.0f};
        glm::vec3 propNamePos{propGrpPos.x, propGrpPos.y + propGrpSize.y - propNameSize.y - offset.y, 0.13f};
        glm::vec2 valNameSize{valGrpSize.x, 25.0f};
        glm::vec3 valNamePos{valGrpPos.x, valGrpPos.y + valGrpSize.y - valNameSize.y - offset.y, 0.13f};
        { // income 
            TextLabel * nameLab = new TextLabel{propNamePos, propNameSize, weirdBrown, AM::FontSize::PX16, true, glm::vec3{propNameSize * 0.5f, 0.13f}};
            propGrp->tLabels.push_back(nameLab);
            nameLab->setText("Income");
            
            TextLabel * valLab = new TextLabel{valNamePos, valNameSize, weirdBrown, AM::FontSize::PX16, true, glm::vec3{valNameSize * 0.5f, 0.13f}};
            valGrp->tLabels.push_back(valLab);
            valLab->setText("0");
            valLab->id = 1;
        }

        propNamePos.y -= (propNameSize.y + offset.y);
        valNamePos.y -= (valNameSize.y + offset.y);

        { // armyMaintenance 
            TextLabel * nameLab = new TextLabel{propNamePos, propNameSize, weirdBrown, AM::FontSize::PX16, true, glm::vec3{propNameSize * 0.5f, 0.13f}};
            propGrp->tLabels.push_back(nameLab);
            nameLab->setText("Army maintenance");
            
            TextLabel * valLab = new TextLabel{valNamePos, valNameSize, weirdBrown, AM::FontSize::PX16, true, glm::vec3{valNameSize * 0.5f, 0.13f}};
            valGrp->tLabels.push_back(valLab);
            valLab->setText("0");
            valLab->id = 2;
        }
    }

    { // date group
        glm::vec2 dateGrpSize{200.0f, wSize.y - 2.0f * offset.y};
        glm::vec3 dateGrpPos{wPos.x + wSize.x - dateGrpSize.x - offset.x, wPos.y + offset.y, 0.1f};
        Group * dateG = new Group{dateGrpPos, dateGrpSize, brown, true};
        dateG->id = 4;
        w->groups.push_back(dateG);
 
        glm::vec2 dateSize{dateGrpSize.x - 2 * offset.x, 25.0f};
        glm::vec3 datePos{dateGrpPos.x + offset.x, dateGrpPos.y + offset.y, dateGrpPos.z + 0.05f};
        
        TextLabel * text = new TextLabel{datePos, dateSize, weirdBrown, AM::FontSize::PX16, true, glm::vec3{dateSize * 0.5f, .2}};
        dateG->tLabels.push_back(text);
        text->setText("12 Jan 2010");
        text->id = 1;
    
        datePos.y += dateSize.y + offset.y;
        TextLabel * speed = new TextLabel{datePos, dateSize, weirdBrown, AM::FontSize::PX16, true, glm::vec3{dateSize * 0.5f, .2}};
        dateG->tLabels.push_back(speed);
        speed->setText("0");
        speed->id = 2;
 
    }
    return &w->observer;
}


} // Gui::TopBar


namespace Gui::Prov {

Observer * Open(const glm::vec2 & resolution)
{
    for (std::size_t i = 0; i < Base::windows.size(); ++i) {
        if (Base::windows[i]->type == WindowType::UNIT || 
            Base::windows[i]->type == WindowType::PROV || 
            Base::windows[i]->type == WindowType::PROV_SIEGE || 
            Base::windows[i]->type == WindowType::UNITS_LIST ||
            Base::windows[i]->type == WindowType::MY_COUNTRY ||
            Base::windows[i]->type == WindowType::COUNTRY) { 
            
            assert(Base::windows[i]);
            delete Base::windows[i];
            Base::windows.erase(Base::windows.begin() + i);
            --i;
         }
    }

    glm::vec2 wSize{400.0f, 400.0f};
    glm::vec3 wPos{20.0f, 20.0f, 0.0f};
    
    glm::vec2 offset{5.0f, 2.5f};

    Window * w = new Window{wPos, wSize, darkBrown, true, WindowType::PROV};
    Gui::Base::windows.push_back(w);

    glm::vec2 provNameSize{wSize.x - 2 * offset.x, 40.0f - 2 * offset.y};
    glm::vec3 provNamePos{wPos.x + offset.x , wPos.y + wSize.y - provNameSize.y - offset.y, 0.1f};
    { // prov name
        Group * grp = new Group{provNamePos, provNameSize, brown, false};
        w->groups.push_back(grp);
        grp->id = 10;

        glm::vec3 textPos{provNameSize.x * 0.5f, provNameSize.y * 0.5f, 0.11f};
        TextLabel * provName = new TextLabel{provNamePos, provNameSize, brown, AM::FontSize::PX16, true, textPos};
        grp->tLabels.push_back(provName);
        provName->setText("Mazowsze");
        //provName->evName = ClickEventType::OPEN_PROV;
        provName->id = 1;
    }
    
    glm::vec2 iconTabGrpSize{wSize.x - 2.0f * offset.x, 40.0f + offset.y * 2.0f};
    glm::vec3 iconTabGrpPos{wPos.x + offset.x, provNamePos.y - offset.y - iconTabGrpSize.y, 0.1f};
    { // tabs icons
        Group * grp = new Group{iconTabGrpPos, iconTabGrpSize, brown, false};
        w->groups.push_back(grp);
        grp->id = 200;

        glm::vec2 iconSize{40.0f, 40.0f};
        glm::vec3 iconPos{iconTabGrpPos.x + offset.x, iconTabGrpPos.y + offset.y, 0.12f};

        IconLabel * icon = new IconLabel{iconPos, iconSize};
        grp->iLabels.push_back(icon);
        icon->setIcon("src/img/gold.png");
        icon->id = 1;
        icon->evName = ClickEventType::PROV_SWITCH_TAB; 

        iconPos.x += offset.x + iconSize.x;
        IconLabel * icon2 = new IconLabel{iconPos, iconSize};
        grp->iLabels.push_back(icon2);
        icon2->setIcon("src/img/gold.png");
        icon2->id = 2;
        icon2->evName = ClickEventType::PROV_SWITCH_TAB; 
   }

    glm::vec2 tabGrpSize{wSize.x - offset.x * 2.0f, iconTabGrpPos.y - wPos.y - 2 * offset.y};
    glm::vec3 tabGrpPos{wPos.x + offset.x, wPos.y + offset.y, 0.1f};

    { // tab grp I.
        Group * tab1 = new Group{tabGrpPos, tabGrpSize, darkBrown, false};
        tab1->id = 1;
        w->groups.push_back(tab1);

        { // prov properties group
            glm::vec2 groupSize{tabGrpSize.x * 0.57f, 100.0f};
            glm::vec3 groupPos{tabGrpPos.x, tabGrpPos.y + tabGrpSize.y - offset.y - groupSize.y, 0.11f};
            Group * grp = new Group{groupPos, groupSize, brown, false};
            tab1->groups.push_back(grp);
           
            glm::vec2 propGrpSize{groupSize.x * 0.65f - offset.x * 2.0f, groupSize.y};
            glm::vec3 propGrpPos{groupPos.x + offset.x, groupPos.y + groupSize.y - offset.y - propGrpSize.y, 0.12f};
            Group * propGrp = new Group{propGrpPos, propGrpSize, brown, false};
            grp->groups.push_back(propGrp);
            
            glm::vec2 valGrpSize = glm::vec2{groupSize.x - propGrpSize.x - offset.x, propGrpSize.y};//{groupSize.x * 0.5f, groupSize.y};
            glm::vec3 valGrpPos{groupPos.x + propGrpSize.x, groupPos.y + groupSize.y - offset.y - valGrpSize.y, 0.12f};
            Group * valGrp = new Group{valGrpPos, valGrpSize, brown, false};
            grp->groups.push_back(valGrp);
            valGrp->id = 11;

            glm::vec2 propNameSize{propGrpSize.x, 25.0f};
            glm::vec3 propNamePos{propGrpPos.x, propGrpPos.y + propGrpSize.y - propNameSize.y - offset.y, 0.13f};
            glm::vec2 valNameSize{valGrpSize.x, 25.0f};
            glm::vec3 valNamePos{valGrpPos.x, valGrpPos.y + valGrpSize.y - valNameSize.y - offset.y, 0.13f};
            { // manpower
                TextLabel * nameLab = new TextLabel{propNamePos, propNameSize, weirdBrown, AM::FontSize::PX16, true, glm::vec3{propNameSize * 0.5f, 0.13f}};
                propGrp->tLabels.push_back(nameLab);
                nameLab->setText("Manpower");
                
                TextLabel * valLab = new TextLabel{valNamePos, valNameSize, weirdBrown, AM::FontSize::PX16, true, glm::vec3{valNameSize * 0.5f, 0.13f}};
                valGrp->tLabels.push_back(valLab);
                valLab->setText("3400");
                valLab->id = 1;
            }

            propNamePos.y -= (propNameSize.y + offset.y);
            valNamePos.y -= (valNameSize.y + offset.y);

            { // manpower2
                TextLabel * nameLab = new TextLabel{propNamePos, propNameSize, weirdBrown, AM::FontSize::PX16, true, glm::vec3{propNameSize * 0.5f, 0.13f}};
                propGrp->tLabels.push_back(nameLab);
                nameLab->setText("Manpower2");
                
                TextLabel * valLab = new TextLabel{valNamePos, valNameSize, weirdBrown, AM::FontSize::PX16, true, glm::vec3{valNameSize * 0.5f, 0.13f}};
                valGrp->tLabels.push_back(valLab);
                valLab->setText("3400");
                valLab->id = 2;
            }


        }
            
        { // country group
            glm::vec2 groupSize{tabGrpSize.x * 0.4f, 100.0f};
            glm::vec3 groupPos{tabGrpPos.x + tabGrpSize.x - groupSize.x, tabGrpPos.y + tabGrpSize.y - offset.y - groupSize.y, 0.11f};
            Group * grp = new Group{groupPos, groupSize, brown, false};
            tab1->groups.push_back(grp);
            grp->id = 12;
            glm::vec2 textLabSize{groupSize.x - 2.0f * offset.x, 20.0f};
            glm::vec3 textLabPos{groupPos.x + offset.x, groupPos.y + groupSize.y - textLabSize.y - offset.y, 0.12f};
            glm::vec3 textPos{textLabSize.x * 0.5f, textLabSize.y * 0.5f, 0.13f};
            TextLabel * ctrName = new TextLabel{textLabPos, textLabSize, weirdBrown, AM::FontSize::PX16, true, textPos};
            grp->tLabels.push_back(ctrName);
            ctrName->setText("Poland");
            ctrName->evName = ClickEventType::OPEN_COUNTRY;
            ctrName->id = 1;
        }
    }

    { // tab grp II.
        Group * tab2 = new Group{tabGrpPos, tabGrpSize, darkBrown, false};
        w->groups.push_back(tab2);
        tab2->id = 2;
        tab2->visible = false;
        { // country group
            glm::vec2 groupSize{tabGrpSize.x * 0.4f, 100.0f};
            glm::vec3 groupPos{tabGrpPos.x + tabGrpSize.x - groupSize.x, tabGrpPos.y + tabGrpSize.y - offset.y - groupSize.y, 0.11f};
            Group * grp = new Group{groupPos, groupSize, brown, false};
            tab2->groups.push_back(grp);

            glm::vec2 textLabSize{groupSize.x - 2.0f * offset.x, 20.0f};
            glm::vec3 textLabPos{groupPos.x + offset.y, groupPos.y + groupSize.y - textLabSize.y, 0.12f};
            glm::vec3 textPos{textLabSize.x * 0.5f, textLabSize.y * 0.5f, 0.13f};
            TextLabel * ctrName = new TextLabel{textLabPos, textLabSize, weirdBrown, AM::FontSize::PX16, true, textPos};
            grp->tLabels.push_back(ctrName);
            ctrName->setText("Russia");
        }
    }
    return &w->observer;
}

void Close()
{
    for (auto it = Gui::Base::windows.begin(); it != Gui::Base::windows.end(); ++it) {
        if ((*it)->type == WindowType::PROV) {
            delete *it;
            Gui::Base::windows.erase(it);
            break;
        }
    }
}

void SwitchTab()
{
    assert(clickData.window && clickData.group && clickData.val >= 1);
    for (auto g : clickData.window->groups) {
        if (g->id == 1 || g->id == 2) {
            g->visible = false;
        }
        if (g->id == clickData.val) {
            g->visible = true;
        }
    }
}

} // Gui::Prov


namespace Gui::ProvSiege {

Observer * Open(const glm::vec2 & resolution)
{
    for (std::size_t i = 0; i < Base::windows.size(); ++i) {
        if (Base::windows[i]->type == WindowType::UNIT || 
            Base::windows[i]->type == WindowType::PROV || 
            Base::windows[i]->type == WindowType::PROV_SIEGE || 
            Base::windows[i]->type == WindowType::UNITS_LIST ||
            Base::windows[i]->type == WindowType::MY_COUNTRY ||
            Base::windows[i]->type == WindowType::COUNTRY)
        { 
            
            assert(Base::windows[i]);
            delete Base::windows[i];
            Base::windows.erase(Base::windows.begin() + i);
            --i;
        }
    }

    glm::vec2 wSize{400.0f, 400.0f};
    glm::vec3 wPos{20.0f, 20.0f, 0.0f};
    
    glm::vec2 offset{5.0f, 2.5f};

    Window * w = new Window{wPos, wSize, darkBrown, true, WindowType::PROV};
    Gui::Base::windows.push_back(w);

    glm::vec2 provNameSize{wSize.x - 2 * offset.x, 40.0f - 2 * offset.y};
    glm::vec3 provNamePos{wPos.x + offset.x , wPos.y + wSize.y - provNameSize.y - offset.y, 0.1f};
    { // prov name
        Group * grp = new Group{provNamePos, provNameSize, brown, false};
        w->groups.push_back(grp);
        grp->id = 10;

        glm::vec3 textPos{provNameSize.x * 0.5f, provNameSize.y * 0.5f, 0.11f};
        TextLabel * provName = new TextLabel{provNamePos, provNameSize, brown, AM::FontSize::PX16, true, textPos};
        grp->tLabels.push_back(provName);
        provName->setText("Mazowsze");
        provName->evName = ClickEventType::OPEN_PROV;
        provName->id = 1;
    }
    
    glm::vec2 tabGrpSize{wSize.x - offset.x * 2.0f, provNamePos.y - wPos.y - 2 * offset.y};
    glm::vec3 tabGrpPos{wPos.x + offset.x, wPos.y + offset.y, 0.1f};

    { // tab grp I.
        Group * tab1 = new Group{tabGrpPos, tabGrpSize, darkBrown, false};
        tab1->id = 1;
        w->groups.push_back(tab1);

        glm::vec2 listSize{tabGrpSize.x * 0.57f, 200.0f};
        glm::vec3 listPos{tabGrpPos.x, tabGrpPos.y + tabGrpSize.y - offset.y - listSize.y, 0.11f};
        { // attackers list (list is not inside group ;(
            List * attackersList = new List{listPos, listSize, 30.0f, darkBrown, 
                                                  brown, 5.0f};
            w->lists.push_back(attackersList);
            attackersList->id = 0;
            attackersList->SetTitle("Attackers", AM::FontSize::PX16);

            for (int att = 0; att < 10; ++att) {
                Group * grp = new Group{glm::vec3{0.0, 0.0, .12}, glm::vec2{listSize.x, 20.0}, weirdBrown, false};
   
                TextLabel * title2 = new TextLabel{glm::vec3{0.0, 0.0, .13}, glm::vec2{listSize.x, 20.0}, weirdBrown, AM::FontSize::PX16,
                                                   true, glm::vec3{grp->backgr->GetSize() * 0.5f, .2}};
                grp->tLabels.push_back(title2);
                title2->evName = ClickEventType::OPEN_COUNTRY;
                title2->setText("Spain " + std::to_string(att));
   
                attackersList->AddGroup(grp);
            }
        }

        { // country group
            glm::vec2 groupSize{tabGrpSize.x * 0.4f, 100.0f};
            glm::vec3 groupPos{tabGrpPos.x + tabGrpSize.x - groupSize.x, tabGrpPos.y + tabGrpSize.y - offset.y - groupSize.y, 0.11f};
            Group * grp = new Group{groupPos, groupSize, brown, false};
            tab1->groups.push_back(grp);
            grp->id = 11;

            glm::vec2 textLabSize{groupSize.x - 2.0f * offset.x, 20.0f};
            glm::vec3 textLabPos{groupPos.x + offset.x, groupPos.y + groupSize.y - textLabSize.y - offset.y, 0.12f};
            glm::vec3 textPos{textLabSize.x * 0.5f, textLabSize.y * 0.5f, 0.13f};
            TextLabel * ctrName = new TextLabel{textLabPos, textLabSize, weirdBrown, AM::FontSize::PX16, true, textPos};
            grp->tLabels.push_back(ctrName);
            ctrName->setText("Poland");
            ctrName->evName = ClickEventType::OPEN_COUNTRY;
            ctrName->id = 1;
        }


        { // prov properties group
            glm::vec2 groupSize{tabGrpSize.x * 0.9f, 100.0f};
            glm::vec3 groupPos{tabGrpPos.x, listPos.y - offset.y - groupSize.y, 0.11f};
            //glm::vec3 groupPos{tabGrpPos.x, tabGrpPos.y + tabGrpSize.y - offset.y - groupSize.y, 0.11f};
            Group * grp = new Group{groupPos, groupSize, brown, false};
            tab1->groups.push_back(grp);
           
            glm::vec2 propGrpSize{(groupSize.x - offset.x * 2.0f) * 0.65f, groupSize.y};
            glm::vec3 propGrpPos{groupPos.x + offset.x, groupPos.y + groupSize.y - offset.y - propGrpSize.y, 0.12f};
            Group * propGrp = new Group{propGrpPos, propGrpSize, brown, false};
            grp->groups.push_back(propGrp);
            
            glm::vec2 valGrpSize = glm::vec2{(groupSize.x - offset.x * 2.0f) - propGrpSize.x, propGrpSize.y};//{groupSize.x * 0.5f, groupSize.y};
            glm::vec3 valGrpPos{groupPos.x + propGrpSize.x + offset.x, groupPos.y + groupSize.y - offset.y - valGrpSize.y, 0.12f};
            Group * valGrp = new Group{valGrpPos, valGrpSize, brown, false};
            grp->groups.push_back(valGrp);
            valGrp->id = 12;

            glm::vec2 propNameSize{propGrpSize.x, 25.0f};
            glm::vec3 propNamePos{propGrpPos.x, propGrpPos.y + propGrpSize.y - propNameSize.y - offset.y, 0.13f};
            glm::vec2 valNameSize{valGrpSize.x, 25.0f};
            glm::vec3 valNamePos{valGrpPos.x, valGrpPos.y + valGrpSize.y - valNameSize.y - offset.y, 0.13f};
            { // sieger
                TextLabel * nameLab = new TextLabel{propNamePos, propNameSize, weirdBrown, AM::FontSize::PX16, true, glm::vec3{propNameSize * 0.5f, 0.13f}};
                propGrp->tLabels.push_back(nameLab);
                nameLab->setText("Sieged by");
                
                TextLabel * valLab = new TextLabel{valNamePos, valNameSize, weirdBrown, AM::FontSize::PX16, true, glm::vec3{valNameSize * 0.5f, 0.13f}};
                valGrp->tLabels.push_back(valLab);
                valLab->setText("Poland Poland");
                valLab->id = 1;
                valLab->evName = ClickEventType::OPEN_COUNTRY;
            }

            propNamePos.y -= (propNameSize.y + offset.y);
            valNamePos.y -= (valNameSize.y + offset.y);

            { // sieger size 
                TextLabel * nameLab = new TextLabel{propNamePos, propNameSize, weirdBrown, AM::FontSize::PX16, true, glm::vec3{propNameSize * 0.5f, 0.13f}};
                propGrp->tLabels.push_back(nameLab);
                nameLab->setText("Siege soldiers");
                
                TextLabel * valLab = new TextLabel{valNamePos, valNameSize, weirdBrown, AM::FontSize::PX16, true, glm::vec3{valNameSize * 0.5f, 0.13f}};
                valGrp->tLabels.push_back(valLab);
                valLab->setText("3400");
                valLab->id = 2;
            }

            propNamePos.y -= (propNameSize.y + offset.y);
            valNamePos.y -= (valNameSize.y + offset.y);

            { // siege lvl 
                TextLabel * nameLab = new TextLabel{propNamePos, propNameSize, weirdBrown, AM::FontSize::PX16, true, glm::vec3{propNameSize * 0.5f, 0.13f}};
                propGrp->tLabels.push_back(nameLab);
                nameLab->setText("Siege lvl");
                
                TextLabel * valLab = new TextLabel{valNamePos, valNameSize, weirdBrown, AM::FontSize::PX16, true, glm::vec3{valNameSize * 0.5f, 0.13f}};
                valGrp->tLabels.push_back(valLab);
                valLab->setText("0");
                valLab->id = 3;
            }
        }
            
    }
    return &w->observer;
}

void Close()
{
    for (auto it = Gui::Base::windows.begin(); it != Gui::Base::windows.end(); ++it) {
        if ((*it)->type == WindowType::PROV_SIEGE) {
            delete *it;
            Gui::Base::windows.erase(it);
            break;
        }
    }
}

} // Gui::ProvSiege



namespace Gui::Unit {

Observer * Open(const glm::vec2 & resolution)
{
    for (std::size_t i = 0; i < Base::windows.size(); ++i) {
        if (Base::windows[i]->type == WindowType::UNIT || 
            Base::windows[i]->type == WindowType::PROV || 
            Base::windows[i]->type == WindowType::PROV_SIEGE || 
            Base::windows[i]->type == WindowType::UNITS_LIST ||
            Base::windows[i]->type == WindowType::MY_COUNTRY ||
            Base::windows[i]->type == WindowType::COUNTRY) { 
            
            assert(Base::windows[i]);
            delete Base::windows[i];
            Base::windows.erase(Base::windows.begin() + i);
            --i;

         }
    
    }

    glm::vec2 wSize{400.0f, 400.0f};
    glm::vec3 wPos{20.0f, 20.0f, 0.0f};
    
    glm::vec2 offset{5.0f, 2.5f};

    Window * w = new Window{wPos, wSize, darkBrown, true, WindowType::UNIT};
    Gui::Base::windows.push_back(w);

    glm::vec2 unitNameSize{wSize.x - 2 * offset.x, 40.0f - 2 * offset.y};
    glm::vec3 unitNamePos{wPos.x + offset.x , wPos.y + wSize.y - unitNameSize.y - offset.y, 0.1f};
    { // unit name
        Group * grp = new Group{unitNamePos, unitNameSize, brown, false};
        w->groups.push_back(grp);
        grp->id = 1;

        glm::vec3 textPos{unitNameSize.x * 0.5f, unitNameSize.y * 0.5f, 0.11f};
        TextLabel * unitName = new TextLabel{unitNamePos, unitNameSize, brown, AM::FontSize::PX16, true, textPos};
        grp->tLabels.push_back(unitName);
        unitName->setText("Mazowsze unit");
        unitName->id = 1;
    }

    glm::vec2 countrySize{unitNameSize.x, unitNameSize.y};
    glm::vec3 countryPos{unitNamePos.x, unitNamePos.y - countrySize.y  - offset.y, 0.11f};
    { // country group
        Group * grp = new Group{countryPos, countrySize, brown, false};
        w->groups.push_back(grp);
        grp->id = 2;

        glm::vec2 textLabSize{countrySize.x, countrySize.y};
        glm::vec3 textLabPos{countryPos.x, countryPos.y, 0.12f};
        glm::vec3 textPos{textLabSize.x * 0.5f, textLabSize.y * 0.5f, 0.13f};
        TextLabel * ctrName = new TextLabel{textLabPos, textLabSize, weirdBrown, AM::FontSize::PX16, true, textPos};
        grp->tLabels.push_back(ctrName);
        ctrName->setText("Poland");
        ctrName->evName = ClickEventType::OPEN_COUNTRY;
        ctrName->id = 1;
    }


    { // unit properties group
        glm::vec2 groupSize{countrySize.x, countryPos.y - wPos.y - offset.y * 2.0f};
        glm::vec3 groupPos{countryPos.x, wPos.y + offset.y, 0.11f};
        Group * grp = new Group{groupPos, groupSize, brown, false};
        w->groups.push_back(grp);
        grp->id = 3;
           
        glm::vec2 propGrpSize{(groupSize.x - offset.x * 2.0f) * 0.65f, groupSize.y};
        glm::vec3 propGrpPos{groupPos.x + offset.x, groupPos.y + groupSize.y - offset.y - propGrpSize.y, 0.12f};
        Group * propGrp = new Group{propGrpPos, propGrpSize, brown, false};
        grp->groups.push_back(propGrp);
        propGrp->id = 4;

        glm::vec2 valGrpSize = glm::vec2{(groupSize.x - offset.x * 2.0f) - propGrpSize.x, propGrpSize.y};//{groupSize.x * 0.5f, groupSize.y};
        glm::vec3 valGrpPos{groupPos.x + propGrpSize.x + offset.x, groupPos.y + groupSize.y - offset.y - valGrpSize.y, 0.12f};
        Group * valGrp = new Group{valGrpPos, valGrpSize, brown, false};
        grp->groups.push_back(valGrp);
        valGrp->id = 5;

        glm::vec2 propNameSize{propGrpSize.x, 25.0f};
        glm::vec3 propNamePos{propGrpPos.x, propGrpPos.y + propGrpSize.y - propNameSize.y - offset.y, 0.13f};
        glm::vec2 valNameSize{valGrpSize.x, 25.0f};
        glm::vec3 valNamePos{valGrpPos.x, valGrpPos.y + valGrpSize.y - valNameSize.y - offset.y, 0.13f};
        { // size 
            TextLabel * nameLab = new TextLabel{propNamePos, propNameSize, weirdBrown, AM::FontSize::PX16, true, glm::vec3{propNameSize * 0.5f, 0.13f}};
            propGrp->tLabels.push_back(nameLab);
            nameLab->setText("Size");
            
            TextLabel * valLab = new TextLabel{valNamePos, valNameSize, weirdBrown, AM::FontSize::PX16, true, glm::vec3{valNameSize * 0.5f, 0.13f}};
            valGrp->tLabels.push_back(valLab);
            valLab->setText("130k");
            valLab->id = 1;
        }

        propNamePos.y -= (propNameSize.y + offset.y);
        valNamePos.y -= (valNameSize.y + offset.y);

        { // morale
            TextLabel * nameLab = new TextLabel{propNamePos, propNameSize, weirdBrown, AM::FontSize::PX16, true, glm::vec3{propNameSize * 0.5f, 0.13f}};
            propGrp->tLabels.push_back(nameLab);
            nameLab->setText("Morale");
         
            TextLabel * valLab = new TextLabel{valNamePos, valNameSize, weirdBrown, AM::FontSize::PX16, true, glm::vec3{valNameSize * 0.5f, 0.13f}};
            valGrp->tLabels.push_back(valLab);
            valLab->setText("100");
            valLab->id = 2;
        }

        propNamePos.y -= (propNameSize.y + offset.y);
        valNamePos.y -= (valNameSize.y + offset.y);

        { // discipline
            TextLabel * nameLab = new TextLabel{propNamePos, propNameSize, weirdBrown, AM::FontSize::PX16, true, glm::vec3{propNameSize * 0.5f, 0.13f}};
            propGrp->tLabels.push_back(nameLab);
            nameLab->setText("Discipline");
            
            TextLabel * valLab = new TextLabel{valNamePos, valNameSize, weirdBrown, AM::FontSize::PX16, true, glm::vec3{valNameSize * 0.5f, 0.13f}};
            valGrp->tLabels.push_back(valLab);
            valLab->setText("55");
            valLab->id = 3;
        }
    }

    return &w->observer;
}

void Close()
{
    for (auto it = Gui::Base::windows.begin(); it != Gui::Base::windows.end(); ++it) {
        if ((*it)->type == WindowType::UNIT) {
            delete *it;
            Gui::Base::windows.erase(it);
            break;
        }
    }
}

} // Gui::Unit


namespace Gui::UnitsList {

void Open(const glm::vec2 & resolution)
{
    glm::vec2 wSize{400.0f, 400.0f};
    glm::vec3 wPos{20.0f, 100.0f, 0.0f};
    
    glm::vec2 offset{5.0f, 2.5f};

    Window * w = new Window{wPos, wSize, darkBrown, true, WindowType::UNITS_LIST};
    Gui::Base::windows.push_back(w);

    glm::vec2 winNameSize{wSize.x - 2 * offset.x, 40.0f - 2 * offset.y};
    glm::vec3 winNamePos{wPos.x + offset.x , wPos.y + wSize.y - winNameSize.y - offset.y, 0.1f};
    { // unit name
        Group * grp = new Group{winNamePos, winNameSize, brown, false};
        w->groups.push_back(grp);

        glm::vec3 textPos{winNameSize.x * 0.5f, winNameSize.y * 0.5f, 0.11f};
        TextLabel * winName = new TextLabel{winNamePos, winNameSize, brown, AM::FontSize::PX16, true, textPos};
        grp->tLabels.push_back(winName);
        winName->setText("Units");
    }

    glm::vec2 mergeSize{wSize.x * 0.3f, 40.0f - 2 * offset.y};
    glm::vec3 mergePos{wPos.x + wSize.x - offset.x - mergeSize.x, winNamePos.y - offset.y - mergeSize.y, 0.1f};
    { // merge button
        Group * grp = new Group{mergePos, mergeSize, brown, false};
        w->groups.push_back(grp);

        glm::vec3 textPos{mergeSize.x * 0.5f, mergeSize.y * 0.5f, 0.11f};
        TextLabel * merge = new TextLabel{mergePos, mergeSize, brown, AM::FontSize::PX16, true, textPos};
        grp->tLabels.push_back(merge);
        merge->setText("Merge");
    }

    glm::vec3 listPos{winNamePos.x, wPos.y + offset.y, 0.1f};
    glm::vec2 listSize{winNameSize.x, mergePos.y - offset.y * 2.0f - wPos.y};
    List * unitsList = new List{listPos, listSize, 40.0f, brown, 
                                  lightBrown, 5.0f};
    w->lists.push_back(unitsList);
    unitsList->id = 0;
    //unitsList->SetTitle("Players", AM::FontSize::PX32);

    for (int i = 0; i < 25; ++i) {
        Group * grp = new Group{glm::vec3{0.0, 0.0, .12f}, glm::vec2{listSize.x, 20.0}, weirdBrown, false};
        TextLabel * title2 = new TextLabel{glm::vec3{0.0, 0.0, .13}, glm::vec2{listSize.x, 20.0}, weirdBrown, AM::FontSize::PX16,
                                           true, glm::vec3{grp->backgr->GetSize() * 0.5f, .2}};
        grp->tLabels.push_back(title2);
        title2->evName = ClickEventType::OPEN_UNIT;
        title2->setText("Unit " + std::to_string(i));
        unitsList->AddGroup(grp);
    }

}


void Close()
{
    for (auto it = Gui::Base::windows.begin(); it != Gui::Base::windows.end(); ++it) {
        if ((*it)->type == WindowType::UNITS_LIST) {
            delete *it;
            Gui::Base::windows.erase(it);
            break;
        }
    }
}

} // Gui::UnitsList



namespace Gui::Country {

Observer * Open(const glm::vec2 & resolution, int ctrId)
{
    for (std::size_t i = 0; i < Base::windows.size(); ++i) {
        if (Base::windows[i]->type == WindowType::UNIT || 
            Base::windows[i]->type == WindowType::PROV || 
            Base::windows[i]->type == WindowType::PROV_SIEGE || 
            Base::windows[i]->type == WindowType::UNITS_LIST ||
            Base::windows[i]->type == WindowType::MY_COUNTRY ||
            Base::windows[i]->type == WindowType::COUNTRY) { 
            
            assert(Base::windows[i]);
            delete Base::windows[i];
            Base::windows.erase(Base::windows.begin() + i);
            --i;
         }
    }

    glm::vec2 wSize{600.0f, 500.0f};
    glm::vec3 wPos{20.0f, 20.0f, 0.0f};
    
    glm::vec2 offset{5.0f, 2.5f};

    Window * w = new Window{wPos, wSize, darkBrown, true, WindowType::COUNTRY};
    Gui::Base::windows.push_back(w);
    w->id = ctrId;
    
    glm::vec2 ctrNameSize{wSize.x - 2 * offset.x, 40.0f - 2 * offset.y};
    glm::vec3 ctrNamePos{wPos.x + offset.x , wPos.y + wSize.y - ctrNameSize.y - offset.y, 0.1f};
    { // prov name
        Group * grp = new Group{ctrNamePos, ctrNameSize, brown, false};
        w->groups.push_back(grp);
        grp->id = 10;

        glm::vec3 textPos{ctrNameSize.x * 0.5f, ctrNameSize.y * 0.5f, 0.11f};
        TextLabel * ctrName = new TextLabel{ctrNamePos, ctrNameSize, brown, AM::FontSize::PX16, true, textPos};
        grp->tLabels.push_back(ctrName);
        ctrName->setText("Poland");
        //provName->evName = ClickEventType::OPEN_PROV;
        ctrName->id = 1;
    }
    
    glm::vec2 iconTabGrpSize{wSize.x - 2.0f * offset.x, 40.0f + offset.y * 2.0f};
    glm::vec3 iconTabGrpPos{wPos.x + offset.x, ctrNamePos.y - offset.y - iconTabGrpSize.y, 0.1f};
    { // tabs icons
        Group * grp = new Group{iconTabGrpPos, iconTabGrpSize, brown, false};
        w->groups.push_back(grp);
        grp->id = 200;

        glm::vec2 iconSize{40.0f, 40.0f};
        glm::vec3 iconPos{iconTabGrpPos.x + offset.x, iconTabGrpPos.y + offset.y, 0.12f};

        IconLabel * icon = new IconLabel{iconPos, iconSize};
        grp->iLabels.push_back(icon);
        icon->setIcon("src/img/gold.png");
        icon->id = 1;
        icon->evName = ClickEventType::PROV_SWITCH_TAB; 

        iconPos.x += offset.x + iconSize.x;
        IconLabel * icon2 = new IconLabel{iconPos, iconSize};
        grp->iLabels.push_back(icon2);
        icon2->setIcon("src/img/gold.png");
        icon2->id = 2;
        icon2->evName = ClickEventType::PROV_SWITCH_TAB; 
   }

    glm::vec2 tabGrpSize{wSize.x - offset.x * 2.0f, iconTabGrpPos.y - wPos.y - 2 * offset.y};
    glm::vec3 tabGrpPos{wPos.x + offset.x, wPos.y + offset.y, 0.1f};

    { // tab grp I.
        Group * tab1 = new Group{tabGrpPos, tabGrpSize, darkBrown, false};
        tab1->id = 1;
        w->groups.push_back(tab1);

        { // ctr properties group
            glm::vec2 groupSize{tabGrpSize.x * 0.57f, 100.0f};
            glm::vec3 groupPos{tabGrpPos.x, tabGrpPos.y + tabGrpSize.y - offset.y - groupSize.y, 0.11f};
            Group * grp = new Group{groupPos, groupSize, brown, false};
            tab1->groups.push_back(grp);
           
            glm::vec2 propGrpSize{groupSize.x * 0.65f - offset.x * 2.0f, groupSize.y};
            glm::vec3 propGrpPos{groupPos.x + offset.x, groupPos.y + groupSize.y - offset.y - propGrpSize.y, 0.12f};
            Group * propGrp = new Group{propGrpPos, propGrpSize, brown, false};
            grp->groups.push_back(propGrp);
            
            glm::vec2 valGrpSize = glm::vec2{groupSize.x - propGrpSize.x - offset.x, propGrpSize.y};//{groupSize.x * 0.5f, groupSize.y};
            glm::vec3 valGrpPos{groupPos.x + propGrpSize.x, groupPos.y + groupSize.y - offset.y - valGrpSize.y, 0.12f};
            Group * valGrp = new Group{valGrpPos, valGrpSize, brown, false};
            grp->groups.push_back(valGrp);
            valGrp->id = 11;

            glm::vec2 propNameSize{propGrpSize.x, 25.0f};
            glm::vec3 propNamePos{propGrpPos.x, propGrpPos.y + propGrpSize.y - propNameSize.y - offset.y, 0.13f};
            glm::vec2 valNameSize{valGrpSize.x, 25.0f};
            glm::vec3 valNamePos{valGrpPos.x, valGrpPos.y + valGrpSize.y - valNameSize.y - offset.y, 0.13f};
            { // attitude
                TextLabel * nameLab = new TextLabel{propNamePos, propNameSize, weirdBrown, AM::FontSize::PX16, true, glm::vec3{propNameSize * 0.5f, 0.13f}};
                propGrp->tLabels.push_back(nameLab);
                nameLab->setText("Attitude");
                
                TextLabel * valLab = new TextLabel{valNamePos, valNameSize, weirdBrown, AM::FontSize::PX16, true, glm::vec3{valNameSize * 0.5f, 0.13f}};
                valGrp->tLabels.push_back(valLab);
                valLab->setText("3400");
                valLab->id = 1;
            }

            propNamePos.y -= (propNameSize.y + offset.y);
            valNamePos.y -= (valNameSize.y + offset.y);

            { // manpower2
                TextLabel * nameLab = new TextLabel{propNamePos, propNameSize, weirdBrown, AM::FontSize::PX16, true, glm::vec3{propNameSize * 0.5f, 0.13f}};
                propGrp->tLabels.push_back(nameLab);
                nameLab->setText("Manpower2");
                
                TextLabel * valLab = new TextLabel{valNamePos, valNameSize, weirdBrown, AM::FontSize::PX16, true, glm::vec3{valNameSize * 0.5f, 0.13f}};
                valGrp->tLabels.push_back(valLab);
                valLab->setText("3400");
                valLab->id = 2;
            }


        }
            
        { // actions list
            glm::vec2 listSize{tabGrpSize.x * 0.4f, iconTabGrpPos.y - wPos.y - offset.y * 2.0f};
            glm::vec3 listPos{tabGrpPos.x + tabGrpSize.x - listSize.x, wPos.y + offset.y, 0.11f};
            List * actionsList = new List{listPos, listSize, 40.0f, brown, lightBrown, 5.0f};
            w->lists.push_back(actionsList);
            actionsList->id = 0;
            actionsList->SetTitle("Actions", AM::FontSize::PX32);
        

//            for (int i = 0; i < 25; ++i) {
                Group * grp = new Group{glm::vec3{0.0, 0.0, .12f}, glm::vec2{listSize.x, 20.0}, weirdBrown, false};
                TextLabel * declareWar = new TextLabel{glm::vec3{0.0, 0.0, .13}, glm::vec2{listSize.x, 20.0}, weirdBrown, AM::FontSize::PX16,
                                                       true, glm::vec3{grp->backgr->GetSize() * 0.5f, .2}};
                grp->tLabels.push_back(declareWar);
                declareWar->evName = ClickEventType::DECLARE_WAR;
                declareWar->setText("Declare war");
                actionsList->AddGroup(grp);
//            }


       }
    }

    { // tab grp II.
        Group * tab2 = new Group{tabGrpPos, tabGrpSize, darkBrown, false};
        w->groups.push_back(tab2);
        tab2->id = 2;
        tab2->visible = false;
        { // country group
            glm::vec2 groupSize{tabGrpSize.x * 0.4f, 100.0f};
            glm::vec3 groupPos{tabGrpPos.x + tabGrpSize.x - groupSize.x, tabGrpPos.y + tabGrpSize.y - offset.y - groupSize.y, 0.11f};
            Group * grp = new Group{groupPos, groupSize, brown, false};
            tab2->groups.push_back(grp);

            glm::vec2 textLabSize{groupSize.x - 2.0f * offset.x, 20.0f};
            glm::vec3 textLabPos{groupPos.x + offset.y, groupPos.y + groupSize.y - textLabSize.y, 0.12f};
            glm::vec3 textPos{textLabSize.x * 0.5f, textLabSize.y * 0.5f, 0.13f};
            TextLabel * ctrName = new TextLabel{textLabPos, textLabSize, weirdBrown, AM::FontSize::PX16, true, textPos};
            grp->tLabels.push_back(ctrName);
            ctrName->setText("Russia");
        }
    }
    return &w->observer;
}

void Close()
{
    for (auto it = Gui::Base::windows.begin(); it != Gui::Base::windows.end(); ++it) {
        if ((*it)->type == WindowType::COUNTRY) {
            delete *it;
            Gui::Base::windows.erase(it);
            break;
        }
    }
}

void SwitchTab()
{/*
    assert(clickData.window && clickData.group && clickData.val >= 1);
    for (auto g : clickData.window->groups) {
        if (g->id == 1 || g->id == 2) {
            g->visible = false;
        }
        if (g->id == clickData.val) {
            g->visible = true;
        }
    }*/
}

int GetId()
{
    for (auto it = Gui::Base::windows.begin(); it != Gui::Base::windows.end(); ++it) {
        if ((*it)->type == WindowType::COUNTRY) {
            return (*it)->id;
        }
    }
    return -1;
}

} // Gui::Country




namespace Gui::MyCountry {

Observer * Open(const glm::vec2 & resolution)
{
    for (std::size_t i = 0; i < Base::windows.size(); ++i) {
        if (Base::windows[i]->type == WindowType::UNIT || 
            Base::windows[i]->type == WindowType::PROV || 
            Base::windows[i]->type == WindowType::PROV_SIEGE || 
            Base::windows[i]->type == WindowType::UNITS_LIST ||
            Base::windows[i]->type == WindowType::MY_COUNTRY ||
            Base::windows[i]->type == WindowType::COUNTRY) { 
            
            assert(Base::windows[i]);
            delete Base::windows[i];
            Base::windows.erase(Base::windows.begin() + i);
            --i;
         }
    }

    glm::vec2 wSize{600.0f, 500.0f};
    glm::vec3 wPos{20.0f, 20.0f, 0.0f};
    
    glm::vec2 offset{5.0f, 2.5f};

    Window * w = new Window{wPos, wSize, darkBrown, true, WindowType::MY_COUNTRY};
    Gui::Base::windows.push_back(w);

    glm::vec2 ctrNameSize{wSize.x - 2 * offset.x, 40.0f - 2 * offset.y};
    glm::vec3 ctrNamePos{wPos.x + offset.x , wPos.y + wSize.y - ctrNameSize.y - offset.y, 0.1f};
    { // ctr name
        Group * grp = new Group{ctrNamePos, ctrNameSize, brown, false};
        w->groups.push_back(grp);
        grp->id = 10;

        glm::vec3 textPos{ctrNameSize.x * 0.5f, ctrNameSize.y * 0.5f, 0.11f};
        TextLabel * ctrName = new TextLabel{ctrNamePos, ctrNameSize, brown, AM::FontSize::PX16, true, textPos};
        grp->tLabels.push_back(ctrName);
        ctrName->setText("Poland");
        //provName->evName = ClickEventType::OPEN_PROV;
        ctrName->id = 1;
    }
    
    glm::vec2 iconTabGrpSize{wSize.x - 2.0f * offset.x, 40.0f + offset.y * 2.0f};
    glm::vec3 iconTabGrpPos{wPos.x + offset.x, ctrNamePos.y - offset.y - iconTabGrpSize.y, 0.1f};
    { // tabs icons
        Group * grp = new Group{iconTabGrpPos, iconTabGrpSize, brown, false};
        w->groups.push_back(grp);
        grp->id = 200;

        glm::vec2 iconSize{40.0f, 40.0f};
        glm::vec3 iconPos{iconTabGrpPos.x + offset.x, iconTabGrpPos.y + offset.y, 0.12f};

        IconLabel * icon = new IconLabel{iconPos, iconSize};
        grp->iLabels.push_back(icon);
        icon->setIcon("src/img/gold.png");
        icon->id = 1;
        icon->evName = ClickEventType::PROV_SWITCH_TAB; 

        iconPos.x += offset.x + iconSize.x;
        IconLabel * icon2 = new IconLabel{iconPos, iconSize};
        grp->iLabels.push_back(icon2);
        icon2->setIcon("src/img/gold.png");
        icon2->id = 2;
        icon2->evName = ClickEventType::PROV_SWITCH_TAB; 
   }

    glm::vec2 tabGrpSize{wSize.x - offset.x * 2.0f, iconTabGrpPos.y - wPos.y - 2 * offset.y};
    glm::vec3 tabGrpPos{wPos.x + offset.x, wPos.y + offset.y, 0.1f};

    { // tab grp I.
        Group * tab1 = new Group{tabGrpPos, tabGrpSize, darkBrown, false};
        tab1->id = 1;
        w->groups.push_back(tab1);

        { // ctr properties group
            glm::vec2 groupSize{tabGrpSize.x * 0.57f, 100.0f};
            glm::vec3 groupPos{tabGrpPos.x, tabGrpPos.y + tabGrpSize.y - offset.y - groupSize.y, 0.11f};
            Group * grp = new Group{groupPos, groupSize, brown, false};
            tab1->groups.push_back(grp);
           
            glm::vec2 propGrpSize{groupSize.x * 0.65f - offset.x * 2.0f, groupSize.y};
            glm::vec3 propGrpPos{groupPos.x + offset.x, groupPos.y + groupSize.y - offset.y - propGrpSize.y, 0.12f};
            Group * propGrp = new Group{propGrpPos, propGrpSize, brown, false};
            grp->groups.push_back(propGrp);
            
            glm::vec2 valGrpSize = glm::vec2{groupSize.x - propGrpSize.x - offset.x, propGrpSize.y};//{groupSize.x * 0.5f, groupSize.y};
            glm::vec3 valGrpPos{groupPos.x + propGrpSize.x, groupPos.y + groupSize.y - offset.y - valGrpSize.y, 0.12f};
            Group * valGrp = new Group{valGrpPos, valGrpSize, brown, false};
            grp->groups.push_back(valGrp);
            valGrp->id = 11;

            glm::vec2 propNameSize{propGrpSize.x, 25.0f};
            glm::vec3 propNamePos{propGrpPos.x, propGrpPos.y + propGrpSize.y - propNameSize.y - offset.y, 0.13f};
            glm::vec2 valNameSize{valGrpSize.x, 25.0f};
            glm::vec3 valNamePos{valGrpPos.x, valGrpPos.y + valGrpSize.y - valNameSize.y - offset.y, 0.13f};
            { // attitude
                TextLabel * nameLab = new TextLabel{propNamePos, propNameSize, weirdBrown, AM::FontSize::PX16, true, glm::vec3{propNameSize * 0.5f, 0.13f}};
                propGrp->tLabels.push_back(nameLab);
                nameLab->setText("Attitude");
                
                TextLabel * valLab = new TextLabel{valNamePos, valNameSize, weirdBrown, AM::FontSize::PX16, true, glm::vec3{valNameSize * 0.5f, 0.13f}};
                valGrp->tLabels.push_back(valLab);
                valLab->setText("3400");
                valLab->id = 1;
            }

            propNamePos.y -= (propNameSize.y + offset.y);
            valNamePos.y -= (valNameSize.y + offset.y);

            { // manpower2
                TextLabel * nameLab = new TextLabel{propNamePos, propNameSize, weirdBrown, AM::FontSize::PX16, true, glm::vec3{propNameSize * 0.5f, 0.13f}};
                propGrp->tLabels.push_back(nameLab);
                nameLab->setText("Manpower2");
                
                TextLabel * valLab = new TextLabel{valNamePos, valNameSize, weirdBrown, AM::FontSize::PX16, true, glm::vec3{valNameSize * 0.5f, 0.13f}};
                valGrp->tLabels.push_back(valLab);
                valLab->setText("3400");
                valLab->id = 2;
            }
            
        }
    }

    { // tab grp II.
        Group * tab2 = new Group{tabGrpPos, tabGrpSize, darkBrown, false};
        w->groups.push_back(tab2);
        tab2->id = 2;
        tab2->visible = false;
        { // country group
            glm::vec2 groupSize{tabGrpSize.x * 0.4f, 100.0f};
            glm::vec3 groupPos{tabGrpPos.x + tabGrpSize.x - groupSize.x, tabGrpPos.y + tabGrpSize.y - offset.y - groupSize.y, 0.11f};
            Group * grp = new Group{groupPos, groupSize, brown, false};
            tab2->groups.push_back(grp);

            glm::vec2 textLabSize{groupSize.x - 2.0f * offset.x, 20.0f};
            glm::vec3 textLabPos{groupPos.x + offset.y, groupPos.y + groupSize.y - textLabSize.y, 0.12f};
            glm::vec3 textPos{textLabSize.x * 0.5f, textLabSize.y * 0.5f, 0.13f};
            TextLabel * ctrName = new TextLabel{textLabPos, textLabSize, weirdBrown, AM::FontSize::PX16, true, textPos};
            grp->tLabels.push_back(ctrName);
            ctrName->setText("Russia");
        }
    }
    return &w->observer;
}

void Close()
{
    for (auto it = Gui::Base::windows.begin(); it != Gui::Base::windows.end(); ++it) {
        if ((*it)->type == WindowType::MY_COUNTRY) {
            delete *it;
            Gui::Base::windows.erase(it);
            break;
        }
    }
}

void SwitchTab()
{/*
    assert(clickData.window && clickData.group && clickData.val >= 1);
    for (auto g : clickData.window->groups) {
        if (g->id == 1 || g->id == 2) {
            g->visible = false;
        }
        if (g->id == clickData.val) {
            g->visible = true;
        }
    }*/
}

} // Gui::MyCountry








