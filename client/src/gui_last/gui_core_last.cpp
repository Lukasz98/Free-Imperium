#include "gui_core_last.h"

#include "../gui/gui_structs.h"

Batch batch;
std::vector<List2> lists;

void drawRect(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& col)
{
    Vertex v[] = {Vertex{.pos = pos, .color = col},
                  Vertex{.pos = glm::vec3{pos.x, pos.y + size.y, pos.z}, .color = col},
                  Vertex{.pos = glm::vec3{pos.x + size.x, pos.y + size.y, pos.z}, .color = col},
                  Vertex{.pos = glm::vec3{pos.x + size.x, pos.y, pos.z}, .color = col}};
    batch.Push(v);
}

int createList(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& col)
{
    drawRect(pos, size, col);
    lists.emplace_back(pos, size);
    return lists.size() - 1;
}

std::size_t drawRectToList(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& col, std::size_t listId)
{
    assert(listId < lists.size());

    lists[listId].v.push_back(Vertex{.pos = pos, .color = col});
    lists[listId].v.push_back(Vertex{.pos = glm::vec3{pos.x, pos.y + size.y, pos.z}, .color = col});
    lists[listId].v.push_back(Vertex{.pos = glm::vec3{pos.x + size.x, pos.y + size.y, pos.z}, .color = col});
    lists[listId].v.push_back(Vertex{.pos = glm::vec3{pos.x + size.x, pos.y, pos.z}, .color = col});

    return lists[listId].v.size() - 4;
}

void drawLists()
{
    batch.Flush();
    for (std::size_t i = 0; i < lists.size(); ++i) {
        batch.Begin();
        glScissor(lists[i].pos.x, lists[i].pos.y, lists[i].size.x, lists[i].size.y);
        glm::vec2 rsize{5.0f, 10.0f};
        if (lists[i].v.size() > 2) {  // draw bar
            float firsty = lists[i].v[1].pos.y;
            float totalh = firsty - lists[i].lastY;
            rsize.y = (lists[i].size.y / totalh) * lists[i].size.y;
            if (rsize.y > lists[i].size.y - lists[i].ypadding * 2)
                rsize.y = lists[i].size.y - lists[i].ypadding * 2;

            float bottomYOffset = lists[i].pos.y - lists[i].lastY;
            float tmp = bottomYOffset / totalh;
            tmp *= lists[i].size.y;
            glm::vec3 rpos{lists[i].pos.x + lists[i].size.x - rsize.x, lists[i].pos.y, lists[i].pos.z + 0.5f};
            rpos.y = lists[i].pos.y + tmp;
            if (rpos.y < lists[i].pos.y + lists[i].ypadding)
                rpos.y = lists[i].pos.y + lists[i].ypadding;
            else if (rpos.y + rsize.y > lists[i].pos.y + lists[i].size.y - lists[i].ypadding)
                rpos.y = lists[i].pos.y + lists[i].size.y - rsize.y - lists[i].ypadding;
            glm::vec4 rcol{1.0f, 0.0f, 0.0f, 1.0f};
            drawRectToList(rpos, rsize, rcol, i);
        }
        for (std::size_t j = 0; j < lists[i].v.size(); j += 4) {
            batch.Push(&lists[i].v[j]);
        }
        batch.Flush();
    }
    lists.clear();
}

