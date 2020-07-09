//#include <memory>
#include <array>
#include <algorithm>
#include <SFML/Network.hpp>
#include "log.h"
#include "graphics/window.h"
#include "graphics/shader.h"

#include "menu.h"
#include "room.h"
#include "game.h"

#include "asset_manager.h"
#include "settings.h"

#include "graphics/model_3d.h"

struct Prov {
    std::shared_ptr<Rectangle> rect;
    std::shared_ptr<Texture> t;
};
std::vector<std::shared_ptr<Rectangle>> getRects(Map *map);
std::vector<std::shared_ptr<Prov>> setTextures(Map *map, Texture *map2,std::vector<std::unique_ptr<Province>> & p);

std::shared_ptr<Texture> setCountryBorders(std::vector<std::shared_ptr<Country>> & countries, 
        std::vector<std::unique_ptr<Province>> & provinces, Map * map);
std::shared_ptr<Texture> setProvsBorders(Map * map);
//std::shared_ptr<Texture> setCountryBorders(Map * map);

int main()
{
    glm::vec2 windowSize = {1600, 900};
    glm::vec2 resolution = {1920, 1080};
    std::string title = "Free_Imperium";

    Window window(windowSize, title);
    if (glewInit() != GLEW_OK) {
        std::cout << "glew failed\n";
		return -1;
    }

    AM::Init();
//auto qoweih = std::make_shared<Texture>("src/img/unit.png", 20, 40);
    Settings settings;
    settings.resolution = resolution;
    auto countries = CountryLoader::Load();
    std::sort(countries.begin(), countries.end(), [](const std::shared_ptr<Country> & c1, const std::shared_ptr<Country> & c2){
        return c1->GetId() < c2->GetId();
    });
    Map map;
    auto provinces = ProvinceLoader::Load(map.GetPixels(), 1920, 1088);
    std::sort(provinces.begin(), provinces.end(), [](const std::unique_ptr<Province> & p1, const std::unique_ptr<Province> & p2) {
        return p1->GetId() < p2->GetId();
    });
    //setCountryMap(countries, provinces, &map);
Shader shader{"src/graphics/shaders/vert.v", "src/graphics/shaders/frag.f",
               "std/graphics/shaders/tes_ster.ts",
               "src/graphics/shaders/tes_w.tw"};

std::shared_ptr<Texture> texture = std::make_shared<Texture>("../shared/map_height.png", 1920, 1088);
Texture grassT{"../shared/grass1.png", 64, 64};
Texture stoneT{"../shared/smoothstone.png", 64, 64};
Texture water{"../shared/water2.png", 64, 64};
auto countryBorders = setCountryBorders(countries, provinces, &map);
auto provsBorders = setProvsBorders(&map);
Log(grassT.GetId());
auto rects = getRects(&map);
Log(rects.size());
    glUseProgram(shader.GetProgram());
    glPatchParameteri(GL_PATCH_VERTICES, 3);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    Camera camera{window.GetSize()};


Shader basicShader{"src/graphics/shaders/basic/vert.v",
                    "src/graphics/shaders/basic/frag.f", "", ""};
Rectangle testRect = Rectangle{
                    glm::vec3{0.0, 0.0, 1.0}, 
                    glm::vec3{0.0, 200.0, 1.0}, 
                    glm::vec3{100.0, 0.0, 1.0}, 
                    glm::vec3{100.0, 200.0, 1.0}, 
                    glm::vec4{1.0,0.0,0.0,1.0}
};
Rectangle unitRect = Rectangle{glm::vec3{50.0,50.0,0.1}, glm::vec2{40.0,15*4}};
Texture unitT = Texture{"src/img/unit_1.png", 40, 40};
//auto provs = setTextures(&map, &texture, provinces);

//Model3D model = Model3D{"src/img/xd_tux_0.blend.obj", glm::vec3{0.0, 0, 0.1}};
//Model3D model = Model3D{"src/img/DudeDonePose.obj", glm::vec3{0.0, 0, 0.1}};
Model3D model = Model3D{"src/img/DudeDonePosefix.obj", glm::vec3{0.0, 0, 0.1}};
Texture modelTexture = Texture{"src/img/Done3.png"};
Shader shader3d{"src/graphics/shaders/model_3d/vert.v",
                    "src/graphics/shaders/model_3d/frag.f", "", ""};

glm::vec3 modelPos{100.0,100.0,0.0};
//glm::vec3 modelPos{0.0,100.0,0.0};
    int frames = 0;
    float frameTime = 0.0f, waterTime = 0.0f;
    float dt = 0.0f, currTime = glfwGetTime();
    while (!window.ShouldClose()) {
        window.Refresh();
glm::mat4 matrix = camera.GetMat();
    
    glUseProgram(shader.GetProgram());
    glUniformMatrix4fv(glGetUniformLocation(shader.GetProgram(), "matrix"), 1, GL_FALSE, glm::value_ptr(matrix));
    {
        float fov = camera.GetFov();
        float fovDelta = camera.GetFovDelta();// - 0.5f;
        float d = 1.5f - fovDelta;
        float trCount = 20.0f;
        trCount += d * 44;
        //Log(trCount);
        glUniform1f(glGetUniformLocation(shader.GetProgram(), "level"), trCount);
    }
    glUniform1f(glGetUniformLocation(shader.GetProgram(), "waterTime"), waterTime);

        if (window.keys['A']) camera.MoveHor(-1, dt);
        if (window.keys['D']) camera.MoveHor(1,dt);
        if (window.keys['W']) camera.MoveVer(1,dt);
        if (window.keys['S']) camera.MoveVer(-1,dt);
        if (window.keys['R']) camera.Rotate(1, dt);
        if (window.keys['T']) camera.Rotate(-1, dt);
     if (window.scrollOffset) {
            camera.Scroll(window.scrollOffset);

        window.scrollOffset = 0.0f;
    }
#if 0
GLuint ts[] = {0,map.mapTexture->GetId(), texture.GetId()};
glBindTextures(ts[0], 3, ts);
//// glBindSamplers(ts[0], 1, ts);
 map.Draw();
 //ts[0] = 0; ts[1]=0;
 //glBindTextures(ts[0],2,ts);
#elseif 0
for (auto & p : provs) {
    //GLuint ts[] = {p->t->GetId(), p->rect->texture->GetId()};
    GLuint ts[] = { 0, p->rect->texture->GetId(), p->t->GetId()};
    glBindTextures(ts[0], 3, ts);
 // Log(ts[0]<< " "<<ts[1]);  
    p->rect->Draw(true);
//window.Update();
//break;
}
#else
GLuint ts[] = {0, map.mapTexture->GetId(), texture->GetId(), grassT.GetId(), stoneT.GetId(), water.GetId(), countryBorders->GetId(), provsBorders->GetId() };
//GLuint ts[] = {0, texture.GetId(), map.mapTexture->GetId()};
glBindTextures(ts[0], 8, ts);
for (auto & r : rects) {
    auto rpos = r->GetPosition();
    auto rsize = r->GetSize();
///*
    if (camera.IsPointInFrustum(rpos) ||
        camera.IsPointInFrustum(rpos + glm::vec3{rsize.x, 0,0}) ||
        camera.IsPointInFrustum(rpos + glm::vec3{0.0, rsize.y,0}) ||
        camera.IsPointInFrustum(rpos + glm::vec3{rsize.x, rsize.y, 0}))
        r->Draw(true);
//*/
}
#endif
glm::mat4 ml = glm::mat4(1.0);
GLuint tss[] = {0};
glBindTextures(tss[0], 1, tss);
glUseProgram(basicShader.GetProgram());
glUniformMatrix4fv(glGetUniformLocation(basicShader.GetProgram(), "matrix"), 1, GL_FALSE, glm::value_ptr(matrix));
glUniformMatrix4fv(glGetUniformLocation(basicShader.GetProgram(), "ml"), 1, GL_FALSE, glm::value_ptr(ml));
//testRect.Draw(false);
unitT.Bind();
unitRect.Draw();
unitT.Unbind();

texture->Bind();
Rectangle reop{glm::vec3{-1920.0, 0.0, 1.1}, glm::vec2{1920,1088}};
reop.SetTexture(texture);
reop.Draw();
texture->Unbind();


ml = glm::translate(ml, modelPos);
ml = glm::scale(ml, glm::vec3{10.0, 10.0, 10.0});
glm::mat4 rotat = glm::rotate(glm::mat4{1.0}, 20.0f, glm::vec3{1.0, 0.0, 0.0});
//ml = ml * rotat;
ml = glm::rotate(ml, 20.0f, glm::vec3{1.0, 0.0, 0.0});
//ml = glm::translate(ml, glm::vec3{0.0, 0.0, 1.0});

glUniformMatrix4fv(glGetUniformLocation(basicShader.GetProgram(), "ml"), 1, GL_FALSE, glm::value_ptr(ml));
model.DrawRect(ml);
glUseProgram(shader3d.GetProgram());
//ml = glm::rotate(ml, sin(waterTime) * 10, glm::vec3{0.0, 1.0, 0.0});
glUniformMatrix4fv(glGetUniformLocation(shader3d.GetProgram(), "matrix"), 1, GL_FALSE, glm::value_ptr(matrix));
glUniformMatrix4fv(glGetUniformLocation(shader3d.GetProgram(), "ml"), 1, GL_FALSE, glm::value_ptr(ml));
modelTexture.Bind();

model.Draw();


if (window.mouseL) {
    camera.Update(window.xMouse, window.size.y - window.yMouse, texture->GetOriginPixels());
    glm::vec3 mouse = camera.GetMouseInWorld();
    Log("MOUSE: " << mouse.x << ", " << mouse.y);
    int texX = 0.5f + mouse.x, texY = 0.5f + mouse.y;
    int texMapIndex = (1920 * (texY) + (texX));// + 0.5f;
    float colZ = texture->GetOriginPixels()[texMapIndex * 4 + 0];
    Log("Color: " <<colZ<<" x="<<texX<<" y="<<texY);
    if (colZ != 0.0f) colZ = colZ / 255.0f;
    float newZ = 20.0f * colZ;
    Log("newz="<<newZ);
    unitRect.SetPosition(glm::vec3{mouse.x, mouse.y,newZ});// mouse.z});
    window.mouseL = false;
    model.Click(ml, rotat, camera.rrrr, camera.GetEye());
    //model.Click(ml, mouse, camera.GetEye());
    //modelPos = glm::vec3{mouse.x, mouse.y, 0.1};
}

auto uPPos = unitRect.GetPosition();
if (uPPos.z > 0.1 && 0) {
    uPPos.z -= dt * 200.0;
    if (uPPos.z < 0.1)
        uPPos.z = 0.1;
    unitRect.SetPosition(uPPos);
}
        frames++;
        window.Update();
        dt = glfwGetTime() - currTime;

        waterTime += dt;    
        frameTime += dt;
        if (frameTime > 1.0f) {
            frameTime = 0.0;
            //Log("FPS: " << frames);
            frames = 0;
        }
    
        currTime = glfwGetTime();
    
    }

    //Menu menu(window, settings, countries);
    //Room room(window, settings, countries);
    //room.Play(true);
    //menu.Play();
    
    AM::Dispose();
    //system("./../server/build/Word_and_Sword");
    //socket.disconnect();
    
}

