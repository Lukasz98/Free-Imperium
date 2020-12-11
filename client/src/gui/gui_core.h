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

namespace Gui::Core {

enum class WindowType {
    MENU_SCREEN = 0,
    ROOM_SCREEN = 1,
    TOP_BAR = 3,
    UNITS_LIST = 4
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
    int id;
    
    //glm::vec3 relPos; // relative to Group
    ClickEventType evName = ClickEventType::NONE;
    std::unique_ptr<Rectangle> backgr;
    ClickEventType Click(const glm::vec2 & mPos);
    void setText(const std::string & text);
    void Draw();
    //TextLabel(const glm::vec3 & relPos, const glm::vec2 & size, enum AM::FontSize fsize);
    ~TextLabel();
};


struct IconLabel {
    struct Icon {
        std::unique_ptr<Texture> texture;
        std::string iconPath;
    };
    Icon icon;
    int id;
    ClickEventType evName = ClickEventType::NONE;
    std::unique_ptr<Rectangle> backgr;
    glm::vec3 pos;
    glm::vec2 size;

    void Draw();
    ClickEventType Click(const glm::vec2 & mPos);
    void setIcon(const std::string & path);
    ~IconLabel();
};

struct Window;
struct Group;

struct ClickData {
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
    int id;
    bool hoverable = false, hovered = false, visible = true; // visible used only for groups in List
    std::unique_ptr<Rectangle> backgr;
    bool Click(ClickData & clickData, const glm::vec2 & mPos);
    bool Hover(const glm::vec2 & mPos);
    void Scroll(int amount); // used by List
    void Draw();
    void SetPos(const glm::vec3 & newPos);
    ~Group();
};


struct List {
    std::vector<Group*> groups;

    int id = 0, freeGrpId = 0;
    float groupsOffset = 0.0f;
    //lastItemY = 0.0f;
    std::unique_ptr<Rectangle> backgr, topRect, bottRect;
    std::unique_ptr<Rectangle> scroll;
    bool scrollVisible = false;
    float scrollBarSpeed = 0.0f, scrollSpeed = 30.0f;
    bool Click(ClickData & clickData, const glm::vec2 & mPos);
    void Scroll(int y); // y = -1 : +1
    bool Hover(const glm::vec2 & mPos);
    
    void Draw();
    void AddGroup(Group * g); // group has to be preset, but its position will be changed
    void DeleteGroup(int gid);
    void SetPos(const glm::vec3 & newPos);
    List(const glm::vec3 & pos, const glm::vec2 & size, float relYOfContent, const glm::vec4 & bgColor, const glm::vec4 & barColor, float groupsOffset);
    ~List();
};


struct Window
{
    std::vector<Group*> groups;
    std::vector<List*> lists;
    WindowType type;
    int id; // for ex. province id
    bool dragable = false, dragging = false;
    std::unique_ptr<Rectangle> backgr;
//    glm::vec3 defaultPos;
    ClickEventType GetClick(ClickData & clickData, glm::vec2 mousPos);
    void Hover(const glm::vec2 & mPos);
    void Drag(const glm::vec2 & mPos, float dt);
    void Draw();
    ~Window();
};

//void OpenUnitsList(); 
//void DelFromUnitsList(ClickData & clickData);





}
