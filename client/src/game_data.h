#pragma once
#include <vector>

#include "battle.h"
#include "country.h"
#include "map2.h"
#include "province.h"
#include "unit.h"
#include "war.h"
#include "settings.h"

struct GameData {
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

    void initMap()
    {
        provsCols.reserve(provinces.size());
        // for (auto& p : provsData) provsCols.push_back(Color3{p.r, p.g, p.b});
        for (auto &p : provinces) provsCols.push_back(Color3{p.color.r, p.color.g, p.color.b});

        map = new Map2(provsCols, settings.scale);
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
