#pragma once
#include <vector>

#include "battle.h"
#include "camera.h"
#include "country.h"
#include "load_data.h"
#include "map2.h"
#include "province.h"
#include "settings.h"
#include "unit.h"
#include "war.h"

struct GameData {
    Window *window;
    std::vector<Country> countries;
    std::vector<Province> provinces;
    std::vector<Battle> battles;
    std::vector<War> wars;
    std::vector<Unit> units;
    std::map<unsigned int, int> colorToId;
    Map2 *map;
    std::vector<Color3> provsCols;
    std::vector<std::vector<int>> ctrProvs;
    std::vector<FontVertex> fontVerts;
    GLuint fontCtrVao, fontCtrVbo;
    Settings settings;
    Camera camera;
    float waterTime = 0.0f;
    float ctrNamesFade = 0.0f, ctrNamesFadeIn = 0.0f;
    GLuint fontTexID[32];
    GLint tex[32];
    Shader fontShader;
    const float zPoint = 1500.0f;

    void updateTime(float dt)
    {
        waterTime += dt;
    }

    void initMap()
    {
        for (GLint i = 0; i < 32; ++i) {
            tex[i] = i;
        }
        for (int i = 0; i <= (int)AM::FontSize::PX160; ++i) {
            fontTexID[i] = AM::atlasTexture[i]->GetId();
        }
        fontShader = Shader{"src/graphics/shaders/fonts.vert", "src/graphics/shaders/fonts.frag", "", ""};
        provsCols.reserve(provinces.size());
        // for (auto& p : provsData) provsCols.push_back(Color3{p.r, p.g, p.b});
        for (auto &p : provinces) provsCols.push_back(Color3{p.color.r, p.color.g, p.color.b});

        map = new Map2(provsCols, settings.scale);

        map->occupiedPix.resize(provinces.size() * 4);
        map->occupyingPix.resize(provinces.size() * 4);
        std::memset(&map->occupiedPix[0], 0, map->occupiedPix.size());
        std::memset(&map->occupyingPix[0], 0, map->occupyingPix.size());
        if (map->occupiedText != nullptr)
            delete map->occupiedText;
        map->occupiedText = new Texture(&map->occupiedPix[0], provinces.size(), 1);
        if (map->occupyingText != nullptr)
            delete map->occupyingText;
        map->occupyingText = new Texture(&map->occupyingPix[0], provinces.size(), 1);

        resetMarkedCountry();
    }

    void resetMarkedCountry()
    {
        map->markedCtrPix.resize(provinces.size() * 4);
        std::memset(&map->markedCtrPix[0], 0, map->markedCtrPix.size());
        if (map->markedCtrText != nullptr)
            delete map->markedCtrText;
        map->markedCtrText = new Texture(&map->markedCtrPix[0], provinces.size(), 1);

    }

