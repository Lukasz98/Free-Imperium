#include "new_map.h"

#include <cstring>
//#include <fstream>
#include <map>
#include <queue>
#include <set>
//#include <string>

#include "asset_manager.h"
#include "border_batch.h"
#include "color.h"
#include "graphics/texture.h"
#include "load_data.h"
#include "map_batch.h"
#include "save_data.h"

// int nodeHash(unsigned short x, unsigned short y)
int nodeHash(int x, int y)
{
    int r = x | (y << 16);
    // r = r | (((int)y) << 16);
    return r;
}
struct Node {
    int x = 2, y = 2, chunk;
    std::vector<int> next;
    std::vector<int> provId;
    bool visited = false;
};

struct Line {
    int x, y;
};

std::map<int, Node> nodes;
void saveBorders(const unsigned char* pix, int ww, int hh, std::vector<ProvData> provD);
std::vector<BorderVertex> loadBorders();

void newTesMapTest(Window& window, glm::vec2 resolution, glm::vec2 windowSize)
{
    GLuint err = glGetError();
    if (err)
        Log("Opengl error: " << err);
    Shader shader("src/graphics/shaders/tes_new_map/vert.v", "src/graphics/shaders/tes_new_map/frag.f",
                  "src/graphics/shaders/tes_new_map/tes_ster.ts", "src/graphics/shaders/tes_new_map/tes_w.tw");
    Shader borderShader{"src/graphics/shaders/borders/vert.v", "src/graphics/shaders/borders/frag.f", "", "",
                        "src/graphics/shaders/borders/geom.g"};
    Shader waterShader("src/graphics/shaders/water/vert.v", "src/graphics/shaders/water/frag.f",
                       "src/graphics/shaders/water/tes_ster.ts", "src/graphics/shaders/water/tes_w.tw");
    Shader colorMapShader("src/graphics/shaders/map_pick/vert.v", "src/graphics/shaders/map_pick/frag.f",
                          "src/graphics/shaders/map_pick/tes_ster.ts", "src/graphics/shaders/map_pick/tes_w.tw");
    Shader polyShader{"src/graphics/shaders/poly/vert.v", "src/graphics/shaders/poly/frag.f", "", ""};
    Camera camera{window.GetSize()};
    glUseProgram(shader.GetProgram());
    glPatchParameteri(GL_PATCH_VERTICES, 3);
    float trCount = 150;
    glUniform1f(glGetUniformLocation(shader.GetProgram(), "level"), trCount);
    err = glGetError();
    if (err)
        Log("Opengl error: " << err);

    int mapWidth = 5632, mapHeight = 2048;
    Texture terrainTexture{"src/img/terrain_map.png", mapWidth, mapHeight};
    Texture waterMap{"src/img/Blank_map.png", mapWidth, mapHeight};
    Texture heightMap{"src/img/Heightmap.png", mapWidth, mapHeight};
    Texture provTexture{"src/img/Provinces_org.png", mapWidth, mapHeight};
    Texture grassT{"../shared/grass1.png", 64, 64, GL_REPEAT};
    Texture stoneT{"../shared/smoothstone.png", 64, 64, GL_REPEAT};
    Texture waterT{"../shared/water1.png", 64, 64, GL_REPEAT};
    Texture sandT{"src/img/Sand_1.png", 32, 32, GL_REPEAT};
    Texture ctrsText{"src/img/countries_map.png", mapWidth, mapHeight};

    // saveProvinceFromImg(provTexture.GetPixels(), waterMap.GetPixels(), mapWidth, mapHeight);
    std::vector<ProvData> provinces;
    std::map<unsigned int, int> colorToId;
    std::vector<CountryData> ctrsData;
    loadProvData(provinces, colorToId);
    loadCountriesData(ctrsData);
    //// saveBorders(provTexture.GetPixels(), waterMap.GetPixels(), mapWidth, mapHeight);
    // saveBorders(provTexture.GetPixels(), mapWidth, mapHeight, provinces);
    // return;
    err = glGetError();
    if (err)
        Log("Opengl error: " << err);
    int texID[32];
    for (int i = 0; i < 32; i++) texID[i] = i;
    texID[0] = grassT.GetId();
    Log("grass: " << texID[0]);
    texID[1] = provTexture.GetId();
    Log("prov: " << texID[1]);
    texID[2] = stoneT.GetId();
    Log("stone: " << texID[2]);
    texID[3] = heightMap.GetId();
    Log("height: " << texID[3]);
    texID[4] = waterT.GetId();
    Log("water: " << texID[4]);
    texID[5] = terrainTexture.GetId();
    Log("terrain: " << texID[5]);
    texID[6] = sandT.GetId();
    texID[7] = AM::am.modelTexture->GetId();
    texID[8] = ctrsText.GetId();
    glUniform1iv(glGetUniformLocation(shader.GetProgram(), "tex"), 32, texID);
    err = glGetError();
    if (err)
        Log("Opengl error: " << err);

    for (int i = 0; i < 9; ++i) {
        // glActiveTexture((GLuint)texID[i]);
        // glActiveTexture(GL_TEXTURE0 + i);
        glActiveTexture(GL_TEXTURE0 + texID[i]);
        glBindTexture(GL_TEXTURE_2D, (GLuint)texID[i]);
        err = glGetError();
        if (err)
            Log("Opengl error: " << err << " " << i);
        Log("Opengl error: " << err << " " << (GLuint)texID[i]);
    }

    glUseProgram(waterShader.GetProgram());
    glUniform1f(glGetUniformLocation(waterShader.GetProgram(), "level"), 32);
    glUniform1iv(glGetUniformLocation(waterShader.GetProgram(), "tex"), 32, texID);

    glUseProgram(colorMapShader.GetProgram());
    glUniform1f(glGetUniformLocation(colorMapShader.GetProgram(), "level"), 32);
    glUniform1iv(glGetUniformLocation(colorMapShader.GetProgram(), "tex"), 32, texID);

    const unsigned char* pix = provTexture.GetPixels();
    const unsigned char* h = heightMap.GetPixels();

    Log("tu1");
    std::vector<MapVertex> vertexes;
    float tid = 0.0f;
    Vec4 color{1.0f, 0.0f, 0.0f, 1.0f};
    float scale = 4.0f;
    float w = 64.0f;  // * scale;
    float ww = w * scale;
    float x = 0.0f, y = 0.0f, z = 0.0f;
    glm::vec2 tCL{w / mapWidth, w / mapHeight};
    glm::vec2 texC{0.0f, 0.0f};
    for (int i = 0; i < mapHeight; i += w) {
        for (int j = 0; j < mapWidth / 1; j += w) {
            Vec2 tc1{texC.x, texC.y};
            Vec2 tc2{texC.x, texC.y + tCL.y};
            Vec2 tc3{texC.x + tCL.x, texC.y + tCL.y};
            Vec2 tc4{texC.x + tCL.x, texC.y};

            vertexes.push_back(MapVertex{.pos = Vec3{x, y, z}, .tc = tc1});       //, .normal=Vec2{0.0f, 0.0f} });
            vertexes.push_back(MapVertex{.pos = Vec3{x, y + ww, z}, .tc = tc2});  //, .normal=Vec2{0.0f, 0.0f} });
            vertexes.push_back(
                MapVertex{.pos = Vec3{x + ww, y + ww, z}, .tc = tc3});            //, .normal=Vec2{0.0f, 0.0f} });
            vertexes.push_back(MapVertex{.pos = Vec3{x + ww, y, z}, .tc = tc4});  //, .normal=Vec2{0.0f, 0.0f} });
            x += ww;
            texC.x += tCL.x;
        }
        x = 0;
        y += ww;
        texC.x = 0.0f;
        texC.y += tCL.y;
    }
    Log("tu");
    int chunkCount = vertexes.size() / 4;
    tid = 5.0f;
    color = {10.0f, 20.0f, 30.0f, 1.0f};

    MapBatch batch;
    batch.Init();

    BorderBatch borderBatch;
    borderBatch.Init();

    err = glGetError();
    if (err)
        Log("Opengl error: " << err);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    bool chunkVisible[chunkCount] = {false};
    std::vector<BorderVertex> borVerts;
    std::vector<int> bordChunkId;
    std::fstream file;
    file.open("BordersData.txt", std::fstream::in);
    std::string ss;
    while (file >> ss) {
        int x1 = std::atoi(ss.data());
        file >> ss;
        int y1 = std::atoi(ss.data());
        file >> ss;
        int x2 = std::atoi(ss.data());
        file >> ss;
        int y2 = std::atoi(ss.data());
        // file >> ss;
        int chunkX = x1 / (int)w;
        int chunkY = y1 / (int)w;
        int chunkId = chunkX + chunkY * (mapWidth / w);
        int i1 = x1 * 3 + y1 * mapWidth * 3;
        if (i1 > mapHeight * mapWidth * 3 - 3)
            i1 = mapHeight * mapWidth * 3 - 3;
        int i2 = x2 * 3 + y2 * mapWidth * 3;
        if (i2 > mapHeight * mapWidth * 3 - 3)
            i2 = mapHeight * mapWidth * 3 - 3;
        borVerts.push_back(
            // BorderVertex{.pos = Vec3{((float)node.x + 0.5f) * scale, ((float)node.y + 0.5f) * scale, 200.0},
            BorderVertex{.pos = Vec3{((float)x1) * scale, ((float)y1) * scale, h[i1]},
                         .tc = Vec2{(float)x1 / mapWidth, (float)y1 / mapHeight}});
        borVerts.push_back(
            BorderVertex{.pos = Vec3{((float)x2) * scale, ((float)y2) * scale, h[i2]},
                         //.pos = Vec3{((float)x2 + 0.5f) * scale, ((float)y2 + 0.5f) * scale, 200.0},
                         .tc = Vec2{(float)x2 / mapWidth, (float)y2 / mapHeight}});
        bordChunkId.push_back(chunkId);
    }
    file.close();
    Log("nodes.size: " << nodes.size());
    Log("borVerts.size: " << borVerts.size());

    struct PolyVert {
        float x, y, z;
        // float r = 1.0f, g = 0, b = 0, a = 1.0;
        glm::vec3 col{1.0f, 0.0f, 0.0f};
        float tx;
    };
    int polyCount = 0;
    struct Map {
        std::vector<PolyVert> pverts;
        GLuint polyVao, polyVbo;
    };
    Map initMap, countryMap;
    {  // load prov polygon
        file.open("polygon.txt", std::fstream::in);
        std::string ss;
        int provId = -1;
        int vertCount = 0;
        while (file >> ss) {
            if (ss == "id:") {
                file >> ss;
                if (provId != -1)
                    provinces[provId].vertCount = vertCount;

                provId = std::atoi(ss.data());
                file >> ss;
                provinces[provId].firstVertId = initMap.pverts.size();
                vertCount = 0;
            }
            int x1 = std::atoi(ss.data());
            file >> ss;
            int y1 = std::atoi(ss.data());
            file >> ss;
            int x2 = std::atoi(ss.data());
            file >> ss;
            int y2 = std::atoi(ss.data());
            file >> ss;
            int x3 = std::atoi(ss.data());
            file >> ss;
            int y3 = std::atoi(ss.data());
            float z = 200.0f;
            glm::vec3 col{(float)provinces[provId].r / 255.0f, (float)provinces[provId].g / 255.0f,
                          (float)provinces[provId].b / 255.0f};
            float tx = (float)provId + 0.5f;
            initMap.pverts.push_back(PolyVert{.x = x1 * scale, .y = y1 * scale, .z = z, .col = col, .tx = tx});
            initMap.pverts.push_back(PolyVert{.x = x2 * scale, .y = y2 * scale, .z = z, .col = col, .tx = tx});
            initMap.pverts.push_back(PolyVert{.x = x3 * scale, .y = y3 * scale, .z = z, .col = col, .tx = tx});
            vertCount += 3;
            // if (pverts.size() > 1500) break;
        }
        polyCount = initMap.pverts.size();
        glCreateVertexArrays(1, &initMap.polyVao);
        glBindVertexArray(initMap.polyVao);
        glCreateBuffers(1, &initMap.polyVbo);

        glBindBuffer(GL_ARRAY_BUFFER, initMap.polyVbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(PolyVert) * initMap.pverts.size(), initMap.pverts.data(),
                     GL_STATIC_DRAW);
        glEnableVertexArrayAttrib(initMap.polyVao, 0);
        glEnableVertexArrayAttrib(initMap.polyVao, 1);
        glEnableVertexArrayAttrib(initMap.polyVao, 2);
        GLuint err = glGetError();
        if (err)
            Log("Opengl error: " << err);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(PolyVert), NULL);  //(const GLvoid*)0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(PolyVert),
                              (const GLvoid*)(offsetof(PolyVert, PolyVert::col)));
        glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(PolyVert),
                              (const GLvoid*)(offsetof(PolyVert, PolyVert::tx)));
        Log(polyCount * sizeof(PolyVert));
    }
    float mapCreateTime = glfwGetTime();
    {  // copy to country map
        countryMap.pverts = std::vector<PolyVert>{initMap.pverts};
        for (auto& prov : provinces) {
            if (prov.ctrId <= -1)
                continue;

            glm::vec3 col{(float)ctrsData[prov.ctrId].r / 255.0f, (float)ctrsData[prov.ctrId].g / 255.0f,
                         (float)ctrsData[prov.ctrId].b / 255.0f};
            for (int i = prov.firstVertId; i < prov.firstVertId + prov.vertCount; ++i) {
                if (i < countryMap.pverts.size())
                    countryMap.pverts[i].col = col;
                else {
                    break;
                }
            }
        }
        mapCreateTime = glfwGetTime();
        glCreateVertexArrays(1, &countryMap.polyVao);
        glBindVertexArray(countryMap.polyVao);
        glCreateBuffers(1, &countryMap.polyVbo);

        glBindBuffer(GL_ARRAY_BUFFER, countryMap.polyVbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(PolyVert) * countryMap.pverts.size(), countryMap.pverts.data(),
                     GL_STATIC_DRAW);
        glEnableVertexArrayAttrib(countryMap.polyVao, 0);
        glEnableVertexArrayAttrib(countryMap.polyVao, 1);
        glEnableVertexArrayAttrib(countryMap.polyVao, 2);
        GLuint err = glGetError();
        if (err)
            Log("Opengl error: " << err);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(PolyVert), NULL);  //(const GLvoid*)0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(PolyVert),
                              (const GLvoid*)(offsetof(PolyVert, PolyVert::col)));
        glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(PolyVert),
                              (const GLvoid*)(offsetof(PolyVert, PolyVert::tx)));
    }
    Log("mapTime: " << glfwGetTime() - mapCreateTime);

    glUseProgram(borderShader.GetProgram());
    glUniform1iv(glGetUniformLocation(borderShader.GetProgram(), "tex"), 32, texID);
    bool drawBorders = true;
    glm::vec3 provColor;
    float markedProvId = -1.0f;
    float tesLevel = 32.0f;
    glm::vec3 unitPos;
    float dt = 0.0f, waterTime = 0.0f;
    float time = glfwGetTime();
    while (!window.ShouldClose()) {
        // glEnable(GL_DEPTH_TEST);   // Enable depth testing for z-culling
        // glDepthFunc(GL_LESS);
        window.Refresh();
        if (window.keys['A'])
            camera.MoveHor(-1, dt);
        if (window.keys['D'])
            camera.MoveHor(1, dt);
        if (window.keys['W'])
            camera.MoveVer(1, dt);
        if (window.keys['S'])
            camera.MoveVer(-1, dt);
        if (window.keys['F'])
            camera.Rotate(1, dt);
        if (window.keys['T'])
            camera.Rotate(-1, dt);
        if (window.keys['M']) {
            shader = Shader("src/graphics/shaders/tes_new_map/vert.v", "src/graphics/shaders/tes_new_map/frag.f",
                            "src/graphics/shaders/tes_new_map/tes_ster.ts",
                            "src/graphics/shaders/tes_new_map/tes_w.tw");
            borderShader =
                Shader{"src/graphics/shaders/borders/vert.v", "src/graphics/shaders/borders/frag.f", "", ""};
            waterShader = Shader("src/graphics/shaders/water/vert.v", "src/graphics/shaders/water/frag.f",
                                 "src/graphics/shaders/water/tes_ster.ts", "src/graphics/shaders/water/tes_w.tw");
            colorMapShader =
                Shader("src/graphics/shaders/map_pick/vert.v", "src/graphics/shaders/map_pick/frag.f",
                       "src/graphics/shaders/map_pick/tes_ster.ts", "src/graphics/shaders/map_pick/tes_w.tw");
        }

        if (window.scrollOffset) {
            camera.Scroll(window.scrollOffset);
            window.scrollOffset = 0;
            // float tesLevel = camera.GetFovDelta() * 20.0f + 15.0f;
            tesLevel = camera.GetScrollPerc() + 15.0f;
            // tesLevel = sin(time) * 64.0f;
            if (tesLevel < 0.0f)
                tesLevel *= -1.0f;
            // Log(tesLevel);
            tesLevel = 32.0f;
            glUseProgram(shader.GetProgram());
            glUniform1f(glGetUniformLocation(shader.GetProgram(), "level"), tesLevel);
        }
        camera.Update(window.xMouse, windowSize.y - window.yMouse, pix);
        glm::mat4 matrix = camera.GetMat();

        for (int i = 0, c = 0; i < vertexes.size(); i += 4, ++c) {
            if (camera.IsPointInFrustum(glm::vec3{vertexes[i].pos.x, vertexes[i].pos.y, vertexes[i].pos.z}) ||
                camera.IsPointInFrustum(
                    glm::vec3{vertexes[i + 1].pos.x, vertexes[i + 1].pos.y, vertexes[i + 1].pos.z}) ||
                camera.IsPointInFrustum(
                    glm::vec3{vertexes[i + 2].pos.x, vertexes[i + 2].pos.y, vertexes[i + 2].pos.z}) ||
                camera.IsPointInFrustum(
                    glm::vec3{vertexes[i + 3].pos.x, vertexes[i + 3].pos.y, vertexes[i + 3].pos.z}))
            {
                chunkVisible[c] = true;
            }
            else
                chunkVisible[c] = false;
            // batch.Push(&vertexes[i]);
        }

        if (window.keys['I']) {
            // glUseProgram(colorMapShader.GetProgram());
            // glUniformMatrix4fv(glGetUniformLocation(colorMapShader.GetProgram(), "pr_matrix"), 1, GL_FALSE,
            //                   glm::value_ptr(matrix));
            // glUniformMatrix4fv(glGetUniformLocation(colorMapShader.GetProgram(), "matrix"), 1, GL_FALSE,
            //                   glm::value_ptr(matrix));
            /*
            batch.Begin();
            for (int i = 0, c = 0; i < vertexes.size(); i += 4, ++c) {
                if (chunkVisible[c])
                    batch.Push(&vertexes[i]);
            }
            batch.Flush();
            */
            glUseProgram(polyShader.GetProgram());
            glUniformMatrix4fv(glGetUniformLocation(polyShader.GetProgram(), "matrix"), 1, GL_FALSE,
                               glm::value_ptr(matrix));
            glUniform1iv(glGetUniformLocation(polyShader.GetProgram(), "tex"), 32, texID);

            glBindVertexArray(initMap.polyVao);
            glBindBuffer(GL_ARRAY_BUFFER, initMap.polyVbo);
            glDrawArrays(GL_TRIANGLES, 0, polyCount);

            unsigned char pixel[4];
            int pixx = window.xMouse, pixy = windowSize.y - window.yMouse;
            glReadPixels(pixx, pixy, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);

            window.Refresh();
            provColor = {(int)pixel[0] / 255.0, (int)pixel[1] / 255.0, (int)pixel[2] / 255.0};
            unsigned int phash = getHash(pixel[0], pixel[1], pixel[2]);
            if (colorToId.find(phash) != colorToId.end()) {
                int pid = colorToId[phash];
                markedProvId = (float)pid + 0.5f;
                //Log(provinces[pid].id << ", water: " << provinces[pid].water << ", " << provinces[pid].name
                //                      << ", col: " << provinces[pid].r << ", " << provinces[pid].g << ", "
                //                      << provinces[pid].b);
                //std::cout << "Neighb: ";
                //for (auto i : provinces[pid].neighb) std::cout << i << " ";
                //std::cout << "\n";
                //std::cout << "NeighbSea: ";
                //for (auto i : provinces[pid].neighbSea) std::cout << i << " ";
                //std::cout << "\n";
                unitPos.x = provinces[pid].x * scale;
                unitPos.y = provinces[pid].y * scale;
                unitPos.z = heightMap.GetPixels()[(int)(provinces[pid].x * 3 + provinces[pid].y * mapWidth * 3)];
            }
            //std::cout << "R: " << (double)pixel[0] << "< ";
            //std::cout << "G: " << (int)pixel[0] << "< ";
            //std::cout << "B: " << (int)pixel[2] << "< \n";
        }

        glUseProgram(shader.GetProgram());
        glUniform1f(glGetUniformLocation(shader.GetProgram(), "level"), tesLevel);
        glUniform3fv(glGetUniformLocation(shader.GetProgram(), "provColor"), 1, glm::value_ptr(provColor));
        glUniformMatrix4fv(glGetUniformLocation(shader.GetProgram(), "pr_matrix"), 1, GL_FALSE,
                           glm::value_ptr(matrix));

        glUniformMatrix4fv(glGetUniformLocation(shader.GetProgram(), "matrix"), 1, GL_FALSE,
                           glm::value_ptr(matrix));

        if (window.keys['L'])
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        if (window.keys['K'])
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        if (window.keys['B']) {
            drawBorders = !drawBorders;
        }

        //    glUniform1f(glGetUniformLocation(shader.GetProgram(), "waterTime"), waterTime);

        glUniform1f(glGetUniformLocation(shader.GetProgram(), "borderTime"), waterTime);
        glUniform1iv(glGetUniformLocation(shader.GetProgram(), "tex"), 32, texID);

        batch.Begin();
        // for (int i = 0, c = 0; i < vertexes.size(); i += 4, ++c) {
        //    if (chunkVisible[c]) {
        //        batch.Push(&vertexes[i]);
        //    }
        //}
        /*
        for (int i = 0; i < vertexes.size(); i += 4) {
            // if (camera.IsPointInFrustum((glm::vec3)vertexes[i].pos))
            if (camera.IsPointInFrustum(glm::vec3{vertexes[i].pos.x, vertexes[i].pos.y, vertexes[i].pos.z}) ||
                camera.IsPointInFrustum(
                    glm::vec3{vertexes[i + 1].pos.x, vertexes[i + 1].pos.y, vertexes[i + 1].pos.z}) ||
                camera.IsPointInFrustum(
                    glm::vec3{vertexes[i + 2].pos.x, vertexes[i + 2].pos.y, vertexes[i + 2].pos.z}) ||
                camera.IsPointInFrustum(
                    glm::vec3{vertexes[i + 3].pos.x, vertexes[i + 3].pos.y, vertexes[i + 3].pos.z}))
                batch.Push(&vertexes[i]);
        }
        */
        batch.Flush();

        glUseProgram(waterShader.GetProgram());
        glUniform1f(glGetUniformLocation(waterShader.GetProgram(), "level"), 32);
        glUniform1iv(glGetUniformLocation(waterShader.GetProgram(), "tex"), 32, texID);
        glUniformMatrix4fv(glGetUniformLocation(waterShader.GetProgram(), "pr_matrix"), 1, GL_FALSE,
                           glm::value_ptr(matrix));

        glUniformMatrix4fv(glGetUniformLocation(waterShader.GetProgram(), "matrix"), 1, GL_FALSE,
                           glm::value_ptr(matrix));
        glUniform1f(glGetUniformLocation(waterShader.GetProgram(), "waterTime"), waterTime);

        batch.Begin();
        MapVertex vertsWater[8];
        vertsWater[0] = MapVertex{.pos = Vec3{0.0, 0.0, 0.0}, .tc = Vec2{0.0, 0.0}};
        vertsWater[1] = MapVertex{.pos = Vec3{mapWidth * scale * 0.5, 0.0, 0.0}, .tc = Vec2{0.5, 0.0}};
        vertsWater[2] =
            MapVertex{.pos = Vec3{mapWidth * scale * 0.5, mapHeight * scale, 0.0}, .tc = Vec2{.5, 1.0}};
        vertsWater[3] = MapVertex{.pos = Vec3{0.0, mapHeight * scale, 0.0}, .tc = Vec2{0.0, 1.0}};

        vertsWater[4] = MapVertex{.pos = Vec3{mapWidth * scale * 0.5, 0.0, 0.0}, .tc = Vec2{0.5, 0.0}};
        vertsWater[5] = MapVertex{.pos = Vec3{mapWidth * scale, 0.0, 0.0}, .tc = Vec2{1.0, 0.0}};
        vertsWater[6] = MapVertex{.pos = Vec3{mapWidth * scale, mapHeight * scale, 0.0}, .tc = Vec2{1.0, 1.0}};
        vertsWater[7] =
            MapVertex{.pos = Vec3{mapWidth * scale * 0.5, mapHeight * scale, 0.0}, .tc = Vec2{0.5, 1.0}};

        batch.Push(vertsWater);
        batch.Push(&vertsWater[4]);
        batch.Flush();

        {  // poly
            glUseProgram(polyShader.GetProgram());
            glUniformMatrix4fv(glGetUniformLocation(polyShader.GetProgram(), "matrix"), 1, GL_FALSE,
                               glm::value_ptr(matrix));
            glUniform1iv(glGetUniformLocation(polyShader.GetProgram(), "tex"), 32, texID);
            glUniform3fv(glGetUniformLocation(polyShader.GetProgram(), "provColor"), 1, glm::value_ptr(provColor));
            glUniform1f(glGetUniformLocation(polyShader.GetProgram(), "provId"), markedProvId);
            

            if (window.keys['P']) {
                glBindVertexArray(initMap.polyVao);
                glBindBuffer(GL_ARRAY_BUFFER, initMap.polyVbo);
                glDrawArrays(GL_TRIANGLES, 0, polyCount);
            }
            else {
                glBindVertexArray(countryMap.polyVao);
                glBindBuffer(GL_ARRAY_BUFFER, countryMap.polyVbo);
                glDrawArrays(GL_TRIANGLES, 0, polyCount);
            }
        }

        glUseProgram(borderShader.GetProgram());
        glUniformMatrix4fv(glGetUniformLocation(borderShader.GetProgram(), "matrix"), 1, GL_FALSE,
                           glm::value_ptr(matrix));
        if (drawBorders) {
            // glDisable(GL_DEPTH_TEST);   // Enable depth testing for z-culling
            // glDepthFunc(GL_GEQUAL);//QUAL);
            glUseProgram(borderShader.GetProgram());
            glUniform1iv(glGetUniformLocation(borderShader.GetProgram(), "tex"), 32, texID);
            borderBatch.Begin();
            double dxd = glfwGetTime();
            borderBatch.flushtime = 0.0;
            borderBatch.pushtime = 0.0;
            for (std::size_t i = 0, c = 0; i < borVerts.size(); i += 2, ++c) {
                if (chunkVisible[bordChunkId[c]])
                    borderBatch.Push(&borVerts[i]);
            }
            borderBatch.Flush();
            // Log("push" << borderBatch.pushtime);
            // Log("flush: " << borderBatch.flushtime);
            // Log(glfwGetTime() - dxd);
            // bordChunkId.push_back(chunkId);
        }

        {
            glm::mat4 unitModel = glm::mat4(1.0);
            unitModel = glm::translate(unitModel, unitPos);

            float rotateX = 60.0f * 3.1459265f / 180.0f, yScale = 10.0f;
            glm::mat4 rotate = glm::mat4{1.0f};
            rotate = glm::rotate(glm::mat4{1.0}, rotateX, glm::vec3{1.0, 0.0, 0.0});
            unitModel = unitModel * rotate;
            unitModel = glm::scale(unitModel, glm::vec3{20.0, yScale, 20.0});

            glUseProgram(AM::am.shader->GetProgram());
            glUniformMatrix4fv(glGetUniformLocation(AM::am.shader->GetProgram(), "matrix"), 1, GL_FALSE,
                               glm::value_ptr(matrix));
            glUniformMatrix4fv(glGetUniformLocation(AM::am.shader->GetProgram(), "ml"), 1, GL_FALSE,
                               glm::value_ptr(unitModel));
            glUniform1iv(glGetUniformLocation(AM::am.shader->GetProgram(), "tex"), 32, texID);

            // AM::am.model->DrawRect(model);
            AM::am.model->Draw();
        }
        float tt2 = glfwGetTime();

        window.Update();
        waterTime += dt;
        dt = glfwGetTime() - time;
        // Log(dt);
        time = glfwGetTime();
    }
}
struct HashPair {
    int left, right;
};
std::vector<BorderVertex> loadBorders() {}

