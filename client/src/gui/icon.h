#pragma once
#include <sstream>

#include "../graphics/texture.h"
#include "../graphics/rectangle.h"
#include "object.h"
#include "log.h"

class Icon : public Object
{
    std::unique_ptr<Rectangle> content;
    std::shared_ptr<Texture> texture;    
    std::string valueName, impact, textContent;
    
public:
    Icon(const Icon & t) = delete;
    Icon(glm::vec3 parentPos, std::unordered_map<std::string, std::string> & values, int id = 0);
    ~Icon();
    
    void SetClick(GuiClick c) override;    
    void SetPos(glm::vec3 p) override;
    void Draw() override;
    void Move(glm::vec3 p) override;
    std::string GetValueName() const override { return valueName; }
};

