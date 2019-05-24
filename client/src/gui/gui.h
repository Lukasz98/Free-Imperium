#pragma once

#include <memory>
#include <vector>

#include "window.h"
#include "object.h"

class Gui
{
    glm::vec2 resolution;
    std::vector<std::unique_ptr<GuiWindow>> windows;

    void eraseWins(std::vector<std::size_t> indices);
    void eraseWins(std::string priority);
   
public:
    Gui(glm::vec2 res);
    ~Gui();

    void Reload();
    GuiClick Click(int x, int y);

    void Draw();
    void AddWin(std::string path);
    void Update(const std::unordered_map<std::string,std::string> & values, std::string type);
    void ClearList(const std::string & win, const std::string & listName);
    void Hover(int x, int y);
    void EraseObj(std::string winType, int objId);
    bool Scroll(double offset, double xMouse, double yMouse);
    bool IsOpen(std::string type) const;
    int AddToList(DataObj * newObj, const std::string & win, const std::string & listName);
    void ObservationOfLastWin(Subject * s);
    
    std::unordered_map<std::string,std::string> GetWinValues(std::string type) const;
    std::vector<std::string> GetContentOf(std::string win, std::string valueName) const;
};
