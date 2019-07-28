#include "list.h"

RegularList::RegularList(glm::vec3 parentPos, std::unordered_map<std::string, std::string> & values, std::vector<DataObj*> obs, std::vector<GuiClick> clickPatterns)
    : freeContentId{1}
{
    type = "regularList";
    labelColor = "150 50 0 255";
    textColor = "0 50 150 255";
    contentHeight = 30.0f;
    contentWidth = 0.0f;
    for (auto & value : values) {
        std::stringstream stream;
        stream << value.second;
        std::string val;
        stream >> val;
        if (value.first == "position:") {
            position.x = std::stof(val);
            stream >> val;
            position.y = std::stof(val);
            stream >> val;
            position.z = std::stof(val);
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
        else if (value.first == "frameColor:") {
            frameColor.r = std::stoul(val);
            stream >> val;
            frameColor.g = std::stoul(val);
            stream >> val;
            frameColor.b = std::stoul(val);
            stream >> val;
            frameColor.a = std::stoul(val);
        }
        else if (value.first == "barColor:") {
            barColor.r = std::stoul(val);
            stream >> val;
            barColor.g = std::stoul(val);
            stream >> val;
            barColor.b = std::stoul(val);
            stream >> val;
            barColor.a = std::stoul(val);
        }
        else if (value.first == "labelColor:") {
            labelColor = val;
            stream >> val;
            labelColor += " " + val;
            stream >> val;
            labelColor += " " + val;
            stream >> val;
            labelColor += " " + val;
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
        else if (value.first == "name:") {
            name = value.second;
        }
        else if (value.first == "contentHeight:") {
            contentHeight = std::stof(value.second);
        }
        else if (value.first == "contentWidth:") {
            contentWidth = std::stof(value.second);
        }
        else if (value.first == "visible:") {
            visible = std::stoi(value.second);
        }
    }

    this->clickPatterns = clickPatterns;
    for (auto & cp : this->clickPatterns)
        cp.values["listName:"] = name;

    position += parentPos;
    glm::vec4 bgColVec = { (double)frameColor.r / 255, (double)frameColor.g / 255,
                           (double)frameColor.b / 255, (double)frameColor.a / 255
                         };

    glm::vec2 frameSize = { size.x, contentHeight };

    bottomRect = std::make_unique<Rectangle>(glm::vec3{position.x, position.y, position.z - 0.2f}, frameSize, bgColVec);
    topRect = std::make_unique<Rectangle>(glm::vec3{position.x, position.y + size.y - frameSize.y, position.z - .5f}, frameSize, bgColVec);

    bgColVec = { (double)bgColor.r / 255, (double)bgColor.g / 255,
                 (double)bgColor.b / 255, (double)bgColor.a / 255
               };
    
    glm::vec2 bgSize = {size.x, size.y - 2 * frameSize.y};
    background = std::make_unique<Rectangle>(glm::vec3{position.x, position.y + frameSize.y, position.z}, bgSize, bgColVec);

    offsetX = 5.0f;
    offsetY = 5.0f;
    scrollBarWidth = 15.0f;

    for (auto & ob : obs) {
        if (ob->type == "label") {
            AddLabel(ob);
        }
    }

    speed = 6.0f;
    maxButtons = (int)(((float)(bgSize.y) - offsetY) / (contentHeight + offsetY) + 0.5f);
    
    glm::vec3 scrollPos = background->GetPosition();
    scrollPos.x += background->GetSize().x - offsetX;
    scrollPos.y += offsetY;
    scrollBar = std::make_unique<ScrollBar>(scrollPos, glm::vec2{offsetX*3, bgSize.y - 2 * offsetY}, 0, barColor);
    
    scrollBar->MaxButtons(maxButtons);

    if (contentWidth == 0.0f)
        contentWidth = background->GetSize().x - 2 * offsetX - scrollBarWidth;
}

RegularList::~RegularList()
{

}

int RegularList::AddLabel(DataObj * ob)
{
    if (ob->type != "label")
        return -1;
    
    glm::vec3 contentPos = {0.0, 0, 0};
    if (objects.size()) {
        contentPos = objects.back()->GetRelativePos();
        contentPos.y -= contentHeight + offsetY;
    }
    else {
        contentPos = background->GetPosition();
        contentPos.x = offsetX;
        contentPos.y = background->GetSize().y - contentHeight - offsetY;
    }

    ob->values["position:"] = std::to_string(contentPos.x) + " " + std::to_string(contentPos.y) + " " + std::to_string(contentPos.z);
    ob->values["size:"] = std::to_string(contentWidth) + " " + std::to_string(contentHeight);
    ob->values["contentAlign:"] = "center";
    ob->values["bgColor:"] = labelColor;
    ob->values["textColor:"] = textColor;
    
    objects.emplace_back(std::make_unique<Label>(background->GetPosition(), ob->values, ob->objects, clickPatterns, freeContentId));
    freeContentId++;    

    int scrollCount = 0;
    if (objects.size() > 1)
        if (background->GetPosition().y >= objects.back()->GetPos().y) {
            float howMuchPxDoIScroll = background->GetPosition().y - objects.back()->GetPos().y; 
            scrollCount = (int)(howMuchPxDoIScroll / speed + 0.5f);
            if (scrollCount & 2 != 0)
                scrollCount++;
        }

    return freeContentId - 1;
}

void RegularList::ClearList()
{
    objects.clear();
}

void RegularList::SetClick(GuiClick c)
{
    c.values["listName:"] = name;
    Object::SetClick(c);
}

void RegularList::Draw()
{
    if (background && visible)
        background->Draw();
    for (auto & ob : objects)
        ob->Draw();
    if (topRect && visible)
        topRect->Draw();
    if (bottomRect && visible)
        bottomRect->Draw();
    if (scrollBar && visible)
        scrollBar->Draw();
}

bool RegularList::Hover(int x, int y)
{
    for (auto & o : objects)
        o->Hover(x,y);
    
    if (background)
        return background->Click(x, y);
    return false;
}

bool RegularList::Scroll(int offset, int x, int y)
{
    if (!objects.size())
        return false;
    if (objects.back()->GetPos().y > background->GetPosition().y && offset < 0)
        return false;
    
    glm::vec3 topRectPos = topRect->GetPosition(), bottomRectPos = bottomRect->GetPosition();

    if (objects[0]->GetPos().y < topRectPos.y - offsetY - objects[0]->GetSize().y && offset > 0)
        return false;
    
    for (auto & o : objects) {
        if (o->GetPos().y > topRectPos.y)
            o->SetVisible(false);
        else if (o->GetPos().y + o->GetSize().y < bottomRectPos.y + bottomRect->GetSize().y)
            o->SetVisible(false);
        else
            o->SetVisible(true);
        o->Move(glm::vec3{0.0, -offset, 0});
    }
    
    scrollBar->Scroll(-offset);
    return true;
}

bool RegularList::DeleteObj(int _id)
{
    auto deleteIt = objects.end();
    float deletedHeight = 0.0f;
    for (auto i = objects.begin(); i < objects.end(); i++) {
        if (deleteIt == objects.end() && (*i)->GetId() == _id) {
            deleteIt = i;
            deletedHeight = (*i)->GetSize().y;
            continue;
        }
        else if (deleteIt != objects.end()) {
            (*i)->Move(glm::vec3{0.0f, deletedHeight + offsetY, 0.0f});
        }
    }
    
    bool deleted = (deleteIt == objects.end());
    objects.erase(deleteIt);
    return deleted;
}

GuiClick RegularList::GetClick(int x, int y) const
{
    for (auto & o : objects)
        if (o->IsClicked(x, y)) {
            return o->GetClick(x, y);
        }

    if (IsClicked(x, y) && visible) {
        return guiClick;
    }

    return GuiClick{};
}