void drawTextToList(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& col, const std::string& text,
                    int flags, std::size_t listid)
{
    assert(listid < lists.size());
    glm::vec3 textPos = pos;
    // GLuint tid = AM::atlasTexture[AM::FontSize::PX32]->GetId();
    // GLuint tid = (int)fontSize;
    AM::FontSize fontSize = AM::FontSize::PX16;
    // GLuint tid = AM::atlasTexture[fontSize]->GetId();
    GLuint tid = (GLuint)fontSize;
    // GLuint tid = AM::atlasTexture[AM::FontSize::PX16]->GetId();
    float spaceWidth = 10.0f;
    float padding = 5.0f;
    float off = 0.0f, betweenWidth = 1.0f;

    float widthSum = 0.0f, maxY = 0.0f;
    for (int i = 0; i < text.size(); ++i) {
        // for (int i = 0; i < text.size() && i < max; ++i) {
        if (AM::atlasInfo[fontSize][text[i]].set) {
            widthSum += AM::atlasInfo[fontSize][text[i]].width + betweenWidth;
            if (maxY < AM::atlasInfo[fontSize][text[i]].height)
                maxY = AM::atlasInfo[fontSize][text[i]].height;
        }
        else
            widthSum += spaceWidth;
    }
    if (flags & TEXT_CENTER)  // centered
        textPos.x = (textPos.x + size.x * 0.5f) - widthSum * 0.5f;
    else if (flags & TEXT_RIGHT)  // aligned to right
        textPos.x = (textPos.x + size.x) - widthSum - padding;
    else {  // aligned left
        textPos.x += padding;
    }
    textPos.y = (textPos.y + size.y * 0.5f) - maxY * 0.5f;

    for (int i = 0; i < text.size(); ++i) {
        // for (int i = 0; i < text.size() && i < max; ++i) {
        glm::vec3 rPos;
        glm::vec2 rSize, rLBTC, tcLen;
        if (AM::atlasInfo[fontSize][text[i]].set) {
            rPos = {textPos.x + off, textPos.y + AM::atlasInfo[fontSize][text[i]].yPos, textPos.z};
            // rPos += lists[listid].pos;
            // this 2 if's are walkaround around bug. If we draw char at x or y with floating point
            // it gets more blury the closer it gets to .5f, at .0f it looks fine
            if (rPos.x > (((float)((int)rPos.x))))
                rPos.x = ((int)(rPos.x + 0.5f));
            if (rPos.y > (((float)((int)rPos.y))))
                rPos.y = ((int)(rPos.y + 0.5f));
            rSize = {AM::atlasInfo[fontSize][text[i]].width, AM::atlasInfo[fontSize][text[i]].height};
            // rLBTC = {(float)AM::atlasInfo[fontSize][text[i]].tcX, (float)AM::atlasInfo[fontSize][text[i]].tcY};
            // tcLen = {(float)AM::atlasInfo[fontSize][text[i]].tcWidth,
            // (float)AM::atlasInfo[fontSize][text[i]].tcHeight};
            rLBTC = {AM::atlasInfo[fontSize][text[i]].tcX, AM::atlasInfo[fontSize][text[i]].tcY};
            tcLen = {AM::atlasInfo[fontSize][text[i]].tcWidth, AM::atlasInfo[fontSize][text[i]].tcHeight};
            off += rSize.x + betweenWidth;

            // Vertex v[] = {
            lists[listid].v.push_back(
                Vertex{.pos = rPos, .color = col, .tc = glm::vec2{rLBTC.x, rLBTC.y}, .textureId = (float)tid});
            lists[listid].v.push_back(Vertex{.pos = glm::vec3{rPos.x, rPos.y + rSize.y, rPos.z},
                                             .color = col,
                                             .tc = glm::vec2{rLBTC.x, rLBTC.y + tcLen.y},
                                             .textureId = (float)tid});
            lists[listid].v.push_back(Vertex{.pos = glm::vec3{rPos.x + rSize.x, rPos.y + rSize.y, rPos.z},
                                             .color = col,
                                             .tc = glm::vec2{rLBTC.x + tcLen.x, rLBTC.y + tcLen.y},
                                             .textureId = (float)tid});
            lists[listid].v.push_back(Vertex{.pos = glm::vec3{rPos.x + rSize.x, rPos.y, rPos.z},
                                             .color = col,
                                             .tc = glm::vec2{rLBTC.x + tcLen.x, rLBTC.y},
                                             .textureId = (float)tid});
            // batch.Push(v);
        }
        else {
            rPos = {textPos.x + off, textPos.y, textPos.z};
            rSize = {spaceWidth, 10.0};
            rLBTC = {0, 0};
            tcLen = {0, 0};
            off += rSize.x;
        }
        //  Log(rPos.x << " " << rPos.y << " "<<rPos.z);
    }
}

