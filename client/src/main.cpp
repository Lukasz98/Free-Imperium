//#include <memory>
#include <array>
#include <SFML/Network.hpp>
#include "log.h"
#include "graphics/window.h"
#include "graphics/shader.h"

#include "menu.h"
#include "room.h"
#include "game.h"

#include "asset_manager.h"
#include "settings.h"

struct Prov {
    std::shared_ptr<Rectangle> rect;
    std::shared_ptr<Texture> t;
};
std::vector<std::shared_ptr<Rectangle>> getRects(Map *map);
std::vector<std::shared_ptr<Prov>> setTextures(Map *map, Texture *map2,std::vector<std::unique_ptr<Province>> & p);

void setCountryMap(std::vector<std::shared_ptr<Country>> & countries, 
        std::vector<std::unique_ptr<Province>> & provinces, Map * map);

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

Texture texture{"../shared/map2.png", 1920, 1088};
Texture grassT{"../shared/grass1.png", 64, 64};
Texture stoneT{"../shared/smoothstone.png", 64, 64};
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

//auto provs = setTextures(&map, &texture, provinces);

    int frames = 0;
    float frameTime = 0.0f;
    float dt = 0.0f, currTime = glfwGetTime();
    while (!window.ShouldClose()) {
        window.Refresh();
glm::mat4 matrix = camera.GetMat();
    
    glUseProgram(shader.GetProgram());
    glUniformMatrix4fv(glGetUniformLocation(shader.GetProgram(), "matrix"), 1, GL_FALSE, glm::value_ptr(matrix));


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
GLuint ts[] = {0, map.mapTexture->GetId(), texture.GetId(), grassT.GetId(), stoneT.GetId() };
//GLuint ts[] = {0, texture.GetId(), map.mapTexture->GetId()};
glBindTextures(ts[0], 5, ts);
for (auto & r : rects)
r->Draw(true);
#endif
GLuint tss[] = {0};
glBindTextures(tss[0], 1, tss);
glUseProgram(basicShader.GetProgram());
glUniformMatrix4fv(glGetUniformLocation(basicShader.GetProgram(), "matrix"), 1, GL_FALSE, glm::value_ptr(matrix));
testRect.Draw(false);

        frames++;
        window.Update();
        dt = glfwGetTime() - currTime;
    
        frameTime += dt;
        if (frameTime > 1.0f) {
            frameTime = 0.0;
            Log("FPS: " << frames);
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
    float w = 64.0f;
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
                std::make_shared<Rectangle>(glm::vec3{(float)j, i, -1.0}, glm::vec2{w, w}, texC, tCL)
            );
            texC.x += tCL.x;
        }
        texC.x = 0.0f;
        texC.y += tCL.y;
    }
    return rects;
}

