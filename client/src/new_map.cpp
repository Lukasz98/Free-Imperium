#include "new_map.h"

#include <cstring>
//#include <fstream>
#include <map>
#include <queue>
#include <set>
//#include <string>

#include "asset_manager.h"
#include "color.h"
#include "font_batch.h"
#include "graphics/map_texture.h"
#include "graphics/texture.h"
#include "load_data.h"
#include "map2.h"
#include "map_batch.h"
#include "save_borders.h"
#include "save_borders_triangles.h"
#include "save_data.h"

// int nodeHash(unsigned short x, unsigned short y)
struct Line {
    int x, y;
};

void f(std::vector<FontVertex>& vec, int ctrId, const std::vector<ProvData>& provinces,
       const std::vector<std::vector<int>>& ctrProvs, int mapWidth, int mapHeight, int scale,
       const unsigned char* h)
{
    int maxxid, minxid, maxyid, minyid;
    glm::vec2 maxx{-1.0f, 0.0f}, minx{10000.0f, 0.0f};
    glm::vec2 maxy{0.0f, -1.0f}, miny{0.0f, 10000.0f};
    {
        // float minx 10000.0f, maxx = -1.0f, miny = 10000.0f, maxy = -1.0f;
        for (int i = 0; i < ctrProvs[ctrId].size(); ++i) {
            float x = provinces[ctrProvs[ctrId][i]].x;
            float y = provinces[ctrProvs[ctrId][i]].y;
            if (x > maxx.x) {
                maxx = {x, y};
                maxxid = provinces[ctrProvs[ctrId][i]].id;
            }
            if (x < minx.x) {
                minx = {x, y};
                minxid = provinces[ctrProvs[ctrId][i]].id;
            }
            if (y > maxy.y) {
                maxy = {x, y};
                maxyid = provinces[ctrProvs[ctrId][i]].id;
            }
            if (y < miny.y) {
                miny = {x, y};
                minyid = provinces[ctrProvs[ctrId][i]].id;
            }
        }
    }

    float distX = (maxx.x - minx.x);
    float distY = (maxy.y - miny.y);

    float dist = distX;
    if (distX <= distY) {
        maxx.x = maxy.x;
        maxx.y = maxy.y;
        minx.x = miny.x;
        minx.y = miny.y;
        dist = distY;
    }

    glm::mat4 textml{1.0f};
    // Mat4 textml;
    glm::vec2 center{(maxx.x + minx.x) * 0.5f, (maxx.y + minx.y) * 0.5f};
    textml = glm::translate(textml, glm::vec3{center.x * scale, center.y * scale, 0.0f});
    float rotateText = atan((maxx.y - minx.y) / abs(maxx.x - minx.x));  // * 180.0f * 3.1456f;
    if (minx.x - maxx.x >= 0.0f) {
        rotateText *= -1.0f;
    }

    std::string text = "ctr " + std::to_string(ctrId);
    AM::FontSize fontSize = AM::FontSize::PX16;

    int len = 0, hei = 0;
    while (len < dist * scale * 0.75f && (fontSize) <= AM::FontSize::PX160) {
        len = 0;
        hei = 0;
        for (auto c : text) {
            glm::vec3 rPos;
            glm::vec2 rSize, rLBTC, tcLen;
            if (AM::atlasInfo[fontSize][c].set) {
                len += AM::atlasInfo[fontSize][c].width;
                hei += AM::atlasInfo[fontSize][c].height;
            }
            else {
                len += 10;
                hei += 10;
            }
        }
        fontSize = (AM::FontSize)(fontSize + 1);
    }
    // Log("fontSize=" << (int)fontSize);
    if (fontSize > AM::FontSize::PX160)
        fontSize = AM::FontSize::PX160;
    if (len > dist * scale * 0.75f && fontSize > 0)
        fontSize = (AM::FontSize)(fontSize - 1);
    len = 0;
    hei = 0;
    for (auto c : text) {
        glm::vec3 rPos;
        glm::vec2 rSize, rLBTC, tcLen;
        if (AM::atlasInfo[fontSize][c].set) {
            len += AM::atlasInfo[fontSize][c].width;
            hei += AM::atlasInfo[fontSize][c].height;
        }
        else {
            len += 10;
            hei += 10;
        }
    }
    // Log("fontSize=" << (int)fontSize);
    hei = hei / text.size();
    int cx = -len / 2, cy = -hei / 2;
    // textml = glm::rotate(textml, abs(sin(waterTime)) * 90.0f, glm::vec3{0.0f, 1.0f, 0.0f});
    int cz = 80.0f;
    {
        int ind = (cx + maxx.x) * 3 + (cy + maxx.y) * mapWidth * 3;
        assert(ind >= 0 && ind < mapWidth * mapHeight * 3);
        cz = h[ind];
        // Log("cz="<<cz);
    }
    // textml.translate(Vec3{center.x * scale, center.y * scale, 0.0f});

    // if (window.keys['Y'])
    //    rotateText = sin(waterTime) * 90.0f;
    textml = glm::rotate(textml, rotateText, glm::vec3{0.0f, 0.0f, 1.0f});
    // const float pi = 3.14159f;
    // rotateText = 0.5f * pi;
    // rotateText = pi;
    // textml.rotate(Vec3{0.0f, 0.0f, 1.0f}, rotateText);
    // glUniformMatrix4fv(glGetUniformLocation(fontShader.GetProgram(), "ml"), 1, GL_FALSE,
    // glm::value_ptr(textml));
    int off = 0.0f;
    Vec4 color{1.0f, 1.0f, 1.0f, 1.0f};
    float ml[16];
    auto* mlptr = glm::value_ptr(textml);
    for (int i = 0; i < 16; ++i) {
        ml[i] = *(mlptr + i);
    }
    for (auto c : text) {
        glm::vec3 rPos;
        glm::vec2 rSize, rLBTC, tcLen;
        if (AM::atlasInfo[fontSize][c].set) {
            rPos = {cx + off, cy + AM::atlasInfo[fontSize][c].yPos, cz};
            // rPos = textml * glm::vec4{rPos, 1.0f};
            rSize = {(float)AM::atlasInfo[fontSize][c].width, (float)AM::atlasInfo[fontSize][c].height};
            rLBTC = {AM::atlasInfo[fontSize][c].tcX, AM::atlasInfo[fontSize][c].tcY};
            tcLen = {AM::atlasInfo[fontSize][c].tcWidth, AM::atlasInfo[fontSize][c].tcHeight};
            off += rSize.x + 1;

            FontVertex fv{
                .pos = Vec3{rPos.x, rPos.y, rPos.z},
                .tc = Vec2{rLBTC.x, rLBTC.y},
                .color = color,
                .tid = (int)fontSize,
            };

            for (int i = 0; i < 16; ++i) {
                fv.ml[i] = ml[i];
            }
            // FontVertex fvv = fv;
            vec.push_back(fv);

            fv.pos.y += rSize.y;
            fv.tc.y += tcLen.y;
            vec.push_back(fv);

            fv.pos.x += rSize.x;
            fv.tc.x += tcLen.x;
            vec.push_back(fv);

            vec.push_back(fv);

            fv.pos.y -= rSize.y;
            fv.tc.y -= tcLen.y;
            vec.push_back(fv);

            fv.pos.x -= rSize.x;
            fv.tc.x -= tcLen.x;
            vec.push_back(fv);
        }
        else {
            rPos = {cx + off, cy, cz};
            rSize = {10.0, 10.0};
            rLBTC = {0, 0};
            tcLen = {0, 0};
            off += rSize.x;
        }
        //  Log(rPos.x << " " << rPos.y << " "<<rPos.z);
    }
    //}
}

