#pragma once
#include <string>
#include <memory>
#include <vector>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include "../graphics/rectangle.h"
#include "../graphics/texture.h"
#include "../color.h"
#include "../asset_manager.h"
#include "click_event_enum.h"
#include "../observer.h"

namespace Gui::Core {

enum class WindowType {
    MENU_SCREEN = 0,
    ROOM_SCREEN = 1,
    TOP_BAR = 3,
    UNITS_LIST = 4,
    PROV = 5,
    PROV_SIEGE = 6,
    UNIT = 7,
    COUNTRY = 8,
    MY_COUNTRY = 9,
    OFFER_PEACE,
    SIDE_BAR,
    WAR
};

struct TextLabel {
    struct Text {
        enum AM::FontSize fontSize;
        bool centered = false;
        std::vector<std::unique_ptr<Rectangle>> rects;

        std::string content;
        Color textC, bgC;
        glm::vec3 relPos, relCenterTo; // relative to TextLabel
    };
    Text text;
    int id = -1, hiddenValue = -1;
    
    ClickEventType evName = ClickEventType::NONE;
    std::unique_ptr<Rectangle> backgr;
    bool Click(const glm::vec2 & mPos);
    void setText(const std::string & text);
    void Draw();
    TextLabel(const glm::vec3 & pos, const glm::vec2 & size, const glm::vec4 & bgColor, enum AM::FontSize fsize, bool centered, const glm::vec3 & textPos);
    ~TextLabel();
};


struct IconLabel {
    struct Icon {
        std::unique_ptr<Texture> texture;
        std::string iconPath;
    };
    Icon icon;
    int id = -1;
    ClickEventType evName = ClickEventType::NONE;
    std::unique_ptr<Rectangle> backgr;

    void Draw();
    bool Click(const glm::vec2 & mPos);
    void setIcon(const std::string & path);
    IconLabel(const glm::vec3 & pos, const glm::vec2 & size);
    ~IconLabel();
};

struct Window;
struct Group;

struct ClickData {
    int val = -1, hiddenValue = -1;
    Window * window = nullptr;
    Group * group = nullptr;
    ClickEventType evType = ClickEventType::NONE;
    ClickData(){ Log("ClickData()");}
    ~ClickData(){ Log("~ClickData()");}
};
 


struct Group {
    std::vector<Group*> groups;
    std::vector<TextLabel*> tLabels;
    std::vector<IconLabel*> iLabels;
    int id = -1, hiddenVal = -1;
    bool hoverable = false, hovered = false, visible = true; // visible used only for groups in List
    std::unique_ptr<Rectangle> backgr;
    bool Click(ClickData & clickData, const glm::vec2 & mPos);
    bool Hover(const glm::vec2 & mPos);
    void Scroll(int amount); // used by List
    void Draw();
    void SetPos(const glm::vec3 & newPos);
    
    Group(const glm::vec3 & pos, const glm::vec2 & size, const glm::vec4 & bgCol, bool hoverable);
    ~Group();
};


struct List {
    std::vector<Group*> groups;

    int id = -1, freeGrpId = 0;
    float groupsOffset = 0.0f;
    //lastItemY = 0.0f;
    std::unique_ptr<Rectangle> backgr, topRect, bottRect;
    std::unique_ptr<Rectangle> scroll;
    std::unique_ptr<TextLabel> titleLabel;
    bool scrollVisible = false;
    float scrollBarSpeed = 0.0f, scrollSpeed = 30.0f;
    bool Click(ClickData & clickData, const glm::vec2 & mPos);
    void Scroll(int y); // y = -1 : +1
    bool Hover(const glm::vec2 & mPos);
    
    void Draw();
    void AddGroup(Group * g); // group has to be preset, but its position will be changed
    void DeleteGroup(int gid, int hiddenVal = -1);
    void SetPos(const glm::vec3 & newPos);
    void SetTitle(const std::string & text, AM::FontSize fsize); 
    List(const glm::vec3 & pos, const glm::vec2 & size, float relYOfContent, const glm::vec4 & bgColor, const glm::vec4 & barColor, float groupsOffset);
    ~List();
};


struct Window
{
    Observer observer;
    std::vector<Group*> groups;
    std::vector<List*> lists;
    WindowType type;
    int id = -1; // for ex. province id
    bool dragable = false, dragging = false;
    std::unique_ptr<Rectangle> backgr;
//    glm::vec3 defaultPos;
    ClickEventType GetClick(ClickData & clickData, glm::vec2 mousPos);
    void Hover(const glm::vec2 & mPos);
    void Drag(const glm::vec2 & mPos, float dt);
    void Draw();
    void UpdateValues();
    Window(const glm::vec3 & pos, const glm::vec2 & size, const glm::vec4 & bgCol, bool dragable, WindowType wType);
    ~Window();
};

//void OpenUnitsList(); 
//void DelFromUnitsList(ClickData & clickData);





}
