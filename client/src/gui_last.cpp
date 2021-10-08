#include "gui_last.h"

#include "asset_manager.h"
#include "gui_2/gui_batch.h"
// using namespace GuiLast;

// Camera camera;
Window* window;
glm::vec2 res, winSize;

Batch batch;
void GuiLast::init(Window* win, glm::vec2 resolution, glm::vec2 windowSize)
{
    // camera = Camera{window.GetSize()};
    res = resolution;
    winSize = windowSize;
    window = win;
    batch.Init();
}

struct List2 {
    std::vector<Vertex> v;
    glm::vec3 pos;
    glm::vec2 size;
    float lastY, ypadding = 5.0f;
    int scr;
    List2(const glm::vec3& pos, const glm::vec2& size, int scr) : pos(pos), size(size), scr(scr)
    {
        lastY = pos.y + size.y;
        //if (scr > 0)
        //    lastY -= scr;
    }
};

std::vector<List2> lists;

void drawLists()
{
    batch.Flush();
    for (std::size_t i = 0; i < lists.size(); ++i) {
            glScissor(lists[i].pos.x, lists[i].pos.y, lists[i].size.x, lists[i].size.y);
        for (std::size_t j = 0; j + 4 <= lists[i].v.size(); j += 4) {
            batch.Push(&lists[i].v[j]);
        }
            batch.Flush();
    }
    lists.clear();
}
// struct Vertex {
//    Vec3 pos;
//    Vec4 color;
//    Vec2 tc;
//    float textureId = 443;
//};
glm::vec4 brownCol{56.0f / 255.0f, 31.0f / 255.0f, 19.0f / 255.0f, 1.0f};
glm::vec4 greenCol{0.0f / 255.0f, 255.0f / 255.0f, 0.0f / 255.0f, 1.0f};

void drawRect(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& col)
{
    Vertex v[] = {Vertex{.pos = pos, .color = col},
                  Vertex{.pos = glm::vec3{pos.x, pos.y + size.y, pos.z}, .color = col},
                  Vertex{.pos = glm::vec3{pos.x + size.x, pos.y + size.y, pos.z}, .color = col},
                  Vertex{.pos = glm::vec3{pos.x + size.x, pos.y, pos.z}, .color = col}};
    batch.Push(v);
}
int createList(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& col, int scr)
{
    drawRect(pos, size, col);
    lists.emplace_back(pos, size, scr);
    return lists.size() - 1;
}

void drawRectToList(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& col, std::size_t listId)
{
    assert(listId < lists.size());

    lists[listId].v.push_back(Vertex{.pos = pos, .color = col});
    lists[listId].v.push_back(Vertex{.pos = glm::vec3{pos.x, pos.y + size.y, pos.z}, .color = col});
    lists[listId].v.push_back(Vertex{.pos = glm::vec3{pos.x + size.x, pos.y + size.y, pos.z}, .color = col});
    lists[listId].v.push_back(Vertex{.pos = glm::vec3{pos.x + size.x, pos.y, pos.z}, .color = col});
    // lists[listId].v[lists[listId].v.size() - 1].pos += lists[listId].pos;
    // lists[listId].v[lists[listId].v.size() - 2].pos += lists[listId].pos;
    // lists[listId].v[lists[listId].v.size() - 3].pos += lists[listId].pos;
    // lists[listId].v[lists[listId].v.size() - 4].pos += lists[listId].pos;
}

int TEXT_CENTER = 2;
int TEXT_RIGHT = 3;
int TEXT_LEFT = 4;

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
              int flags)
{
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

void doTestList(int scroll)
{
    static int scr = 0;
    scr += scroll;
    Log(scr);
        glm::vec3 lpos{300.0f, 50.0f, 0.1f};
        glm::vec2 lsize{100.0f, 200.0f};
        int listid = createList(lpos, lsize, brownCol, scr);
        for (int i = 0; i < 20; ++i) {
            glm::vec2 elSize{lsize.x, 40.0f};
            glm::vec3 elPos{lpos.x, lists[listid].lastY - lists[listid].ypadding - elSize.y, lpos.z + 0.01f};
            //elPos.y -= scr;
            drawRectToList(elPos, elSize, greenCol, listid);
            glm::vec3 tElPos = elPos;
            tElPos.z += 0.01f;
            drawTextToList(tElPos, elSize, brownCol, "Lukasz Kadracki", TEXT_CENTER, listid);
            lists[listid].lastY = elPos.y;
        }
}


void GuiLast::start()
{
    Shader shader("src/graphics/shaders/gui_batch/a.vert", "src/graphics/shaders/gui_batch/a.frag", "", "");
    glUseProgram(shader.GetProgram());
    glm::mat4 matrix = glm::ortho(0.0f, (float)winSize.x, 0.0f, (float)winSize.y);
    glUniformMatrix4fv(glGetUniformLocation(shader.GetProgram(), "pr_matrix"), 1, GL_FALSE,
                       glm::value_ptr(matrix));

    GLint tex[32];
    for (GLint i = 0; i < 32; ++i) {
        tex[i] = i;
    }
    GLuint fontTexID[32];
    for (int i = 0; i <= (int)AM::FontSize::PX160; ++i) {
        fontTexID[i] = AM::atlasTexture[i]->GetId();
    }
    for (int i = 0; i <= (int)AM::FontSize::PX160; ++i) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, (GLuint)fontTexID[i]);
    }
    glUniform1iv(glGetUniformLocation(shader.GetProgram(), "tex"), 32, tex);

    float dt = 0.0f, time = 0.0f;
    while (!window->ShouldClose()) {
        glEnable(GL_DEPTH_TEST);  // Enable depth testing for z-culling
        glDepthFunc(GL_LESS);
        glEnable(GL_SCISSOR_TEST);  
        glScissor(0, 0, winSize.x, winSize.y);
        window->Refresh();
        glm::vec2 mousePos{(float)window->xMouse * res.x / winSize.x,
                           (float)(winSize.y - window->yMouse) * (res.y / winSize.y)};

        if (window->keys['M']) {
            shader =
                Shader("src/graphics/shaders/gui_batch/a.vert", "src/graphics/shaders/gui_batch/a.frag", "", "");
        }

        batch.Begin();
        glm::vec3 pos{10.0f, 10.0f, 0.0f};
        glm::vec2 size{100.0f, 200.0f};

        drawRect(pos, size, brownCol);
        glm::vec3 tpos{10.0f, 10.0f, 0.1f};
        glm::vec2 tsize{100.0f, 200.0f};

        drawText(tpos, tsize, greenCol, "Lukasz Kadracki", TEXT_CENTER);

        // struct List2
        //{
        //    std::vector<Vertex> v;
        //    glm::vec3 pos;
        //    glm::vec2 size;
        //    float lastY, ypadding = 5.0f;
        //    List2(const glm::vec3 & pos, const glm::vec2 & size) : pos(pos), size(size)
        //    {
        //        lastY = pos.y + size.y;
        //    }
        //};
        //
        // std::vector<List2> lists;
        // int createList((const glm::vec3& pos, const glm::vec2& size, const glm::vec4& col)
        //{
        //    drawRect(pos, size, col);
        //    lists.emplace_back(pos, size);
        //    return lists.size() - 1;
        // int createList((const glm::vec3& pos, const glm::vec2& size, const glm::vec4& col)
        //}



        doTestList(window->scrollOffset);
window->scrollOffset = 0;
        drawLists();

        batch.Flush();

        window->Update();
        dt = glfwGetTime() - time;
        time = glfwGetTime();
    }
}
