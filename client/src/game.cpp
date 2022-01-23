#include "game.h"

#include "ctr_data.h"
#include "font_batch.h"
#include "int_to_string.h"
#include "load_data.h"
#include "prov_data.h"
#include "save_borders.h"
#include "prepare_packet.h"

struct OfferPeaceData {
    struct pair {
        int provId = -1, recipantId = -1;
    };
    std::vector<pair> provs;
    int warId = -1;
    int recipantId = -1;
} offerPeaceData;  // it represents peace offer that i'm currently making, it resets with opening new peace offer
                   // making window (even if same war)

TopBarData topBarData;

int openedProvId = -1;

static void f(std::vector<FontVertex> &vec, int ctrId, const std::vector<ProvData> &provinces,
              const std::vector<std::vector<int>> &ctrProvs, int mapWidth, int mapHeight, int scale,
              const unsigned char *h)
{
    int maxxid, minxid, maxyid, minyid;
    glm::vec2 maxx{-1.0f, 0.0f}, minx{10000.0f, 0.0f};
    glm::vec2 maxy{0.0f, -1.0f}, miny{0.0f, 10000.0f};
    {
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
    hei = hei / text.size();
    int cx = -len / 2, cy = -hei / 2;
    int cz = 80.0f;
    {
        int ind = (cx + maxx.x) * 3 + (cy + maxx.y) * mapWidth * 3;
        if (ind >= 0 && ind < mapWidth * mapHeight * 3)
            cz = h[ind];
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
    }
}

void labelText(const glm::vec3 &pos, const glm::vec2 &size, const glm::vec4 &col, const std::string &text,
               AM::FontSize fontSize, std::vector<Vertex> &labelVerts)
{
    glm::vec3 textPos = pos;
    GLuint tid = (GLuint)fontSize;
    float spaceWidth = 10.0f;
    float padding = 5.0f;
    float off = 0.0f, betweenWidth = 1.0f;

    float widthSum = 0.0f, maxY = 0.0f;
    for (int i = 0; i < text.size(); ++i) {
        if (AM::atlasInfo[fontSize][text[i]].set) {
            widthSum += AM::atlasInfo[fontSize][text[i]].width + betweenWidth;
            if (maxY < AM::atlasInfo[fontSize][text[i]].height)
                maxY = AM::atlasInfo[fontSize][text[i]].height;
        }
        else
            widthSum += spaceWidth;
    }
    textPos.x = (textPos.x + size.x * 0.5f) - widthSum * 0.5f;
    textPos.y = (textPos.y + size.y * 0.5f) - maxY * 0.5f;

    for (int i = 0; i < text.size(); ++i) {
        // for (int i = 0; i < text.size() && i < max; ++i) {
        glm::vec3 rPos;
        glm::vec2 rSize, rLBTC, tcLen;
        if (AM::atlasInfo[fontSize][text[i]].set) {
            rPos = {textPos.x + off, textPos.y + AM::atlasInfo[fontSize][text[i]].yPos, textPos.z};
            // this 2 if's are walkaround around bug. If we draw char at x or y with floating point
            // it gets more blury the closer it gets to .5f, at .0f it looks fine
            if (rPos.x > (((float)((int)rPos.x))))
                rPos.x = ((int)(rPos.x + 0.5f));
            if (rPos.y > (((float)((int)rPos.y))))
                rPos.y = ((int)(rPos.y + 0.5f));
            rSize = {AM::atlasInfo[fontSize][text[i]].width, AM::atlasInfo[fontSize][text[i]].height};
            rLBTC = {AM::atlasInfo[fontSize][text[i]].tcX, AM::atlasInfo[fontSize][text[i]].tcY};
            tcLen = {AM::atlasInfo[fontSize][text[i]].tcWidth, AM::atlasInfo[fontSize][text[i]].tcHeight};
            off += rSize.x + betweenWidth;

            labelVerts.push_back(
                Vertex{.pos = rPos, .color = col, .tc = glm::vec2{rLBTC.x, rLBTC.y}, .textureId = (float)tid});
            labelVerts.push_back(Vertex{.pos = glm::vec3{rPos.x, rPos.y + rSize.y, rPos.z},
                                        .color = col,
                                        .tc = glm::vec2{rLBTC.x, rLBTC.y + tcLen.y},
                                        .textureId = (float)tid});
            labelVerts.push_back(Vertex{.pos = glm::vec3{rPos.x + rSize.x, rPos.y + rSize.y, rPos.z},
                                        .color = col,
                                        .tc = glm::vec2{rLBTC.x + tcLen.x, rLBTC.y + tcLen.y},
                                        .textureId = (float)tid});
            labelVerts.push_back(labelVerts.back());
            labelVerts.push_back(Vertex{.pos = glm::vec3{rPos.x + rSize.x, rPos.y, rPos.z},
                                        .color = col,
                                        .tc = glm::vec2{rLBTC.x + tcLen.x, rLBTC.y},
                                        .textureId = (float)tid});
            labelVerts.push_back(
                Vertex{.pos = rPos, .color = col, .tc = glm::vec2{rLBTC.x, rLBTC.y}, .textureId = (float)tid});
        }
        else {
            rPos = {textPos.x + off, textPos.y, textPos.z};
            rSize = {spaceWidth, 10.0};
            rLBTC = {0, 0};
            tcLen = {0, 0};
            off += rSize.x;
        }
    }
}
void Game::makeCountryNames(const unsigned char *h)
{
    Log("makecountrynames");
    ctrProvs.clear();
    fontVerts.clear();
    for (std::size_t i = 0; i < ctrsData.size(); ++i) ctrProvs.push_back(std::vector<int>{});
    for (auto &pd : provsData) {
        if (pd.ctrId == -1)
            continue;
        ctrProvs[pd.ctrId].push_back(pd.id);
    }
    for (std::size_t i = 0; i < ctrsData.size(); ++i) {
        f(fontVerts, ctrsData[i].id, provsData, ctrProvs, mapWidth, mapHeight, scale, h);
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

Game::Game(Window &win, sf::TcpSocket &sock, std::string countryName, glm::vec2 res,
           std::vector<std::shared_ptr<Country>> &countries)
    : Scene{win, res},
      socket(sock),
      resolution(res),
      countries(countries),
      model3d("src/img/DudeDonePosefix.obj", glm::vec3{0.0, 0.0, 0.1})
{
    windowSize = window.GetSize();

    provinces = ProvinceLoader::Load(colorToId);
    std::sort(provinces.begin(), provinces.end(),
              [](const std::unique_ptr<Province> &p1, const std::unique_ptr<Province> &p2) {
                  return p1->GetId() < p2->GetId();
              });

    // setCountryMap();
    Log("Ctr name:" << countryName);
    auto it = std::find_if(countries.begin(), countries.end(), [countryName](const std::shared_ptr<Country> c) {
        return c->GetName() == countryName;
    });
    if (it != countries.end()) {
        myCountry = *it;
        Log(countryName);
        for (auto &prov : provinces) {
            if (prov->GetCountryId() == myCountry->GetId()) {
                camera.SetPos(prov->GetUnitPos().x * scale, prov->GetUnitPos().y * scale);
                break;
            }
        }
    }
    else
        Log("There is no country for you ;(");

    ctype = ClickEventType::MISS;
}

Game::~Game() {}

void Game::editCountryMap2(MapTexture *mt)
{
    for (std::size_t i = 0; i < provinces.size(); ++i) {
        map2->mapTextures.country.pix[i * 3 + 0] = ctrsData[provinces[i]->GetCountryId()].r;
        map2->mapTextures.country.pix[i * 3 + 1] = ctrsData[provinces[i]->GetCountryId()].g;
        map2->mapTextures.country.pix[i * 3 + 2] = ctrsData[provinces[i]->GetCountryId()].b;
    }
    map2->mapTextures.country.Update();
}

void Game::setCountryMap()
{
    //std::unordered_map<Color, Color, CCC::Hash> provCToCountryC;
    //for (auto &p : provinces) {
    //    provCToCountryC[p->GetColor()] = countries[p->GetCountryId()]->GetColor();
    //}

    //map.DrawCountries(provCToCountryC);

    //for (auto &prov : provinces) {
    //    if (prov->GetSieged() == 100) {
    //        assert(prov->GetSiegeCountryId() >= 0 && prov->GetSiegeCountryId() < (int)countries.size());
    //        auto siegeCountry = countries.at(prov->GetSiegeCountryId());
    //        map.DrawSieged(prov->GetColor(), siegeCountry->GetColor());
    //    }
    //}
}

void Game::Play()
{
    float currTime = 0.0f;
    float displayTime = 0.0f;
    float framesTime = 0.0f;
    bool display = true;
    int frames = 0;

    Shader fontShader{"src/graphics/shaders/fonts.vert", "src/graphics/shaders/fonts.frag", "", ""};
    pickModelShader =
        Shader{"src/graphics/shaders/pick_model/vert.v", "src/graphics/shaders/pick_model/frag.f", "", ""};
    labelShader = Shader{"src/graphics/shaders/label/vert.v", "src/graphics/shaders/label/frag.f", "", ""};
    glPatchParameteri(GL_PATCH_VERTICES, 3);
    float trCount = 150;
    int err = glGetError();
    if (err)
        Log("Opengl error: " << err);

    Texture terrainTexture{"src/img/terrain_map.png", mapWidth, mapHeight};
    Texture waterMap{"src/img/Blank_map.png", mapWidth, mapHeight};
    heightMap = std::make_unique<Texture>("src/img/Heightmap.png", mapWidth, mapHeight);
    Texture ctrsText{"src/img/countries_map.png", mapWidth, mapHeight};

    GLint tex[32];
    for (GLint i = 0; i < 32; ++i) {
        tex[i] = i;
    }

    std::map<int, Node> nodes;
    loadProvData(provsData, colorToId);
    loadCountriesData(ctrsData);
    ctrProvs.reserve(ctrsData.size());

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

    const unsigned char *h = heightMap->GetPixels();
    makeCountryNames(h);

    err = glGetError();
    if (err)
        Log("Opengl error: " << err);

    std::vector<Color3> provsCols;
    provsCols.reserve(provinces.size());
    for (auto &p : provsData) provsCols.push_back(Color3{p.r, p.g, p.b});

    map2 = std::make_unique<Map2>(h, mapWidth, mapHeight, provsCols, scale, heightMap->GetId());
    map2->ReloadShaders();
    {
        map2->mapTextures.country.pix = new unsigned char[provinces.size() * 3];
        map2->mapTextures.province.pix = new unsigned char[provinces.size() * 3];
        map2->mapTextures.country.w = provinces.size();
        map2->mapTextures.country.h = 1;
        map2->mapTextures.province.w = provinces.size();
        map2->mapTextures.province.h = 1;

        editCountryMap2(&map2->mapTextures.country);
        for (std::size_t i = 0; i < provinces.size(); ++i) {
            map2->mapTextures.province.pix[i * 3 + 0] = provsCols[i].r;
            map2->mapTextures.province.pix[i * 3 + 1] = provsCols[i].g;
            map2->mapTextures.province.pix[i * 3 + 2] = provsCols[i].b;
        }

        map2->mapTextures.province.Update();
    }

    const unsigned char *pix = map2->provTexture.GetPixels();

    GLuint otherTexID[32];
    for (GLint i = 0; i < 32; ++i) otherTexID[i] = i;
    otherTexID[0] = AM::am.modelTexture->GetId();

    struct TempUnit {
        glm::vec3 pos;
        int provId;
    };
    std::vector<TempUnit> tempUnits;
    for (std::size_t i = 0; i < provsData.size(); ++i) {
        if (provsData[i].water)
            continue;

        float z = (float)heightMap->GetPixels()[(int)(provsData[i].x * 3 + provsData[i].y * mapWidth * 3)];
        tempUnits.push_back(TempUnit{.pos = glm::vec3{provsData[i].x * scale, provsData[i].y * scale, z},
                                     .provId = provsData[i].id});
    }

    GLuint unitBuffer;
    {
        int maxMats = 500;
        int matSize = sizeof(glm::mat4);
        int maxUMatSize = maxMats * matSize;
        glBindVertexArray(model3d.vao);
        glGenBuffers(1, &unitBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, unitBuffer);
        glBufferData(GL_ARRAY_BUFFER, maxUMatSize, NULL, GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void *)0);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void *)(sizeof(glm::vec4)));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void *)(2 * sizeof(glm::vec4)));
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void *)(3 * sizeof(glm::vec4)));
        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);
        glBindVertexArray(0);
    }

    bool drawBorders = true;
    float tesLevel = 32.0f;
    float waterTime = 0.0f;
    float ctrNamesFade = 0.0f, ctrNamesFadeIn = 0.0f;
    float time = glfwGetTime();
    float rotateText;
    guiLast.init(&window, resolution, windowSize);
    while (!window.ShouldClose()) {
        //
        receivePackets();
        updateGui();
        sendPackets();
        updateBattles();

        glEnable(GL_DEPTH_TEST);  // Enable depth testing for z-culling
        glDepthFunc(GL_LESS);
        window.Refresh();

        if (window.keys['M']) {
            map2->ReloadShaders();
            labelShader = Shader{"src/graphics/shaders/label/vert.v", "src/graphics/shaders/label/frag.f", "", ""};
            fontShader = Shader{"src/graphics/shaders/fonts.vert", "src/graphics/shaders/fonts.frag", "", ""};
            pickModelShader =
                Shader{"src/graphics/shaders/pick_model/vert.v", "src/graphics/shaders/pick_model/frag.f", "", ""};
        }

        glm::mat4 matrix = camera.GetMat();

        map2->ActivateTextures();
        float rotateX = 60.0f * 3.1459265f / 180.0f, yScale = 10.0f;
        glm::mat4 rotate = glm::mat4{1.0f};
        rotate = glm::rotate(glm::mat4{1.0}, rotateX, glm::vec3{1.0, 0.0, 0.0});
        for (auto &unit : units) {
            if (abs(unit.GetFakePos().x - camera.eye.x) > 400)
                continue;
            if (abs(unit.GetFakePos().y - camera.eye.y) > 200)
                continue;
            glm::mat4 unitModel = glm::mat4(1.0);
            unitModel = glm::translate(unitModel, unit.GetFakePos());

            unitModel = unitModel * rotate;
            unitModel = glm::scale(unitModel, glm::vec3{20.0, yScale, 20.0});
            uMat.push_back(unitModel);
            pids.push_back(unit.GetProvId());
        }

        input();

        if (window.keys['I']) {
        }

        if (window.keys['L'])
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        if (window.keys['K'])
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        if (window.keys['B']) {
            drawBorders = !drawBorders;
        }

        if (window.keys['P']) {
            map2->DrawForColorPick(matrix, (float)provinces.size());
        }
        else {
            map2->DrawWater(matrix, camera.eye);
            if (openPeaceOfferId == -1) {
                map2->DrawLand(matrix, camera.eye, markedProvId, provinces.size(), map2->MAPID_COUNTRY, waterTime);
            }
            else {  // open offering peace mode
                    // Log("peace mode");
                unsigned char *nexpix = new unsigned char[mapWidth * 4];
                Color lightBlue{25, 94, 176, 255};
                Color green{5, 142, 72, 255};
                Color lightRed{159, 95, 95, 255};
                Color red{229, 0, 0, 255};
                Color neutral{160, 160, 160, 255};
                Color *col = &neutral;
                int peaceind = -1;
                for (std::size_t i = 0; i < peaceOffers.size(); ++i) {
                    if (peaceOffers[i].peaceId == openPeaceOfferId) {
                        peaceind = i;
                        break;
                    }
                }
                if (peaceind >= 0 && peaceind < peaceOffers.size()) {
                    for (int i = 0; i < provinces.size(); ++i) {
                        if (std::find_if(
                                peaceOffers[peaceind].lostProv.begin(), peaceOffers[peaceind].lostProv.end(),
                                [id = i](const std::tuple<int, int, int> &t) { return std::get<0>(t) == id; }) !=
                            peaceOffers[peaceind].lostProv.end())
                        {
                            col = &red;
                        }
                        else if (std::find_if(peaceOffers[peaceind].gainProv.begin(),
                                              peaceOffers[peaceind].gainProv.end(),
                                              [id = i](const std::tuple<int, int, int> &t) {
                                                  return std::get<0>(t) == id;
                                              }) != peaceOffers[peaceind].gainProv.end())
                        {
                            col = &green;
                        }
                        else if (peaceOffers[peaceind].offeredBy == provinces[i]->GetCountryId() ||
                                 peaceOffers[peaceind].recipant == provinces[i]->GetCountryId())
                        {
                            if (peaceOffers[peaceind].offeredBy == provinces[i]->GetCountryId())
                                col = &lightBlue;
                            else
                                col = &lightRed;
                        }
                        else {
                            col = &neutral;
                        }
                        if (i == 3018) {
                            // Log((int)col->r << " " << (int)col->g << " " << (int)col->b);
                        }

                        nexpix[i * 4] = col->r;
                        nexpix[i * 4 + 1] = col->g;
                        nexpix[i * 4 + 2] = col->b;
                        nexpix[i * 4 + 3] = col->a;
                    }
                    Texture newtex = Texture(nexpix, provinces.size(), 1);
                    delete [] nexpix;
                    map2->texID[(int)(map2->MAPID_PEACE_OFFER + 0.5f)] = newtex.GetId();
                    map2->ActivateTextures();
                    map2->DrawLand(matrix, camera.eye, markedProvId, provinces.size(), map2->MAPID_PEACE_OFFER,
                                   0.0f);
                }
            }
        }

        if (drawBorders) {
            map2->DrawBorders(matrix);
        }

        std::vector<std::size_t> uinds;
        float zPoint = 1500.0f;
        if (camera.eye.z < zPoint && !window.keys['U']) {

            if (openUnitId != -1) {
                auto unit = std::find_if(units.begin(), units.end(),
                                         [id = openUnitId](const Unit &u) { return u.GetId() == id; });
                if (unit != units.end()) {
                    std::vector<BorderVertex> verts;
                    if (unit->movesVec.size()) {
                        verts.push_back(
                            BorderVertex{.pos = Vec3{unit->fakePos.x, unit->fakePos.y, unit->fakePos.z},
                                         .tc = Vec2{unit->fakePos.x / mapWidth, unit->fakePos.y / mapHeight}});
                    }
                    for (std::size_t i = 0; i < unit->movesVec.size(); ++i) {
                        glm::vec3 pos = unit->movesVec[i];
                        Log(pos.x << " " << pos.y << " " << pos.z);
                        int ind = pos.x * 3 + pos.y * mapWidth * 3;
                        if (ind > mapHeight * mapWidth * 3 - 3)
                            ind = mapHeight * mapWidth * 3 - 3;
                        verts.push_back(BorderVertex{.pos = Vec3{pos.x, pos.y, pos.z},
                                                     .tc = Vec2{pos.x / mapWidth, pos.y / mapHeight}});
                        if (i < unit->movesVec.size() - 1) {
                            verts.push_back(BorderVertex{.pos = Vec3{pos.x, pos.y, pos.z},
                                                         .tc = Vec2{pos.x / mapWidth, pos.y / mapHeight}});
                        }
                    }
                    GLuint err = 0;
                    // glGetError();
                    // if (err)
                    //     Log("Opengl error: " << err);
                    GLuint arrvao, arrvbo;
                    glCreateVertexArrays(1, &arrvao);
                    glBindVertexArray(arrvao);
                    glCreateBuffers(1, &arrvbo);
                    err = glGetError();
                    if (err)
                        Log("Opengl error: " << err);

                    glDisable(GL_DEPTH_TEST);  // Enable depth testing for z-culling
                    glBindBuffer(GL_ARRAY_BUFFER, arrvbo);
                    glBufferData(GL_ARRAY_BUFFER, sizeof(BorderVertex) * verts.size(), verts.data(),
                                 GL_STATIC_DRAW);
                    err = glGetError();
                    if (err)
                        Log("Opengl error: " << err);
                    glEnableVertexArrayAttrib(arrvao, 0);
                    glEnableVertexArrayAttrib(arrvao, 1);
                    err = glGetError();
                    if (err)
                        Log("Opengl error: " << err);
                    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(BorderVertex), NULL);
                    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(BorderVertex),
                                          (const GLvoid *)(offsetof(BorderVertex, BorderVertex::tc)));
                    glUseProgram(map2->seaBorderShader.GetProgram());
                    glUniformMatrix4fv(glGetUniformLocation(map2->seaBorderShader.GetProgram(), "matrix"), 1,
                                       GL_FALSE, glm::value_ptr(camera.GetMat()));
                    glBindVertexArray(arrvao);
                    glBindBuffer(GL_ARRAY_BUFFER, arrvbo);
                    glDrawArrays(GL_LINES, 0, verts.size());
                    err = glGetError();
                    if (err)
                        Log("Opengl error: " << err);
                    glEnable(GL_DEPTH_TEST);  // Enable depth testing for z-culling
                }
            }
            float rotateX = 60.0f * 3.1459265f / 180.0f, yScale = 10.0f;
            glm::mat4 rotate = glm::mat4{1.0f};
            rotate = glm::rotate(glm::mat4{1.0}, rotateX, glm::vec3{1.0, 0.0, 0.0});
            for (std::size_t i = 0; i < units.size(); ++i) {
                if (abs(units[i].GetFakePos().x - camera.eye.x) > 400)
                    continue;
                if (abs(units[i].GetFakePos().y - camera.eye.y) > 200)
                    continue;
                uinds.push_back(i);
            }

            {                              // print unit labels;
                glDisable(GL_DEPTH_TEST);  // Enable depth testing for z-culling

                std::vector<Vertex> labelVerts;
                // for (std::size_t i = 0; i < uinds.size(); ++i) {
                for (std::size_t i : uinds) {
                    // for (auto &unit : units) {
                    //     if (abs(unit.GetFakePos().x - camera.eye.x) > 400)
                    //         continue;
                    //     if (abs(unit.GetFakePos().y - camera.eye.y) > 200)
                    //         continue;
                    glm::mat4 unitModel = glm::mat4(1.0);
                    unitModel = glm::translate(unitModel, units[i].GetFakePos());
                    auto asd = units[i].GetFakePos();
                    // unitModel = unitModel * rotate;
                    unitModel = glm::scale(unitModel, glm::vec3{20.0, yScale, 20.0});

                    int fontSize = 2;
                    float ww = 70.0f, hh = 25.0f;
                    Vertex fv{
                        .pos = glm::vec3{asd.x - ww * 0.6f, asd.y - hh - 8.0f, asd.z},
                        //.pos = Vec3{-1.5f, -1.5f, 0.0f},
                        .color = glm::vec4{1.0f, 0.0f, 0.0f, 1.0f},
                        .tc = glm::vec2{0.0f, 0.0f},
                        .textureId = (int)22,
                    };
                    // auto *mlptr = glm::value_ptr(unitModel);
                    // for (int i = 0; i < 16; ++i) {
                    //     fv.ml[i] = *(mlptr + i);
                    // }
                    labelVerts.push_back(fv);
                    fv.pos.y += hh;
                    labelVerts.push_back(fv);
                    fv.pos.x += ww;
                    labelVerts.push_back(fv);
                    labelVerts.push_back(fv);
                    fv.pos.y -= hh;
                    labelVerts.push_back(fv);
                    fv.pos.x -= ww;
                    labelVerts.push_back(fv);

                    fv.pos.z += 1.0f;
                    std::string labtext = int_to_string(units[i].soldiers);
                    labelText(fv.pos, glm::vec2{ww, hh}, glm::vec4{0.0f, 1.0f, 0.0f, 1.0f}, labtext,
                              AM::FontSize::PX32, labelVerts);
                }
                glUseProgram(labelShader.GetProgram());
                for (int i = 0; i <= (int)AM::FontSize::PX160; ++i) {
                    glActiveTexture(GL_TEXTURE0 + i);
                    glBindTexture(GL_TEXTURE_2D, (GLuint)fontTexID[i]);
                }
                glUniformMatrix4fv(glGetUniformLocation(labelShader.GetProgram(), "matrix"), 1, GL_FALSE,
                                   glm::value_ptr(matrix));
                glUniform1iv(glGetUniformLocation(labelShader.GetProgram(), "tex"), 32, tex);
                GLuint labelVao, labelVbo;
                glCreateVertexArrays(1, &labelVao);
                glBindVertexArray(labelVao);
                glCreateBuffers(1, &labelVbo);

                glBindBuffer(GL_ARRAY_BUFFER, labelVbo);
                glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * labelVerts.size(), labelVerts.data(),
                             GL_STATIC_DRAW);
                glEnableVertexArrayAttrib(labelVao, 0);
                glEnableVertexArrayAttrib(labelVao, 1);
                glEnableVertexArrayAttrib(labelVao, 2);
                glEnableVertexArrayAttrib(labelVao, 3);
                GLuint err = glGetError();
                if (err)
                    Log("Opengl error: " << err);
                glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL);  //(const GLvoid*)0);
                glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                                      (const GLvoid *)(offsetof(Vertex, Vertex::tc)));
                glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                                      (const GLvoid *)(offsetof(Vertex, Vertex::color)));
                glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                                      (const GLvoid *)(offsetof(Vertex, Vertex::textureId)));

                glBindVertexArray(labelVao);
                glDrawArrays(GL_TRIANGLES, 0, labelVerts.size());

                glEnable(GL_DEPTH_TEST);  // Enable depth testing for z-culling
            }
            for (int i = 0; i < 32; ++i) {
                glActiveTexture(GL_TEXTURE0 + i);
                glBindTexture(GL_TEXTURE_2D, otherTexID[i]);
            }
            for (auto i : uinds) {
                if (abs(units[i].GetFakePos().x - camera.eye.x) > 400)
                    continue;
                if (abs(units[i].GetFakePos().y - camera.eye.y) > 200)
                    continue;
                glm::mat4 unitModel = glm::mat4(1.0);
                unitModel = glm::translate(unitModel, units[i].GetFakePos());

                unitModel = unitModel * rotate;
                unitModel = glm::scale(unitModel, glm::vec3{20.0, yScale, 20.0});

                glUseProgram(AM::am.shader->GetProgram());
                glUniformMatrix4fv(glGetUniformLocation(AM::am.shader->GetProgram(), "matrix"), 1, GL_FALSE,
                                   glm::value_ptr(matrix));
                glUniformMatrix4fv(glGetUniformLocation(AM::am.shader->GetProgram(), "ml"), 1, GL_FALSE,
                                   glm::value_ptr(unitModel));
                glUniform1iv(glGetUniformLocation(AM::am.shader->GetProgram(), "tex"), 32, tex);
                model3d.Draw();
            }
        }
        uMat.clear();

        for (int i = 0; i <= (int)AM::FontSize::PX160; ++i) {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, (GLuint)fontTexID[i]);
        }

        if (camera.eye.z > zPoint) {
            ctrNamesFade = 0.0f;
        }
        else {
            ctrNamesFadeIn = 0.0f;
        }
        // Log(ctrNamesFadeIn << ", fade: " << ctrNamesFade);
        if (((camera.eye.z > zPoint) || ctrNamesFade < 1.0f) && openPeaceOfferId == -1) {
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

        //
        // glDisable(GL_DEPTH_TEST);  // Enable depth testing for z-culling
        glEnable(GL_DEPTH_TEST);  // Enable depth testing for z-culling
        // glDepthFunc(GL_GREATER);
        matrix = glm::ortho(0.0f, (float)resolution.x, 0.0f, (float)resolution.y);
        glUseProgram(shader.GetProgram());
        glUniformMatrix4fv(glGetUniformLocation(shader.GetProgram(), "matrix"), 1, GL_FALSE,
                           glm::value_ptr(matrix));

        GLuint ts[] = {0};
        glBindTextures(ts[0], 1, ts);
        glActiveTexture(GL_TEXTURE0);
        guiLast.start();

        guiDraw();
        guiLast.flush();

        window.scrollOffset = 0.0f;
        window.Update();
        pids.clear();
        waterTime += dt;
        dt = glfwGetTime() - time;
        ctrNamesFade += dt;
        if (ctrNamesFade < 0.0f)
            ctrNamesFade = 10.0f;
        ctrNamesFadeIn -= dt;
        if (ctrNamesFadeIn > 0.0f)
            ctrNamesFadeIn = -10.0f;
        time = glfwGetTime();
    }
}

