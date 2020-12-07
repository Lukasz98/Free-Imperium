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
    NONE = 0
};

enum class WindowType {
    TOP_BAR = 0
};

struct TextLabel {
    struct Text {
        enum AM::FontSize fontSize;
        bool centered = false;//, breakLine = false;
        //std::unique_ptr<Rectangle> backgr;
        std::vector<std::unique_ptr<Rectangle>> rects;
        //std::unique_ptr<Texture> texture;

        //std::string text;
        Color textC, bgC;
        //int maxWidth;
        glm::vec3 position, centerTo;
        //glm::vec2 size;
    };
    Text * text = nullptr;
    int id;
    //glm::vec3 position, o;
    
    ClickEventType evName;
    std::unique_ptr<Rectangle> backgr;
    ClickEventType Click(const glm::vec2 & mPos);
    void setText(const std::string & text);
    void Draw();
    ~TextLabel();
};


struct IconLabel {
    struct Icon {
        //int id;
        //std::unique<Rectangle> backgr;
        std::unique_ptr<Texture> texture;
        std::string iconPath;
    };
    Icon * icon = nullptr;
    int id;
    ClickEventType evName;
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
    ClickEventType evType;
};
 


struct Group {
    std::vector<Group*> groups;
    std::vector<TextLabel*> tLabels;
    std::vector<IconLabel*> iLabels;
    int id;
    bool hoverable = false, hovered = false;
    std::unique_ptr<Rectangle> backgr;
    bool Click(ClickData & clickData, const glm::vec2 & mPos);
    bool Hover(const glm::vec2 & mPos);
    void Draw();
    ~Group();
};


struct List {
    std::vector<Group*> groups;
    int id;
    std::unique_ptr<Rectangle> backgr;
    bool Click(ClickData & clickData, const glm::vec2 & mPos);
    void Draw();
    ~List();
};


struct Window
{
    std::vector<Group*> groups;
    WindowType type;
    int id; // for ex. province id
    std::unique_ptr<Rectangle> backgr;
    bool GetClick(ClickData & clickData, glm::vec2 mousPos);
    void Hover(const glm::vec2 & mPos);
    void Draw();
    ~Window();
};

void Hover(const glm::vec2 & mousePos);
void Draw();
void OpenTopBar(const std::vector<std::string> & values);

}