void drawText(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& col, const std::string& text,
              int flags, AM::FontSize fontSize)
{
    glm::vec3 textPos = pos;
    // GLuint tid = AM::atlasTexture[AM::FontSize::PX32]->GetId();
    // GLuint tid = (int)fontSize;
    // AM::FontSize fontSize = AM::FontSize::PX16;
    // GLuint tid = AM::atlasTexture[fontSize]->GetId();
    GLuint tid = (GLuint)fontSize;
    // GLuint tid = AM::atlasTexture[AM::FontSize::PX16]->GetId();
    float spaceWidth = 10.0f;
    float padding = 5.0f;
    float off = 0.0f, betweenWidth = 1.0f;

    float widthSum = 0.0f, maxY = 0.0f;
    for (int i = 0; i < text.size(); ++i) {
        // for (int i = 0; i < text.size() && i < max; ++i) {
        if (AM::atlasInfo[fontSize][text[i]].set) {
            widthSum += AM::atlasInfo[fontSize][text[i]].width + betweenWidth;
            if (maxY < AM::atlasInfo[fontSize][text[i]].height)
                maxY = AM::atlasInfo[fontSize][text[i]].height;
        }
        else
            widthSum += spaceWidth;
    }
    if (flags & TEXT_CENTER)  // centered
        textPos.x = (textPos.x + size.x * 0.5f) - widthSum * 0.5f;
    else if (flags & TEXT_RIGHT)  // aligned to right
        textPos.x = (textPos.x + size.x) - widthSum - padding;
    else {  // aligned left
        textPos.x += padding;
    }
    textPos.y = (textPos.y + size.y * 0.5f) - maxY * 0.5f;

    for (int i = 0; i < text.size(); ++i) {
        // for (int i = 0; i < text.size() && i < max; ++i) {
        glm::vec3 rPos;
        glm::vec2 rSize, rLBTC, tcLen;
        if (AM::atlasInfo[fontSize][text[i]].set) {
            rPos = {textPos.x + off, textPos.y + AM::atlasInfo[fontSize][text[i]].yPos, textPos.z};
            // this 2 if's are walkaround around bug. If we draw char at x or y with floating point
            // it gets more blury the closer it gets to .5f, at .0f it looks fine
            if (rPos.x > (((float)((int)rPos.x))))
                rPos.x = ((int)(rPos.x + 0.5f));
            if (rPos.y > (((float)((int)rPos.y))))
                rPos.y = ((int)(rPos.y + 0.5f));
            rSize = {AM::atlasInfo[fontSize][text[i]].width, AM::atlasInfo[fontSize][text[i]].height};
            // rLBTC = {(float)AM::atlasInfo[fontSize][text[i]].tcX, (float)AM::atlasInfo[fontSize][text[i]].tcY};
            // tcLen = {(float)AM::atlasInfo[fontSize][text[i]].tcWidth,
            // (float)AM::atlasInfo[fontSize][text[i]].tcHeight};
            rLBTC = {AM::atlasInfo[fontSize][text[i]].tcX, AM::atlasInfo[fontSize][text[i]].tcY};
            tcLen = {AM::atlasInfo[fontSize][text[i]].tcWidth, AM::atlasInfo[fontSize][text[i]].tcHeight};
            off += rSize.x + betweenWidth;

            Vertex v[] = {
                Vertex{.pos = rPos, .color = col, .tc = glm::vec2{rLBTC.x, rLBTC.y}, .textureId = (float)tid},
                Vertex{.pos = glm::vec3{rPos.x, rPos.y + rSize.y, rPos.z},
                       .color = col,
                       .tc = glm::vec2{rLBTC.x, rLBTC.y + tcLen.y},
                       .textureId = (float)tid},
                Vertex{.pos = glm::vec3{rPos.x + rSize.x, rPos.y + rSize.y, rPos.z},
                       .color = col,
                       .tc = glm::vec2{rLBTC.x + tcLen.x, rLBTC.y + tcLen.y},
                       .textureId = (float)tid},
                Vertex{.pos = glm::vec3{rPos.x + rSize.x, rPos.y, rPos.z},
                       .color = col,
                       .tc = glm::vec2{rLBTC.x + tcLen.x, rLBTC.y},
                       .textureId = (float)tid}};
            batch.Push(v);
        }
        else {
            rPos = {textPos.x + off, textPos.y, textPos.z};
            rSize = {spaceWidth, 10.0};
            rLBTC = {0, 0};
            tcLen = {0, 0};
            off += rSize.x;
        }
        //  Log(rPos.x << " " << rPos.y << " "<<rPos.z);
    }
}