void Game::guiDraw()
{
    ctype.ct = ClickEventType::MISS;
    glm::vec2 mp{window.xMouse * resolution.x / window.GetSize().x,
                 (window.GetSize().y - window.yMouse) * resolution.y / window.GetSize().y};

    GuiLast::GuiEv tmpctype;
    tmpctype = guiLast.game_topBar(topBarData, mp.x, mp.y);
    if (tmpctype.ct != ClickEventType::MISS)
        ctype = tmpctype;

    tmpctype = guiLast.game_SideBar(sideBarData, mp.x, mp.y, window.mouseLClicked, window.scrollOffset);
    if (tmpctype.ct != ClickEventType::MISS)
        ctype = tmpctype;

    if (openMyCountry) {
        tmpctype = guiLast.game_myCountry(*myCountry, mp.x, mp.y);
        if (window.mouseLClicked && tmpctype.ct == ClickEventType::CLOSE_WINDOW)
            openMyCountry = false;
        else if (tmpctype.ct != ClickEventType::MISS)
            ctype = tmpctype;
    }

    if (openProvId != -1) {
        tmpctype = guiLast.game_prov(*provinces[openProvId], mp.x, mp.y, window.mouseLClicked,
                                     provinces[openProvId]->GetCountryId() == myCountry->GetId());
        if (window.mouseLClicked && tmpctype.ct == ClickEventType::CLOSE_WINDOW)
            openProvId = -1;
        else if (tmpctype.ct != ClickEventType::MISS)
            ctype = tmpctype;
    }

    if (openBattleId != -1) {
        assert(battles[openBattleId].provId >= 0 && battles[openBattleId].provId < provinces.size());
        tmpctype = guiLast.game_battle(battles[openBattleId], *provinces[battles[openBattleId].provId], mp.x, mp.y,
                                       window.mouseLClicked, countries, units);
        if (window.mouseLClicked && tmpctype.ct == ClickEventType::CLOSE_WINDOW)
            openBattleId = -1;
        else if (tmpctype.ct != ClickEventType::MISS)
            ctype = tmpctype;
    }

    if (openCountryId != -1) {
        tmpctype = guiLast.game_country(countries[openCountryId].get(), myCountry.get(), wars, mp.x, mp.y);
        if (window.mouseLClicked && tmpctype.ct == ClickEventType::CLOSE_WINDOW)
            openCountryId = -1;
        else if (tmpctype.ct != ClickEventType::MISS)
            ctype = tmpctype;
    }

    if (openUnitId != -1) {
        auto unit =
            std::find_if(units.begin(), units.end(), [id = openUnitId](const Unit &u) { return u.GetId() == id; });
        assert(unit != units.end());
        tmpctype = guiLast.game_unit((*unit), mp.x, mp.y, window.mouseLClicked, countries);
        if (window.mouseLClicked && tmpctype.ct == ClickEventType::CLOSE_WINDOW)
            openUnitId = -1;
        else if (tmpctype.ct != ClickEventType::MISS)
            ctype = tmpctype;
    }

    if (openWarId != -1) {
        int warind;
        for (std::size_t i = 0; i < wars.size(); ++i) {
            if (wars[i].id == openWarId) {
                warind = i;
                break;
            }
        }
        tmpctype = guiLast.game_war(&wars[warind], mp.x, mp.y, window.mouseLClicked);
        if (window.mouseLClicked && tmpctype.ct == ClickEventType::CLOSE_WINDOW)
            openWarId = -1;
        else if (tmpctype.ct != ClickEventType::MISS)
            ctype = tmpctype;
    }

    if (openUnitsList == true) {
        std::vector<Unit *> clickedUnits_ptr;
        for (auto uid : clickedUnits) {
            auto unit =
                std::find_if(units.begin(), units.end(), [id = uid](const Unit &u) { return u.GetId() == id; });
            if (unit != units.end()) {
                clickedUnits_ptr.push_back(&(*unit));
            }
        }
        tmpctype = guiLast.game_unitsList(clickedUnits_ptr, mp.x, mp.y, window.mouseLClicked, window.scrollOffset,
                                          provinces[openUnitsListProvId].get(), countries);
        if (window.mouseLClicked && tmpctype.ct == ClickEventType::CLOSE_WINDOW)
            openUnitsList = false;
        else if (tmpctype.ct != ClickEventType::MISS)
            ctype = tmpctype;
    }

    if (openPeaceOfferId != -1) {
        int peaceind;
        for (std::size_t i = 0; i < peaceOffers.size(); ++i) {
            if (peaceOffers[i].peaceId == openPeaceOfferId) {
                peaceind = i;
                break;
            }
        }
        assert(peaceind >= 0 && peaceind < peaceOffers.size());
        tmpctype = guiLast.game_peaceOffer(&peaceOffers[peaceind], mp.x, mp.y, myCountry->GetId(), provinces,
                                           countries, window.mouseLClicked, window.scrollOffset);
        if (window.mouseLClicked && tmpctype.ct == ClickEventType::CLOSE_WINDOW) {
            openPeaceOfferId = -1;
        }
        else if (tmpctype.ct != ClickEventType::MISS)
            ctype = tmpctype;
    }

    if (window.mouseLClicked) {
        std::vector<sf::Packet> packets;
        switch (ctype.ct) {
            case ClickEventType::OPEN_MY_COUNTRY: {
                resetGuiWindows();
                openMyCountry = true;
                break;
            }
            case ClickEventType::OPEN_COUNTRY: {
                resetGuiWindows();
                if (myCountry->GetId() == ctype.val)
                    openMyCountry = true;
                else
                    openCountryId = ctype.val;
                break;
            }
            case ClickEventType::OPEN_PROV: {
                resetGuiWindows();
                openProvId = ctype.val;
                break;
            }
            case ClickEventType::OPEN_COUNTRY_FROM_PROV: {
                resetGuiWindows();
                openCountryId = provinces[openProvId]->GetCountryId();
                break;
            }
            case ClickEventType::CREATE_UNIT: {
                packets.emplace_back(
                    PreparePacket::NewUnit(provinces[openProvId]->GetCountryId(), openProvId, ctype.val));
                break;
            }
            case ClickEventType::OPEN_UNIT: {
                resetGuiWindows();
                openUnitId = ctype.val;
                break;
            }
            case ClickEventType::SET_SPEED: {
                packets.emplace_back(PreparePacket::SetSpeed(ctype.val));
                break;
            }
            case ClickEventType::SIDEBAR_LEFTC: {
                int ind = ctype.val;
                assert(ind >= 0 && ind < sideBarData.elements.size());
                if (sideBarData.elements[ind].type == SideBarData::IType::PEACE_OFFER) {
                    resetGuiWindows();
                    openPeaceOfferId = sideBarData.elements[ind].val;  // ctype.val;
                }
                else if (sideBarData.elements[ind].type == SideBarData::IType::WAR) {
                    resetGuiWindows();
                    openWarId = sideBarData.elements[ind].val;
                }
                else {
                    sideBarData.elements.erase(sideBarData.elements.begin() + ind);
                }
                break;
            }
            case ClickEventType::DECLARE_WAR: {
                packets.emplace_back(PreparePacket::DeclareWar(ctype.val));
                break;
            }
            case ClickEventType::OPEN_WAR_WINDOW: {
                resetGuiWindows();
                openWarId = ctype.val;
            }
            case ClickEventType::OPEN_OFFER_PEACE: {
                Log(openPeaceOfferId);
                if (openPeaceOfferId == -1) {
                    int warind; 
                    for (int i = peaceOffers.size() - 1; i >= 0; --i) {
                        if (peaceOffers[i].peaceId == -2) {
                            peaceOffers.erase(peaceOffers.begin() + i);
                        }
                    }
                    for (std::size_t i = 0; i < wars.size(); ++i) {
                        if ((openWarId != -1 && wars[i].id == openWarId) || (openWarId == -1 && wars[i].id == ctype.val)) {
                            warind = i;
                            break;
                        }
                    }
                    Log(warind);
                    int recipantId = wars[warind].attackersIds[0];
                    if (recipantId == myCountry->GetId())
                        recipantId = wars[warind].defendersIds[0];
                    peaceOffers.push_back(PeaceOffer{myCountry->GetId(), wars[warind].id, recipantId});
                    resetGuiWindows();
                    openPeaceOfferId = -2;
                }
                break;
            }
            case ClickEventType::SEND_PEACE_OFFER: {
                int peaceind;
                for (std::size_t i = 0; i < peaceOffers.size(); ++i) {
                    if (peaceOffers[i].peaceId == ctype.val) {
                        peaceind = i;
                        break;
                    }
                }
                assert(peaceind >= 0 && peaceind < peaceOffers.size());
                for (std::size_t ind = 0; ind < sideBarData.elements.size(); ++ind) {
                    if (sideBarData.elements[ind].type == SideBarData::IType::PEACE_OFFER) {
                        sideBarData.elements.erase(sideBarData.elements.begin() + ind);
                        break;
                    }
                }
                if (peaceOffers[peaceind].peaceId == -2) {
                    sf::Packet packet;
                    packet << "PeaceOffer";
                    packet << peaceOffers[peaceind].warId;
                    packet << peaceOffers[peaceind].recipant;
                    packet << (unsigned int)(peaceOffers[peaceind].gainProv.size() +
                                             peaceOffers[peaceind].lostProv.size());
                    for (std::size_t i = 0; i < peaceOffers[peaceind].lostProv.size(); ++i) {
                        packet << std::get<0>(peaceOffers[peaceind].lostProv[i]);
                        packet << std::get<1>(peaceOffers[peaceind].lostProv[i]);
                    }
                    for (std::size_t i = 0; i < peaceOffers[peaceind].gainProv.size(); ++i) {
                        packet << std::get<0>(peaceOffers[peaceind].gainProv[i]);
                        packet << std::get<1>(peaceOffers[peaceind].gainProv[i]);
                    }
                    peaceOffers.erase(peaceOffers.begin() + peaceind);
                    packets.push_back(packet);
                }
                else if (peaceOffers[peaceind].peaceId != -1) {
                    sf::Packet packet;
                    packet << "AcceptPeace";
                    packet << peaceOffers[peaceind].peaceId;
                    packets.push_back(packet);
                    for (std::size_t ind = 0; ind < sideBarData.elements.size(); ++ind) {
                        if (sideBarData.elements[ind].type == SideBarData::IType::WAR &&
                            sideBarData.elements[ind].val == peaceOffers[peaceind].warId)
                        {
                            sideBarData.elements.erase(sideBarData.elements.begin() + ind);
                            break;
                        }
                    }
                    peaceOffers.erase(peaceOffers.begin() + peaceind);
                }
                resetGuiWindows();
                break;
            }
            case ClickEventType::REJECT_PEACE_OFFER: {
                int peaceind;
                for (std::size_t i = 0; i < peaceOffers.size(); ++i) {
                    if (peaceOffers[i].peaceId == ctype.val) {
                        peaceind = i;
                        break;
                    }
                }
                assert(peaceind >= 0 && peaceind < peaceOffers.size());
                sf::Packet packet;
                packet << "DeclinePeace";
                packet << peaceOffers[peaceind].peaceId;
                packets.push_back(packet);

                for (std::size_t ind = 0; ind < sideBarData.elements.size(); ++ind) {
                    if (sideBarData.elements[ind].type == SideBarData::IType::PEACE_OFFER) {
                        sideBarData.elements.erase(sideBarData.elements.begin() + ind);
                        break;
                    }
                }
                resetGuiWindows();
                break;
            }
            case ClickEventType::MERGE_UNITS: {
                if (openUnitsList != true)
                    break;
                std::vector<Unit *> clickedUnits_ptr;
                for (auto uid : clickedUnits) {
                    auto unit = std::find_if(units.begin(), units.end(),
                                             [id = uid](const Unit &u) { return u.GetId() == id; });
                    if (unit != units.end()) {
                        clickedUnits_ptr.push_back(&(*unit));
                    }
                }
                sf::Packet packet;
                packet << "MergeUnits";
                packet << (int)clickedUnits_ptr.size();
                for (auto uptr : clickedUnits_ptr) {
                    packet << uptr->id;
                }
                packets.push_back(packet);
                break;
            }
            case ClickEventType::DEL_FROM_UNITS_LIST: {
                for (std::size_t i = 0; i < clickedUnits.size(); ++i) {
                    if (clickedUnits[i] == ctype.val) {
                        clickedUnits.erase(clickedUnits.begin() + i);
                        break;
                    }
                }
                break;
            }
        };
        if (packets.size())
            toSend.insert(toSend.end(), packets.begin(), packets.end());

        window.mouseLClicked = false;
        window.mouseRClicked = false;
    }
}

