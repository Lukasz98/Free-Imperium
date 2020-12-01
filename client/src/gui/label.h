#pragma once
#include <fstream>
#include <sstream>

#include "../graphics/texture.h"
#include "../graphics/rectangle.h"
#include "object.h"
#include "text.h"
#include "icon.h"

class Label : public Object
{
    std::unique_ptr<Rectangle> background;
    Color bgColor;
    std::vector<GuiClick> clickPatterns;
    std::string contentAlign = "";
    
public:
    Label(const Label & t) = delete;
    Label(glm::vec3 parentPos, const std::unordered_map<std::string, std::string> & values, const std::vector<DataObj*> & obs, const std::vector<GuiClick> & clickPatternss, int id = 0);
    ~Label();
    
    void SetPos(glm::vec3 p) override;
    void Draw() override;
    void Move(glm::vec3 p) override;


    std::vector<std::unordered_map<std::string,std::string>> GetContentsValues();
    
};


