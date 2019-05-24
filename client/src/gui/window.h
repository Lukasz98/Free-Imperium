#pragma once
#include <fstream>
#include <string>
#include <sstream>

#include "../observer.h"
#include "object.h"
#include "text.h"
#include "label.h"
#include "list.h"

class GuiWindow : public Object, public Observer
{
    glm::vec2 resolution;
    std::string priority, path;
    std::vector<GuiClick> clickPatterns;
    void loadClicks();
    
public:
    GuiWindow(std::string path, glm::vec2 resolution);
    ~GuiWindow();

    int UpdateList(DataObj * newObj, const std::string & type);
    void ClearList(const std::string & listName);
    std::string GetPriority() const { return priority; }
    std::string GetPath() const { return path; }

    void Update(const std::unordered_map<std::string, std::string> & values) override;
    void Draw() override;
    bool Scroll(int offset, int x, int y) override;
    GuiClick GetClick(int x, int y) const override;

    void Notification(std::any n) override;
};