void Game::receivePackets()
{
    sf::Packet packet;
    while (socket.receive(packet) == sf::Socket::Done) {
        processPacket(packet);
    }
}

void Game::processPacket(sf::Packet packet)
{
    std::string type;
    packet >> type;
    // Log(type);
    if (type == "daily") {
        ProcessPacket::DailyUpdate(packet, wars, provinces, countries, topBarData, map2.get());
    }
    else if (type == "hourly") {
        ProcessPacket::HourlyUpdate(packet, units, battles, scale, heightMap->GetPixels(), mapWidth);
    }
    else if (type == "PeaceAccepted") {
        int warId = ProcessPacket::PeaceAccepted(packet, provinces, countries, wars, sideBarData, provsData);
        editCountryMap2(&map2->mapTextures.country);
        const unsigned char *h = heightMap->GetPixels();
        makeCountryNames(h);
        if (openedProvId >= 0) {
            assert(openedProvId < (int)provinces.size());
        }
    }
    else if (type == "ProvincePopulation") {
        ProcessPacket::ProvincePopulation(packet, provinces);
    }
    else if (type == "NewWar") {
        ProcessPacket::NewWar(packet, wars, myCountry->GetId(), countries, sideBarData);
    }
    else if (type == "SetGold") {
        ProcessPacket::SetGold(packet, myCountry);
    }
    else if (type == "Speed") {
        ProcessPacket::SetSpeed(packet);
    }
    else if (type == "monthly") {
        ProcessPacket::MonthlyUpdate(packet, myCountry->GetName(), countries);
    }
    else if (type == "BotPeaceOffer") {
        ProcessPacket::BotPeaceOffer(packet, peaceOffers, countries, sideBarData, myCountry);
    }
    else if (type == "PeaceDeclined") {
        ProcessPacket::PeaceDeclined(packet, sideBarData, countries);
    }
}