void newTesMapTest(Window& window, glm::vec2 resolution, glm::vec2 windowSize)
{
    GLuint err = glGetError();
    if (err)
        Log("Opengl error: " << err);
    Shader fontShader{"src/graphics/shaders/fonts.vert", "src/graphics/shaders/fonts.frag", "", ""};
    Camera camera{window.GetSize()};
    glPatchParameteri(GL_PATCH_VERTICES, 3);
    float trCount = 150;
    err = glGetError();
    if (err)
        Log("Opengl error: " << err);

    int mapWidth = 5632, mapHeight = 2048;
    Texture terrainTexture{"src/img/terrain_map.png", mapWidth, mapHeight};
    Texture waterMap{"src/img/Blank_map.png", mapWidth, mapHeight};
    Texture heightMap{"src/img/Heightmap.png", mapWidth, mapHeight};
    // Texture stoneT{"../shared/smoothstone.png", 64, 64, GL_REPEAT};
    // Texture waterT{"../shared/water1.png", 64, 64, GL_REPEAT};
    // Texture sandT{"src/img/Sand_1.png", 32, 32, GL_REPEAT};
    Texture ctrsText{"src/img/countries_map.png", mapWidth, mapHeight};

    GLint tex[32];
    for (GLint i = 0; i < 32; ++i) {
        tex[i] = i;
    }

    // saveProvinceFromImg(provTexture.GetPixels(), waterMap.GetPixels(), mapWidth, mapHeight);
    std::vector<ProvData> provinces;
    std::vector<CountryData> ctrsData;
    std::vector<std::vector<int>> ctrProvs;
    std::map<unsigned int, int> colorToId;
    std::map<int, Node> nodes;
    loadProvData(provinces, colorToId);
    loadCountriesData(ctrsData);
    ctrProvs.reserve(ctrsData.size());

    // saveBorders(provTexture.GetPixels(), mapWidth, mapHeight, provinces, nodes);
    // saveSeaBorders(provTexture.GetPixels(), mapWidth, mapHeight, provinces, nodes, colorToId);
    // createSaveProvPoints(provTexture.GetPixels(), mapWidth, mapHeight, provinces, colorToId);
    float scale = 4.0f;
    // saveBordersTriangles(mapWidth, mapHeight, scale, heightMap.GetPixels());
    // return;
    err = glGetError();
    if (err)
        Log("Opengl error: " << err);
    err = glGetError();
    if (err)
        Log("Opengl error: " << err);

    GLuint fontTexID[32];
    for (int i = 0; i <= (int)AM::FontSize::PX160; ++i) {
        fontTexID[i] = AM::atlasTexture[i]->GetId();
    }

    const unsigned char* h = heightMap.GetPixels();

    std::vector<FontVertex> fontVerts;
    GLuint fontCtrVao, fontCtrVbo;
    {
        for (std::size_t i = 0; i < ctrsData.size(); ++i) ctrProvs.push_back(std::vector<int>{});
        for (auto& pd : provinces) {
            if (pd.ctrId == -1)
                continue;
            ctrProvs[pd.ctrId].push_back(pd.id);
        }
        for (std::size_t i = 0; i < ctrsData.size(); ++i) {
            f(fontVerts, ctrsData[i].id, provinces, ctrProvs, mapWidth, mapHeight, scale, h);
        }
        Log("fontVerts.size = " << fontVerts.size());
        glCreateVertexArrays(1, &fontCtrVao);
        glBindVertexArray(fontCtrVao);
        glCreateBuffers(1, &fontCtrVbo);

        glBindBuffer(GL_ARRAY_BUFFER, fontCtrVbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(FontVertex) * fontVerts.size(), fontVerts.data(), GL_STATIC_DRAW);
        glEnableVertexArrayAttrib(fontCtrVao, 0);
        glEnableVertexArrayAttrib(fontCtrVao, 1);
        glEnableVertexArrayAttrib(fontCtrVao, 2);
        glEnableVertexArrayAttrib(fontCtrVao, 3);
        glEnableVertexArrayAttrib(fontCtrVao, 4);
        glEnableVertexArrayAttrib(fontCtrVao, 5);
        glEnableVertexArrayAttrib(fontCtrVao, 6);
        glEnableVertexArrayAttrib(fontCtrVao, 7);
        GLuint err = glGetError();
        if (err)
            Log("Opengl error: " << err);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FontVertex), NULL);  //(const GLvoid*)0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(FontVertex),
                              (const GLvoid*)(offsetof(FontVertex, FontVertex::tc)));
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(FontVertex),
                              (const GLvoid*)(offsetof(FontVertex, FontVertex::color)));
        glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(FontVertex),
                              (const GLvoid*)(offsetof(FontVertex, FontVertex::tid)));
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(FontVertex),
                              (const GLvoid*)(offsetof(FontVertex, FontVertex::ml)));
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(FontVertex),
                              (const GLvoid*)(offsetof(FontVertex, FontVertex::ml) + 4 * 4));
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(FontVertex),
                              (const GLvoid*)(offsetof(FontVertex, FontVertex::ml) + 8 * 4));
        glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(FontVertex),
                              (const GLvoid*)(offsetof(FontVertex, FontVertex::ml) + 12 * 4));
    }

    err = glGetError();
    if (err)
        Log("Opengl error: " << err);

    std::vector<Color3> provsCols;
    provsCols.reserve(provinces.size());
    for (auto& p : provinces) provsCols.push_back(Color3{p.r, p.g, p.b});

    Map2 map2{h, mapWidth, mapHeight, provsCols, scale};
    map2.ReloadShaders();
    {
        map2.mapTextures.country.pix = new unsigned char[provinces.size() * 3];
        map2.mapTextures.province.pix = new unsigned char[provinces.size() * 3];

        for (std::size_t i = 0; i < provinces.size(); ++i) {
            map2.mapTextures.province.pix[i * 3 + 0] = provsCols[i].r;
            map2.mapTextures.province.pix[i * 3 + 1] = provsCols[i].g;
            map2.mapTextures.province.pix[i * 3 + 2] = provsCols[i].b;
        }
        for (std::size_t i = 0; i < provinces.size(); ++i) {
            map2.mapTextures.country.pix[i * 3 + 0] = ctrsData[provinces[i].ctrId].r;
            map2.mapTextures.country.pix[i * 3 + 1] = ctrsData[provinces[i].ctrId].g;
            map2.mapTextures.country.pix[i * 3 + 2] = ctrsData[provinces[i].ctrId].b;
        }

        map2.mapTextures.country.Update(provinces.size(), 1);
        map2.mapTextures.province.Update(provinces.size(), 1);
    }

    const unsigned char* pix = map2.provTexture.GetPixels();

    GLuint otherTexID[32];
    for (GLint i = 0; i < 32; ++i) otherTexID[i] = i;
    otherTexID[0] = AM::am.modelTexture->GetId();

    bool drawBorders = true;
    glm::vec3 provColor;
    float markedProvId = -1.0f;
    float tesLevel = 32.0f;
    glm::vec3 unitPos;
    float dt = 0.0f, waterTime = 0.0f;
    float ctrNamesFade = 0.0f, ctrNamesFadeIn = 0.0f;
    float time = glfwGetTime();
    float rotateText;
    unsigned int clickedProviPhash;
    while (!window.ShouldClose()) {
        glEnable(GL_DEPTH_TEST);  // Enable depth testing for z-culling
        glDepthFunc(GL_LESS);
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
        if (window.keys['G'])
            camera.Reset();
        if (window.keys['M']) {
            map2.ReloadShaders();
        }

        if (window.scrollOffset) {
            camera.Scroll(window.scrollOffset);
            window.scrollOffset = 0;
        }
        camera.Update(window.xMouse, windowSize.y - window.yMouse, pix);
        glm::mat4 matrix = camera.GetMat();

        map2.ActivateTextures();

        if (window.keys['I']) {
            map2.DrawForColorPick(matrix, (float)provinces.size());

            unsigned char pixel[4];
            int pixx = window.xMouse, pixy = windowSize.y - window.yMouse;
            glReadPixels(pixx, pixy, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);

            window.Refresh();
            provColor = {(int)pixel[0] / 255.0, (int)pixel[1] / 255.0, (int)pixel[2] / 255.0};
            clickedProviPhash = getHash(pixel[0], pixel[1], pixel[2]);
            if (colorToId.find(clickedProviPhash) != colorToId.end()) {
                int pid = colorToId[clickedProviPhash];
                markedProvId = (float)pid;
                // Log(provinces[pid].id << ", water: " << provinces[pid].water << ", " << provinces[pid].name
                //                      << ", col: " << provinces[pid].r << ", " << provinces[pid].g << ", "
                //                      << provinces[pid].b);
                // std::cout << "Neighb: ";
                // for (auto i : provinces[pid].neighb) std::cout << i << " ";
                // std::cout << "\n";
                // std::cout << "NeighbSea: ";
                // for (auto i : provinces[pid].neighbSea) std::cout << i << " ";
                // std::cout << "\n";
                unitPos.x = provinces[pid].x * scale;
                unitPos.y = provinces[pid].y * scale;
                unitPos.z = heightMap.GetPixels()[(int)(provinces[pid].x * 3 + provinces[pid].y * mapWidth * 3)];
            }
            //std::cout << "R: " << (int)pixel[0] << "< ";
            //std::cout << "G: " << (int)pixel[1] << "< ";
            //std::cout << "B: " << (int)pixel[2] << "< \n";
        }

        if (window.keys['L'])
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        if (window.keys['K'])
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        if (window.keys['B']) {
            drawBorders = !drawBorders;
        }

        if (window.keys['P']) {
            map2.DrawForColorPick(matrix, (float)provinces.size());
        }
        else {
            map2.DrawWater(matrix, camera.eye);
            map2.DrawLand(matrix, camera.eye, markedProvId, provinces.size(), map2.MAPID_COUNTRY);
        }

        if (drawBorders) {
            map2.DrawBorders(matrix);
        }

        {
            for (int i = 0; i < 32; ++i) {
                glActiveTexture(GL_TEXTURE0 + i);
                glBindTexture(GL_TEXTURE_2D, otherTexID[i]);
            }
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
            glUniform1iv(glGetUniformLocation(AM::am.shader->GetProgram(), "tex"), 32, tex);
            AM::am.model->Draw();
        }

        for (int i = 0; i <= (int)AM::FontSize::PX160; ++i) {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, (GLuint)fontTexID[i]);
        }

        if (camera.eye.z > 1000.0f) {
            ctrNamesFade = 0.0f;
        }
        else {
            ctrNamesFadeIn = 0.0f;
        }
        // Log(ctrNamesFadeIn << ", fade: " << ctrNamesFade);
        if ((camera.eye.z > 1000.0f) || ctrNamesFade < 1.0f) {
            glDisable(GL_DEPTH_TEST);  // Enable depth testing for z-culling

            glUseProgram(fontShader.GetProgram());
            glUniformMatrix4fv(glGetUniformLocation(fontShader.GetProgram(), "matrix"), 1, GL_FALSE,
                               glm::value_ptr(matrix));
            glUniform1iv(glGetUniformLocation(fontShader.GetProgram(), "tex"), 32, tex);
            glUniform1f(glGetUniformLocation(fontShader.GetProgram(), "fade"), ctrNamesFade);
            glUniform1f(glGetUniformLocation(fontShader.GetProgram(), "fadein"), ctrNamesFadeIn);

            {  // chars
                glBindVertexArray(fontCtrVao);
                glDrawArrays(GL_TRIANGLES, 0, fontVerts.size());
            }
        }
        float tt2 = glfwGetTime();

        window.Update();
        waterTime += dt;
        dt = glfwGetTime() - time;
        ctrNamesFade += dt;
        if (ctrNamesFade < 0.0f)
            ctrNamesFade = 10.0f;
        ctrNamesFadeIn -= dt;
        if (ctrNamesFadeIn > 0.0f)
            ctrNamesFadeIn = -10.0f;
        // Log(dt);
        time = glfwGetTime();
    }
}

/*
provs error on map
id:
430, 710, 825, 1120, 1892, 2445, 2994, 3020, 3321, 3554, 3590, 3663 3723, 3764, 3765
*/