Color currCol;
int provId;
const unsigned char* pix;
int wW, hH;
std::vector<int> left, right;
bool ok = false;

void bord2(const unsigned char* pix, int x, int y, int w, int h)
{
    std::map<int, int> tmp;
    std::queue<int> que;
    // int initx = x, inity = y;
    que.push(x | (y << 16));
    while (que.size()) {
        int key = que.front();
        que.pop();
        y = key >> 16;
        x = key ^ (y << 16);
        int nh = nodeHash(x, y);
        if (tmp.find(nh) != tmp.end())
            continue;
        tmp[nh] = nh;
        int lx = x - 1, ly1 = y, ly2 = y + 1;
        int tx1 = x, tx2 = x + 1, ty = y + 1;
        int rx = x + 1, ry1 = y + 1, ry2 = y;
        int bx1 = x + 1, bx2 = x, by = y - 1;

        int currI = x * 3 + y * w * 3;

        int nextI = lx * 3 + y * w * 3;
        if (lx < 0)
            nextI = (w - 1) * 3 + ly1 * w * 3;

        Color nc{pix[nextI + 0], pix[nextI + 1], pix[nextI + 2]};
        bool goL = (currCol == nc);
        if (goL == false) {
            int thash = nodeHash(lx + 1, ly1);
            if (nodes.find(thash) == nodes.end()) {
                nodes[thash] = Node{.x = lx + 1, .y = ly1};
            }
            int thash2 = nodeHash(lx + 1, ly2);
            if (nodes.find(thash2) == nodes.end()) {
                nodes[thash2] = Node{.x = lx + 1, .y = ly2};
            }
            nodes[thash].provId.push_back(provId);
            nodes[thash2].provId.push_back(provId);
            nodes[thash].next.push_back(thash2);
            nodes[thash2].next.push_back(thash);
        }
        else {
            que.push(lx | (ly1 << 16));
        }

        bool goT = false;
        if (ty < h)  // {
            nextI = tx1 * 3 + ty * w * 3;

        nc = Color{pix[nextI + 0], pix[nextI + 1], pix[nextI + 2]};
        goT = (currCol == nc);
        if (goT == false || ty >= h) {
            int thash = nodeHash(tx1, ty);
            if (nodes.find(thash) == nodes.end()) {
                nodes[thash] = Node{.x = tx1, .y = ty};
            }
            int thash2 = nodeHash(tx2, ty);
            if (nodes.find(thash2) == nodes.end()) {
                nodes[thash2] = Node{.x = tx2, .y = ty};
            }
            nodes[thash].next.push_back(thash2);
            nodes[thash2].next.push_back(thash);
            nodes[thash].provId.push_back(provId);
            nodes[thash2].provId.push_back(provId);
            // if (goL == false) {
            //    nodes[nodeHash(lx, ly2)].next.push_back(thash);
            //}
        }
        else {
            que.push(tx1 | (ty << 16));
        }
        //}

        bool goR = false;
        if (rx < w) {
            nextI = rx * 3 + ry2 * w * 3;
            nc = Color{pix[nextI + 0], pix[nextI + 1], pix[nextI + 2]};
            goR = (currCol == nc);
            if (goR == false) {
                int thash = nodeHash(rx, ry2);
                if (nodes.find(thash) == nodes.end()) {
                    nodes[thash] = Node{.x = rx, .y = ry2};
                }
                int thash2 = nodeHash(rx, ry1);
                if (nodes.find(thash2) == nodes.end()) {
                    nodes[thash2] = Node{.x = rx, .y = ry1};
                }
                nodes[thash].provId.push_back(provId);
                nodes[thash2].provId.push_back(provId);
                nodes[thash].next.push_back(thash2);
                nodes[thash2].next.push_back(thash);
                // if (goT == false) {
                //    nodes[nodeHash(tx2, ty)].next.push_back(thash2);
                //}
            }
            else {
                que.push(rx | (ry2 << 16));
            }
        }

        bool goB = false;
        if (by > 0) {
            nextI = bx2 * 3 + by * w * 3;
            nc = Color{pix[nextI + 0], pix[nextI + 1], pix[nextI + 2]};
            goB = (currCol == nc);
            if (goB == false) {
                int thash = nodeHash(bx2, by + 1);
                if (nodes.find(thash) == nodes.end())
                    nodes[thash] = Node{.x = bx2, .y = by + 1};
                int thash2 = nodeHash(bx1, by + 1);
                if (nodes.find(thash2) == nodes.end())
                    nodes[thash2] = Node{.x = bx1, .y = by + 1};
                nodes[thash].next.push_back(thash2);
                nodes[thash2].next.push_back(thash);
                nodes[thash].provId.push_back(provId);
                nodes[thash2].provId.push_back(provId);
                // if (goR == false) {
                //    nodes[nodeHash(rx, ry2)].next.push_back(thash2);
                //}
                // if (goL == false) {
                //    nodes[nodeHash(lx, ly1)].next.push_back(thash);
                //}
            }
            else {
                que.push(bx2 | (by << 16));
            }
        }
    }
}

