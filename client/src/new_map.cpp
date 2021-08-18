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
    for (auto& nodeK : nodes) {
        auto node = nodeK.second;
        if (node.visited)
            continue;
        node.visited = 1;
        // if (borVerts.size() > 10)
        //    break;
        for (auto nn : node.next) {
            // Log("--------------");
            // Log((float)node.x << ", " << (float)node.y);
            // Log(nodes[nn].x << ", " << nodes[nn].y);
            // Log("-!-!-!-!-!");
            int chunkX = node.x / (int)w;
            int chunkY = node.y / (int)w;
            int chunkId = chunkX + chunkY * (mapWidth / w);
            borVerts.push_back(
                //BorderVertex{.pos = Vec3{((float)node.x + 0.5f) * scale, ((float)node.y + 0.5f) * scale, 200.0},
                              BorderVertex{.pos = Vec3{((float)node.x) * scale, ((float)node.y) * scale, 200.0},
                             .tc = Vec2{(float)node.x / mapWidth, (float)node.y / mapHeight}});
            borVerts.push_back(BorderVertex{
                .pos = Vec3{((float)nodes[nn].x) * scale, ((float)nodes[nn].y) * scale, 200.0},
                //.pos = Vec3{((float)nodes[nn].x + 0.5f) * scale, ((float)nodes[nn].y + 0.5f) * scale, 200.0},
                .tc = Vec2{(float)nodes[nn].x / mapWidth, (float)nodes[nn].y / mapHeight}});
            bordChunkId.push_back(chunkId);
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

    glm::vec3 provColor;
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
            glUseProgram(colorMapShader.GetProgram());
            glUniformMatrix4fv(glGetUniformLocation(colorMapShader.GetProgram(), "pr_matrix"), 1, GL_FALSE,
                               glm::value_ptr(matrix));
            glUniformMatrix4fv(glGetUniformLocation(colorMapShader.GetProgram(), "matrix"), 1, GL_FALSE,
                               glm::value_ptr(matrix));
            batch.Begin();
            for (int i = 0, c = 0; i < vertexes.size(); i += 4, ++c) {
                if (chunkVisible[c])
                    batch.Push(&vertexes[i]);
            }
            /*
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
            */
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
        for (int i = 0, c = 0; i < vertexes.size(); i += 4, ++c) {
            if (chunkVisible[c]) {
                batch.Push(&vertexes[i]);
            }
        }
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

        glUseProgram(borderShader.GetProgram());
        glUniformMatrix4fv(glGetUniformLocation(borderShader.GetProgram(), "matrix"), 1, GL_FALSE,
                           glm::value_ptr(matrix));

        borderBatch.Begin();
        for (std::size_t i = 0, c = 0; i < borVerts.size(); i += 2, ++c) {
            if (chunkVisible[bordChunkId[c]])
                borderBatch.Push(&borVerts[i]);
        }
        borderBatch.Flush();
        // bordChunkId.push_back(chunkId);

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
const unsigned char* pix;
int wW, hH;
std::vector<int> left, right;
bool ok = false;

HashPair borRec(int x, int y, int up)
{
    // if (x == 5471 && y == 19)
    // if (x == 5485 && y == 81)
    // if (!ok) {ok = 1; Log((long)pix); }
    bool nextLxUp = false, nextRxUp = false;
    int firstLeft = -1, firstRight = -1;
    while (1) {
        int lx = x;
        int llx = lx;  // - 1;
        std::vector<int> nextRowX;
        int findUp = 1;
        while (1) {
            if (currCol.r == 119 && currCol.g == 212 && currCol.b == 150 && llx == 2788)
                Log("a");
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
                            nextLxUp = true;
                        }
                    }
                    else {  // if (findUp == 0)
                        findUp = 1;
                        nextLxUp = false;
                    }
                }
                llx -= 1;
                continue;
            }
            else {
                lx = llx;  // + 1;
                /*
                if (y + 1 * up < hH && y + 1 * up >= 0) {  // up
                    int ulindex = llx * 3 + (y + 1 * up) * wW * 3;
                    Color ulCol{pix[ulindex + 0], pix[ulindex + 1], pix[ulindex + 2]};
                    if (ulCol != currCol) {
                        nextLxUp = false;
                    }
                }
                */
                break;
            }
        }
        int rx = x;
        int rrx = rx;  // + 1;
        findUp = 1;
        while (1) {
            if (rrx >= wW)
                rrx = wW - rrx;
            int rindex = rrx * 3 + y * wW * 3;
            Color rcol{pix[rindex + 0], pix[rindex + 1], pix[rindex + 2]};
            // Log("rcol: " << (int)rcol.r << ", " << (int)rcol.g << ", " << (int)rcol.b);
            // Log("col: " << (int)currCol.r << ", " << (int)currCol.g << ", " << (int)currCol.b);
            if (rcol == currCol) {
                if (y + 1 * up < hH && y + 1 * up >= 0) {  // up
                    int urindex = rrx * 3 + (y + 1 * up) * wW * 3;
                    Color urCol{pix[urindex + 0], pix[urindex + 1], pix[urindex + 2]};
                    //                  Log("AAAAAAAAAAAAAAAAAAAAAAAAAAA R");
                    if (urCol == currCol) {
                        if (findUp) {
                            nextRowX.push_back(rrx);
                            findUp = 0;
                            nextRxUp = true;
                        }
                    }
                    else {  // if (findUp == 0)
                        findUp = 1;
                        nextRxUp = false;
                    }
                }
                rrx += 1;
                continue;
            }
            else {
                rx = rrx - 1;
                /*
                if (y + 1 * up < hH && y + 1 * up >= 0) {  // up
                    int ulindex = rrx * 3 + (y + 1 * up) * wW * 3;
                    Color ulCol{pix[ulindex + 0], pix[ulindex + 1], pix[ulindex + 2]};
                    if (ulCol != currCol) {
                        nextRxUp = false;
                    }
                }
                */
                break;
            }
        }
        // Log("DDDDDDDDDDDD " << nextRowX.size());
        if (lx == 0 || rx == 0)
            Log("CO JEST");
        // if (left.size() || right.size())
        // Log("left: " << left.size() << ", right: " << right.size());
        for (std::size_t i = 1; i < nextRowX.size(); ++i) {
            if (nextRowX[i] == nextRowX[0]) {
                nextRowX.erase(nextRowX.begin() + i);
                break;
            }
        }
        // if (nextRowX.size()) Log("nextrow.size: " << nextRowX.size());
        if (nextRowX.size()) {
            std::sort(nextRowX.begin(), nextRowX.end());
        }
        // if (left.size() && (nodes[left.back()].x != lx)) {
        if (left.size()) {  // && (nodes[left.back()].x != lx || (nextRowX.size() && nextRowX[0] != lx))) {
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
            if (firstLeft == -1)
                firstLeft = lhash;
            if (nodes.find(lhash) == nodes.end()) {
                nodes[lhash] = Node{};
                nodes[lhash].x = lx;
                nodes[lhash].y = y;
            }
            if (left.size()) {
                int lastHash = left.back();
                nodes[lastHash].next.push_back(lhash);
            }
            left.push_back(lhash);
        }
        if (right.size())
        {  // && (nodes[right.back()].x != rx || (nextRowX.size() && nextRowX[nextRowX.size() - 1] != rx))) {
            // if (right.size() && (nodes[right.back()].x != rx)) {
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
            if (firstRight == -1)
                firstRight = rhash;
            if (nodes.find(rhash) == nodes.end()) {
                nodes[rhash] = Node{};
                nodes[rhash].x = rx;
                nodes[rhash].y = y;
            }
            if (right.size()) {
                int lastHash = right.back();
                nodes[lastHash].next.push_back(rhash);
            }
            right.push_back(rhash);
        }
        // Log("podejrzane1");
        if (nextRowX.size() == 1) {
            x = nextRowX[0];
            y += 1 * up;
            continue;
        }
        else if (nextRowX.size() == 0) {
            int lhash = nodeHash(lx, y);
            int rhash = nodeHash(rx, y);
            nodes[lhash].next.push_back(rhash);
            // return HashPair{lhash, rhash};
            assert(left.size() && right.size());
            // return HashPair{left[0], right[0]};
            return HashPair{firstLeft, firstRight};
            // break;
        }
        else {
            int lastLeftH = -1;
            if (left.size())
                lastLeftH = left.back();
            int lastRightH = -1;
            if (right.size())
                lastRightH = right.back();
            left.clear();
            right.clear();
            HashPair hpair;
            int lhash, rhash;
            for (std::size_t i = 0; i < nextRowX.size(); ++i) {
                // Log("podejrzane2 y = " << y);
                if (i == 0) {
                    // Log("1");
                    if (lastLeftH != -1)
                        left.push_back(lastLeftH);
                    hpair = borRec(nextRowX[i], y + 1 * up, up);
                    lhash = hpair.left;
                    left.clear();
                    right.clear();
                }
                else if (i == nextRowX.size() - 1) {
                    // Log("2");
                    if (lastRightH != -1)
                        right.push_back(lastRightH);
                    HashPair hp = borRec(nextRowX[i], y + 1 * up, up);
                    nodes[hpair.right].next.push_back(hp.left);
                    hpair = hp;
                    rhash = hpair.right;
                    left.clear();
                    right.clear();
                }
                else {
                    // Log("3");
                    HashPair hp = borRec(nextRowX[i], y + 1 * up, up);
                    nodes[hpair.right].next.push_back(hp.left);
                    hpair = hp;
                    // rhash = hpair.right;
                    left.clear();
                    right.clear();
                }
            }

            return HashPair{firstLeft, firstRight};
            // return HashPair{lhash, rhash};
            // Log("podejrzane3");
            break;
        }
        // break;
    }
}

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
            if (nodes.find(thash) == nodes.end())
                nodes[thash] = Node{.x = lx + 1, .y = ly1};
            int thash2 = nodeHash(lx + 1, ly2);
            if (nodes.find(thash2) == nodes.end())
                nodes[thash2] = Node{.x = lx + 1, .y = ly2};
            nodes[thash].next.push_back(thash2);
        }
        else {
            que.push(lx | (ly1 << 16));
        }

        bool goT = false;
        if (ty < h) {
            nextI = tx1 * 3 + ty * w * 3;
            nc = Color{pix[nextI + 0], pix[nextI + 1], pix[nextI + 2]};
            goT = (currCol == nc);
            if (goT == false) {
                int thash = nodeHash(tx1, ty);
                if (nodes.find(thash) == nodes.end())
                    nodes[thash] = Node{.x = tx1, .y = ty};
                int thash2 = nodeHash(tx2, ty);
                if (nodes.find(thash2) == nodes.end())
                    nodes[thash2] = Node{.x = tx2, .y = ty};
                nodes[thash].next.push_back(thash2);
                //if (goL == false) {
                //    nodes[nodeHash(lx, ly2)].next.push_back(thash);
                //}
            }
            else {
                que.push(tx1 | (ty << 16));
            }
        }

        bool goR = false;
        if (rx < w) {
            nextI = rx * 3 + ry2 * w * 3;
            nc = Color{pix[nextI + 0], pix[nextI + 1], pix[nextI + 2]};
            goR = (currCol == nc);
            if (goR == false) {
                int thash = nodeHash(rx, ry2);
                if (nodes.find(thash) == nodes.end())
                    nodes[thash] = Node{.x = rx, .y = ry2};
                int thash2 = nodeHash(rx, ry1);
                if (nodes.find(thash2) == nodes.end())
                    nodes[thash2] = Node{.x = rx, .y = ry1};
                nodes[thash].next.push_back(thash2);
                //if (goT == false) {
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
                //if (goR == false) {
                //    nodes[nodeHash(rx, ry2)].next.push_back(thash2);
                //}
                //if (goL == false) {
                //    nodes[nodeHash(lx, ly1)].next.push_back(thash);
                //}
            }
            else {
                que.push(bx2 | (by << 16));
            }
        }
    }
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
}

