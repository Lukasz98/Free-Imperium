#include "object.h"

Object::Object(int id)
    : relativePos{0.0, 0.0, 0.1}, size{0.0, 0.0}, visible{true}, id{id}, hover{false}, position{0.0, 0.0, 0.1}, clickless{false}

{
}

Object::~Object()
{
    //Log("~Object()-" << type);
}

void Object::Update(const std::unordered_map<std::string, std::string> & values)
{
    this->values = values;
    for (auto & o : objects) {
        if (o->GetType() == "text") {
            auto valIt = values.find(o->GetValueName());
            if (valIt != values.end())
                o->SetContent(valIt->second);
        }
        else
            o->Update(values);
    }    
}

void Object::SetClick(GuiClick c)
{
    guiClick = c;
    int counter = 0;
    while (counter < guiClick.values.size()) {
        counter++;    
        for (auto & val : guiClick.values) {
        /*
            if (val.first != "name:" && val.first != "listName:" && val.second.size() != 0) {                
                // eg. in gui_event.txt we have values like value1: ' provName '
                // it says that this event wants to know provName
                // here we switch above to provName: ' '
                // leter ' ' are filled with value
                std::string valName = val.first;
                std::string valSec = val.second;
                auto nodeHandle = guiClick.values.extract(val.first);
                nodeHandle.key() = valSec;
                guiClick.values.insert(std::move(nodeHandle));
                    Log(val.second << " ]]]]]]]" << val.first);
                val.second = "";

                // here we check if we dont know the value already
                auto valuesIt = this->values.find(val.first);
                if (valuesIt != this->values.end()) {
                    val.second = valuesIt->second;
                    Log(val.second << " ][][][][]" << val.first);
                }
                break;
            }
            */
                 auto valuesIt = this->values.find(val.first);
                if (valuesIt != this->values.end()) {
                    val.second = valuesIt->second;
                    Log(val.second << " ][][][][]" << val.first);
                }
        }
        break;
    }
    guiClick.values["id:"] = std::to_string(id);
}

void Object::SetVisible(bool v)
{
    visible = v;
    for (auto & o : objects)
        o->SetVisible(v);
}

bool Object::Hover(int x, int y)
{
    if (this->IsClicked(x, y) && !clickless) {
        for (auto & o : objects)
            o->Hover(x, y);
        if (hover) {
            this->SetVisible(true);
        }
        return true;
        
    }
    else {
        for (auto & o : objects)
            o->Hover(x, y);
        if (hover)
            this->SetVisible(false);
        return false;
    }
    
    return false;
}

bool Object::DeleteObj(int _id)
{
    for (int i = 0; i < objects.size(); i++) {
        if (objects[i]->GetId() == _id) {
            objects.erase(objects.begin() + i);
            return true;
        }
    }
    for (auto & o : objects) {
        if (o->DeleteObj(_id))
            return true;
    }
    return false;
}

bool Object::IsHoldingValue(const std::string & valueName, int objId) const
{
    if (this->GetValueName() == valueName && (this->GetId() == objId || type == "regularList"))
        return true;
    for (auto & o : objects) {
        if (o->IsHoldingValue(valueName, objId))
            return true;
    }
    return false;
}

bool Object::IsClicked(int x, int y) const
{ 
    return (x > position.x && x < position.x + size.x && y > position.y && y < position.y + size.y);
}    

GuiClick Object::GetClick(int x, int y) const
{
    for (auto & o : objects)
        if (o->IsClicked(x, y)) {
            auto click = o->GetClick(x, y);
            if (click.values.size())
                return click; // should check pos.z of object
        }

    if (IsClicked(x, y) && visible) {
        return guiClick;
    }

    return GuiClick{};
}

std::vector<std::string> Object::GetContent(const std::string & valueName, int objId) const
{
    if (this->GetValueName() == valueName && (this->GetId() == objId || objId == 0  || type == "regularList"))
        return std::vector<std::string>{this->GetContent()};

    std::vector<std::string> result;
    for (auto & o : objects)
        if (o->GetContent(valueName, objId).size()) {
            auto vv = o->GetContent(valueName, objId);
            for (auto & v : vv)
                result.emplace_back(v);
        }
    return result;
}    