std::vector<std::shared_ptr<Rectangle>> getRects(Map *map)
{
    std::vector<std::shared_ptr<Rectangle>> rects;
    float w = 64.0f, wScale = 1.0f;
    float x = 0.0f, y = 0.0f;
    int mapW = 1920, mapH = 1088;
    //float texCLen = w / mapW;
    glm::vec2 tCL{w / mapW, w / mapH};
    glm::vec2 texC{0.0f, 0.0f};
    for (int i = 0; i < mapH; i += w) {
        for (int j = 0; j < mapW; j += w) {
            //glm::vec2 texC{(float)j / mapW, (float)i / mapH}; 
            //Log(texC.x);
            rects.push_back(
                std::make_shared<Rectangle>(glm::vec3{(float)j * wScale, i * wScale, -1.0}, glm::vec2{w * wScale, w * wScale}, texC, tCL)
            );
            texC.x += tCL.x;
        }
        texC.x = 0.0f;
        texC.y += tCL.y;
    }
    return rects;
}

std::shared_ptr<Texture> setCountryBorders(std::vector<std::shared_ptr<Country>> & countries,
         std::vector<std::unique_ptr<Province>> & provinces, Map *map)
{
    std::vector<std::pair<std::string, Color>> cColor;
    std::vector<std::pair<Color, std::string>> pColor;
    for (auto & c : countries)
        cColor.push_back(std::make_pair(c->GetName(), c->GetColor()));
    for (auto & p : provinces) {
        if (p->GetSieged() != 0)
            continue;
        pColor.push_back(std::make_pair(p->GetColor(), p->GetCountry()));
    }

    std::vector<std::pair<Color, Color>> rainbow;
    for (auto & p : pColor) {
        for (auto & c : cColor) {
            if (p.second == c.first) {
                rainbow.push_back(std::make_pair(p.first, c.second));
                break;
            }
        }    
    }
    map->DrawCountries(cColor, pColor);
    
    int originW = 1920, originH = 1088;
    const unsigned char * pixelsOrigin = map->mapTexture->GetPixels();
    unsigned char countryPixels[1920 * 1088 * 4];
    std::fill(countryPixels, countryPixels + 1920 * 1088 * 4, 0);
    Color lastColor{255,255,255,255};
    for (int y = 0; y < originH; y++) {
        for (int x = 0; x < originW; x++) {
            int index = y * originW * 4 + x * 4;
            Color currentColor{ pixelsOrigin[index + 0], pixelsOrigin[index + 1], pixelsOrigin[index + 2], pixelsOrigin[index + 3] };
            if (currentColor == lastColor)
                continue;
            lastColor = currentColor;
                
            for (int off = 0; off < 3 && x + off < originW; off++) {
                countryPixels[index + off * 4 + 0] = currentColor.r;
                countryPixels[index + off * 4 + 1] = currentColor.g;
                countryPixels[index + off * 4 + 2] = currentColor.b;
                countryPixels[index + off * 4 + 3] = 255 - (off + 1) * 60;
            }
        }
    }
    lastColor = {255,255,255,255};
    for (int y = 0; y < originH; y++) {
        for (int x = originW - 1; x >= 0; x--) {
            int index = y * originW * 4 + x * 4;
            Color currentColor{ pixelsOrigin[index + 0], pixelsOrigin[index + 1], pixelsOrigin[index + 2], pixelsOrigin[index + 3] };
            if (currentColor == lastColor)
                continue;
            lastColor = currentColor;
                
            for (int off = 0; off < 3 && x - off >= 0; off++) {
                countryPixels[index - off * 4 + 0] = currentColor.r;
                countryPixels[index - off * 4 + 1] = currentColor.g;
                countryPixels[index - off * 4 + 2] = currentColor.b;
                countryPixels[index - off * 4 + 3] = 255 - (off + 1) * 60;
            }
        }
    }
    lastColor = {255,255,255,255};
    for (int x = 0; x < originW; x++) {
        for (int y = originH - 1; y >= 0; y--) {
            int index = y * originW * 4 + x * 4;
            Color currentColor{ pixelsOrigin[index + 0], pixelsOrigin[index + 1], pixelsOrigin[index + 2], pixelsOrigin[index + 3] };
            if (currentColor == lastColor)
                continue;
            lastColor = currentColor;
                
            for (int off = 0; off < 3 && y - off >= 0; off++) {
                unsigned char myAlfa = 255 - (off + 1) * 60;
                unsigned char a = countryPixels[index - off * originW * 4 + 3];
                if (a > myAlfa)
                    continue;
                countryPixels[index - off * originW * 4 + 0] = currentColor.r;
                countryPixels[index - off * originW * 4 + 1] = currentColor.g;
                countryPixels[index - off * originW * 4 + 2] = currentColor.b;
                countryPixels[index - off * originW * 4 + 3] = myAlfa;
            }
        }
    }
    
    lastColor = {255,255,255,255};
    for (int x = 0; x < originW; x++) {
        for (int y = 0; y < originH; y++) {
            int index = y * originW * 4 + x * 4;
            Color currentColor{ pixelsOrigin[index + 0], pixelsOrigin[index + 1], pixelsOrigin[index + 2], pixelsOrigin[index + 3] };
            if (currentColor == lastColor)
                continue;
            lastColor = currentColor;
                
            for (int off = 0; off < 3 && y + off < originH; off++) {
                unsigned char myAlfa = 255 - (off + 1) * 60;
                unsigned char a = countryPixels[index + off * originW * 4 + 3];
                if (a > myAlfa)
                    continue;
                countryPixels[index + off * originW * 4 + 0] = currentColor.r;
                countryPixels[index + off * originW * 4 + 1] = currentColor.g;
                countryPixels[index + off * originW * 4 + 2] = currentColor.b;
                countryPixels[index + off * originW * 4 + 3] = myAlfa;
            }
        }
    }

    return std::make_shared<Texture>(countryPixels, originW, originH);
}

