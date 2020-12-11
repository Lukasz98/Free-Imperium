#include "gui_bridge.h"
#include "gui_core.h"

using namespace Gui::Core;

static ClickData clickData;

namespace Gui::Base {
std::vector<Gui::Core::Window*> windows;
bool dragging = false;

ClickEventType Click(const glm::vec2 & mousePos)
{
    ClickEventType clickType = ClickEventType::NONE;
    if (dragging) {
        dragging = false;
        for (auto w : Base::windows)
            w->dragging = false;
    }
    for (auto w : Base::windows) {
        clickType = w->GetClick(clickData, mousePos);
        if (clickType != ClickEventType::NONE) {
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

} // Gui::Base




namespace Gui::Menu {

void Open(const glm::vec2 & resolution)
{
    Window * w = new Window{};
    Gui::Base::windows.push_back(w);
    w->type = WindowType::MENU_SCREEN;
    w->backgr = std::make_unique<Rectangle>(glm::vec3{0.0f, 0.0f, 0.0f}, resolution, glm::vec4{0.2f, 0.2f, 0.2f, 1.0f});
    
    Group * g = new Group{};
    w->groups.push_back(g);
    g->backgr = std::make_unique<Rectangle>(glm::vec3{0.0f, 0.0f, 0.0f}, resolution, glm::vec4{0.2f, 0.2f, 0.2f, 0.0f});

    TextLabel * titleLabel = new TextLabel{};
    g->tLabels.push_back(titleLabel);
    titleLabel->backgr = std::make_unique<Rectangle>(glm::vec3{resolution.x * 0.5f, resolution.y * 0.5f, 0.1f}, glm::vec2{500.0f, 200.0f}, glm::vec4{0.5f, 0.5f, 0.5f, 1.0f});
    titleLabel->text.centered = true;
    titleLabel->text.relCenterTo = glm::vec3{250.0f, 100.0f, 0.11f};
    titleLabel->text.fontSize = AM::FontSize::PX64;
    titleLabel->setText("Free Imperium");


    float labelsOffset = 30.0f;
    glm::vec2 labelSize{450.f, 100.0f};
    glm::vec3 labelPos{100.0f, 450.0f, 0.1f};
   
    TextLabel * playLabel = new TextLabel{};
    g->tLabels.push_back(playLabel);
    playLabel->backgr = std::make_unique<Rectangle>(labelPos, labelSize, glm::vec4{0.5f, 0.5f, 0.5f, 1.0f});
    playLabel->evName = ClickEventType::START_ROOM;
    playLabel->text.centered = true;
    playLabel->text.relCenterTo = glm::vec3{labelSize.x * 0.5f, labelSize.y * 0.5f, 0.11f};
    playLabel->text.fontSize = AM::FontSize::PX64;
    playLabel->setText("Play");

    labelPos.y -= labelsOffset + labelSize.y;

    TextLabel * quitLabel = new TextLabel{};
    g->tLabels.push_back(quitLabel);
    quitLabel->backgr = std::make_unique<Rectangle>(labelPos, labelSize, glm::vec4{0.0f, 0.5f, 0.5f, 1.0f});
    quitLabel->evName = ClickEventType::QUIT_GAME;
    quitLabel->text.centered = true;
    quitLabel->text.relCenterTo = glm::vec3{labelSize.x * 0.5f, labelSize.y * 0.5f, 0.11f};
    //quitLabel->text->relPos = glm::vec3{181.5f, 10.0f, 0.11f};
    quitLabel->text.fontSize = AM::FontSize::PX16;
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
    Window * w = new Window{};
    Gui::Base::windows.push_back(w);
    w->type = WindowType::ROOM_SCREEN; 
    w->backgr = std::make_unique<Rectangle>(glm::vec3{0.0, 0.0, 0.0}, resolution, glm::vec4{.2, 0.2, .2, 1.0});
   
    float listsXOffset = 100.0f;
    glm::vec3 listPos{100.0f, 50.0f, 0.1f};
    glm::vec2 listSize{500.0f, 600.0f};

    { // bar with titles below lists
        glm::vec3 pos = glm::vec3{listPos.x, listPos.y + listSize.y + 50.0f, 0.0f};
        Group * listTitlesGroup = new Group{};
        w->groups.push_back(listTitlesGroup);
        listTitlesGroup->backgr = std::make_unique<Rectangle>(pos, glm::vec3{listSize.x * 2 + listsXOffset, 100.0f, 0.1f}, 
                                                              glm::vec4{0.0f, 1.0f, 0.0f, 1.0f});
    
        pos.x += 0.0f;
        pos.y += 50.0f;
        pos.z += 0.1f;
        glm::vec2 labelSize{listSize.x,  50.0f};
        TextLabel * playersLabel = new TextLabel{};
        listTitlesGroup->tLabels.push_back(playersLabel);
        playersLabel->backgr = std::make_unique<Rectangle>(pos, labelSize, glm::vec4{.4, 1.0, 0.4, 1.0});
        playersLabel->id = 0;
        playersLabel->text.fontSize = AM::FontSize::PX32;
        playersLabel->text.textC = Color{0, 0, 255, 255};
        playersLabel->text.bgC = Color{0, 255, 255, 255};
        playersLabel->text.centered = true;
        playersLabel->text.relCenterTo = glm::vec3{labelSize * 0.5f, 0.1f};
        playersLabel->setText("Players");

        pos.x += listSize.x + listsXOffset;
        TextLabel * countryLabel = new TextLabel{};
        listTitlesGroup->tLabels.push_back(countryLabel);
        countryLabel->backgr = std::make_unique<Rectangle>(pos, labelSize, glm::vec4{.4, 1.0, 0.4, 1.0});
        countryLabel->id = 0;
        countryLabel->text.fontSize = AM::FontSize::PX32;
        countryLabel->text.textC = Color{0, 0, 255, 255};
        countryLabel->text.bgC = Color{0, 255, 255, 255};
        countryLabel->text.centered = true;
        countryLabel->text.relCenterTo = glm::vec3{labelSize * 0.5f, 0.1f};
        countryLabel->setText("Pick country");
    }

    List * playerList = new List{listPos, listSize, 40.0f, glm::vec4{1.0, 0.5, 0.5, 1.0}, 
                                  glm::vec4{1.0, 1.0, 0.0, 1.0}, 5.0f};
    w->lists.push_back(playerList);
    playerList->id = 0;

    listPos.x += listSize.x + listsXOffset;
    List * countryList = new List{listPos, listSize, 40.0f, glm::vec4{1.0, 0.5, 0.5, 1.0}, 
                                  glm::vec4{1.0, 1.0, 0.0, 1.0}, 5.0f};
    w->lists.push_back(countryList);
    countryList->id = 1;

    { // group with start game button
        glm::vec3 pos = glm::vec3{listPos.x + listSize.x + listsXOffset, listPos.y, 0.0f};
        Group * grp = new Group{};
        w->groups.push_back(grp);
        grp->backgr = std::make_unique<Rectangle>(pos, glm::vec3{200.0f, 100.0f, 0.1f}, 
                                                              glm::vec4{0.0f, 1.0f, 0.0f, 1.0f});
    
        glm::vec2 labelSize{200.0f,  50.0f};
        TextLabel * startLabel = new TextLabel{};
        grp->tLabels.push_back(startLabel);
        startLabel->backgr = std::make_unique<Rectangle>(pos, labelSize, glm::vec4{.4, 1.0, 0.4, 1.0});
        startLabel->id = 0;
        startLabel->text.fontSize = AM::FontSize::PX32;
        startLabel->text.textC = Color{0, 0, 255, 255};
        startLabel->text.bgC = Color{0, 255, 255, 255};
        startLabel->text.centered = true;
        startLabel->text.relCenterTo = glm::vec3{labelSize * 0.5f, 0.1f};
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

    Group * grp = new Group{};
    grp->id = 0;
    grp->backgr = std::make_unique<Rectangle>(glm::vec3{0.0, 0.0, .1}, glm::vec2{500.0, 20.0}, glm::vec4{.0, 1.0, 1.0, 1.0});
    
    TextLabel * title2 = new TextLabel{};
    grp->tLabels.push_back(title2);
    title2->backgr = std::make_unique<Rectangle>(glm::vec3{0.0, 0.0, .11}, glm::vec2{500.0, 20.0}, glm::vec4{.4, 1.0, 0.4, 1.0});
   
    title2->id = 0;
    title2->text.fontSize = AM::FontSize::PX16;
    title2->text.textC = Color{0, 0, 255, 255};
    title2->text.bgC = Color{0, 255, 255, 255};
    title2->text.centered = true;
    title2->text.relCenterTo = glm::vec3{title2->backgr->GetSize() * 0.5f, .2};
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

    Group * grp = new Group{};
    grp->id = 0;
    grp->backgr = std::make_unique<Rectangle>(glm::vec3{0.0, 0.0, .1}, glm::vec2{500.0, 20.0}, glm::vec4{.0, 1.0, 1.0, 1.0});
    
    TextLabel * title2 = new TextLabel{};
    grp->tLabels.push_back(title2);
    title2->backgr = std::make_unique<Rectangle>(glm::vec3{0.0, 0.0, .11}, glm::vec2{500.0, 20.0}, glm::vec4{.4, 1.0, 0.4, 1.0});
   
    title2->id = 0;
    title2->text.fontSize = AM::FontSize::PX16;
    title2->text.textC = Color{0, 0, 255, 255};
    title2->text.bgC = Color{0, 255, 255, 255};
    title2->text.centered = true;
    title2->text.relCenterTo = glm::vec3{title2->backgr->GetSize() * 0.5f, .2};
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

void Open(const std::vector<std::string> & values, const glm::vec2 & resolution)
{
    Window * w = new Window{};
    Gui::Base::windows.push_back(w);
    w->dragable = true;
    w->type = WindowType::TOP_BAR;
    w->id = 0;
    w->backgr = std::make_unique<Rectangle>(glm::vec3{100.0, 100.0, 0.0}, glm::vec2{400.0, 400.0}, glm::vec4{1.0, 0.0, 0.0, .1});

    Group * grp = new Group{};
    grp->id = 0;
    w->groups.push_back(grp);
    grp->backgr = std::make_unique<Rectangle>(glm::vec3{110.0, 310.0, .1}, glm::vec2{400.0, 74.0}, glm::vec4{1.0, 1.0, 0.0, .1});

    TextLabel * title2 = new TextLabel{};
    grp->tLabels.push_back(title2);
    title2->backgr = std::make_unique<Rectangle>(glm::vec3{120.0, 320.0, .2}, glm::vec2{400.0, 74.0}, glm::vec4{.4, 1.0, 0.4, .2});
   
    title2->id = 0;
    title2->text.fontSize = AM::FontSize::PX32;
    title2->text.textC = Color{0, 0, 255, 255};
    title2->text.bgC = Color{0, 255, 255, 255};
    title2->text.relPos = glm::vec3{0.0, 0.0, .1};
    title2->text.centered = true;
    title2->text.relCenterTo = glm::vec3{200.0, 37, .1};
    title2->evName = ClickEventType::TEST;
    title2->setText(values[0] + "j");
 

    Group * hovG = new Group{};
    w->groups.push_back(hovG);
    hovG->hoverable = true;
    hovG->backgr = std::make_unique<Rectangle>(glm::vec3{120.0, 320.0, .1}, glm::vec2{400.0, 74.0}, glm::vec4{1.0, 1.0, 0.0, .2});
    TextLabel * title3 = new TextLabel{};
    hovG->tLabels.push_back(title3);
    title3->backgr = std::make_unique<Rectangle>(glm::vec3{120.0, 320.0 - 74.0, .2}, glm::vec2{400.0, 74.0}, glm::vec4{.4, 1.0, 0.4, .2});
   
    title3->id = 0;
    title3->text.fontSize = AM::FontSize::PX64;
    title3->text.textC = Color{0, 0, 255, 255};
    title3->text.bgC = Color{0, 255, 255, 255};
    title3->text.relPos = glm::vec3{5.0, 5.0, .1};
    //title3->text->centered = true;
    //title3->text->centerTo = glm::vec3{270.0, 150.0 + 17, .3};
    title3->setText("Poljska2");
}

} // Gui::TopBar