void Game::resetGuiWindows()
{
    openProvId = -1;
    openUnitId = -1;
    openCountryId = -1;
    openPeaceOfferId = -1;
    openMyCountry = false;
    openUnitsList = false;
    openBattleId = -1;
    openWarId = -1;
}

void Game::input()
{
    if (window.keys['A'])
        camera.MoveHor(-1, dt);
    if (window.keys['D'])
        camera.MoveHor(1, dt);
    if (window.keys['W'])
        camera.MoveVer(1, dt);
    if (window.keys['S'])
        camera.MoveVer(-1, dt);
    if (window.keys['R']) {
        window.keys['R'] = false;
    }
    if (window.keys['P']) {
        window.keys['P'] = false;
        sf::Packet packet;
        packet << "Stop";
        toSend.emplace_back(packet);
    }
    if (window.keys['F'])
        camera.Rotate(1, dt);
    if (window.keys['T'])
        camera.Rotate(-1, dt);
    if (window.keys['G'])
        camera.Reset();

    //camera.Update(window.xMouse, windowSize.y - window.yMouse, map.GetHeightTerrain());
    camera.Update(window.xMouse, windowSize.y - window.yMouse, heightMap->GetPixels());

    if (window.scrollOffset && ctype.ct == ClickEventType::MISS) {
        camera.Scroll(window.scrollOffset);
    }

    glm::vec2 mouseInWorld = camera.GetMouseInWorld();

    if (window.mouseLClicked && !window.mouseRClicked) {
        if (ctype.ct == ClickEventType::MISS) {
            if (!unitClick(mouseInWorld)) {
                int pid = provClick(mouseInWorld);
                if (pid != -1) {
                    if (openPeaceOfferId == -1) {
                        resetGuiWindows();
                        bool foundBattle = false;
                        for (std::size_t i = 0; i < battles.size(); ++i) {
                            if (battles[i].GetProvId() == pid) {
                                foundBattle = true;
                                openBattleId = i;
                                Log("openBattleId == " << openBattleId);
                                break;
                            }
                        }

                        if (!foundBattle) {
                            openProvId = pid;
                        }
                    }
                    else {  // offer peace view mode
                        int peaceind;
                        for (std::size_t i = 0; i < peaceOffers.size(); ++i) {
                            if (peaceOffers[i].peaceId == openPeaceOfferId) {
                                peaceind = i;
                                break;
                            }
                        }
                        if ((provinces[pid]->GetCountryId() == peaceOffers[peaceind].offeredBy ||
                             provinces[pid]->GetCountryId() == peaceOffers[peaceind].recipant) &&
                            peaceOffers[peaceind].offeredBy == myCountry->GetId())
                        {
                            auto tmpit = std::find_if(
                                peaceOffers[peaceind].lostProv.begin(), peaceOffers[peaceind].lostProv.end(),
                                [id = pid](const std::tuple<int, int, int> &t) { return std::get<0>(t) == id; });
                            if (tmpit != peaceOffers[peaceind].lostProv.end()) {
                                peaceOffers[peaceind].lostProv.erase(tmpit);
                            }
                            else if (provinces[pid]->GetCountryId() == myCountry->GetId()) {
                                peaceOffers[peaceind].lostProv.push_back(std::make_tuple(
                                    pid, peaceOffers[peaceind].recipant, provinces[pid]->GetDevelopment()));
                            }
                            tmpit = std::find_if(
                                peaceOffers[peaceind].gainProv.begin(), peaceOffers[peaceind].gainProv.end(),
                                [id = pid](const std::tuple<int, int, int> &t) { return std::get<0>(t) == id; });
                            if (tmpit != peaceOffers[peaceind].gainProv.end()) {
                                peaceOffers[peaceind].gainProv.erase(tmpit);
                            }
                            else if (provinces[pid]->GetCountryId() != myCountry->GetId()) {
                                peaceOffers[peaceind].gainProv.push_back(std::make_tuple(
                                    pid, peaceOffers[peaceind].offeredBy, provinces[pid]->GetDevelopment()));
                            }
                        }
                    }
                }
            }
        }
    }
    else if (window.mouseRClicked) {
        if (ctype.ct == ClickEventType::MISS && openUnitId != -1) {
            int pid = provClick(mouseInWorld);
            if (pid != -1) {
                toSend.emplace_back(PreparePacket::MoveUnit(openUnitId, pid));
                Log("Try unit move: " << openUnitId << " => topid: " << pid);
            }
        }
    }
    window.mouseRClicked = false;
}

