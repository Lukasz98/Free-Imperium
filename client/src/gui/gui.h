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


namespace Gui {

enum class ClickEventType {
    NONE = 0,
    TEST = 1
};

enum class WindowType {
    TOP_BAR = 0,
    UNITS_LIST = 1
};

struct TextLabel {
    struct Text {
        enum AM::FontSize fontSize;
        bool centered = false;
        std::vector<std::unique_ptr<Rectangle>> rects;

        std::string content;
        Color textC, bgC;
        glm::vec3 position, centerTo;
    };
    Text * text = nullptr;
    int id;
    
    ClickEventType evName = ClickEventType::NONE;
    std::unique_ptr<Rectangle> backgr;
    ClickEventType Click(const glm::vec2 & mPos);
    void setText(const std::string & text);
    void Draw();
    ~TextLabel();
};


struct IconLabel {
    struct Icon {
        std::unique_ptr<Texture> texture;
        std::string iconPath;
    };
    Icon * icon = nullptr;
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
    ~Group();
};


struct List {
    std::vector<Group*> groups;

    int id;
    std::unique_ptr<Rectangle> backgr, topRect, bottRect;
    bool Click(ClickData & clickData, const glm::vec2 & mPos);
    void Scroll(int y); // y = -1 : +1
    void Draw();
    ~List();
};


struct Window
{
    std::vector<Group*> groups;
    std::vector<List*> lists;
    WindowType type;
    int id; // for ex. province id
    std::unique_ptr<Rectangle> backgr;
    bool GetClick(ClickData & clickData, glm::vec2 mousPos);
    void Hover(const glm::vec2 & mPos);
    void Draw();
    ~Window();
};

ClickData Click(const glm::vec2 & mousePos);
void Hover(const glm::vec2 & mousePos);
void Scroll(int y);
void Draw();

void OpenTopBar(const std::vector<std::string> & values, const glm::vec2 & resolution);
void OpenUnitsList(); 
}

