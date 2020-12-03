#include "window.h"

GuiWindow::GuiWindow(std::string path, glm::vec2 resolution)
    : resolution(resolution), path{path}
{
    loadClicks();
    std::fstream file{path};
    if (file.is_open()) {
        DataLoader loader;
        DataObj * win = loader.LoadDataObj(file, "window");
        file.close();

        glm::vec4 bgColor{0.1, 0, 0 , 0};
        for (auto & value : win->values) {
            std::stringstream stream;
            stream << value.second;
            std::string val;
            stream >> val;
            if (value.first == "type:") {
                type = val;
            }
            else if (value.first == "position:") {
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
                bgColor.x = std::stof(val) / 255;
                stream >> val;
                bgColor.y = std::stof(val) / 255;
                stream >> val;
                bgColor.z = std::stof(val) / 255;
                stream >> val;
                bgColor.w = std::stof(val) / 255;
            }
            else if (value.first == "priority:") {
                priority = val;
            }
            else if (value.first == "clickless:") {
                clickless = std::stoi(value.second);
            }
        }
        
        if (type == "topBar") {
            position.y = resolution.y - size.y;
            size.x = resolution.x;
        }
        else if (type == "notifications") {
            position.x = resolution.x - size.x - 10;
        }
        else if (type == "war") {
            position.x = (resolution.x / 2) - (size.x / 2);
        }

        background = std::make_unique<Rectangle>(position, size, bgColor);

        for (auto & ob : win->objects) {
            if (ob->type == "label") {
                objects.emplace_back(std::make_unique<Label>(position, ob->values, ob->objects, clickPatterns));
            }
            else if (ob->type == "regularList") {
                objects.emplace_back(std::make_unique<RegularList>(position, ob->values, ob->objects, clickPatterns));
            }
        }
       
        delete win;
    }
}

GuiWindow::~GuiWindow()
{
    
}

int GuiWindow::UpdateList(DataObj * newObj, const std::string & type)
{
    for (auto & o : objects) {
        if (o->GetType() == "regularList" && ((RegularList*)o.get())->GetName() == type) {
           return ((RegularList*)o.get())->AddLabel(newObj);
        }
    }
    return -1;
}

void GuiWindow::ClearList(const std::string & listName)
{
    for (auto & o : objects) {
        if (o->GetType() == "regularList") {
            if (((RegularList*)o.get())->GetName() == listName) {
                ((RegularList*)o.get())->ClearList();
                break;
            }
        }
    }
}

void GuiWindow::loadClicks()
{
    std::fstream file{"src/gui/gui_event.txt"};
    if (!file.is_open())
        return;
    
    DataLoader loader;
    DataObj * events = loader.LoadDataObj(file, "events");

    for (auto & o : events->objects) {
        GuiClick gc;
        gc.values = o->values;
        clickPatterns.emplace_back(gc);
    }
    
    delete events;
}

void GuiWindow::Update(const std::unordered_map<std::string, std::string> & values)
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

void GuiWindow::Draw()
{
    if (background)
        background->Draw();
    for (auto & ob : objects)
        ob->Draw();
}

bool GuiWindow::Scroll(int offset, int x, int y)
{
    for (auto & o : objects) {
        auto scroll = o->Scroll(offset, x, y);
        if (scroll)
            return true;
    }
    return false;
}

GuiClick GuiWindow::GetClick(int x, int y) const
{
    GuiClick click{};
    for (auto & o : objects) {
        if (!o->IsClicked(x, y))
            continue;
        click = o->GetClick(x, y);
        if (click.values.size())
            click.values["windowType:"] = type;
        break;
    }
    for (auto & value : click.values) {
        if (value.second.size() != 0)
            continue;

        // try to find value with correct id
        for (auto & o : objects) {
            if (!o->IsHoldingValue(value.first, click.parentId))
                continue;
            value.second = o->GetContent(value.first, click.parentId)[0]; // not checking size because of IsHoldingValue()
        
            auto nodeHandle = click.values.extract(value.first);
            nodeHandle.key() += ":";
            click.values.insert(std::move(nodeHandle));
            break;
        }
        // if failure, try to find value with default id
        if (value.second.size() == 0) {
            for (auto & o : objects) {
                if (!o->IsHoldingValue(value.first, 0))
                    continue;
                value.second = o->GetContent(value.first, 0)[0]; // not checking size because of IsHoldingValue()

                auto nodeHandle = click.values.extract(value.first);
                nodeHandle.key() += ":";
                click.values.insert(std::move(nodeHandle));
                break;
            }
        }
    }

    if (click.values.size() == 0 && !clickless) {
        if (this->IsClicked(x, y)) {
            click.values["type:"] = "nothing";
        }
    }

    #if 0 
    Log("\n\n\nClickValues");
    for (auto & v : click.values)
        Log(v.first << " " << v.second);
    Log("\n\n");
    #endif
    
    return click;
}

std::vector<std::unordered_map<std::string,std::string>> GuiWindow::GetValuesOfListContents(const std::string & listName) {
    for (auto & o : objects) {
        if (o->GetType() == "regularList") {
            if (((RegularList*)o.get())->GetName() == listName) {
                return ((RegularList*)o.get())->GetContentsValues();
                break;
            }
        }
    }
    return std::vector<std::unordered_map<std::string,std::string>>{}; 
}

void GuiWindow::Notification(std::any n)
{
    //try {
    auto nn = std::any_cast<std::unordered_map<std::string,std::string>>(n);
    //}
    //catch(const std::bad_any_cast & e) {
    //    Log(e.what());
    //}
    //Log("Notification " << nn.size());
    Update(nn);
}
