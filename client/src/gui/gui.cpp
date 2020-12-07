#include "gui.h"
#include "load_from_png.h"


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

    if (this->text->centered) {
        int wSum = 0, maxY = 0;
        for (auto c : text) {
            if (AM::atlasInfo[this->text->fontSize][c].set) {
                wSum += AM::atlasInfo[this->text->fontSize][c].width;
                if (maxY <  AM::atlasInfo[this->text->fontSize][c].height)
                    maxY = AM::atlasInfo[this->text->fontSize][c].height;
            }
            else {
                wSum += 20;
            }
        }
        this->text->position.x = this->text->centerTo.x - (float)wSum / 2;
        this->text->position.y = this->text->centerTo.y - (float)maxY / 2;
    }
    
    this->text->rects.clear();
    float off = 0.0f;
    for (auto c : text) {
        glm::vec3 rPos;
        glm::vec2 rSize, rLBTC, tcLen;
        if (AM::atlasInfo[this->text->fontSize][c].set) {
            rPos = {this->text->position.x + off, this->text->position.y + AM::atlasInfo[this->text->fontSize][c].yPos, this->text->position.z};
            rSize = {AM::atlasInfo[this->text->fontSize][c].width, AM::atlasInfo[this->text->fontSize][c].height};
            rLBTC = {AM::atlasInfo[this->text->fontSize][c].tcX, AM::atlasInfo[this->text->fontSize][c].tcY};
            tcLen = {AM::atlasInfo[this->text->fontSize][c].tcWidth, AM::atlasInfo[this->text->fontSize][c].tcHeight};
            off += rSize.x;
        }
        else {
            rPos = {this->text->position.x + off, this->text->position.y, this->text->position.z};
            rSize = {20, 20};
            rLBTC = {0, 0};
            tcLen = {0, 0};
            off += rSize.x;
        }
        Log(rSize.x << " " << rSize.y << " "<<rPos.z);
        //this->text->rects.push_back(std::make_unique<Rectangle>(rPos, rSize, rLBTC, tcLen)); 
    }
}
void Gui::TextLabel::Draw()
{
    if (backgr)
        backgr->Draw();
    //if (text && text->textur && text->backgr) {
    if (text) {
        AM::atlasTexture[this->text->fontSize]->Bind();
        for (auto & rect : text->rects) {
            rect->Draw();
        }
        AM::atlasTexture[this->text->fontSize]->Unbind();
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


    TextLabel * title3 = new TextLabel{};
    grp->tLabels.push_back(title3);
    title3->backgr = std::make_unique<Rectangle>(glm::vec3{220.0, 150.0, .2}, glm::vec2{100.0, 34.0}, glm::vec4{.4, 1.0, 0.4, 1.0});
   
    title3->id = 0;
    title3->text = new TextLabel::Text{};
    title3->text->fontSize = AM::FontSize::PX16;
    title3->text->textC = Color{0, 0, 255, 255};
    title3->text->bgC = Color{0, 255, 255, 255};
    title3->text->position = glm::vec3{125.0, 325.0, .3};
    title3->text->centered = true;
    title3->text->centerTo = glm::vec3{270.0, 150.0 + 17, .3};
    title3->setText("Poljska");
 

  
    TextLabel * title2 = new TextLabel{};
    grp->tLabels.push_back(title2);
    title2->backgr = std::make_unique<Rectangle>(glm::vec3{120.0, 320.0, .2}, glm::vec2{400.0, 74.0}, glm::vec4{.4, 1.0, 0.4, 1.0});
   
    title2->id = 0;
    title2->text = new TextLabel::Text{};
    title2->text->fontSize = AM::FontSize::PX64;
    title2->text->textC = Color{0, 0, 255, 255};
    title2->text->bgC = Color{0, 255, 255, 255};
    title2->text->position = glm::vec3{125.0, 325.0, .3};
    title2->text->centered = true;
    title2->text->centerTo = glm::vec3{320.0, 320.0 + 37, .3};
    title2->setText(values[0] + "j");
 

  


   
  TextLabel * title = new TextLabel{};
    grp->tLabels.push_back(title);
    title->backgr = std::make_unique<Rectangle>(glm::vec3{120.0, 120.0, .2}, glm::vec2{100.0, 50.0}, glm::vec4{.0, 1.0, 0.0, .5});
   
    title->id = 0;
    title->text = new TextLabel::Text{};
    title->text->fontSize = AM::FontSize::PX32;
    title->text->textC = Color{0, 0, 255, 255};
    title->text->bgC = Color{0, 255, 255, 255};
    title->text->position = glm::vec3{125.0, 125.0, .3};
    title->setText("hujala \"Dag,.");//values[0]);
 

}