    void f(int ctrId)
    {
        int maxxid, minxid, maxyid, minyid;
        glm::vec2 maxx{-1.0f, 0.0f}, minx{10000.0f, 0.0f};
        glm::vec2 maxy{0.0f, -1.0f}, miny{0.0f, 10000.0f};
        {
            for (int i = 0; i < ctrProvs[ctrId].size(); ++i) {
                float x = provinces[ctrProvs[ctrId][i]].unitPosition.x;
                float y = provinces[ctrProvs[ctrId][i]].unitPosition.y;
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
        glm::vec2 center{(maxx.x + minx.x) * 0.5f, (maxx.y + minx.y) * 0.5f};
        textml = glm::translate(textml, glm::vec3{center.x * settings.scale, center.y * settings.scale, 0.0f});
        float rotateText = atan((maxx.y - minx.y) / abs(maxx.x - minx.x));  // * 180.0f * 3.1456f;
        if (minx.x - maxx.x >= 0.0f) {
            rotateText *= -1.0f;
        }

        std::string text = "ctr " + std::to_string(ctrId);
        AM::FontSize fontSize = AM::FontSize::PX16;

        int len = 0, hei = 0;
        while (len < dist * settings.scale * 0.75f && (fontSize) <= AM::FontSize::PX160) {
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
        if (fontSize > AM::FontSize::PX160)
            fontSize = AM::FontSize::PX160;
        if (len > dist * settings.scale * 0.75f && fontSize > 0)
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
        hei = hei / text.size();
        int cx = -len / 2, cy = -hei / 2;
        int cz = 80.0f;
        {
            int ind = (cx + maxx.x) * 3 + (cy + maxx.y) * map->mapWidth * 3;
            if (ind >= 0 && ind < map->mapWidth * map->mapHeight * 3)
                cz = map->heightMap.GetPixels()[ind];
        }

        textml = glm::rotate(textml, rotateText, glm::vec3{0.0f, 0.0f, 1.0f});
        int off = 0.0f;
        Vec4 color{1.0f, 1.0f, 1.0f, 1.0f};
        float ml[16];
        auto *mlptr = glm::value_ptr(textml);
        for (int i = 0; i < 16; ++i) {
            ml[i] = *(mlptr + i);
        }
        for (auto c : text) {
            glm::vec3 rPos;
            glm::vec2 rSize, rLBTC, tcLen;
            if (AM::atlasInfo[fontSize][c].set) {
                rPos = {cx + off, cy + AM::atlasInfo[fontSize][c].yPos, cz};
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
                fontVerts.push_back(fv);

                fv.pos.y += rSize.y;
                fv.tc.y += tcLen.y;
                fontVerts.push_back(fv);

                fv.pos.x += rSize.x;
                fv.tc.x += tcLen.x;
                fontVerts.push_back(fv);

                fontVerts.push_back(fv);

                fv.pos.y -= rSize.y;
                fv.tc.y -= tcLen.y;
                fontVerts.push_back(fv);

                fv.pos.x -= rSize.x;
                fv.tc.x -= tcLen.x;
                fontVerts.push_back(fv);
            }
            else {
                rPos = {cx + off, cy, cz};
                rSize = {10.0, 10.0};
                rLBTC = {0, 0};
                tcLen = {0, 0};
                off += rSize.x;
            }
        }
    }

    void makeCountryNames()
    {
        Log("makecountrynames");
        if (fontVerts.size()) {
            glDeleteVertexArrays(1, &fontCtrVao);
            glDeleteBuffers(1, &fontCtrVbo);
        }
        ctrProvs.clear();
        fontVerts.clear();

        for (std::size_t i = 0; i < countries.size(); ++i) ctrProvs.push_back(std::vector<int>{});
        for (auto &pd : provinces) {
            if (pd.GetCountryId() == -1)
                continue;
            ctrProvs[pd.GetCountryId()].push_back(pd.id);
        }
        for (std::size_t i = 0; i < countries.size(); ++i) {
            f(countries[i].GetId());
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
                              (const GLvoid *)(offsetof(FontVertex, FontVertex::tc)));
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(FontVertex),
                              (const GLvoid *)(offsetof(FontVertex, FontVertex::color)));
        glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(FontVertex),
                              (const GLvoid *)(offsetof(FontVertex, FontVertex::tid)));
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(FontVertex),
                              (const GLvoid *)(offsetof(FontVertex, FontVertex::ml)));
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(FontVertex),
                              (const GLvoid *)(offsetof(FontVertex, FontVertex::ml) + 4 * 4));
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(FontVertex),
                              (const GLvoid *)(offsetof(FontVertex, FontVertex::ml) + 8 * 4));
        glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(FontVertex),
                              (const GLvoid *)(offsetof(FontVertex, FontVertex::ml) + 12 * 4));
    }

    void editCountryMap()
    {
        for (std::size_t i = 0; i < provinces.size(); ++i) {
            map->mapTextures.country.pix[i * 3 + 0] = countries[provinces[i].GetCountryId()].color.r;
            map->mapTextures.country.pix[i * 3 + 1] = countries[provinces[i].GetCountryId()].color.g;
            map->mapTextures.country.pix[i * 3 + 2] = countries[provinces[i].GetCountryId()].color.b;
        }
        map->mapTextures.country.Update();
    }

    void loadProvinceTexture()
    {
        map->ReloadShaders();
        {
            map->mapTextures.country.pix = new unsigned char[provinces.size() * 3];
            map->mapTextures.province.pix = new unsigned char[provinces.size() * 3];
            map->mapTextures.country.w = provinces.size();
            map->mapTextures.country.h = 1;
            map->mapTextures.province.w = provinces.size();
            map->mapTextures.province.h = 1;

            editCountryMap();
            for (std::size_t i = 0; i < provinces.size(); ++i) {
                map->mapTextures.province.pix[i * 3 + 0] = provsCols[i].r;
                map->mapTextures.province.pix[i * 3 + 1] = provsCols[i].g;
                map->mapTextures.province.pix[i * 3 + 2] = provsCols[i].b;
            }

            map->mapTextures.province.Update();
        }
    }
};