std::shared_ptr<Texture> setProvsBorders(Map * map)
{
    const int originW = 1920, originH = 1088;
    const unsigned char * pixelsOrigin = map->mapTexture->GetOriginPixels();
    //const int scale = 2;
    unsigned char provsPixels[originW * originH * 4];
    std::fill(provsPixels, provsPixels + originW * originH * 4, 0);
    //Log("ti");
    Color lastColor{255,255,255,255};
    Color bColor{50, 50, 50, 55};
    for (int y = 0; y < originH; y++) {
        for (int x = 0; x < originW; x++) {
            int index = y * originW * 4 + x * 4;
            Color currentColor{ pixelsOrigin[index + 0], pixelsOrigin[index + 1], pixelsOrigin[index + 2], pixelsOrigin[index + 3] };
            if (currentColor == lastColor)
                continue;
            lastColor = currentColor;
            //index = (y*scale) * originW * 4 + (x*scale) * 4;
            int off = 0;
            //for (int off = 0; off < 3 && x + off < originW; off++) {
                provsPixels[index + off * 4 + 0] = bColor.r;
                provsPixels[index + off * 4 + 1] = bColor.g;
                provsPixels[index + off * 4 + 2] = bColor.b;
                provsPixels[index + off * 4 + 3] = bColor.a;// - (off + 2) * 60;
            //}
        }
    }
    lastColor = {255,255,255,255};
    for (int y = 0; y < originH; y++) {
        for (int x = originW - 1; x >= 0; x--) {
            int index = y * originW * 4 + x * 4;
            Color currentColor{ pixelsOrigin[index + 0], pixelsOrigin[index + 1], pixelsOrigin[index + 2], pixelsOrigin[index + 3] };
            if (currentColor == lastColor)
                continue;
            lastColor = currentColor;
                
            int off = 0;
            //for (int off = 0; off < 3 && x - off >= 0; off++) {
                provsPixels[index - off * 4 + 0] = bColor.r;
                provsPixels[index - off * 4 + 1] = bColor.g;
                provsPixels[index - off * 4 + 2] = bColor.b;
                provsPixels[index - off * 4 + 3] = bColor.a;//255 - (off + 2) * 60;
            //}
        }
    }
    lastColor = {255,255,255,255};
    for (int x = 0; x < originW; x++) {
        for (int y = originH - 1; y >= 0; y--) {
            int index = y * originW * 4 + x * 4;
            Color currentColor{ pixelsOrigin[index + 0], pixelsOrigin[index + 1], pixelsOrigin[index + 2], pixelsOrigin[index + 3] };
            if (currentColor == lastColor)
                continue;
            lastColor = currentColor;
                
            int off = 0;
            //for (int off = 0; off < 3 && y - off >= 0; off++) {
            //    unsigned char myAlfa = 255 - (off + 2) * 60;
            //    unsigned char a = provsPixels[index - off * originW * 4 + 3];
            //    if (a > myAlfa)
            //        continue;
                provsPixels[index - off * originW * 4 + 0] = bColor.r;
                provsPixels[index - off * originW * 4 + 1] = bColor.g;
                provsPixels[index - off * originW * 4 + 2] = bColor.b;
                provsPixels[index - off * originW * 4 + 3] = bColor.a;//myAlfa;
            //}
        }
    }
    
    lastColor = {255,255,255,255};
    for (int x = 0; x < originW; x++) {
        for (int y = 0; y < originH; y++) {
            int index = y * originW * 4 + x * 4;
            Color currentColor{ pixelsOrigin[index + 0], pixelsOrigin[index + 1], pixelsOrigin[index + 2], pixelsOrigin[index + 3] };
            if (currentColor == lastColor)
                continue;
            lastColor = currentColor;
                
            int off = 0;
            //for (int off = 0; off < 3 && y + off < originH; off++) {
            //    unsigned char myAlfa = 255 - (off + 2) * 60;
            //    unsigned char a = provsPixels[index + off * originW * 4 + 3];
            //    if (a > myAlfa)
            //        continue;
                provsPixels[index + off * originW * 4 + 0] = bColor.r;
                provsPixels[index + off * originW * 4 + 1] = bColor.g;
                provsPixels[index + off * originW * 4 + 2] = bColor.b;
                provsPixels[index + off * originW * 4 + 3] = bColor.a;//myAlfa;
            //}
        }
    }

    return std::make_shared<Texture>(provsPixels, originW, originH);

   
}