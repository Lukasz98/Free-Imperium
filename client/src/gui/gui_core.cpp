#include "gui_core.h"
#include "load_from_png.h"

bool dragging = false;
glm::vec2 dragRelative;
const float DRAGING_SPEED = 50.0f;

using namespace Gui::Core;

TextLabel::~TextLabel()
{
}
ClickEventType TextLabel::Click(const glm::vec2 & mPos)
{
    if (backgr && backgr->Click(mPos.x, mPos.y))
        return evName;
    return ClickEventType::NONE;
}
void TextLabel::setText(const std::string & content)
{
    text.content = content;

    glm::vec3 textPos = text.relPos + backgr->GetPosition();

    if (text.centered) {
        glm::vec3 centerTo = backgr->GetPosition() + text.relCenterTo;
        unsigned int wSum = 0, maxY = 0;
        for (auto c : content) {
            if (AM::atlasInfo[text.fontSize][c].set) {
                wSum += AM::atlasInfo[text.fontSize][c].width;
                if (maxY <  AM::atlasInfo[text.fontSize][c].height)
                    maxY = AM::atlasInfo[text.fontSize][c].height;
            }
            else {
                wSum += 10;
            }
        }
        textPos.x = centerTo.x - ((float)wSum) / 2.0f;
        textPos.y = centerTo.y - ((float)maxY) / 2.0f;
    }
    
    text.rects.clear();
    float off = 0.0f;
    for (auto c : content) {
        glm::vec3 rPos;
        glm::vec2 rSize, rLBTC, tcLen;
        if (AM::atlasInfo[text.fontSize][c].set) {
            rPos = {textPos.x + off, textPos.y + AM::atlasInfo[text.fontSize][c].yPos, textPos.z};
            rSize = {(float)AM::atlasInfo[text.fontSize][c].width, (float)AM::atlasInfo[text.fontSize][c].height};
            rLBTC = {AM::atlasInfo[text.fontSize][c].tcX, AM::atlasInfo[text.fontSize][c].tcY};
            tcLen = {AM::atlasInfo[text.fontSize][c].tcWidth, AM::atlasInfo[text.fontSize][c].tcHeight};
            off += rSize.x + 1;
            text.rects.push_back(std::make_unique<Rectangle>(rPos, rSize, rLBTC, tcLen)); 
            if (content == "Quit game") {
                //rSize.s += 0.1f; 
                Log("tu:"<<text.rects.back()->GetPosition().x <<", "<<text.rects.back()->GetSize().x
                    << ":: "<<text.rects.back()->GetPosition().x + text.rects.back()->GetSize().x );
                //Log("tu2"<<":"<<AM::atlasInfo[this->text->fontSize][c].width);
            }
        }
        else {
            rPos = {textPos.x + off, textPos.y, textPos.z};
            rSize = {10.0, 10.0};
            rLBTC = {0, 0};
            tcLen = {0, 0};
            off += rSize.x;
        }
        //Log(rPos.x << " " << rPos.y << " "<<rPos.z);
    }
}
void TextLabel::Draw()
{
    if (backgr)
        backgr->Draw();
    AM::atlasTexture[text.fontSize]->Bind();
    for (auto & rect : text.rects) {
        rect->Draw();
    }
    AM::atlasTexture[text.fontSize]->Unbind();
}







IconLabel::~IconLabel()
{
}
ClickEventType IconLabel::Click(const glm::vec2 & mPos)
{
    if (backgr && backgr->Click(mPos.x, mPos.y))
        return evName;
    return ClickEventType::NONE;
}
void IconLabel::setIcon(const std::string & path)
{
    icon.iconPath = path;
    icon.texture = std::make_unique<Texture>(path);
    backgr = std::make_unique<Rectangle>(pos, size);
}
void IconLabel::Draw()
{
    if (icon.texture)
        icon.texture->Bind();
    if (backgr)
        backgr->Draw();
    if (icon.texture)
        icon.texture->Unbind();
}


