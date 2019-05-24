#include "icon.h"

Icon::Icon(glm::vec3 parentPos, std::unordered_map<std::string, std::string> & values, int id)
    : Object{id}
{
    type = "icon";
    for (auto & value : values) {
        std::stringstream stream;
        stream << value.second;
        std::string val;
        stream >> val;
        if (value.first == "position:") {
            relativePos.x = std::stof(val);
            stream >> val;
            relativePos.y = std::stof(val);
            stream >> val;
            relativePos.z = std::stof(val);
        }
        else if (value.first == "size:") {
            size.x = std::stof(val);
            stream >> val;
            size.y = std::stof(val);
        }
        else if (value.first == "texturePath:") {
            valueName = value.second;
        }
        else if (value.first == "clickEvent:") {
            clickEvent = value.second;
        }
        else if (value.first == "impact:") {
            impact = value.second;
        }
        else if (value.first == "content:") {
            textContent = value.second;
        }
    }
    position = relativePos + parentPos;

    background = std::make_unique<Rectangle>(position, size);
    texture = std::make_shared<Texture>(valueName);
    background->SetTexture(texture);
}

Icon::~Icon()
{
    //Log("~Icon");
}

void Icon::SetClick(GuiClick c)
{
    Object::SetClick(c);
    guiClick.values["impact:"] = impact;
    guiClick.values["content:"] = textContent;
}

void Icon::SetPos(glm::vec3 p)
{
    position = relativePos + p;
    if (background)
        background->SetPosition(position);
}

void Icon::Draw()
{
    if (!visible)
        return;

    if (background)
        background->Draw();
}

void Icon::Move(glm::vec3 p)
{
    position += p;
    if (background)
        background->SetPosition(position);
}
