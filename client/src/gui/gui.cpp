#include "gui.h"
#include "load_from_png.h"

//Gui::TextLabel::TextLabel(const glm::vec3 & relPos, const glm::vec2 & size, enum AM::FontSize fsize)
//{
//
//}
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
    this->text->content = text;

    glm::vec3 textPos = backgr->GetPosition() + this->text->relPos;

    if (this->text->centered) {
        glm::vec3 centerTo = backgr->GetPosition() + this->text->relCenterTo;
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
        textPos.x = centerTo.x - (float)wSum / 2;
        textPos.y = centerTo.y - (float)maxY / 2;
        //this->text->position.x = this->text->centerTo.x - (float)wSum / 2;
        //this->text->position.y = this->text->centerTo.y - (float)maxY / 2;
    }
    
    this->text->rects.clear();
    float off = 0.0f;
    for (auto c : text) {
        glm::vec3 rPos;
        glm::vec2 rSize, rLBTC, tcLen;
        if (AM::atlasInfo[this->text->fontSize][c].set) {
            rPos = {textPos.x + off, textPos.y + AM::atlasInfo[this->text->fontSize][c].yPos, textPos.z};
            //rPos = {this->text->position.x + off, this->text->position.y + AM::atlasInfo[this->text->fontSize][c].yPos, this->text->position.z};
            rSize = {AM::atlasInfo[this->text->fontSize][c].width, AM::atlasInfo[this->text->fontSize][c].height};
            rLBTC = {AM::atlasInfo[this->text->fontSize][c].tcX, AM::atlasInfo[this->text->fontSize][c].tcY};
            tcLen = {AM::atlasInfo[this->text->fontSize][c].tcWidth, AM::atlasInfo[this->text->fontSize][c].tcHeight};
            off += rSize.x;
        }
        else {
            rPos = {textPos.x + off, textPos.y, textPos.z};
            //rPos = {this->text->position.x + off, this->text->position.y, this->text->position.z};
            rSize = {20, 20};
            rLBTC = {0, 0};
            tcLen = {0, 0};
            off += rSize.x;
        }
        //Log(rPos.x << " " << rPos.y << " "<<rPos.z);
        this->text->rects.push_back(std::make_unique<Rectangle>(rPos, rSize, rLBTC, tcLen)); 
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
        //Log("dt");
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
    if ((hoverable && !hovered) || !visible)
        return false;
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
bool Gui::Group::Hover(const glm::vec2 & mPos)
{
    if (!visible)
        return false;
    for (auto g : groups) {
        g->Hover(mPos);
    }
    if (hoverable && backgr->Click(mPos.x, mPos.y)) {
        hovered = true;
    }
    return hovered;
        
}
void Gui::Group::Scroll(int amount) // used only by List
{
    backgr->MoveRect(0, amount);
    for (auto g : groups) {
        g->Scroll(amount);
    }
    for (auto tL : tLabels) {
        assert(tL->backgr && tL->text); 
        tL->backgr->MoveRect(0, amount);
        //tL->text->position.y += amount;
        //tL->text->centerTo.y += amount;
        //for (auto & textRect : tL->text->rects) {
        //    textRect->MoveRect(0, amount);
        //}
        tL->setText(tL->text->content);
    }
    for (auto iL : iLabels) {
        assert(iL->backgr);
        iL->pos.y += amount;
        iL->backgr->MoveRect(0, amount);
    }
}
void Gui::Group::Draw()
{
    if ((hoverable && !hovered) || !visible)
        return;
    hovered = false;
    if (backgr)
        backgr->Draw();
    for (auto text : tLabels) {
        text->Draw();
    }
    for (auto icon : iLabels) {
        icon->Draw();
    }
}
void Gui::Group::SetPos(const glm::vec3 & newPos)
{
    auto mPos = backgr->GetPosition();
    for (auto g : groups) {
        g->SetPos(newPos + (g->backgr->GetPosition() - backgr->GetPosition()));
    }
    for (auto iconL : iLabels) {
        iconL->backgr->SetPosition(newPos + (iconL->backgr->GetPosition() - backgr->GetPosition()));
    }
    for (auto textL : tLabels) {
        textL->backgr->SetPosition(newPos + (textL->backgr->GetPosition() - backgr->GetPosition()));
        textL->setText(textL->text->content);
    }
    backgr->SetPosition(newPos);
}


Gui::List::List(const glm::vec3 & pos, const glm::vec2 & size, float relYOfContent, const glm::vec4 & bgColor, const glm::vec4 & barColor, float groupsOffset)
: groupsOffset(groupsOffset)
{
    backgr = std::make_unique<Rectangle>(pos, size, bgColor);
    bottRect = std::make_unique<Rectangle>(
        glm::vec3{pos.x, pos.y + relYOfContent, pos.z + 0.5},  glm::vec2{size.x, relYOfContent}, barColor);
    topRect = std::make_unique<Rectangle>(
        glm::vec3{pos.x, pos.y + size.y - relYOfContent, pos.z + 0.5}, glm::vec2{size.x, relYOfContent}, barColor
    );
    lastItemY = topRect->GetPosition().y - groupsOffset;

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
    if (topRect)
        topRect->Draw();
    if (bottRect)
        bottRect->Draw();
    for (auto grp : groups)
        grp->Draw();
}
void Gui::List::Scroll(int y)
{
    if (groups.size() == 0)
        return;
    if (y > 0 && groups.back()->backgr->GetPosition().y > bottRect->GetPosition().y + bottRect->GetSize().y)
        return;
    if (y < 0 && groups[0]->backgr->GetPosition().y + groups[0]->backgr->GetSize().y + groupsOffset <= topRect->GetPosition().y)
        return;
    int scrollSpeed = 30;
    assert(topRect && bottRect);
    float yTop = topRect->GetPosition().y;
    float yBott = bottRect->GetPosition().y + bottRect->GetSize().y;
    for (auto g : groups) {
        g->Scroll(y * scrollSpeed);
        int gYBott = g->backgr->GetPosition().y;
        int gYTop = g->backgr->GetPosition().y + g->backgr->GetSize().y;
        if (gYBott > yTop || gYTop < yBott)
            g->visible = false;
        else
            g->visible = true;
    }
}
void Gui::List::AddGroup(Group * g)
{
    auto gPos = g->backgr->GetPosition();
    auto gSize = g->backgr->GetSize();
    gPos.x = backgr->GetPosition().x;
    gPos.y = lastItemY - gSize.y;
    lastItemY = gPos.y - groupsOffset;

    g->SetPos(gPos);
    if (gPos.y + gSize.y < bottRect->GetPosition().y + bottRect->GetSize().y)
        g->visible = false;
    groups.push_back(g);
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
    for (auto list : lists) {
        if (list->Click(clickData, mousPos))
            return true;
    }
    return false;
}
void Gui::Window::Hover(const glm::vec2 & mPos)
{
    for (auto g : groups) {
        g->Hover(mPos);
    }
}
void Gui::Window::Draw()
{
    if (backgr)
        backgr->Draw();
    for (auto grp : groups) {
        grp->Draw();
    }
    for (auto list: lists) {
        list->Draw();
    }
}

std::vector<Gui::Window*> windows;

Gui::ClickData Gui::Click(const glm::vec2 & mousePos)
{
    Gui::ClickData clickData;
    for (auto w : windows) {
        if (w->GetClick(clickData, mousePos)) {
            clickData.window = w;
            return clickData;
        }
    }
    return clickData;
}

void Gui::Hover(const glm::vec2 & mousePos)
{
    for (auto w : windows)
        w->Hover(mousePos);
}

void Gui::Scroll(int y)
{
    for (auto w : windows) {
        for (auto l : w->lists)
            l->Scroll(y);
    }
}

void Gui::Draw()
{
    for (auto w : windows) {
        w->Draw();
    }
}

void Gui::OpenTopBar(const std::vector<std::string> & values, const glm::vec2 & resolution)
{
    Window * w = new Window{};
    windows.push_back(w);
    w->type = WindowType::TOP_BAR;
    w->id = 0;
    w->backgr = std::make_unique<Rectangle>(glm::vec3{100.0, 100.0, 0.0}, glm::vec2{400.0, 400.0}, glm::vec4{1.0, 0.0, 0.0, .1});

    Group * grp = new Group{};
    grp->id = 0;
    w->groups.push_back(grp);
    grp->backgr = std::make_unique<Rectangle>(glm::vec3{110.0, 310.0, .1}, glm::vec2{400.0, 74.0}, glm::vec4{1.0, 1.0, 0.0, .1});

    TextLabel * title2 = new TextLabel{};
    grp->tLabels.push_back(title2);
    title2->backgr = std::make_unique<Rectangle>(glm::vec3{120.0, 320.0, .2}, glm::vec2{400.0, 74.0}, glm::vec4{.4, 1.0, 0.4, .2});
   
    title2->id = 0;
    title2->text = new TextLabel::Text{};
    title2->text->fontSize = AM::FontSize::PX32;
    title2->text->textC = Color{0, 0, 255, 255};
    title2->text->bgC = Color{0, 255, 255, 255};
    title2->text->relPos = glm::vec3{0.0, 0.0, .1};
    title2->text->centered = true;
    title2->text->relCenterTo = glm::vec3{200.0, 37, .1};
    title2->evName = ClickEventType::TEST;
    title2->setText(values[0] + "j");
 

    Group * hovG = new Group{};
    w->groups.push_back(hovG);
    hovG->hoverable = true;
    hovG->backgr = std::make_unique<Rectangle>(glm::vec3{120.0, 320.0, .1}, glm::vec2{400.0, 74.0}, glm::vec4{1.0, 1.0, 0.0, .2});
    TextLabel * title3 = new TextLabel{};
    hovG->tLabels.push_back(title3);
    title3->backgr = std::make_unique<Rectangle>(glm::vec3{120.0, 320.0 - 74.0, .2}, glm::vec2{400.0, 74.0}, glm::vec4{.4, 1.0, 0.4, .2});
   
    title3->id = 0;
    title3->text = new TextLabel::Text{};
    title3->text->fontSize = AM::FontSize::PX64;
    title3->text->textC = Color{0, 0, 255, 255};
    title3->text->bgC = Color{0, 255, 255, 255};
    title3->text->relPos = glm::vec3{5.0, 5.0, .1};
    //title3->text->centered = true;
    //title3->text->centerTo = glm::vec3{270.0, 150.0 + 17, .3};
    title3->setText("Poljska2");
 

  
 
  

/*
   
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
 */

}

void Gui::OpenUnitsList()
{
    Window * w = new Window{};
    windows.push_back(w);
    w->type = WindowType::UNITS_LIST;
    w->id = 0;
    w->backgr = std::make_unique<Rectangle>(glm::vec3{700.0, 50.0, 0.0}, glm::vec2{400.0, 600.0}, glm::vec4{1.0, 0.0, 0.0, .1});

    List * list = new List{glm::vec3{700.0, 50.0, 0.1}, glm::vec2{390.0, 590.0}, 40.0, glm::vec4{1.0, 0.0, 0.0, .1}, glm::vec4{1.0, 1.0, 0.0, .5}, 5.0f};
    w->lists.push_back(list);
    //list->backgr = std::make_unique<Rectangle>( }, });
    //list->topRect = std::make_unique<Rectangle>(glm::vec3{400.0, 540.0, 0.5}, glm::vec2{390.0, 40.0}, });
   // list->bottRect = std::make_unique<Rectangle>(glm::vec3{400.0, 60.0, 0.5}, glm::vec2{390.0, 40.0}, glm::vec4{1.0, 1.0, 0.0, .5});

for (int i = 0; i < 16; i++) {
    Group * grp = new Group{};
    grp->id = 0;
    //list->groups.push_back(grp);
    grp->backgr = std::make_unique<Rectangle>(glm::vec3{700.0, 310.0, .2}, glm::vec2{390.0, 40.0}, glm::vec4{.0, 1.0, 1.0, 1.0});
    
    TextLabel * title2 = new TextLabel{};
    grp->tLabels.push_back(title2);
    title2->backgr = std::make_unique<Rectangle>(glm::vec3{700.0, 310.0, .3}, glm::vec2{360.0, 40.0}, glm::vec4{.4, 1.0, 0.4, 1.0});
   
    title2->id = 0;
    title2->text = new TextLabel::Text{};
    title2->text->fontSize = AM::FontSize::PX32;
    title2->text->textC = Color{0, 0, 255, 255};
    title2->text->bgC = Color{0, 255, 255, 255};
    title2->text->relPos= glm::vec3{0.0, 0.0, .1};
    title2->text->centered = false;
    title2->text->relCenterTo = glm::vec3{320.0, 320.0 + 37, .4};
    title2->evName = ClickEventType::TEST;
    title2->setText("Mazowsze unit " + std::to_string(i));

    IconLabel * icon = new IconLabel{};
    grp->iLabels.push_back(icon);
    icon->icon = new IconLabel::Icon{};
    icon->icon->iconPath = "src/img/plus.png";
    //icon->icon->texture = std::make_unique<Texture>(icon->icon->path);
    //icon->backgr = std::make_unique<Rectangle>(glm::vec3{765.0, 310.0, 0.4}, glm::vec2{30, 30});
    icon->pos = glm::vec3{765.0, 310.0, 0.4};
    icon->size = glm::vec2{30, 30};
    icon->evName = ClickEventType::TEST;
    icon->setIcon(icon->icon->iconPath);
    
    list->AddGroup(grp);
}
}