static void checkCountryNamesFade(GameData *gd, float dt)
{
    if (gd->camera.eye.z > gd->zPoint) {
        gd->ctrNamesFade = 0.0f;
    }
    else {
        gd->ctrNamesFadeIn = 0.0f;
    }
}
static void updateCountryNamesFade(GameData *gd, float dt)
{
    gd->ctrNamesFade += dt;
    if (gd->ctrNamesFade < 0.0f)
        gd->ctrNamesFade = 10.0f;
    gd->ctrNamesFadeIn -= dt;
    if (gd->ctrNamesFadeIn > 0.0f)
        gd->ctrNamesFadeIn = -10.0f;
}
static void drawCountryNames(GameData *gd)
{
    for (int i = 0; i <= (int)AM::FontSize::PX160; ++i) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, (GLuint)gd->fontTexID[i]);
    }

    // if (gd->camera.eye.z > gd->zPoint) {
    //     gd->ctrNamesFade = 0.0f;
    // }
    // else {
    //     gd->ctrNamesFadeIn = 0.0f;
    // }
    // Log(ctrNamesFadeIn << ", fade: " << ctrNamesFade);
    // if (((gd->camera.eye.z > zPoint) || ctrNamesFade < 1.0f) && openPeaceOfferId == -1) {
    glDisable(GL_DEPTH_TEST);  // Enable depth testing for z-culling

    glUseProgram(gd->fontShader.GetProgram());
    glUniformMatrix4fv(glGetUniformLocation(gd->fontShader.GetProgram(), "matrix"), 1, GL_FALSE,
                       glm::value_ptr(gd->camera.GetMat()));
    glUniform1iv(glGetUniformLocation(gd->fontShader.GetProgram(), "tex"), 32, gd->tex);
    glUniform1f(glGetUniformLocation(gd->fontShader.GetProgram(), "fade"), gd->ctrNamesFade);
    glUniform1f(glGetUniformLocation(gd->fontShader.GetProgram(), "fadein"), gd->ctrNamesFadeIn);

    {  // chars
        glBindVertexArray(gd->fontCtrVao);
        glDrawArrays(GL_TRIANGLES, 0, gd->fontVerts.size());
    }
    //}
    glEnable(GL_DEPTH_TEST);  // Enable depth testing for z-culling
}

static void cameraMovement(GameData *gd, float dt)
{
    if (gd->window->keys['A'])
        gd->camera.MoveHor(-1, dt);
    if (gd->window->keys['D'])
        gd->camera.MoveHor(1, dt);
    if (gd->window->keys['W'])
        gd->camera.MoveVer(1, dt);
    if (gd->window->keys['S'])
        gd->camera.MoveVer(-1, dt);
    if (gd->window->keys['F'])
        gd->camera.Rotate(1, dt);
    if (gd->window->keys['T'])
        gd->camera.Rotate(-1, dt);
    if (gd->window->keys['G'])
        gd->camera.Reset();

    if (gd->window->keys['M'])
        gd->map->ReloadShaders();
}

static int clickedProvId(GameData *gd, glm::vec2 mouseInWorld)
{
    gd->map->DrawForColorPick(gd->camera.GetMat(), (float)gd->provinces.size());
    int pixx = gd->window->xMouse, pixy = gd->window->GetSize().y - gd->window->yMouse;
    unsigned char pixel[4];
    glReadPixels(pixx, pixy, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);
    gd->window->Refresh();
    // clickedProvColor = {(int)pixel[0] / 255.0, (int)pixel[1] / 255.0, (int)pixel[2] / 255.0};
    unsigned int clickedProviPhash = getHash(pixel[0], pixel[1], pixel[2]);
    if (gd->colorToId.find(clickedProviPhash) != gd->colorToId.end()) {
        int pid = gd->colorToId[clickedProviPhash];
        return pid;
    }
    return -1;
}
