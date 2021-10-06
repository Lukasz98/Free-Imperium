//#include <memory>
#include <SFML/Network.hpp>
#include "log.h"
#include "graphics/window.h"
#include "graphics/shader.h"

#include "menu.h"
#include "room.h"
#include "game.h"
#include "graphics/window.h"
#include "camera.h"
//#include <glm::vec2.hpp> 
#include "asset_manager.h"
#include "settings.h"

#include "new_map.h"

#define MAP_TEST 1
#define BATCH_TEST 0
#define GUI_PREVIEW 0
#if GUI_PREVIEW
    #include "gui_2/gui_preview.h"
#endif
#include <time.h>

#if BATCH_TEXT
void batch_test(Window & window, glm::vec2 windowSize);
#endif
int main()
{
    //glm::vec2 windowSize = {800, 600};
    //glm::vec2 windowSize = {1280, 720};
    //glm::vec2 windowSize = {1366, 768};
    glm::vec2 windowSize = {1600, 900};
    //glm::vec2 windowSize = {1920, 1080};

    glm::vec2 resolution = windowSize;
    //glm::vec2 resolution = {windowSize.x * 2, windowSize.y * 2};
    //glm::vec2 resolution = {1280, 720};
    //glm::vec2 resolution = {1920, 1080};
    std::string title = "Free_Imperium";

    Window window(windowSize, title);
    if (glewInit() != GLEW_OK) {
        std::cout << "glew failed\n";
		return -1;
    }

   
   AM::Init();

#if MAP_TEST
    //newMapTest(window, resolution, windowSize);
    newTesMapTest(window, resolution, windowSize);
    
#elif GUI_PREVIEW
    GuiPreview::Play(window, resolution, windowSize);
#elif BATCH_TEST

    batch_test(window, windowSize);

#else
    Settings settings;
    settings.resolution = resolution;

    auto countries = CountryLoader::Load();
    std::sort(countries.begin(), countries.end(), [](const std::shared_ptr<Country> & c1, const std::shared_ptr<Country> & c2){
        return c1->GetId() < c2->GetId();
    });


    Menu menu(window, settings, countries);
    //Room room(window, settings, countries);
    //room.Play(true);
    menu.Play();
#endif

    AM::Dispose();
    //system("./../server/build/Word_and_Sword");
    //socket.disconnect();
    
}
#if BATCH_TEXT
void batch_test(Window & window, glm::vec2 windowSize)
{
    GLuint vao, vbo, ibo;
    int vertCount = 0, indicesCount = 0;
    Shader shader("src/graphics/shaders/gui_batch/a.vert", 
            "src/graphics/shaders/gui_batch/a.frag", "", ""); 
    Camera camera{window.GetSize()};
    glUseProgram(shader.GetProgram());
    glm::mat4 matrix = glm::ortho(0.0f, (float)windowSize.x, 0.0f, (float)windowSize.y);
    glUniformMatrix4fv(glGetUniformLocation(shader.GetProgram(), "pr_matrix"), 1, GL_FALSE, glm::value_ptr(matrix));
    int texID[32];
    for (int i = 0; i < 32; i++)
        texID[i] = i;
    glUniform1iv(glGetUniformLocation(shader.GetProgram(), "tex"), 32, texID);
Texture texture{"src/img/war.png", 40, 40}; 
texID[0] = texture.GetId();
texture.Bind();


    int maxSprites = 10000;
    Vertex * vertexData;// = new Vertex[maxSprites * 4];
    int vertexSize = sizeof(Vertex);
    int spriteSize = vertexSize * 4;
    int vertexDataSize = spriteSize * maxSprites;
    int indicesSize = maxSprites * 6;

    glGenVertexArrays(1, &vao);
    //glGenBuffers(1, &vao);
    glGenBuffers(1, &vbo);


     glBindVertexArray(vao);

glBindBuffer(GL_ARRAY_BUFFER, vbo);
glBufferData(GL_ARRAY_BUFFER, vertexDataSize, NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexSize, NULL);//(const GLvoid*)0);
    
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, vertexSize, (const GLvoid*)(offsetof(Vertex, Vertex::color)));//(const GLvoid*)(3 * GL_FLOAT));
                  
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, vertexSize, (const GLvoid*)(offsetof(Vertex, Vertex::tc)));//(const GLvoid*)(7 * GL_FLOAT));
                          
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, vertexSize, (const GLvoid*)(offsetof(Vertex, Vertex::textureId)));//(const GLvoid*)(7 * GL_FLOAT));
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLushort * indices = new GLushort[indicesSize];
    int offset = 0;
    for (int i = 0; i < indicesSize; i += 6) {
       indices[i] = offset + 0;
       indices[i + 1] = offset + 1;
       indices[i + 2] = offset + 2;
       indices[i + 3] = offset + 2;
       indices[i + 4] = offset + 3;
       indices[i + 5] = offset + 0;
        //indices[i] = offset + 0;
        //indices[i + 1] = offset + 1;
        //indices[i + 2] = offset + 2;
        //indices[i + 3] = offset + 0;
        //indices[i + 4] = offset + 2;
        //indices[i + 5] = offset + 3;
        
        offset += 4;
    }
    
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize * sizeof(GLushort), indices, GL_STATIC_DRAW);
    //glBufferData(GL_ARRAY_BUFFER, vertexSize, vertexData, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