std::vector<std::shared_ptr<Prov>> setTextures(Map * map, Texture * map2, std::vector<std::unique_ptr<Province>> & provinces)
{
    std::vector<std::shared_ptr<Prov>> provs;

//    std::vector<std::shared_ptr<Texture>> texts;
    const unsigned char * pix = map->mapTexture->GetOriginPixels();
    for (auto & p : provinces) {
//        Log("tu1");
        auto color = p->GetColor();
        int y1 = 0, y2 = 0, x1 = 0, x2 = 0;
        for (int y = 0; y < 1080; y++) {
            for (int x = 0; x < 1920; x++) {
                int index = (y * 1920 + x) * 4;
                if (pix[index] == color.r && pix[index+1] == color.g && pix[index+2] == color.b) {
                    y1 = y;
                    break;
                }
            }
            if (y1 != 0) break;
        }
        for (int y =1080 - 1; y >= 0; y--) {
            for (int x = 0; x < 1920; x++) {
                int index = (y * 1920 + x) * 4;
                if (pix[index] == color.r && pix[index+1] == color.g && pix[index+2] == color.b) {
                    y2 = y;
                    break;
                }
            }
            if (y2 != 0) break;
        }

        for (int x = 0; x < 1920; x++) {
            for ( int y = 0; y < 1080; y++) {
                int index = (y * 1920 + x) * 4;
                if (pix[index] == color.r && pix[index+1] == color.g && pix[index+2] == color.b) {
                    x1 = x;
                    break;
                }
            }
            if (x1 != 0) break;
        }
        for (int x = 1920 - 1; x >= 0; x--) {
            for (int y = 0; y < 1080; y++) {
                int index = (y * 1920 + x) * 4;
                if (pix[index] == color.r && pix[index+1] == color.g && pix[index+2] == color.b) {
                    x2 = x;
                    break;
                }
            }
            if (x2 != 0) break;
        }
//Log("tu2");
        int w = x2 - x1, h = y2 - y1;
  //      Log("tu" << x1 << ","<<x2<<","<<y1<<","<<y2);
    //    Log("len = "<<w*h*4);
        std::vector<unsigned char> pix2; pix2.reserve(w*h*4);//{w*h*4};//, 0};//> pix2;
        std::vector<unsigned char> pix3; pix3.reserve(w*h*4);//{w*h*4};//, 0};//, w*h*4> pix3;
        const unsigned char * map2pix = map2->GetOriginPixels();
//Log("tu3");
        for (int x = 0; x < w; x++) {
            for (int y = 0; y < h; y++) {
                int index = ((y + y1)*1920 + x + x1) * 4;
                int index2 = (y * w + x) * 4;
                if (index+3 >= 1080*1920*4 || index2+3 >= w*h*4) {
                    Log("!!!!!!!!!!!!!!!!!!!!!");
                    Log("index="<<index<<" max="<<1080 * 1920*4);
                Log("index2="<<index2<<" max="<<w*h*4);
                }
                if (pix[index] == color.r && pix[index+1] == color.g && pix[index+2] == color.b) {
   // Log("no siema rysuje");
                    pix2[index2] = color.r;
                    pix2[index2 + 1] = color.g;
                    pix2[index2 + 2] = color.b;
                    pix2[index2 + 3] = color.a;
                    //break;
                    pix3[index2] = map2pix[index];
                }
                else {
                    pix2[index2 + 0] = 0;
                    pix2[index2 + 1] = 0;
                    pix2[index2 + 2] = 0;
                    pix2[index2 + 3] = 0;
                    pix3[index2] = 0;

                }
                    pix3[index2] = map2pix[index];
                //pix3[index2] = map2pix[index];//color.r;
                pix3[index2 + 1] = map2pix[index+1];//color.g;
                pix3[index2 + 2] = map2pix[index+2];//color.b;
                pix3[index2 + 3] = map2pix[index+3];//color.a;
                 
            }
        }
//Log("tu4");
//continue;
//auto d = pix2.data();
//for (int i = 0; i < w*h*4;i+=4){
//    Log((int)d[i] << " " << (int)d[i+1]<<" "<<(int)d[i+2]<<" "<<(int)d[i+3]);
//}
        auto t = std::make_shared<Texture>(pix2.data(), w, h);
//Log("tu6");
        provs.push_back(std::make_shared<Prov>());//emplace_back();
//Log("tu7");
        provs.back()->rect = std::make_shared<Rectangle>(glm::vec3((float)x1, y1, -30.5), glm::vec2((float)w, h));
//Log("tu8");
        provs.back()->rect->SetTexture(t);
//Log("tu9");
        provs.back()->t = std::make_shared<Texture>(pix3.data(), w, h);
//Log("tu10");
//        delete [] pix2;
  //      delete [] pix3;
  //      Log("tu" << x1 << ","<<x2<<","<<y1<<","<<y2);
    }
    return provs;
}



void setCountryMap(std::vector<std::shared_ptr<Country>> & countries,
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
    map->DrawCountries(cColor, pColor);

    for (auto & prov : provinces) {
        if (prov->GetSieged() == 100) {
            assert(prov->GetSiegeCountryId() >= 0 && prov->GetSiegeCountryId() < countries.size());
            auto siegeCountry = countries.at(prov->GetSiegeCountryId());
            //auto scIt = std::find_if(countries.begin(), countries.end(), [cccc = prov->GetSiegeCountry()](std::shared_ptr<Country> & ccc) {
            //             return ccc->GetName() == cccc;
            //            });
            //if (scIt != countries.end())
                map->DrawSieged(prov->GetColor(), siegeCountry->GetColor());
        }
    }
}