int Game::provClick(glm::vec2 mouseInWorld)
{
    map2->DrawForColorPick(camera.GetMat(), (float)provinces.size());

    int pixx = window.xMouse, pixy = windowSize.y - window.yMouse;
    unsigned char pixel[4];
    glReadPixels(pixx, pixy, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);

    window.Refresh();
    clickedProvColor = {(int)pixel[0] / 255.0, (int)pixel[1] / 255.0, (int)pixel[2] / 255.0};
    clickedProviPhash = getHash(pixel[0], pixel[1], pixel[2]);
    if (colorToId.find(clickedProviPhash) != colorToId.end()) {
        int pid = colorToId[clickedProviPhash];
        markedProvId = (float)pid;
        unitPos.x = provinces[pid]->GetUnitPos().x * scale;
        unitPos.y = provinces[pid]->GetUnitPos().y * scale;
        unitPos.z = heightMap->GetPixels()[(int)(provinces[pid]->GetUnitPos().x * 3 +
                                                 provinces[pid]->GetUnitPos().y * mapWidth * 3)];

        openedProvId = markedProvId;

        return pid;
    }
    return -1;
}

bool Game::unitClick(glm::vec2 mouseInWorld)
{
    GLuint err;
    {  // check for unit clicks
        glUseProgram(pickModelShader.GetProgram());
        glUniformMatrix4fv(glGetUniformLocation(pickModelShader.GetProgram(), "matrix"), 1, GL_FALSE,
                           glm::value_ptr(camera.GetMat()));
        glBindVertexArray(model3d.rectVao);
        glBindBuffer(GL_ARRAY_BUFFER, model3d.rectVbo);
        for (std::size_t i = 0; i < uMat.size(); ++i) {
            glUniformMatrix4fv(glGetUniformLocation(pickModelShader.GetProgram(), "ml"), 1, GL_FALSE,
                               glm::value_ptr(uMat[i]));
            glm::vec4 pcol{(float)provinces[pids[i]]->GetColor().r / 255.0f,
                           provinces[pids[i]]->GetColor().g / 255.0f, provinces[pids[i]]->GetColor().b / 255.0f,
                           1.0f};
            glUniform4fv(glGetUniformLocation(pickModelShader.GetProgram(), "col"), 1, glm::value_ptr(pcol));

            glDrawArrays(GL_TRIANGLES, 0, model3d.rectVertsCount);
            err = glGetError();
            if (err)
                Log(err);
        }
    }

    unsigned char pixel[4];
    int pixx = window.xMouse, pixy = windowSize.y - window.yMouse;
    glReadPixels(pixx, pixy, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);
    window.Refresh();
    glm::vec3 unitColor = {(int)pixel[0] / 255.0, (int)pixel[1] / 255.0, (int)pixel[2] / 255.0};
    unsigned int clickedUnitPhash = getHash(pixel[0], pixel[1], pixel[2]);

    if (colorToId.find(clickedUnitPhash) != colorToId.end()) {
        Log("Unit click");
        auto provId = colorToId[clickedUnitPhash];
        clickedUnits.clear();
        for (auto &u : units) {
            if (u.GetProvId() == provId)
                clickedUnits.push_back(u.GetId());
        }
        if (clickedUnits.size() == 1) {
            resetGuiWindows();
            openUnitId = clickedUnits[0];
        }
        else if (clickedUnits.size() > 1) {
            resetGuiWindows();
            openUnitsList = true;
        }
        openUnitsListProvId = provId;
        return true;
    }
    else {
        return false;
    }
    return false;
}

void Game::unitMove(std::unordered_map<std::string, std::string> &values, glm::vec2 mouseInWorld)
{
    //Color provinceColor = map.ClickOnProvince(mouseInWorld.x, mouseInWorld.y);
    //auto it = std::find_if(provinces.begin(), provinces.end(), [provinceColor](std::unique_ptr<Province> &p) {
    //    return p->GetColor() == provinceColor;
    //});
    //if (it != provinces.end()) {
    //    auto idIt = values.find("id");
    //    if (idIt == values.end())
    //        return;
    //    sf::Packet packet;
    //    packet << "UnitMove";
    //    packet << std::stoi(idIt->second);
    //    packet << (*it)->GetId();

    //    toSend.push_back(packet);
    //}
}

void Game::processGuiEvent()
{
}

void Game::sendPackets()
{
    for (auto &p : toSend) {
        socket.send(p);
    }
    toSend.clear();
}

void Game::updateBattles()
{
}

void Game::updateGui()
{
}

