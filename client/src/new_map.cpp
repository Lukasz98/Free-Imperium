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
    borVerts.push_back(BorderVertex{.pos = Vec3{0.0, 0.0, 200.0}});  //, .normal=Vec2{0.0f, 0.0f} });
    borVerts.push_back(BorderVertex{.pos = Vec3{1000.0, 1000.0, 100.0}});  //, .normal=Vec2{0.0f, 0.0f} });
    
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
        borderBatch.Push(borVerts.data());
        //borderBatch.Push(borVerts.data() + 2);
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