Group::~Group()
{
    for (auto g : groups)
        delete g;
    for (auto text : tLabels)
        delete text;
    for (auto icon : iLabels)
        delete icon;
}
bool Group::Click(ClickData & clickData, const glm::vec2 & mPos)
{
    if ((hoverable && !hovered) || !visible)
        return false;
    for (auto g : groups) {
        if (g->Click(clickData, mPos))
            return true;
    }
    
    for (auto t : tLabels) {
        auto ev = t->Click(mPos);
        if (ev != ClickEventType::NONE) {
            clickData.group = this;
            clickData.evType = ev;
            return true;
        }
    }

    for (auto i : iLabels) {
        auto ev = i->Click(mPos);
        if (ev != ClickEventType::NONE) {
            clickData.group = this;
            clickData.evType = ev;
            return true;
        }
    }
    return false;
}
bool Group::Hover(const glm::vec2 & mPos)
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
void Group::Scroll(int amount) // used only by List
{
    backgr->MoveRect(0, amount);
    for (auto g : groups) {
        g->Scroll(amount);
    }
    for (auto tL : tLabels) {
        assert(tL->backgr); 
        tL->backgr->MoveRect(0, amount);
        tL->setText(tL->text.content);
    }
    for (auto iL : iLabels) {
        assert(iL->backgr);
        iL->pos.y += amount;
        iL->backgr->MoveRect(0, amount);
    }
}
void Group::Draw()
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
void Group::SetPos(const glm::vec3 & newPos)
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
        textL->setText(textL->text.content);
    }
    backgr->SetPosition(newPos);
}


