#include "gui.h"


Gui::Gui(glm::vec2 res)
    : resolution(res)
{

}

Gui::~Gui()
{

}

void Gui::eraseWins(std::string priority)
{
    bool erased = false;
    do {
        erased = false;
        for (std::size_t i = 0; i < windows.size(); i++)
            if (windows[i]->GetPriority() == priority) {
                windows.erase(windows.begin() + i);
                erased = true;
                break;
            }
    } while (erased);
}

void Gui::eraseWins(std::vector<std::size_t> indices)
{  
    std::sort(indices.rbegin(), indices.rend());
    for (auto i : indices) {
        Log("Eraase win: " << i);
        windows.erase(windows.begin() + i);
    }
}

void Gui::Reload()
{
    windows.clear();
}

GuiClick Gui::Click(int x, int y)
{
    std::vector<std::size_t> winsToErase;
    for (std::size_t i = 0; i < windows.size(); i++) {
        auto click = windows[i]->GetClick(x, y);
        
        if (click.values.size()) {
            if (click.values["name:"] == "close") {
                winsToErase.push_back(i);
                break;
            }
            return click;
        }
    }
    eraseWins(winsToErase);
    return GuiClick{};
}

void Gui::Draw()
{
    for (auto & w : windows) {
        w->Draw();
    }
}

void Gui::AddWin(std::string path)
{
    eraseWins("low");
    for (auto it = windows.begin(); it < windows.end(); it++)
        if ((*it)->GetPath() == path)
            windows.erase(it);
    windows.emplace_back(std::make_unique<GuiWindow>(path, resolution));
}

void Gui::Update(const std::unordered_map<std::string,std::string> & values, std::string type)
{
    for (auto & w : windows)
        if (w->GetType() == type)
            w->Update(values);
}

void Gui::ClearList(const std::string & win, const std::string & listName)
{
    for (auto & w : windows)
        if (w->GetType() == win)
            w->ClearList(listName);
}

void Gui::Hover(int x, int y)
{
    for (auto & w : windows) {
        if (w->Hover(x, y))
            return;
    }
}

void Gui::EraseObj(const std::string & winType, int objId)
{
    for (auto & w : windows) {
        if (w->GetType() == winType) {
            w->DeleteObj(objId);
        }
    }
}

void Gui::EraseWin(const std::string & winType)
{
    for (auto w = windows.begin(); w < windows.end(); w++) {
        if ((*w)->GetType() == winType) {
            windows.erase(w);
            break;
        }
    }
}

bool Gui::Scroll(double offset, double xMouse, double yMouse)
{    
    for (auto & w : windows) {
        if (w->Hover(xMouse, yMouse)) {
            w->Scroll(offset, xMouse, yMouse);
            return true;
        }
    }    
    return false;
}

bool Gui::IsOpen(std::string type) const
{
    for (auto & w : windows) {
        if (w->GetType() == type)
            return true;
    }
    return false;
}

int Gui::AddToList(DataObj * newObj, const std::string & win, const std::string & listName)
{
    for (auto & w : windows)
        if (w->GetType() == win)
            return w->UpdateList(newObj, listName);
    return -1;
}

void Gui::ObservationOfLastWin(Subject * s)
{
    if (windows.size()) {
        windows.back()->SetSubject(s);
        s->AddObserver(windows.back().get());
    }
}

std::unordered_map<std::string,std::string> Gui::GetWinValues(std::string type) const
{
    for (auto & w : windows) {
        if (w->GetType() == type)
            return w->GetValues();
    }
    std::unordered_map<std::string,std::string> ddddd;
    return ddddd;
}

std::vector<std::string> Gui::GetContentOf(std::string win, std::string valueName) const
{
    std::vector<std::string> content;
    for (auto & w : windows) {
        if (w->GetType() == win) {
            content = w->GetContent(valueName, 0);
            break;
        }
    }
    return content;
}
