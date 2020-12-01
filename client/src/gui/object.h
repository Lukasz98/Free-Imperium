#pragma once
#include <vector>
#include <memory>
#include <unordered_map>

#include "../graphics/rectangle.h"
#include "../graphics/texture.h"
#include "data_object_loader.h"
#include "float_to_string.h"
#include "data_object.h"
#include "gui_click.h"
#include "color.h"
#include "log.h"

class Object
{
private:
    int id;
    
protected:
    glm::vec3 position, relativePos;
    glm::vec2 size;
    std::string type;

    bool visible, hover, clickless;

    GuiClick guiClick;
    std::vector<std::unique_ptr<Object>> objects;
    std::unordered_map<std::string, std::string> values;

    std::unique_ptr<Rectangle> background;
    std::string clickEvent;
    
public:
    Object(int id = 0);
    virtual ~Object();

    virtual void Update(const std::unordered_map<std::string, std::string> & values);
    virtual void SetClick(GuiClick c);
    void SetVisible(bool v);
    virtual void SetPos(glm::vec3 p) {} // = 0 ?    
    virtual void Draw() {} // = 0?
    virtual void SetContent(const std::string & ct) { }
    virtual void Move(glm::vec3 p) {} // = 0 ?
    virtual void MakeCentered(glm::vec3 newPos) {}
    virtual bool Hover(int x, int y);
    virtual bool Scroll(int offset, int x, int y) { return false; }
    virtual bool DeleteObj(int _id);
    
    bool IsHoldingValue(const std::string & valueName, int objId = 0) const;
    virtual bool IsClicked(int x, int y) const;
    
    virtual const std::unordered_map<std::string,std::string> & GetValues() const { return values; }
    virtual GuiClick GetClick(int x, int y) const;
    virtual std::string GetValueName() const { return ""; }
    virtual std::string GetContent() const { return ""; }
    std::vector<std::string> GetContent(const std::string & valueName, int objId = 0) const;
    std::string GetClickType() const { return clickEvent; }
    std::string GetType() const { return type; }
    glm::vec3 GetPos() const { return position; }
    glm::vec2 GetSize() const { return size; }
    glm::vec3 GetRelativePos() const { return relativePos; }
    int GetId() const { return id; }    
    

};