List::List(const glm::vec3 & pos, const glm::vec2 & size, float relYOfContent, const glm::vec4 & bgColor, const glm::vec4 & barColor, float groupsOffset)
: groupsOffset(groupsOffset)
{
    backgr = std::make_unique<Rectangle>(pos, size, bgColor);
    bottRect = std::make_unique<Rectangle>(
        glm::vec3{pos.x, pos.y + relYOfContent, pos.z + 0.5},  glm::vec2{size.x, relYOfContent}, barColor);
    topRect = std::make_unique<Rectangle>(
        glm::vec3{pos.x, pos.y + size.y - relYOfContent, pos.z + 0.5}, glm::vec2{size.x, relYOfContent}, barColor
    );

    glm::vec2 scrollSize = glm::vec2{10.0, 100.0};
    scroll = std::make_unique<Rectangle>(
                    glm::vec3{pos.x + size.x - scrollSize.x, topRect->GetPosition().y - groupsOffset - scrollSize.y, pos.z + 0.5}, 
                    scrollSize, glm::vec4{0.1,0.1,0.1,1.0});
}
List::~List()
{
    for (auto g : groups)
        delete g;
}
bool List::Click(ClickData & clickData, const glm::vec2 & mPos)
{
    for (auto g : groups) {
        if (g->Click(clickData, mPos))
            return true;
    }
    return false;
}
void List::Draw()
{
    for (auto grp : groups)
        grp->Draw();
    assert(backgr);
    backgr->Draw();
    assert(topRect);
    topRect->Draw();
    assert(bottRect);
    bottRect->Draw();
    if (scrollVisible) {
        assert(scroll);
        scroll->Draw();
    }
}
void List::Scroll(int y)
{
    if (groups.size() == 0 || scrollVisible == false)
        return;
    if (y > 0 && groups.back()->backgr->GetPosition().y >= bottRect->GetPosition().y + bottRect->GetSize().y)
        return;
    if (y < 0 && groups[0]->backgr->GetPosition().y + groups[0]->backgr->GetSize().y + groupsOffset <= topRect->GetPosition().y)
        return;
    assert(topRect && bottRect);
    float yTop = topRect->GetPosition().y;
    float yBott = bottRect->GetPosition().y + bottRect->GetSize().y;
    
    
    auto sPos = scroll->GetPosition();
    auto sSize = scroll->GetSize();
    if (sPos.y + sSize.y > topRect->GetPosition().y - groupsOffset) {
        sPos.y = topRect->GetPosition().y - groupsOffset - sSize.y;
        scroll->SetPosition(sPos);
        return;
    }
    else if (sPos.y < bottRect->GetPosition().y + bottRect->GetSize().y + groupsOffset) {
        sPos.y = bottRect->GetPosition().y + bottRect->GetSize().y + groupsOffset;
        scroll->SetPosition(sPos);
        return;
    }
    float scrollNewY = sPos.y + scrollBarSpeed * y;
    if (scrollNewY + sSize.y > yTop - groupsOffset) {
        scrollNewY = yTop - groupsOffset - sSize.y;
    }
    else if (scrollNewY < yBott + groupsOffset) {
        scrollNewY = yBott + groupsOffset;
    }
    sPos.y = scrollNewY;
    scroll->SetPosition(sPos);
    if (groups.size() > 0 && y == -1) {
        if (groups[0]->backgr->GetPosition().y + groups[0]->backgr->GetSize().y + y * scrollSpeed < yTop - groupsOffset) {
            scrollSpeed = groups[0]->backgr->GetPosition().y + groups[0]->backgr->GetSize().y - yTop + groupsOffset;
        }
    }
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
void List::AddGroup(Group * g)
{
    assert(topRect && bottRect);
    float yTop = topRect->GetPosition().y;
    float yBott = bottRect->GetPosition().y + bottRect->GetSize().y;
    
    auto gPos = g->backgr->GetPosition();
    auto gSize = g->backgr->GetSize();
    gPos.x = backgr->GetPosition().x;
    if (groups.size() == 0)
        gPos.y = yTop - groupsOffset - g->backgr->GetSize().y;
    else
        gPos.y = groups.back()->backgr->GetPosition().y - groupsOffset - g->backgr->GetSize().y;

    g->SetPos(gPos);
    if (gPos.y < bottRect->GetPosition().y + bottRect->GetSize().y) {
        if (gPos.y + gSize.y < bottRect->GetPosition().y + bottRect->GetSize().y)
            g->visible = false;
        scrollVisible = true;
    }
    g->id = freeGrpId;
    ++freeGrpId;
    groups.push_back(g);
   
    if (!scrollVisible)
        return;
   
    float c1 = groups[0]->backgr->GetPosition().y + groups[0]->backgr->GetSize().y - yTop - groupsOffset;
    float sch = yTop - yBott - 2 * groupsOffset - scroll->GetSize().y;

    float c2 = yBott + groupsOffset - groups.back()->backgr->GetPosition().y;
    float cc = c1 + c2;
    float scy2 = c1 * sch / cc;
    
    auto scrollPos = scroll->GetPosition();
    if (scy2 > 0.0) {
        scrollPos.y = yTop - groupsOffset - scroll->GetSize().y / 2;
        scrollPos.y = scrollPos.y - scy2 - scroll->GetSize().y / 2.0f;
    }
    scroll->SetPosition(scrollPos);
    scrollBarSpeed = sch * scrollSpeed / -cc;
}
bool List::Hover(const glm::vec2 & mPos)
{
    return backgr->Click(mPos.x, mPos.y);    
}
void List::DeleteGroup(int gid)
{
    for (std::size_t i = 0; i < groups.size(); i++) {
        if (groups[i]->id != gid)
            continue;
        Log("GID="<<gid);
        auto gSize = groups[i]->backgr->GetSize();
        
            for (std::size_t j = i + 1; j < groups.size(); j++) {
                Log("GID2="<<groups[j]->id);
                groups[j]->Scroll((int)gSize.y + groupsOffset);
                int yPos = groups[j]->backgr->GetPosition().y;
                if (yPos < topRect->GetPosition().y && yPos + groups[j]->backgr->GetSize().y > bottRect->GetPosition().y + bottRect->GetSize().y)
                    groups[j]->visible = true;
                else
                    groups[j]->visible = false;
            }
        
        
        delete groups[i];
        groups.erase(groups.begin() + i);
        break;
    }
    if (groups.size() == 0) {
        scrollVisible = false;
        return;
    }
    
    float bottY = bottRect->GetPosition().y + bottRect->GetSize().y + groupsOffset;
    float topY = topRect->GetPosition().y - groupsOffset;
    if (groups.back()->backgr->GetPosition().y > bottY) {
        if (groups[0]->backgr->GetPosition().y + groups[0]->backgr->GetSize().y > topY) {
            int tmpY = groups.back()->backgr->GetPosition().y - bottY;
            if (groups[0]->backgr->GetPosition().y + groups[0]->backgr->GetSize().y - tmpY < topY)
                tmpY = groups[0]->backgr->GetPosition().y + groups[0]->backgr->GetSize().y - topY;
            for (int j = 0; j < groups.size(); ++j) {
                groups[j]->Scroll(-tmpY);
                int yPos = groups[j]->backgr->GetPosition().y;
                if (yPos < topRect->GetPosition().y && yPos + groups[j]->backgr->GetSize().y > bottRect->GetPosition().y + bottRect->GetSize().y)
                    groups[j]->visible = true;
                else
                    groups[j]->visible = false;
            }
        }
    }

    float yTop = topRect->GetPosition().y;
    float yBott = bottRect->GetPosition().y + bottRect->GetSize().y;
    
    if (groups[0]->visible && groups.back()->visible && groups[0]->backgr->GetPosition().y + groups[0]->backgr->GetSize().y <= yTop - groupsOffset
        && groups.back()->backgr->GetPosition().y >= yBott + groupsOffset) {
        scrollVisible = false;
        return;
    }
   
    float c1 = groups[0]->backgr->GetPosition().y + groups[0]->backgr->GetSize().y - yTop - groupsOffset;
    float sch = yTop - yBott - 2 * groupsOffset - scroll->GetSize().y;

    float c2 = yBott + groupsOffset - groups.back()->backgr->GetPosition().y;
    float cc = c1 + c2;
    float scy2 = c1 * sch / cc;
    
    auto scrollPos = scroll->GetPosition();
    if (scy2 > 0.0) {
        scrollPos.y = yTop - groupsOffset - scroll->GetSize().y / 2;
        scrollPos.y = scrollPos.y - scy2 - scroll->GetSize().y / 2.0f;
    }
    scroll->SetPosition(scrollPos);
    scrollBarSpeed = sch * scrollSpeed / -cc;
}
void List::SetPos(const glm::vec3 & newPos)
{
    auto mPos = backgr->GetPosition();
    for (auto g : groups) {
        g->SetPos(newPos + (g->backgr->GetPosition() - backgr->GetPosition()));
    }
    topRect->SetPosition(newPos + (topRect->GetPosition() - backgr->GetPosition()));
    bottRect->SetPosition(newPos + (bottRect->GetPosition() - backgr->GetPosition()));
    scroll->SetPosition(newPos + (scroll->GetPosition() - backgr->GetPosition()));
    backgr->SetPosition(newPos);
}



Window::~Window()
{
    for (auto g : groups)
        delete g;
}
ClickEventType Window::GetClick(ClickData & clickData, glm::vec2 mousPos)
{
    for (auto g : groups) {
        if (g->Click(clickData, mousPos))
            return clickData.evType;
    }
    for (auto list : lists) {
        if (list->Click(clickData, mousPos))
            return clickData.evType;
    }
    return clickData.evType;
}
void Window::Hover(const glm::vec2 & mPos)
{
    for (auto g : groups) {
        g->Hover(mPos);
    }
}
void Window::Draw()
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
void Window::Drag(const glm::vec2 & mPos, float dt)
{
    assert(backgr);
    if (dragging == false)
        dragRelative = mPos - glm::vec2{backgr->GetPosition().x, backgr->GetPosition().y};
    glm::vec2 delta = mPos - dragRelative - glm::vec2{backgr->GetPosition().x, backgr->GetPosition().y};
    delta *= (DRAGING_SPEED * dt);
    backgr->MoveRect(delta.x, delta.y);
    for (auto g : groups) {
        assert(g->backgr);
        auto gPos = g->backgr->GetPosition();
        gPos.x += (int)delta.x;
        gPos.y += (int)delta.y;
        g->SetPos(gPos);
    }
    for (auto l : lists) {
        assert(l->backgr);
        auto gPos = l->backgr->GetPosition();
        gPos.x += (int)delta.x;
        gPos.y += (int)delta.y;
        l->SetPos(gPos);
    }
}

/*
void OpenUnitsList()
{
    Window * w = new Window{};
    windows.push_back(w);
    w->dragable = true;
    w->type = WindowType::UNITS_LIST;
    w->id = 0;
    w->backgr = std::make_unique<Rectangle>(glm::vec3{700.0, 50.0, 0.0}, glm::vec2{400.0, 600.0}, glm::vec4{1.0, 0.0, 0.0, .1});

    List * list = new List{glm::vec3{700.0, 50.0, 0.1}, glm::vec2{390.0, 590.0}, 40.0, glm::vec4{1.0, 0.5, 0.5, 1.0}, glm::vec4{1.0, 1.0, 0.0, 1.0}, 5.0f};
    w->lists.push_back(list);

for (int i = 0; i < 22; i++) {
    Group * grp = new Group{};
    grp->id = 0;
    grp->backgr = std::make_unique<Rectangle>(glm::vec3{700.0, 310.0, .2}, glm::vec2{390.0, 40.0}, glm::vec4{.0, 1.0, 1.0, 1.0});
    
    TextLabel * title2 = new TextLabel{};
    grp->tLabels.push_back(title2);
    title2->backgr = std::make_unique<Rectangle>(glm::vec3{700.0, 310.0, .3}, glm::vec2{360.0, 40.0}, glm::vec4{.4, 1.0, 0.4, 1.0});
   
    title2->id = 0;
    title2->text.fontSize = AM::FontSize::PX16;
    title2->text.textC = Color{0, 0, 255, 255};
    title2->text.bgC = Color{0, 255, 255, 255};
    title2->text.relPos= glm::vec3{10.123, 10.0, .1};
    title2->text.centered = false;
    //title2->text->relCenterTo = glm::vec3{320.0, 320.0 + 37, .4};
    title2->evName = ClickEventType::TEST;
    title2->setText("Mazowsze unit" + std::to_string(i));

    IconLabel * icon = new IconLabel{};
    grp->iLabels.push_back(icon);
    icon->icon.iconPath = "src/img/plus.png";
    //icon->icon->texture = std::make_unique<Texture>(icon->icon->path);
    //icon->backgr = std::make_unique<Rectangle>(glm::vec3{765.0, 310.0, 0.4}, glm::vec2{30, 30});
    icon->pos = glm::vec3{1080.0, 310.0, 0.4};
    icon->size = glm::vec2{30, 30};
    icon->evName = ClickEventType::DEL_FROM_UNITS_LIST;
    icon->setIcon(icon->icon.iconPath);
    
    list->AddGroup(grp);
}

}
*/
/*
void DelFromUnitsList(ClickData & clickData)
{
    List * l = clickData.window->lists[0];

    Group * grp = new Group{};
    grp->id = 0;
    //list->groups.push_back(grp);
    grp->backgr = std::make_unique<Rectangle>(glm::vec3{700.0, 310.0, .2}, glm::vec2{390.0, 40.0}, glm::vec4{.0, 1.0, 1.0, 1.0});
    
    TextLabel * title2 = new TextLabel{};
    grp->tLabels.push_back(title2);
    title2->backgr = std::make_unique<Rectangle>(glm::vec3{700.0, 310.0, .3}, glm::vec2{360.0, 40.0}, glm::vec4{.4, 1.0, 0.4, 1.0});
   
    title2->id = 0;
    title2->text.fontSize = AM::FontSize::PX32;
    title2->text.textC = Color{0, 0, 255, 255};
    title2->text.bgC = Color{0, 255, 255, 255};
    title2->text.relPos= glm::vec3{0.0, 0.0, .1};
    title2->text.centered = false;
    title2->text.relCenterTo = glm::vec3{320.0, 320.0 + 37, .4};
    title2->evName = ClickEventType::TEST;
    title2->setText("Mazowsze unit ");

    IconLabel * icon = new IconLabel{};
    grp->iLabels.push_back(icon);
    icon->icon.iconPath = "src/img/plus.png";
    //icon->icon->texture = std::make_unique<Texture>(icon->icon->path);
    //icon->backgr = std::make_unique<Rectangle>(glm::vec3{765.0, 310.0, 0.4}, glm::vec2{30, 30});
    icon->pos = glm::vec3{1080.0, 310.0, 0.4};
    icon->size = glm::vec2{30, 30};
    icon->evName = ClickEventType::DEL_FROM_UNITS_LIST;
    icon->setIcon(icon->icon.iconPath);
    
    l->AddGroup(grp);




//l->DeleteGroup(clickData.group->id);
}
*/