int dirKind(int x1, int y1, int x2, int y2)
{
    if (x1 == x2)
        return 1;
    else if (y1 == y2)
        return 2;
    else if (x1 == x2 + 1 && y1 == y2 + 1)
        return 3;
    else if (x1 == x2 - 1 && y1 == y2 + 1)
        return 4;
    else if (x1 == x2 + 1 && y1 == y2 - 1)
        return 5;
    else if (x1 == x2 - 1 && y1 == y2 - 1)
        return 6;
}

int step(int x1, int y1, int x2, int y2, int x3, int y3)
{
    if (x1 + 1 == x2 && x2 == x3) {
        if (y1 == y2 && y2 + 1 == y3) {
            return 1;
        }
        if (y1 == y2 && y2 - 1 == y3) {
            return 1;
        }
    }
    else if (x1 - 1 == x2 && x2 == x3) {
        if (y1 == y2 && y2 + 1 == y3) {
            return 1;
        }
        if (y1 == y2 && y2 - 1 == y3) {
            return 1;
        }
    }

    return 0;
}

void saveBorders(const unsigned char* ppix, int ww, int hh, std::vector<ProvData> provD)
{
    // unsigned char * ppp = new unsigned char[ww * hh * 3];
    // for (int i = 0; i < ww * hh * 3; ++i) {
    //    ppp[i] = ppix[i];
    //}
    // bord2(ppp, ww, hh);
    // delete ppp[];
    // return;
    pix = ppix;
    // Log((long)ppix);
    wW = ww;
    hH = hh;
    Log("HH = " << hh);
    int ii = 0;
    for (auto& pd : provD) {
        if (pd.water)
            continue;
        provId = pd.id;
        //++ii;
        // if (ii == 1)
        //    Log(pd.r << ", " << pd.g << ", " << pd.b);
        currCol = Color{pd.r, pd.g, pd.b};
        left.clear();
        right.clear();
        // borRec(pd.x, pd.y, 1);
        Log(pd.id);
        bord2(pix, pd.x, pd.y, ww, hh);
        // break;
        // borRec(pd.x, pd.y, -1);
        Log(pd.id);
        // std::cout << std::endl;
    }
    Log("nodes: " << nodes.size());

    std::vector<int> toDelete;
    std::size_t initialSize = nodes.size();
    for (auto& alele : nodes) {
        auto& it = alele.second;
        for (int i = 0; i < it.next.size(); ++i) {
            for (int j = i + 1; j < it.next.size(); ++j) {
                if (it.next[i] == it.next[j]) {
                    it.next.erase(it.next.begin() + j);
                    --j;
                }
            }
        }
    }
    bool deleted = false;
    int deletedLines = 0;
    int endedat = deletedLines;
    bool onlyStep = 1;
label:
    do {
        endedat = deletedLines;
        deleted = 0;
        for (auto& alele : nodes) {
            // for (int i = 0; i < nodes.size(); ++i) {
            // auto it = nodes.begin() + i;
            auto& it = alele.second;
            for (auto n : it.next) {
                if (n == nodeHash(it.x, it.y))
                    continue;
                if (nodes[n].next.size() > 2)
                    continue;
                int kind = dirKind(it.x, it.y, nodes[n].x, nodes[n].y);
                if (kind == -1)
                    continue;
                int kind2 = -1;
                for (auto nn : nodes[n].next) {
                    if (n == nn || nn == nodeHash(it.x, it.y))
                        continue;
                    // if (nodes[nn].next.size() > 2)
                    //    continue;
                    if (onlyStep) {
                        if (step(it.x, it.y, nodes[n].x, nodes[n].y, nodes[nn].x, nodes[nn].y) == 0) {
                            continue;
                            kind2 = kind;
                        }
                    }
                    else {
                        kind2 = dirKind(nodes[n].x, nodes[n].y, nodes[nn].x, nodes[nn].y);
                        if (kind != kind2) {
                            continue;
                        }
                    }
                    ++deletedLines;
                    it.next.push_back(nn);
                    nodes[nn].next.push_back(nodeHash(it.x, it.y));
                    for (int j = 0; j < nodes[nn].next.size(); ++j) {
                        if (nodes[nn].next[j] == n) {
                            nodes[nn].next.erase(nodes[nn].next.begin() + j);
                            --j;
                        }
                    }
                    for (int j = 0; j < nodes[n].next.size(); ++j) {
                        if (nodes[n].next[j] == nn) {
                            nodes[n].next.erase(nodes[n].next.begin() + j);
                            --j;
                        }
                    }
                    for (int j = 0; j < nodes[n].next.size(); ++j) {
                        if (nodes[n].next[j] == nodeHash(it.x, it.y)) {
                            nodes[n].next.erase(nodes[n].next.begin() + j);
                            --j;
                        }
                    }
                    for (int j = 0; j < it.next.size(); ++j) {
                        if (it.next[j] == n) {
                            it.next.erase(it.next.begin() + j);
                            --j;
                        }
                    }
                    deleted = true;
                    break;
                }
                if ((kind2 != -1 && kind2 == kind))
                    break;
            }
        }
        Log("deleted lines: " << deletedLines << ", endedat: " << endedat);
    } while ((deleted && deletedLines < 2000000) || deletedLines == endedat - 1);
    if (onlyStep) {
        onlyStep = false;
        goto label;
    }
    deleted = false;
    int deletedDuplicats = 0;
    do {
        deleted = false;
        for (auto& alele : nodes) {
            auto& it = alele.second;
            for (int i = 0; i < it.next.size(); ++i) {
                if (it.next[i] == nodeHash(it.x, it.y)) {
                    deletedDuplicats++;
                    deleted = true;
                    it.next.erase(it.next.begin() + i);
                    --i;
                }
            }
        }
        Log("deleted duplicats: " << deletedDuplicats);
    } while (deleted && 0);

    int deletedEmpty = 0;
    for (auto it = nodes.begin(); it != nodes.end();) {
        if (it->second.next.size() == 0) {
            it = nodes.erase(it);
            deletedEmpty++;
        }
        else {
            ++it;
        }
    }

    Log("deleted empty: " << deletedEmpty);
    Log("nodes size: " << nodes.size());
    /* // its for borders, for a moment disabling it for polygon provinces
    std::fstream f;
    f.open("BordersData.txt", std::fstream::out);
    int dell = 0;
    for (auto& alel : nodes) {
        auto& it = alel.second;
        for (int i = 0; i < it.next.size(); ++i) {
            int n = it.next[i];
            f << it.x << " " << it.y << " " << nodes[n].x << " " << nodes[n].y << '\n';

            for (int j = 0; j < nodes[n].next.size(); ++j) {
                if (nodes[n].next[j] == nodeHash(it.x, it.y)) {
                    nodes[n].next.erase(nodes[n].next.begin() + j);
                    dell++;
                    --j;
                }
            }
        }
    }
    f.close();
    Log("dell = " << dell);
    */

    /*
    for (auto& nk : nodes) {
        auto node = nk.second;
        f << node.x << " " << node.y << " | next : ";
        for (auto next : node.next) {
            f << nodes[next].x << " " << nodes[next].y << ", ";
        }
        f << '\n';
    }
    */

    std::fstream f2;
    f2.open("polygon.txt", std::fstream::out);
    for (auto& pd : provD) {
        if (pd.water)
            continue;

        f2 << "id: ";
        f2 << std::to_string(pd.id) << "\n";
        // create list
        int firstVert = -1;
        for (auto& alel : nodes) {
            auto& it = alel.second;
            if (std::find(it.provId.begin(), it.provId.end(), pd.id) != it.provId.end()) {
                firstVert = alel.first;
                break;
            }
        }
        assert(firstVert != -1);
        std::vector<int> verts;
        verts.push_back(firstVert);
        int vert = firstVert;
        for (int j = 0; j < nodes[vert].next.size(); ++j) {
            // Log("next size: " << nodes[vert].next.size() << ", vert: " << vert << ", j: " << j);
            if (std::find(verts.begin(), verts.end(), nodes[vert].next[j]) != verts.end())
                continue;
            if (std::find(nodes[nodes[vert].next[j]].provId.begin(), nodes[nodes[vert].next[j]].provId.end(),
                          pd.id) == nodes[nodes[vert].next[j]].provId.end())
                continue;
            verts.push_back(nodes[vert].next[j]);
            vert = nodes[vert].next[j];
            j = -1;
        }
        Log(pd.id);
        // Log("Duzy TEST");
        // Log("pid = " << pd.id << ", x:y = " << pd.x << ":" << pd.y);
        // Log("col: " << pd.r << " " << pd.g << " " << pd.b);
        // Log("raw:");
        // for (auto i : verts) {
        //    std::cout << nodes[i].x << ":" << nodes[i].y << ", ";
        //}
        // Log("");

        // erase from list
        for (int j = 0; j < verts.size(); ++j) {
            int next = j + 1;
            if (next >= verts.size())
                next = 0;
            if (next == j)
                break;
            int next2 = j + 2;
            if (next2 >= verts.size())
                next2 = next2 % verts.size();
            if (next2 == j)
                break;

            if ((nodes[verts[j]].x == nodes[verts[next]].x && nodes[verts[next]].x == nodes[verts[next2]].x) ||
                (nodes[verts[j]].y == nodes[verts[next]].y && nodes[verts[next]].y == nodes[verts[next2]].y))
            {
                verts.erase(verts.begin() + next);
                j = -1;
            }
            else {
                float a1 = (float)(nodes[verts[j]].y - nodes[verts[next]].y) /
                           (float)(nodes[verts[j]].x - nodes[verts[next]].x);
                float b1 = (float)nodes[verts[j]].y - a1 * (float)(nodes[verts[j]].x);
                float a2 = (float)(nodes[verts[j]].y - nodes[verts[next2]].y) /
                           (float)(nodes[verts[j]].x - nodes[verts[next2]].x);
                float b2 = (float)nodes[verts[j]].y - a2 * (float)(nodes[verts[j]].x);
                if (a1 == a2 && b1 == b2) {
                    verts.erase(verts.begin() + next);
                    j = -1;
                }
            }
        }
        /*
        //Log("row2");
        //        for (auto i : verts) {
        //            std::cout << nodes[i].x << ":" << nodes[i].y << ", ";
        //        }
        */
        // check if clockwise
        int sum = 0;
        for (int j = 0; j < verts.size(); ++j) {
            int next = (j + 1) % verts.size();
            int val = (nodes[verts[next]].x - nodes[verts[j]].x) * (nodes[verts[next]].y + nodes[verts[j]].y);
            sum += val;
        }
        if (sum < 0) {  // counter clockwise
            std::reverse(verts.begin(), verts.end());
        }
        // for (auto i : verts) {
        //   std::cout << nodes[i].x << ":" << nodes[i].y << ", ";
        // }
        // Log("");
        // Log("verts count: " << verts.size());
        int dkd = -1;
        while (verts.size() > 3) {
            for (int j = 0; j < verts.size() && verts.size() > 3; ++j) {
                int next = (j + 1) % verts.size();
                int prev = (j - 1);
                if (prev < 0)
                    prev = verts.size() - 1;
                glm::vec2 vp{(float)nodes[verts[prev]].x, (float)nodes[verts[prev]].y};
                glm::vec2 vj{(float)nodes[verts[j]].x, (float)nodes[verts[j]].y};
                glm::vec2 vn{(float)nodes[verts[next]].x, (float)nodes[verts[next]].y};
                glm::vec3 jToP{vp.x - vj.x, vp.y - vj.y, 0.0};
                glm::vec3 jToN{vn.x - vj.x, vn.y - vj.y, 0.0};
                glm::vec3 nToP{vp.x - vn.x, vp.y - vn.y, 0.0};
                // if (glm::cross(jToP, jToN)
                if (jToP.x * jToN.y - jToP.y * jToN.x < 0.0f) {  // cross product ?
                    continue;
                }
                bool ok = true;
                // czy jest w trojkacie
                for (int k = 0; k < verts.size(); ++k) {
                    if (k == j || k == prev || k == next)
                        continue;
                    glm::vec2 vk{(float)nodes[verts[k]].x, (float)nodes[verts[k]].y};
                    // glm::vec3 jToK{vk.x - vj.x, vk.y - vj.y, 0.0};
                    // glm::vec3 nToK{vk.x - vn.x, vk.y - vn.y, 0.0};
                    // glm::vec3 pToK{vk.x - vp.x, vk.y - vp.y, 0.0};
                    // float cr1 = (jToP.x * jToK.y - jToP.y * jToK.x);  // cross product ?
                    // float cr2 = (jToN.x * nToK.y - jToN.y * nToK.x);  // cross product ?
                    // float cr3 = (nToP.x * pToK.y - nToP.y * pToK.x);  // cross product ?
                    // if (cr1 > 0.0f || cr2 > 0.0f || cr3 > 0.0f) {
                    //    ok = false;
                    //    break;
                    //}
                    glm::vec2 ab = vj - vp;
                    glm::vec2 bc = vn - vj;
                    glm::vec2 ca = vp - vn;

                    glm::vec2 ap = vk - vp;
                    glm::vec2 bp = vk - vj;
                    glm::vec2 cp = vk - vn;

                    float cr1 = ab.x * ap.y - ab.y * ap.x;
                    float cr2 = bc.x * bp.y - bc.y * bp.x;
                    float cr3 = ca.x * cp.y - ca.y * cp.x;

                    if (cr1 <= 0.0f && cr2 <= 0.0f && cr3 <= 0.0f) {
                        ok = false;
                        break;
                    }
                }
                if (!ok)
                    continue;
                f2 << vp.x << " " << vp.y << " " << vj.x << " " << vj.y << " " << vn.x << " " << vn.y << "\n";
                verts.erase(verts.begin() + j);
                --j;
            }
            if (pd.id == 3703) {
                Log(verts.size());
            }
            if (dkd == verts.size()) {
                break;
            }
            dkd = verts.size();
        }
        for (int j = 0; j < verts.size() && j < 3; ++j) {
            f2 << nodes[verts[j]].x << " " << nodes[verts[j]].y << " ";
        }
        f2 << "\n";
        // Log("");

        // break;
    }
    f2.close();
}

