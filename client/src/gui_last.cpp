#include "gui_last.h"

#include "asset_manager.h"
#include "gui_2/gui_batch.h"
// using namespace GuiLast;
#include "gui_last/gui_core_last.h"

// Camera camera;
Window* window;
glm::vec2 res, winSize;

//Batch batch;
void GuiLast::init(Window* win, glm::vec2 resolution, glm::vec2 windowSize)
{
    // camera = Camera{window.GetSize()};
    res = resolution;
    winSize = windowSize;
    window = win;
    
    guiBatchInit();
    //batch.Init();
}



glm::vec4 brownCol{56.0f / 255.0f, 31.0f / 255.0f, 19.0f / 255.0f, 1.0f};
glm::vec4 greenCol{0.0f / 255.0f, 255.0f / 255.0f, 0.0f / 255.0f, 1.0f};


void doTestList(int scroll, int mx, int my, float dt)
{
    static int scr = 0;
    int scrspeed = 10;
    glm::vec3 lpos{300.0f, 250.0f, 0.1f};
    glm::vec2 lsize{100.0f, 200.0f};
    
    bool minside = isInRect(lpos, lsize, mx, my);
    if (minside);
        scr -= scroll * scrspeed;
    
    std::size_t listid = createList(lpos, lsize, brownCol);
    std::string text = std::to_string(dt);
    float lastY = getListLastY(listid);
    float ypadding = getListYPadding(listid);
    int listElCount = 6;
    std::size_t lrectids[listElCount];
    //ClickEvent ce[listElCount];
    for (int i = 0; i < listElCount; ++i) {
        glm::vec2 elSize{lsize.x, 40.0f};
        glm::vec3 elPos{lpos.x, lastY - ypadding - elSize.y, lpos.z + 0.01f};
        lastY = elPos.y;
        lrectids[i] = drawRectToList(elPos, elSize, greenCol, listid);
        glm::vec3 tElPos = elPos;
        tElPos.z += 0.01f;
        //drawTextToList(tElPos, elSize, brownCol, "Lukasz Kadracki", TEXT_CENTER, listid);
        drawTextToList(tElPos, elSize, brownCol, text, TEXT_CENTER, listid);
    }
    setListLastY(listid, lastY);
    listScroll(listid, scr);
    
    if (minside)
    for (int i = 0; i < listElCount; ++i) {
        if (isInRectList(listid, lrectids[i], mx, my))
            Log("clicked " << i);
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

        guiBatchBegin();
        //batch.Begin();
        glm::vec3 pos{10.0f, 135.0f, 0.0f};
        glm::vec2 size{100.0f, 200.0f};

        drawRect(pos, size, brownCol);
        glm::vec3 tpos{10.0f, 10.0f, 0.1f};
        glm::vec2 tsize{100.0f, 200.0f};

        drawText(tpos, tsize, greenCol, "Lukasz Kadracki", TEXT_CENTER);


        doTestList(window->scrollOffset, mousePos.x, mousePos.y, dt);
        window->scrollOffset = 0;
        drawLists();
        
        guiBatchFlush();
        //batch.Flush();

        window->Update();
        dt = glfwGetTime() - time;
        time = glfwGetTime();
    }
}
