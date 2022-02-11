#include "gui_core_last.h"

#include "../gui_structs.h"

// std::vector<List2> lists;

void GuiCore::drawRect(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& col)
{
    Vertex v[] = {Vertex{.pos = pos, .color = col},
                  Vertex{.pos = glm::vec3{pos.x, pos.y + size.y, pos.z}, .color = col},
                  Vertex{.pos = glm::vec3{pos.x + size.x, pos.y + size.y, pos.z}, .color = col},
                  Vertex{.pos = glm::vec3{pos.x + size.x, pos.y, pos.z}, .color = col}};
    batch.Push(v);
}

void GuiCore::drawText(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& col, const std::string& text,
                       int flags, AM::FontSize fontSize)
{
    glm::vec3 textPos = pos;
    GLuint tid = (GLuint)fontSize;
    float spaceWidth = 10.0f;
    float padding = 5.0f;
    float off = 0.0f, betweenWidth = 1.0f;

    float widthSum = 0.0f, maxY = 0.0f;
    for (int i = 0; i < text.size(); ++i) {
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
    }
}

void GuiCore::drawIcon(const glm::vec3& pos, const glm::vec2& size, int iconId)
{
    float textWidth = AM::am.iconsTexture->GetWidth();
    const float iconWidth = 50.0f;
    float tcW = iconWidth / textWidth;
    float tcX = iconId * tcW;
    const float tid = 31;
    Vertex v[] = {Vertex{.pos = pos, .color = glm::vec4{}, .tc = glm::vec2{tcX, 0.0f}, .textureId = (float)tid},
                  Vertex{.pos = glm::vec3{pos.x, pos.y + size.y, pos.z},
                         .color = glm::vec4{},
                         .tc = glm::vec2{tcX, 1.0f},
                         .textureId = (float)tid},
                  Vertex{.pos = glm::vec3{pos.x + size.x, pos.y + size.y, pos.z},
                         .color = glm::vec4{},
                         .tc = glm::vec2{tcX + tcW, 1.0f},
                         .textureId = (float)tid},
                  Vertex{.pos = glm::vec3{pos.x + size.x, pos.y, pos.z},
                         .color = glm::vec4{},
                         .tc = glm::vec2{tcX + tcW, 0.0f},
                         .textureId = (float)tid}};
    batch.Push(v);
}

void GuiCore::guiBatchInit()
{
    batch.Init();
}

void GuiCore::guiBatchBegin()
{
    batch.Begin();
}

void GuiCore::guiBatchFlush()
{
    batch.Flush();
}

bool GuiCore::isInRect(const glm::vec3& pos, const glm::vec2& size, int mx, int my)
{
    if (mx > pos.x && mx < pos.x + size.x && my > pos.y && my < pos.y + size.y)
        return true;
    return false;
}