void drawIcon(const glm::vec3& pos, const glm::vec2& size, int iconId)
{
    float textWidth = AM::am.iconsTexture->GetWidth();
    const float iconWidth = 50.0f;
    float tcW = iconWidth / textWidth;
    float tcX = iconId * tcW;
    const float tid = 31;
    //Log(iconId << " " << tcW << " " << textWidth);
    Vertex v[] = {
        Vertex{.pos = pos, .color = glm::vec4{}, .tc = glm::vec2{tcX, 0.0f}, .textureId = (float)tid},
        Vertex{.pos = glm::vec3{pos.x, pos.y + size.y, pos.z}, .color = glm::vec4{}, .tc = glm::vec2{tcX, 1.0f}, .textureId = (float)tid},
        Vertex{.pos = glm::vec3{pos.x + size.x, pos.y + size.y, pos.z}, .color = glm::vec4{}, .tc = glm::vec2{tcX + tcW, 1.0f}, .textureId = (float)tid},
        Vertex{.pos = glm::vec3{pos.x + size.x, pos.y, pos.z}, .color = glm::vec4{}, .tc = glm::vec2{tcX + tcW, 0.0f}, .textureId = (float)tid}
    };
    batch.Push(v);
}

void listScroll(int listid, int& scr)
{
    if (scr == 0 || lists[listid].v.size() == 0)
        return;
    int maxscr = (lists[listid].pos.y + lists[listid].ypadding) - lists[listid].lastY;
    if (maxscr < 0)
        maxscr = 0;
    if (lists[listid].v[1].pos.y - scr < lists[listid].pos.y + lists[listid].size.y - lists[listid].ypadding)
        scr = 0;
    else {
        if (scr < -maxscr)
            scr = -maxscr;
        for (std::size_t i = 0; i < lists[listid].v.size(); ++i) {
            lists[listid].v[i].pos.y -= scr;
        }
        lists[listid].lastY -= scr;
    }
}

float getListLastY(std::size_t listid)
{
    assert(listid < lists.size());
    return lists[listid].lastY;
}

float getListYPadding(std::size_t listid)
{
    assert(listid < lists.size());
    return lists[listid].ypadding;
}

void setListLastY(std::size_t listid, float lastY)
{
    assert(listid < lists.size());
    lists[listid].lastY = lastY;
}

void guiBatchInit()
{
    batch.Init();
}

void guiBatchBegin()
{
    batch.Begin();
}

void guiBatchFlush()
{
    batch.Flush();
}

bool isInRect(const glm::vec3& pos, const glm::vec2& size, int mx, int my)
{
    if (mx > pos.x && mx < pos.x + size.x && my > pos.y && my < pos.y + size.y)
        return true;
    return false;
}

bool isInRectList(std::size_t listid, std::size_t vid, int mx, int my)
{
    assert(listid < lists.size());
    assert(vid + 3 < lists[listid].v.size());
    if (mx > lists[listid].v[vid].pos.x && mx < lists[listid].v[vid + 3].pos.x &&
        my > lists[listid].v[vid].pos.y && my < lists[listid].v[vid + 1].pos.y)
        return true;
    return false;
}

// ClickEventType drawTopBar(const TopBarData& td, const glm::vec2 & resolution)
//{

//}