/*

    int initx = x, inity = y;
    do {
        int lx = x - 1, ly1 = y, ly2 = y + 1;
        int tx1 = x, tx2 = x + 1, ty = y + 1;
        int rx = x + 1, ry1 = y + 1, ry2 = y;
        int bx1 = x + 1, bx2 = x, by = y;

        int currI = x * 3 + y * w * 3;

        int nextI = lx * 3 + y * w * 3;
        if (lx < 0)
            nextI = (w - 1) * 3 + ly1 * w * 3;

        Color nc{pix[nextI + 0], pix[nextI + 1], pix[nextI + 2]};
        bool goL = (currCol == nc);
        if (goL == false) {
                int thash = nodeHash(lx, ly1);
                if (nodes.find(thash) == nodes.end())
                nodes[thash] = Node{.x = lx, .y = ly1};
                int thash2 = nodeHash(lx, ly2);
                if (nodes.find(thash2) == nodes.end())
                nodes[thash2] = Node{.x = lx, .y = ly2};
                //nodes[thash].next.push_back(thash2);
        }

        bool goT = false;
        if (ty < h) {
            nextI = tx1 * 3 + ty * w * 3;
            nc = Color{pix[nextI + 0], pix[nextI + 1], pix[nextI + 2]};
            goT = (currCol == nc);
            if (goT == false) {
                int thash = nodeHash(tx1, ty);
                if (nodes.find(thash) == nodes.end())
                nodes[thash] = Node{.x = tx1, .y = ty};
                int thash2 = nodeHash(tx2, ty);
                if (nodes.find(thash2) == nodes.end())
                nodes[thash2] = Node{.x = tx2, .y = ty};
                nodes[thash].next.push_back(thash2);
                if (goL == false) {
                    nodes[nodeHash(lx, ly2)].next.push_back(thash);
                }
            }
        }

        bool goR = false;
        if (rx < w) {
            nextI = rx * 3 + ry1 * w * 3;
            nc = Color{pix[nextI + 0], pix[nextI + 1], pix[nextI + 2]};
            goR = (currCol == nc);
            if (goR == false) {
                int thash = nodeHash(rx, ry2);
                if (nodes.find(thash) == nodes.end())
                nodes[thash] = Node{.x = rx, .y = ry2};
                int thash2 = nodeHash(rx, ry1);
                if (nodes.find(thash2) == nodes.end())
                nodes[thash2] = Node{.x = rx, .y = ry1};
                nodes[thash].next.push_back(thash2);
                if (goT == false) {
                    nodes[nodeHash(tx2, ty)].next.push_back(thash2);
                }
            }
        }

        bool goB = false;
        if (by > 0) {
            nextI = bx1 * 3 + by * w * 3;
            nc = Color{pix[nextI + 0], pix[nextI + 1], pix[nextI + 2]};
            goB = (currCol == nc);
            if (goB == false) {
                int thash = nodeHash(bx2, by);
                if (nodes.find(thash) == nodes.end())
                nodes[thash] = Node{.x = bx2, .y = by};
                int thash2 = nodeHash(bx1, by);
                if (nodes.find(thash2) == nodes.end())
                nodes[thash2] = Node{.x = bx1, .y = by};
                nodes[thash].next.push_back(thash2);
                if (goR == false) {
                    nodes[nodeHash(rx, ry2)].next.push_back(thash2);
                }
                if (goL == false) {
                    nodes[nodeHash(lx, ly1)].next.push_back(thash);
                }
            }
        }

        if (goL) {
            --x;
        }
        else if (goT) {
            ++y;
        }
        else if (goR) {
            ++x;
        }
        else if (goB) {
            --y;
        }
        else break;

    } while (initx != x || inity != y);

*/