GLenum err = glGetError();
std::cout << err;
Log("tete"); 
int i = 0;
   
//Vertex * vertexDataBase = vertexData;
//vertexData -= vertCount;
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
    //glBufferData(GL_ARRAY_BUFFER, vertexSize * sizeof(GLfloat), vertexDataBase, GL_STATIC_DRAW);
    //glBufferData(GL_ARRAY_BUFFER, maxSprites  * 4* sizeof(Vertex), vertexDataBase, GL_DYNAMIC_DRAW);
    //glBufferData(GL_ARRAY_BUFFER, maxSprites  * 4* sizeof(Vertex), vertexDataBase, GL_STATIC_DRAW);


   while (!window.ShouldClose()) {
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        vertexData = (Vertex*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
        //err = glGetError();
        //            std::cout << err;
        //            Log("ta3"); 
        window.Refresh();
    
        clock_t t1 = clock();
  
 vec2 size{50.0f, 50.0f};
        vec4 color{1.0f, 0.0f, 0.0f, 1.0f};
        vec3 pos{0.0f, 0.0f, 0.0f};
        vec2 tc{0.0f, 0.0f};
        for (i=0; i < maxSprites; ++i) {
            vertCount += 4;
            indicesCount += 6;
            vertexData->pos = pos;
            vertexData->color = color;
            vertexData->tc = tc;
            vertexData->textureId = 0;
            vertexData++;
            vertexData->pos = vec3{pos.x, pos.y + size.y, pos.z};
            vertexData->color = color;
            vertexData->tc = vec2{tc.x, tc.y + 1.0f};
            vertexData->textureId = 0;
            vertexData++;
            vertexData->pos = vec3{pos.x + size.x, pos.y + size.y, pos.z};
            vertexData->color = color;
            vertexData->tc = vec2{tc.x + 1.0f, tc.y + 1.0f};
            vertexData->textureId = 0;
            vertexData++;
            vertexData->pos = vec3{pos.x + size.x, pos.y, pos.z};
            vertexData->color = color;
            vertexData->tc = vec2{tc.x + 1.0f, tc.y};
            vertexData->textureId = 0;
            if (maxSprites - 1 == i)
                vertexData->pos.y += 20.0f;
            vertexData++;
            
            pos.x += size.x + 1.0f;
            color.x = 1.0f / (float)i;
            if (pos.x + size.x >= windowSize.x) {
                pos.x = 0; pos.y += size.y + 1.0f;
            }
            if (i  % 1000 == 0) {//== maxSprites  / 2) {
                color.x = 1.0f; //color.y = 1.0f;
            }
        }
        

      

        ////////////////////////
        clock_t t2 = clock();
        Log("Fill buffer: " << t2 - t1);
        Log("Fill ms: " << (float)(t2-t1) / CLOCKS_PER_SEC);
    //Log("przed lupem");
        //
        //glBindVertexArray(vao);
        //glBindBuffer(GL_ARRAY_BUFFER, vbo);
        //glDrawElements(GL_TRIANGLE_FAN, indicesCount, GL_UNSIGNED_SHORT, NULL);
        //glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_SHORT, indices);
        //glDrawElements(GL_TRIANGLE_STRIP, indicesCount, GL_UNSIGNED_SHORT, NULL);
    //glBufferData(GL_ARRAY_BUFFER, vertexSize * sizeof(GLfloat), vertexData, GL_DYNAMIC_DRAW);
    //glBufferData(GL_ARRAY_BUFFER, vertexSize * sizeof(GLfloat), vertexData, GL_STATIC_DRAW);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(vertexSize), vertexData, GL_DYNAMIC_DRAW);
    //glDrawArrays(GL_TRIANGLES, 0, indicesCount);
      //vertexData->pos = vec3{10.0f, 0.0f, 0.0f}; 
    
    //glBufferSubData(GL_ARRAY_BUFFER, 0, maxSprites *4*sizeof(Vertex),vertexDataBase);
    //glBufferData(GL_ARRAY_BUFFER, maxSprites  * 4* sizeof(Vertex), vertexDataBase, GL_DYNAMIC_DRAW);
    glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_SHORT, NULL);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        vertCount = 0;
        //vertexData -= vertCount;
        indicesCount = 0;
    glUnmapBuffer(GL_ARRAY_BUFFER);

        Log("Draw call: " << clock() - t2);
        window.Update();
        
        
    }
        delete [] indices;


}

#endif




