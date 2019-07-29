#pragma once
#include <sstream>

#include "../graphics/texture.h"
#include "../graphics/rectangle.h"
#include "object.h"
#include "label.h"
#include "scroll_bar.h"

class RegularList : public Object
{
    std::unique_ptr<ScrollBar> scrollBar;
    std::unique_ptr<Rectangle> background;
    std::unique_ptr<Rectangle> topRect;
    std::unique_ptr<Rectangle> bottomRect;

    Color bgColor, frameColor, barColor;
    std::string labelColor, textColor;
    float offsetX, offsetY, scrollBarWidth;
    float contentHeight, contentWidth;
    int maxButtons;
    float speed;
    int freeContentId;

    std::string name;
    std::vector<GuiClick> clickPatterns;

public:
    RegularList(const RegularList & t) = delete;
    RegularList(glm::vec3 parentPos, std::unordered_map<std::string, std::string> & values, std::vector<DataObj*> obs, std::vector<GuiClick> clickPatterns);
    ~RegularList();
    
    int AddLabel(DataObj * ob); // returns new obj id
    void ClearList();
    std::string GetName() const { return name; }

    void SetClick(GuiClick c) override;
    void Draw() override;
    bool Hover(int x, int y) override;
    bool Scroll(int offset, int x, int y) override;
    bool DeleteObj(int _id) override;
    GuiClick GetClick(int x, int y) const override;
    
};

