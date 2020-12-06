#include "gui.h"
#include "load_from_png.h"
#include "../asset_manager.h"


Gui::TextLabel::~TextLabel()
{
    delete text;
}
Gui::ClickEventType Gui::TextLabel::Click(const glm::vec2 & mPos)
{
    if (backgr && backgr->Click(mPos.x, mPos.y))
        return evName;
    return Gui::ClickEventType::NONE;
}
void Gui::TextLabel::setText(const std::string & text)
{
    if (this->text == nullptr)
        this->text = new Text{};

    
    this->text->rects.clear();
    float off = 0.0f;
    for (auto c : text) {
        glm::vec3 rPos;
        glm::vec2 rSize, rLBTC, tcLen;
        if (AM::atlasInfo[c].set) {
            rPos = {this->text->position.x + off, this->text->position.y + AM::atlasInfo[c].yPos, this->text->position.z};
            rSize = {AM::atlasInfo[c].width, AM::atlasInfo[c].height};
            rLBTC = {AM::atlasInfo[c].tcX, AM::atlasInfo[c].tcY};
            tcLen = {AM::atlasInfo[c].tcWidth, AM::atlasInfo[c].tcHeight};
            off += rSize.x;
        }
        else {
            rPos = {this->text->position.x + off, this->text->position.y, this->text->position.z};
            rSize = {20, 20};
            rLBTC = {0, 0};
            tcLen = {0, 0};
            off += rSize.x;
        }
        Log(rPos.x << " " << rPos.y << " "<<rPos.z);
        this->text->rects.push_back(std::make_unique<Rectangle>(rPos, rSize, rLBTC, tcLen)); 
    }
}
void Gui::TextLabel::Draw()
{
    if (backgr)
        backgr->Draw();
    //if (text && text->textur && text->backgr) {
    if (text) {
        AM::atlasTexture->Bind();
        for (auto & rect : text->rects) {
            rect->Draw();
        }
        AM::atlasTexture->Unbind();
        //text->texture->Bind();
        //text->backgr->Draw();
        //text->texture->Unbind();
    }
}







Gui::IconLabel::~IconLabel()
{
    delete icon;
}
Gui::ClickEventType Gui::IconLabel::Click(const glm::vec2 & mPos)
{
    if (backgr && backgr->Click(mPos.x, mPos.y))
        return evName;
    return Gui::ClickEventType::NONE;
}
void Gui::IconLabel::setIcon(const std::string & path)
{
    if (icon == nullptr)
        icon = new Icon{};
    icon->iconPath = path;
    icon->texture = std::make_unique<Texture>(path);
    backgr = std::make_unique<Rectangle>(pos, size);
}
void Gui::IconLabel::Draw()
{
    if (icon && icon->texture)
        icon->texture->Bind();
    if (backgr)
        backgr->Draw();
    if (icon && icon->texture)
        icon->texture->Unbind();
}


Gui::Group::~Group()
{
    for (auto g : groups)
        delete g;
    for (auto text : tLabels)
        delete text;
    for (auto icon : iLabels)
        delete icon;
}
bool Gui::Group::Click(Gui::ClickData & clickData, const glm::vec2 & mPos)
{
    for (auto g : groups) {
        if (g->Click(clickData, mPos))
            return true;
    }
    
    for (auto t : tLabels) {
        auto ev = t->Click(mPos);
        if (ev != Gui::ClickEventType::NONE) {
            clickData.group = this;
            clickData.evType = ev;
            return true;
        }
    }

    for (auto i : iLabels) {
        auto ev = i->Click(mPos);
        if (ev != Gui::ClickEventType::NONE) {
            clickData.group = this;
            clickData.evType = ev;
            return true;
        }
    }
    return false;
}
void Gui::Group::Draw()
{
    if (backgr)
        backgr->Draw();
    for (auto text : tLabels) {
        text->Draw();
    }
    for (auto icon : iLabels) {
        icon->Draw();
    }
}



Gui::List::~List()
{
    for (auto g : groups)
        delete g;
}
bool Gui::List::Click(Gui::ClickData & clickData, const glm::vec2 & mPos)
{
    for (auto g : groups) {
        if (g->Click(clickData, mPos))
            return true;
    }
    return false;
}
void Gui::List::Draw()
{
    if (backgr)
        backgr->Draw();
    for (auto grp : groups)
        grp->Draw();
}



Gui::Window::~Window()
{
    for (auto g : groups)
        delete g;
}
bool Gui::Window::GetClick(Gui::ClickData & clickData, glm::vec2 mousPos)
{
    for (auto g : groups) {
        if (g->Click(clickData, mousPos))
            return true;
    }
    return false;
}
void Gui::Window::Draw()
{
    if (backgr)
        backgr->Draw();
    for (auto grp : groups) {
        grp->Draw();
    }
}


std::vector<Gui::Window*> windows;

void Gui::Draw()
{
    for (auto w : windows) {
        //Log("Draw");
        w->Draw();
    }
}

void Gui::OpenTopBar(const std::vector<std::string> & values)
{
    Window * w = new Window{};
    windows.push_back(w);
    w->type = WindowType::TOP_BAR;
    w->id = 0;
    w->backgr = std::make_unique<Rectangle>(glm::vec3{100.0, 100.0, 0.0}, glm::vec2{400.0, 400.0}, glm::vec4{1.0, 0.0, 0.0, 1.0});

    Group * grp = new Group{};
    grp->id = 0;
    w->groups.push_back(grp);
    grp->backgr = std::make_unique<Rectangle>(glm::vec3{110.0, 110.0, .1}, glm::vec2{200.0, 200.0}, glm::vec4{1.0, 1.0, 0.0, 1.0});

    TextLabel * title = new TextLabel{};
    grp->tLabels.push_back(title);
    title->backgr = std::make_unique<Rectangle>(glm::vec3{120.0, 120.0, .2}, glm::vec2{100.0, 50.0}, glm::vec4{.0, 1.0, 0.0, .5});
   
    title->id = 0;
    title->text = new TextLabel::Text{};
    title->text->height = 70;
    title->text->textC = Color{0, 0, 255, 255};
    title->text->bgC = Color{0, 255, 255, 255};
    title->text->maxWidth = 230;
    title->text->centered = false;
    title->text->position = glm::vec3{125.0, 125.0, .3};
    title->text->size = glm::vec2{170.0, 170.0};
    //title->text->text = values[0];
    title->setText("hujala \"Dag,.");//values[0]);
    
}
