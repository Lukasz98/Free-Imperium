#pragma once
#include <algorithm>
#include <sstream>

#include "../graphics/texture.h"
#include "../graphics/rectangle.h"
#include "../asset_manager.h"
#include "object.h"

class Text : public Object
{
    std::unique_ptr<Rectangle> background;
    std::unique_ptr<Rectangle> content;
    Color color, bgColor;

    float height, maxWidth;
    std::string text, valueName;

    bool centered;
    glm::vec3 centerTo;
    
    void set();
    
public:
    Text(const Text & t) = delete;
    Text(glm::vec3 parentPos, std::unordered_map<std::string, std::string> & values, float maxWidth = 10000.0f, int id = 0);
    ~Text() override;
    
    void SetPos(glm::vec3 p) override;
    void Draw() override;
    void SetClick(GuiClick c) override; 
    
    void SetContent(const std::string & ct) override;
    void Move(glm::vec3 p) override;
    void MakeCentered(glm::vec3 newPos) override;

    std::string GetValueName() const override { return valueName; }
    std::string GetContent() const override { return text; }

};

