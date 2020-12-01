#include "label.h"

Label::Label(glm::vec3 parentPos, const std::unordered_map<std::string, std::string> & values, const std::vector<DataObj*> & obs, const std::vector<GuiClick> & clickPatternss, int id)
    : Object{id}, bgColor{200, 200, 200, 255}, clickPatterns{clickPatternss}
{
    std::string textColor = "";
    type = "label";
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
        else if (value.first == "bgColor:") {
            bgColor.r = std::stoul(val);
            stream >> val;
            bgColor.g = std::stoul(val);
            stream >> val;
            bgColor.b = std::stoul(val);
            stream >> val;
            bgColor.a = std::stoul(val);
        }
        else if (value.first == "textColor:") {
            textColor = val;
            stream >> val;
            textColor += " " + val;
            stream >> val;
            textColor += " " + val;
            stream >> val;
            textColor += " " + val;
        }
        else if (value.first == "contentAlign:") {
            contentAlign = value.second;
        }
        else if (value.first == "hover:") {
            if (value.second == "true") {
                hover = true;
                visible = false;
            }
            else
                hover = false;
        }
        else {
            this->values[value.first] = value.second;
        }
    }

    position = relativePos + parentPos;
    glm::vec4 bgColVec = { (double)bgColor.r / 255, (double)bgColor.g / 255,
                           (double)bgColor.b / 255, (double)bgColor.a / 255
                         };
    background = std::make_unique<Rectangle>(position, size, bgColVec);

    for (auto & ob : obs) {
        bool newContent = false;
        if (ob->type == "text") {
            if (textColor.size())
                ob->values["color:"] = textColor;
            objects.emplace_back(std::make_unique<Text>(position, ob->values, size.x, this->GetId()));
            newContent = true;
        }
        else if (ob->type == "icon") {
            objects.emplace_back(std::make_unique<Icon>(position, ob->values, this->GetId()));
            newContent = true;
        }
        else if (ob->type == "label") {
            objects.emplace_back(std::make_unique<Label>(position, ob->values, ob->objects, this->clickPatterns, this->GetId()));
            newContent = true;
        }
        if (newContent) {
            for (auto & cp : this->clickPatterns) {
                if (objects.size() && objects.back()->GetClickType() == cp.GetEventName()) {
                    cp.parentId = this->GetId();
                    objects.back()->SetClick(cp);
                    break;
                }
            }
        }
        if (objects.size() && newContent && contentAlign == "center") {
            glm::vec3 center = { position.x + size.x / 2, position.y + size.y / 2, position.z };
            objects.back()->MakeCentered(center);
        }
    }    
}

Label::~Label()
{

}

void Label::SetPos(glm::vec3 p)
{
    position = relativePos + p;
    if (background) {
        background->SetPosition(position);
	
    }
    for (auto & ob : objects) {
        if (contentAlign == "center") {
	        //glm::vec3 center = { position.x + size.x / 2, position.y + size.y / 2, 0.0};
	        //glm::vec3 center = { position.x + size.x / 2, position.y + size.y / 2, 0.0};//position.z + 15.1};
	        glm::vec3 center = { position.x + size.x / 2, position.y + size.y / 2, position.z};
            //ob->SetPos(position);
            ob->MakeCentered(center);
        }
        else 
            ob->SetPos(position);
    }
}

void Label::Draw()
{
    if (!visible)
        return;

    if (background)
        background->Draw();
    for (auto & ob : objects)
        ob->Draw();
}    

void Label::Move(glm::vec3 p)
{
    position += p;
    if (background)
        background->SetPosition(position);
    for (auto & o : objects) {
        o->Move(p);
    }
}
    

std::vector<std::unordered_map<std::string,std::string>> Label::GetContentsValues() {
    
    std::vector<std::unordered_map<std::string,std::string>> vals; 

    for (auto & o : objects) {
        if (o->GetType() != "label") {
            vals.push_back(o->GetValues());
        }
        else {
            auto vec = ((Label*)o.get())->GetContentsValues();
            vals.insert(vals.end(), vec.begin(), vec.end());
        }
    }

    return vals;
}
