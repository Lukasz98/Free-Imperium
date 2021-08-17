#include "new_map.h"

#include <cstring>
//#include <fstream>
#include <map>
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
    bool visited = false;
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
    Shader borderShader{"src/graphics/shaders/borders/vert.v", "src/graphics/shaders/borders/frag.f", "", ""};
    Shader waterShader("src/graphics/shaders/water/vert.v", "src/graphics/shaders/water/frag.f",
                       "src/graphics/shaders/water/tes_ster.ts", "src/graphics/shaders/water/tes_w.tw");
    Shader colorMapShader("src/graphics/shaders/map_pick/vert.v", "src/graphics/shaders/map_pick/frag.f",
                          "src/graphics/shaders/map_pick/tes_ster.ts", "src/graphics/shaders/map_pick/tes_w.tw");
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
    // saveBorders(provTexture.GetPixels(), waterMap.GetPixels(), mapWidth, mapHeight);
    saveBorders(provTexture.GetPixels(), mapWidth, mapHeight, provinces);

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

    std::vector<BorderVertex> borVerts;
    for (auto& nodeK : nodes) {
        auto node = nodeK.second;
        if (node.visited)
            continue;
        node.visited = 1;
        //if (borVerts.size() > 10)
        //    break;
        for (auto nn : node.next) {
            //Log("--------------");
            //Log((float)node.x << ", " << (float)node.y);
            //Log(nodes[nn].x << ", " << nodes[nn].y);
            //Log("-!-!-!-!-!");
            borVerts.push_back(BorderVertex{.pos = Vec3{(float)node.x, (float)node.y, 200.0}});
            borVerts.push_back(BorderVertex{.pos = Vec3{(float)nodes[nn].x, (float)nodes[nn].y, 200.0}});
        }
    }
    Log("nodes.size: " << nodes.size());
    Log("borVerts.size: " << borVerts.size());
    // borVerts.push_back(BorderVertex{.pos = Vec3{0.0, 0.0, 200.0}});        //, .normal=Vec2{0.0f, 0.0f} });
    // borVerts.push_back(BorderVertex{.pos = Vec3{1000.0, 1000.0, 200.0}});  //, .normal=Vec2{0.0f, 0.0f} });
    // borVerts.push_back(BorderVertex{.pos = Vec3{100.0, 0.0, 200.0}});      //, .normal=Vec2{0.0f, 0.0f} });
    // borVerts.push_back(BorderVertex{.pos = Vec3{1100.0, 1000.0, 200.0}});  //, .normal=Vec2{0.0f, 0.0f} });

    glUseProgram(borderShader.GetProgram());
    glUniform1iv(glGetUniformLocation(borderShader.GetProgram(), "tex"), 32, texID);

    float tesLevel = 32.0f;
    glm::vec3 unitPos;
    float dt = 0.0f, waterTime = 0.0f;
    float time = glfwGetTime();
    while (!window.ShouldClose()) {
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
        glm::vec3 provColor;
        camera.Update(window.xMouse, windowSize.y - window.yMouse, pix);
        glm::mat4 matrix = camera.GetMat();
        if (window.keys['I']) {
            glUseProgram(colorMapShader.GetProgram());
            glUniformMatrix4fv(glGetUniformLocation(colorMapShader.GetProgram(), "pr_matrix"), 1, GL_FALSE,
                               glm::value_ptr(matrix));
            glUniformMatrix4fv(glGetUniformLocation(colorMapShader.GetProgram(), "matrix"), 1, GL_FALSE,
                               glm::value_ptr(matrix));
            batch.Begin();
            for (int i = 0; i < vertexes.size(); i += 4) {
                if (camera.IsPointInFrustum(glm::vec3{vertexes[i].pos.x, vertexes[i].pos.y, vertexes[i].pos.z}) ||
                    camera.IsPointInFrustum(
                        glm::vec3{vertexes[i + 1].pos.x, vertexes[i + 1].pos.y, vertexes[i + 1].pos.z}) ||
                    camera.IsPointInFrustum(
                        glm::vec3{vertexes[i + 2].pos.x, vertexes[i + 2].pos.y, vertexes[i + 2].pos.z}) ||
                    camera.IsPointInFrustum(
                        glm::vec3{vertexes[i + 3].pos.x, vertexes[i + 3].pos.y, vertexes[i + 3].pos.z}))
                    batch.Push(&vertexes[i]);
            }
            batch.Flush();

            unsigned char pixel[4];
            int pixx = window.xMouse, pixy = windowSize.y - window.yMouse;
            glReadPixels(pixx, pixy, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);

            window.Refresh();
            provColor = {(int)pixel[0] / 255.0, (int)pixel[1] / 255.0, (int)pixel[2] / 255.0};
            unsigned int phash = getHash(pixel[0], pixel[1], pixel[2]);
            if (colorToId.find(phash) != colorToId.end()) {
                int pid = colorToId[phash];
                Log(provinces[pid].id << ", water: " << provinces[pid].water << ", " << provinces[pid].name
                                      << ", col: " << provinces[pid].r << ", " << provinces[pid].g << ", "
                                      << provinces[pid].b);
                std::cout << "Neighb: ";
                for (auto i : provinces[pid].neighb) std::cout << i << " ";
                std::cout << "\n";
                std::cout << "NeighbSea: ";
                for (auto i : provinces[pid].neighbSea) std::cout << i << " ";
                std::cout << "\n";
                unitPos.x = provinces[pid].x * scale;
                unitPos.y = provinces[pid].y * scale;
                unitPos.z = heightMap.GetPixels()[(int)(provinces[pid].x * 3 + provinces[pid].y * mapWidth * 3)];
            }
            std::cout << "R: " << (double)pixel[0] << "< ";
            std::cout << "G: " << (int)pixel[0] << "< ";
            std::cout << "B: " << (int)pixel[2] << "< \n";
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

        if (window.keys['B'])
            glUniform1f(glGetUniformLocation(shader.GetProgram(), "waterTime"), waterTime);

        glUniform1f(glGetUniformLocation(shader.GetProgram(), "borderTime"), waterTime);
        glUniform1iv(glGetUniformLocation(shader.GetProgram(), "tex"), 32, texID);

        batch.Begin();
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

        glUseProgram(borderShader.GetProgram());
        glUniformMatrix4fv(glGetUniformLocation(borderShader.GetProgram(), "matrix"), 1, GL_FALSE,
                           glm::value_ptr(matrix));

        borderBatch.Begin();
        for (std::size_t i = 0; i < borVerts.size(); i += 2) {
            borderBatch.Push(&borVerts[i]);
        }
        borderBatch.Flush();

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

std::vector<BorderVertex> loadBorders() {}

Color currCol;
const unsigned char* pix;
int wW, hH;
std::vector<int> left, right;
bool ok = false;

void borRec(int x, int y, int up)
{
   //if (x == 5471 && y == 19)
   if (x == 5485 && y == 81)
   Log("a");
    // if (!ok) {ok = 1; Log((long)pix); }
    while (1) {
        int lx = x;
        int llx = lx;// - 1;
        std::vector<int> nextRowX;
        int findUp = 1;
        while (1) {
            if (llx < 0)
                llx = wW - llx * -1;
            int lindex = llx * 3 + y * wW * 3;
            Color lcol{pix[lindex + 0], pix[lindex + 1], pix[lindex + 2]};
            if (lcol == currCol) {
                //           Log("h = " << hH);
                if (y + 1 * up < hH && y + 1 * up >= 0) {  // up
                    //                Log("AAAAAAAAAAAAAAAAAAAAAAAAAAA L: " << y + 1 * up);
                    int ulindex = llx * 3 + (y + 1 * up) * wW * 3;
                    Color ulCol{pix[ulindex + 0], pix[ulindex + 1], pix[ulindex + 2]};
                    if (ulCol == currCol) {
                        if (findUp) {
                            nextRowX.push_back(llx);
                            findUp = 0;
                        }
                    }
                    else if (findUp == 0) {
                        findUp = 1;
                    }
                }
                llx -= 1;
                continue;
            }
            else {
                lx = llx;
                break;
            }
        }
        int rx = x;
        int rrx = rx;// + 1;
        findUp = 1;
        while (1) {
            if (rrx >= wW)
                rrx = wW - rrx;
            int rindex = rrx * 3 + y * wW * 3;
            Color rcol{pix[rindex + 0], pix[rindex + 1], pix[rindex + 2]};
            //Log("rcol: " << (int)rcol.r << ", " << (int)rcol.g << ", " << (int)rcol.b);
            //Log("col: " << (int)currCol.r << ", " << (int)currCol.g << ", " << (int)currCol.b);
            if (rcol == currCol) {
                if (y + 1 * up < hH && y + 1 * up >= 0) {  // up
                    int urindex = rrx * 3 + (y + 1 * up) * wW * 3;
                    Color urCol{pix[urindex + 0], pix[urindex + 1], pix[urindex + 2]};
                    //                  Log("AAAAAAAAAAAAAAAAAAAAAAAAAAA R");
                    if (urCol == currCol) {
                        if (findUp) {
                            nextRowX.push_back(rrx);
                            findUp = 0;
                        }
                    }
                    else if (findUp == 0) {
                        findUp = 1;
                    }
                }
                rrx += 1;
                continue;
            }
            else {
                rx = rrx - 1;
                break;
            }
        }
        //Log("DDDDDDDDDDDD " << nextRowX.size());
        if (lx == 0 || rx == 0)
            Log("CO JEST");
        // if (left.size() || right.size())
        // Log("left: " << left.size() << ", right: " << right.size());
        // if (nextRowX.size()) Log("nextrow.size: " << nextRowX.size());
        if (left.size() && nodes[left.back()].x != lx) {
            int lhash = nodeHash(lx, y);
            if (nodes.find(lhash) == nodes.end()) {
                nodes[lhash] = Node{};
                nodes[lhash].x = lx;
                nodes[lhash].y = y;
            }
            // Log("leftPush");
            int lastHash = left.back();
            nodes[lastHash].next.push_back(lhash);
            left.push_back(lhash);
        }
        else if (left.size() == 0 || nextRowX.size() == 0) {
            int lhash = nodeHash(lx, y);
            if (nodes.find(lhash) == nodes.end()) {
                nodes[lhash] = Node{};
                nodes[lhash].x = lx;
                nodes[lhash].y = y;
            }
            left.push_back(lhash);
        }
        if (right.size() && nodes[right.back()].x != rx) {
            int rhash = nodeHash(rx, y);
            if (nodes.find(rhash) == nodes.end()) {
                nodes[rhash] = Node{};
                nodes[rhash].x = rx;
                nodes[rhash].y = y;
            }
            int lastHash = right.back();
            nodes[lastHash].next.push_back(rhash);
            // Log("rightPush");
            right.push_back(rhash);
        }
        else if (right.size() == 0 || nextRowX.size() == 0) {
            int rhash = nodeHash(rx, y);
            if (nodes.find(rhash) == nodes.end()) {
                nodes[rhash] = Node{};
                nodes[rhash].x = rx;
                nodes[rhash].y = y;
            }
            right.push_back(rhash);
        }
//Log("podejrzane1");
        for (std::size_t i = 1; i < nextRowX.size(); ++i) {
            if (nextRowX[i] == nextRowX[0]) {
                nextRowX.erase(nextRowX.begin() + 1);
                break;
            }
        }
        if (nextRowX.size() == 1) {
            x = nextRowX[0];
            y += 1 * up;
            continue;
        }
        else if (nextRowX.size() == 0) {
            int lhash = nodeHash(lx, y);
            int rhash = nodeHash(rx, y);
            nodes[lhash].next.push_back(rhash);
            break;
        }
        else {
            std::sort(nextRowX.begin(), nextRowX.end());
            int lastLeftH = -1;
            if (left.size())
                lastLeftH = left.back();
            int lastRightH = -1;
            if (right.size()) 
                lastRightH = right.back();
            left.clear();
            right.clear();
            for (std::size_t i = 0; i < nextRowX.size(); ++i) {
//Log("podejrzane2 y = " << y);
                if (i == 0) {
                    // Log("1");
                    if (lastLeftH != -1)
                        left.push_back(lastLeftH);
                    borRec(nextRowX[i], y + 1 * up, up);
                    left.clear();
                }
                else if (i == nextRowX.size() - 1) {
                    // Log("2");
                    if (lastRightH != -1)
                        right.push_back(lastRightH);
                    borRec(nextRowX[i], y + 1 * up, up);
                    right.clear();
                }
                else {
                    // Log("3");
                    borRec(nextRowX[i], y + 1 * up, up);
                }
            }

//Log("podejrzane3");
            break;
        }
        break;
    }
}

void saveBorders(const unsigned char* ppix, int ww, int hh, std::vector<ProvData> provD)
{
    pix = ppix;
    // Log((long)ppix);
    wW = ww;
    hH = hh;
    Log("HH = " << hh);
    int ii= 0;
    for (auto& pd : provD) {
        if (pd.water)
            continue;
        //++ii;
        //if (ii == 1)
        //    Log(pd.r << ", " << pd.g << ", " << pd.b);
        currCol = Color{pd.r, pd.g, pd.b};
        left.clear();
        right.clear();
        borRec(pd.x, pd.y, 1);
        //break;
        // borRec(pd.x, pd.y, -1);
        Log(pd.id);
        // std::cout << std::endl;
    }
    Log("nodes: " << nodes.size());

    std::fstream f;
    f.open("BordersData.txt", std::fstream::out);
    for (auto& nk : nodes) {
        auto node = nk.second;
        f << node.x << " " << node.y << " | next : ";
        for (auto next : node.next) {
            f << nodes[next].x << " " << nodes[next].y << ", ";
        }
        f << '\n';
    }
    f.close();
    /*
    for (auto & pd : provD) {
        int rbi = pd.x * 3 + pd.y * w * 3;
        int lbi = (pd.x - 1) * 3 + pd.y * w * 3;
        int rti = pd.x * 3 + (pd.y + 1) * w * 3;
        int lti = (pd.x - 1) * 3 + (pd.y + 1) * w * 3;
        if (pd.x - 1 < 0) {
            lbi = (w - 1) * 3 + pd.y * w * 3;
            lti = (w - 1) * 3 + (pd.y + 1) * w * 3;
        }
        if (pd.y >= h || pd.x >= w)
            continue;
        int nhash = nodeHash(pd.x, pd.y);
        nodes[nhash] = Node{pd.x, pd.y};
        Log(pd.id);
        currC{pd.r, pd.g, pd.b};
        int dir = -1;
        while (1) {
            Color rbiC{pix[rbi], pix[rbi + 1], pid[rbi + 2]};
            Color lbiC{pix[lbi], pix[lbi + 1], pid[lbi + 2]};
            Color rtiC{pix[rti], pix[rti + 1], pid[rti + 2]};
            Color ltiC{pix[lti], pix[lti + 1], pid[lti + 2]};

            if (rbiC == currC && rtiC == currC) {
                if (dir == 2) dir = 2 // down;
                else dir = 1; // up
            }
            else if (rbiC == currC && lbiC == currC) {
                if (dir == 0) dir = 0 // left;
                else dir = 3; // right
            }
            else if (rbiC == currC && rtiC != currC && lbiC != currC && ltiC != currC) {
                if (dir == 1) dir = 3; // if was up then right
                else if (dir == 0) dir == 2; // if was left then bottom;
            }
            else if (rtiC == currC && rbiC != currC && lbiC != currC && ltiC != currC) {
                if (dir == 2) dir = 3; // if was down then right
                else if (dir == 0) dir == 1; // if was left then up;
            }
            else if (lbiC == currC && rtiC != currC && rbiC != currC && ltiC != currC) {
                if (dir == 3) dir = 2; // if was right then down
                else if (dir == 1) dir == 0; // if was up then left;
            }
            else if (ltiC == currC && rtiC != currC && rbiC != currC && lbiC != currC) {
                if (dir == 3) dir = 1; // if was right then up
                else if (dir == 2) dir == 0; // if was down then left;
            }
        }
    }
    */
}

