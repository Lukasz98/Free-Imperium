#include "game.h"

#include "ctr_data.h"
#include "font_batch.h"
#include "load_data.h"
#include "prov_data.h"
#include "save_borders.h"

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
    auto *mlptr = glm::value_ptr(textml);
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

Game::Game(Window &win, sf::TcpSocket &sock, std::string countryName, glm::vec2 res,
           std::vector<std::shared_ptr<Country>> &countries)
    : Scene{win, res},
      socket(sock),
      resolution(res),
      countries(countries),
      model3d("src/img/DudeDonePosefix.obj", glm::vec3{0.0, 0.0, 0.1})
{
    // gui.AddWin("src/gui/top_bar.txt");
    // gui.AddWin("src/gui/notifications.txt");
    // topBarData.subject.AddObserver(Gui::TopBar::Open(std::vector<std::string>{}, resolution));
    // Gui::SideBar::Open(resolution);
    // Gui::SideBar::Add(resolution);
    // Gui::SideBar::Add(resolution);

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

    // sidebar
    sideBarData.elements.push_back(SideBarData::Element{.type = SideBarData::IType::WAR, .hoverText = "test 1"});
    sideBarData.elements.push_back(SideBarData::Element{.type = SideBarData::IType::WAR, .hoverText = "test 2"});
    sideBarData.elements.push_back(SideBarData::Element{.type = SideBarData::IType::WAR, .hoverText = "test 3"});
    sideBarData.elements.push_back(SideBarData::Element{.type = SideBarData::IType::WAR, .hoverText = "test 4"});
    sideBarData.elements.push_back(SideBarData::Element{.type = SideBarData::IType::WAR, .hoverText = "test 5"});

    //

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
    std::unordered_map<Color, Color, CCC::Hash> provCToCountryC;
    for (auto &p : provinces) {
        // if (p->GetSieged() != 0)
        //    continue;

        provCToCountryC[p->GetColor()] = countries[p->GetCountryId()]->GetColor();
    }

    // Log("buckets="<<provCToCountryC.bucket_count());
    // for (int i = 0; i < provCToCountryC.bucket_count(); i++) {
    //    Log(provCToCountryC.bucket_size(i));
    //}

    map.DrawCountries(provCToCountryC);

    for (auto &prov : provinces) {
        if (prov->GetSieged() == 100) {
            assert(prov->GetSiegeCountryId() >= 0 && prov->GetSiegeCountryId() < (int)countries.size());
            auto siegeCountry = countries.at(prov->GetSiegeCountryId());
            map.DrawSieged(prov->GetColor(), siegeCountry->GetColor());
        }
    }
}

void Game::Play()
{
    // float lastTime = glfwGetTime();
    float currTime = 0.0f;
    float displayTime = 0.0f;
    float framesTime = 0.0f;
    bool display = true;
    int frames = 0;
    // Rectangle testRect = Rectangle{glm::vec3{1000.0, 900.0, 10.0}, glm::vec2{200.0, 200},
    // glm::vec4{1.0,0.0,0.0,1.0}};

    Shader fontShader{"src/graphics/shaders/fonts.vert", "src/graphics/shaders/fonts.frag", "", ""};
    pickModelShader =
        Shader{"src/graphics/shaders/pick_model/vert.v", "src/graphics/shaders/pick_model/frag.f", "", ""};
    glPatchParameteri(GL_PATCH_VERTICES, 3);
    float trCount = 150;
    int err = glGetError();
    if (err)
        Log("Opengl error: " << err);

    Texture terrainTexture{"src/img/terrain_map.png", mapWidth, mapHeight};
    Texture waterMap{"src/img/Blank_map.png", mapWidth, mapHeight};
    heightMap = std::make_unique<Texture>("src/img/Heightmap.png", mapWidth, mapHeight);
    // Texture stoneT{"../shared/smoothstone.png", 64, 64, GL_REPEAT};
    // Texture waterT{"../shared/water1.png", 64, 64, GL_REPEAT};
    // Texture sandT{"src/img/Sand_1.png", 32, 32, GL_REPEAT};
    Texture ctrsText{"src/img/countries_map.png", mapWidth, mapHeight};

    GLint tex[32];
    for (GLint i = 0; i < 32; ++i) {
        tex[i] = i;
    }

    // saveProvinceFromImg(provTexture.GetPixels(), waterMap.GetPixels(), mapWidth, mapHeight);
    std::vector<ProvData> provsData;
    std::vector<std::vector<int>> ctrProvs;
    std::map<unsigned int, int> colorToId;
    std::map<int, Node> nodes;
    loadProvData(provsData, colorToId);
    loadCountriesData(ctrsData);
    ctrProvs.reserve(ctrsData.size());

    // saveBorders(provTexture.GetPixels(), mapWidth, mapHeight, provinces, nodes);
    // saveSeaBorders(provTexture.GetPixels(), mapWidth, mapHeight, provinces, nodes, colorToId);
    // createSaveProvPoints(provTexture.GetPixels(), mapWidth, mapHeight, provinces, colorToId);
    // saveBordersTriangles(mapWidth, mapHeight, scale, heightMap->GetPixels());
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

    const unsigned char *h = heightMap->GetPixels();

    std::vector<FontVertex> fontVerts;
    GLuint fontCtrVao, fontCtrVbo;
    {
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
        // glBufferData(GL_ARRAY_BUFFER, amount * 1 * sizeof(glm::mat4), uMat.data(), GL_STATIC_DRAW);
        glBufferData(GL_ARRAY_BUFFER, maxUMatSize, NULL, GL_DYNAMIC_DRAW);
        // glBufferData(GL_ARRAY_BUFFER, maxUMatSize, NULL, GL_DYNAMIC_DRAW);

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
    // float dt = 0.0f;
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
        // Gui::Base::UpdateVals();
        //

        glEnable(GL_DEPTH_TEST);  // Enable depth testing for z-culling
        glDepthFunc(GL_LESS);
        window.Refresh();

        if (window.keys['M']) {
            map2->ReloadShaders();
            fontShader = Shader{"src/graphics/shaders/fonts.vert", "src/graphics/shaders/fonts.frag", "", ""};
            pickModelShader =
                Shader{"src/graphics/shaders/pick_model/vert.v", "src/graphics/shaders/pick_model/frag.f", "", ""};
        }

        // camera.Update(window.xMouse, windowSize.y - window.yMouse, pix);
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
            // unitModel = glm::scale(unitModel, glm::vec3{80.0, 50.0f, 80.0});
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
            map2->DrawLand(matrix, camera.eye, markedProvId, provinces.size(), map2->MAPID_COUNTRY);
        }

        if (drawBorders) {
            map2->DrawBorders(matrix);
        }
        // Gui::Base::Hover(glm::vec2{window.xMouse * resolution.x / windowSize.x,
        //                           (windowSize.y - window.yMouse) * (resolution.y / windowSize.y)});

        float zPoint = 1500.0f;
#if 1
        if (camera.eye.z < zPoint && !window.keys['U']) {
            for (int i = 0; i < 32; ++i) {
                glActiveTexture(GL_TEXTURE0 + i);
                glBindTexture(GL_TEXTURE_2D, otherTexID[i]);
            }
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
                // unitModel = glm::translate(unitModel, unitPos);

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
            /*
            for (std::size_t i = 0; i < tempUnits.size(); ++i) {
                if (abs(tempUnits[i].pos.x - camera.eye.x) > 400)
                    continue;
                if (abs(tempUnits[i].pos.y - camera.eye.y) > 200)
                    continue;
                glm::mat4 unitModel = glm::mat4(1.0);
                unitModel = glm::translate(unitModel, tempUnits[i].pos);
                // unitModel = glm::translate(unitModel, unitPos);

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
*/
        }
#else
        err = glGetError();
        // if (err)
        // Log("Opengl error: " << err);
        if (camera.eye.z < zPoint && !window.keys['U']) {
            for (int i = 0; i < 32; ++i) {
                glActiveTexture(GL_TEXTURE0 + i);
                glBindTexture(GL_TEXTURE_2D, otherTexID[i]);
            }
            err = glGetError();
            // if (err)
            // Log("Opengl error: " << err);
            err = glGetError();
            if (err)
                Log("Opengl error: " << err);

            glUseProgram(AM::am.modelInstancedShader->GetProgram());
            glUniformMatrix4fv(glGetUniformLocation(AM::am.modelInstancedShader->GetProgram(), "matrix"), 1,
                               GL_FALSE, glm::value_ptr(matrix));
            // glUniformMatrix4fv(glGetUniformLocation(AM::am.shader->GetProgram(), "ml"), 1, GL_FALSE,
            //                   glm::value_ptr(unitModel));
            glUniform1iv(glGetUniformLocation(AM::am.modelInstancedShader->GetProgram(), "tex"), 32, tex);
            // Log(AM::am.model->vao);
            err = glGetError();
            if (err)
                Log("Opengl error: " << err);
            glBindVertexArray(model3d.vao);
            // glBindBuffer(GL_ARRAY_BUFFER, model3d.vbo);
            glBindBuffer(GL_ARRAY_BUFFER, unitBuffer);
            // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model3d.ibo);
            err = glGetError();
            if (err)
                Log("Opengl error: " << err);

            float prep = glfwGetTime();
            glm::mat4 *uData = (glm::mat4 *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
            prep = glfwGetTime() - prep;
            err = glGetError();
            if (err)
                Log("Opengl error: " << err);
            for (std::size_t i = 0; i < uMat.size(); ++i) {
                uData[i] = uMat[i];
            }

            Log("prep=" << prep);

            float dr = glfwGetTime();
            // Log("uMat.size = " << uMat.size());
            // Log("ibo.size = " << AM::am.model->iboCount);
            glDrawElementsInstanced(GL_TRIANGLES, model3d.iboCount, GL_UNSIGNED_INT, NULL, uMat.size());
            err = glGetError();
            if (err)
                Log("Opengl error: " << err);
            // glDrawElementsInstanced(GL_TRIANGLES, AM::am.model->iboCount * uMat.size(), GL_UNSIGNED_INT, NULL,
            //                       uMat.size());
            glUnmapBuffer(GL_ARRAY_BUFFER);
            dr = glfwGetTime() - dr;
            // Log("draw="<<dr);
            err = glGetError();
            if (err)
                Log("Opengl error: " << err);

            // AM::am.model->Draw();
        }
#endif
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
        if ((camera.eye.z > zPoint) || ctrNamesFade < 1.0f) {
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
        glDisable(GL_DEPTH_TEST);  // Enable depth testing for z-culling
        // glEnable(GL_DEPTH_TEST);  // Enable depth testing for z-culling
        // glDepthFunc(GL_GREATER);
        matrix = glm::ortho(0.0f, (float)resolution.x, 0.0f, (float)resolution.y);
        glUseProgram(shader.GetProgram());
        glUniformMatrix4fv(glGetUniformLocation(shader.GetProgram(), "matrix"), 1, GL_FALSE,
                           glm::value_ptr(matrix));

        GLuint ts[] = {0};
        glBindTextures(ts[0], 1, ts);
        glActiveTexture(GL_TEXTURE0);

        // Gui::Base::Draw();
        //
        guiLast.start();
        // guiLast.room_playerListDraw({"asd", "dsa"});

        guiDraw();
        guiLast.flush();

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
        // Log(dt);
        time = glfwGetTime();
    }

    /*
    while (!window.ShouldClose()) {
        receivePackets();
        updateGui();
        sendPackets();
        updateBattles();

        for (auto &u : units) u->UpdateDt(dt);

        if (display || 1) {
            window.Refresh();
            input();

            // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDepthRange(0, 1);
            map.Draw(camera, dt);

            glm::mat4 matrix = camera.GetMat();

            for (auto &u : units) u->Draw(matrix, false);

            glUseProgram(shader.GetProgram());
            glUniformMatrix4fv(glGetUniformLocation(shader.GetProgram(), "matrix"), 1, GL_FALSE,
                               glm::value_ptr(matrix));

            // testRect.Draw();

            {
                glDepthRange(0, 0.19);
                GLuint ts[] = {0};
                glBindTextures(ts[0], 1, ts);

                bool selected = false;
                for (auto &u : units) {
                    if (!selected) {
                        if (!selected)
                            u->DrawGuiElements(false);
                    }
                    else
                        u->DrawGuiElements(false);
                }
            }

            matrix = glm::ortho(0.0f, (float)resolution.x, 0.0f, (float)resolution.y);
            glUseProgram(shader.GetProgram());
            glUniformMatrix4fv(glGetUniformLocation(shader.GetProgram(), "matrix"), 1, GL_FALSE,
                               glm::value_ptr(matrix));

            GLuint ts[] = {0};
            glBindTextures(ts[0], 1, ts);

            Gui::Base::Draw();
            // gui.Draw();
            window.Update();
            frames++;
        }

        Gui::Base::UpdateVals();

        dt = glfwGetTime() - currTime;

        displayTime += dt;
        framesTime += dt;

        if (displayTime < 1.0f / 100.0f) {
            display = false;
        }
        else {
            drawDt = displayTime;
            displayTime = 0.0f;
            display = true;
        }

        if (framesTime >= 1.0f) {
            // Log("Dt: "<<dt );
            framesTime = 0.0f;
            // Log("FPS: " << frames);
            frames = 0;
        }

        currTime = glfwGetTime();
    }
*/
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

    tmpctype = guiLast.game_SideBar(sideBarData, mp.x, mp.y, window.mouseLClicked);
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
        tmpctype = guiLast.game_country(*countries[openCountryId], mp.x, mp.y);
        if (window.mouseLClicked && tmpctype.ct == ClickEventType::CLOSE_WINDOW)
            openCountryId = -1;
        else if (tmpctype.ct != ClickEventType::MISS)
            ctype = tmpctype;
    }

    if (openUnitId != -1) {
        auto unit =
            std::find_if(units.begin(), units.end(), [id = openUnitId](const Unit &u) { return u.GetId() == id; });
        assert(unit != units.end());
        tmpctype = guiLast.game_unit((*unit), mp.x, mp.y, window.mouseLClicked);
        if (window.mouseLClicked && tmpctype.ct == ClickEventType::CLOSE_WINDOW)
            openUnitId = -1;
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
        tmpctype = guiLast.game_unitsList(clickedUnits_ptr, mp.x, mp.y, window.mouseLClicked);
        if (window.mouseLClicked && tmpctype.ct == ClickEventType::CLOSE_WINDOW)
            openUnitsList = false;
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
                openCountryId = ctype.val;
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
                sideBarData.elements.erase(sideBarData.elements.begin() + ind);
                break;
            }
            case ClickEventType::DECLARE_WAR: {
                packets.emplace_back(PreparePacket::DeclareWar(ctype.val));
                break;
            }
        };
        if (packets.size())
            toSend.insert(toSend.end(), packets.begin(), packets.end());
    }

    window.mouseLClicked = false;
    window.mouseRClicked = false;
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
        ProcessPacket::DailyUpdate(packet, wars, provinces, countries, map, topBarData);
    }
    else if (type == "NewUnit") {
        // ProcessPacket::NewUnit(packet, units, countries, myCountry->GetName());
    }
    else if (type == "hourly") {
        ProcessPacket::HourlyUpdate(packet, units, battles, scale, heightMap->GetPixels(), mapWidth);
    }
    else if (type == "NewBattle") {
        // ProcessPacket::NewBattle(packet, units, battles, provinces);
    }
    else if (type == "EndBattle") {
        // rocessPacket::EndBattle(packet, battles);
    }
    else if (type == "DeleteUnit") {
        // ProcessPacket::DeleteUnit(packet, units);
    }
    else if (type == "NewUnitInBattle") {
        // ProcessPacket::NewUnitInBattle(packet, units, battles);
    }
    else if (type == "PeaceAccepted") {
        int warId = ProcessPacket::PeaceAccepted(packet, provinces, countries, wars, map, sideBarData);
        editCountryMap2(&map2->mapTextures.country);
        // map.Unbright();
        // setCountryMap();
        if (openedProvId >= 0) {
            assert(openedProvId < (int)provinces.size());
            // map.BrightenProv(provinces[openedProvId]->GetColor());
        }
        // Gui::SideBar::DeleteWarIcon(warId);
        if (warId == Gui::OfferPeace::IsOpened()) {
            //    Gui::OfferPeace::Close();
        }
        else if (warId >= 0) {  // means that some peace offer window is opened, but not related to this peace
            // for (auto pair : offerPeaceData.provs) {
            //     map.BrightenProv(provinces[pair.provId]->GetColor());
            // }
        }
    }
    else if (type == "MergeUnits") {
        // ProcessPacket::MergeUnits(packet, units);
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
        ProcessPacket::BotPeaceOffer(packet, peaceOffers, countries, sideBarData);
    }
    else if (type == "PeaceDeclined") {
        ProcessPacket::PeaceDeclined(packet);
    }
}

void Game::resetGuiWindows()
{
    openProvId = -1;
    openUnitId = -1;
    openCountryId = -1;
    openMyCountry = false;
    openUnitsList = false;
    openBattleId = -1;
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
        // gui.Reload();
        // gui.AddWin("src/gui/top_bar.txt");
        // gui.AddWin("src/gui/notifications.txt");
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

    // map.getheightterrain() do poprawy
    camera.Update(window.xMouse, windowSize.y - window.yMouse, map.GetHeightTerrain());

    if (window.scrollOffset) {
        // if (!Gui::Base::Scroll(window.scrollOffset, glm::vec2{window.xMouse * resolution.x /
        // window.GetSize().x,
        //                                                     (window.GetSize().y - window.yMouse) *
        //                                                     resolution.y
        //                                                     /
        //                                                         window.GetSize().y}))
        camera.Scroll(window.scrollOffset);

        window.scrollOffset = 0.0f;
    }

    glm::vec2 mouseInWorld = camera.GetMouseInWorld();

    if (window.mouseLClicked && !window.mouseRClicked) {
        if (ctype.ct == ClickEventType::MISS) {
            if (!unitClick(mouseInWorld)) {
                int pid = provClick(mouseInWorld);
                if (pid != -1) {
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
                        //&& provinces[pid]->GetSieged() == 0) {
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
    // window.mouseRClicked = false;
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
        // Log(provinces[pid].id << ", water: " << provinces[pid].water << ", " << provinces[pid].name
        //                      << ", col: " << provinces[pid].r << ", " << provinces[pid].g << ", "
        //                      << provinces[pid].b);
        // std::cout << "Neighb: ";
        // for (auto i : provinces[pid].neighb) std::cout << i << " ";
        // std::cout << "\n";
        // std::cout << "NeighbSea: ";
        // for (auto i : provinces[pid].neighbSea) std::cout << i << " ";
        // std::cout << "\n";
        unitPos.x = provinces[pid]->GetUnitPos().x * scale;
        unitPos.y = provinces[pid]->GetUnitPos().y * scale;
        unitPos.z = heightMap->GetPixels()[(int)(provinces[pid]->GetUnitPos().x * 3 +
                                                 provinces[pid]->GetUnitPos().y * mapWidth * 3)];

        openedProvId = markedProvId;

        return pid;
        // auto battleIt = std::find_if(battles.begin(), battles.end(),
        //                              [pId = openedProvId](const Battle &b) { return (b.GetProvId() == pId); });

        // if (battleIt != battles.end()) {
        //     // GuiAid::OpenBattle(gui, battleIt, provIt);
        //     return true;
        // }
    }
    /// Log("prov click");
    /// std::cout << "R: " << (int)pixel[0] << "< ";
    /// std::cout << "G: " << (int)pixel[1] << "< ";
    /// std::cout << "B: " << (int)pixel[2] << "< \n";
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
        // std::cout << "R: " << (int)pixel[0] << "< ";
        // std::cout << "G: " << (int)pixel[1] << "< ";
        // std::cout << "B: " << (int)pixel[2] << "< \n";
        //
        //        std::vector<std::shared_ptr<Unit>> clickedUnits;
        auto provId = colorToId[clickedUnitPhash];
        // for (auto &unit : units) {
        //    if (unit->GetProvId() == provId)
        //        clickedUnits.push_back(unit);
        //}
        clickedUnits.clear();
        for (auto &u : units) {
            if (u.GetProvId() == provId)
                clickedUnits.push_back(u.GetId());
        }
        if (clickedUnits.size() == 1) {
            resetGuiWindows();
            openUnitId = clickedUnits[0];
            // openProvId = -1;
            // openCountryId = -1;
            // openMyCountry = false;
            // openUnitsList = false;
            //  clickedUnits[0]->subject.AddObserver(Gui::Unit::Open(resolution));
            //  clickedUnits[0]->UpdateGuiWin();
        }
        else if (clickedUnits.size() > 1) {
            resetGuiWindows();
            // openUnitId = clickedUnits[0];
            // openProvId = -1;
            // openCountryId = -1;
            // openMyCountry = false;
            openUnitsList = true;
        }
        return true;
    }
    else {
        return false;
    }
    /*
    // std::vector<std::unordered_map<std::string,std::string>> data;
    // Subject * unitForObserver = nullptr;
    for (auto &u : units) {
        // if (u->GetCountry() == "Poland")
        // if (u->Click(mouseInWorld.x, mouseInWorld.y)) {
        if (u->Click(camera.GetMouseRay(), camera.GetEye())) {
            clickedUnits.push_back(u);
            // auto d = u->GetValues();
            // data.push_back(d);
            // unitForObserver = u.get();
        }
    }
    if (clickedUnits.size() == 1) {
        Log("uClick2");
        // gui.AddWin("src/gui/unit.txt");
        // gui.ObservationOfLastWin(unitForObserver);
        // gui.Update(data[0], "unit");
        clickedUnits[0]->subject.AddObserver(Gui::Unit::Open(resolution));
        clickedUnits[0]->UpdateGuiWin();
        return true;
    }
    else if (clickedUnits.size() > 1) {
        //gui.AddWin("src/gui/units.txt");
        //for (auto & d : data) {
        //    DataObj * dobj = new DataObj{"label"};
        //    dobj->objects.push_back(new DataObj{"text"});
        //    dobj->objects.back()->values["valueName:"] = "unitName";
        //    dobj->objects.back()->values["content:"] = "";
        //    for (auto & val : d) {
        //        if (val.first == "id") {
        //            //dobj->values["unitId"] = val.second;// + " ";
        //            dobj->objects.back()->values["unitId"] = val.second;// + " ";
        //            dobj->objects.back()->values["content:"] += val.second + " ";
        //        }
        //        else if (val.first == "name")
        //            dobj->objects.back()->values["content:"] += val.second + " ";
        //    }
        //    dobj->objects.back()->values["clickEvent:"] = "openUnit";

        //    dobj->objects.push_back(new DataObj{"icon"});
        //    dobj->objects.back()->values["clickEvent:"] = "eraseObj";
        //    dobj->objects.back()->values["size:"] = "30 30";
        //    dobj->objects.back()->values["texturePath:"] = "src/img/plus.png";

        //    gui.AddToList(dobj, "units", "units");
        //    delete dobj;
        //}
        return true;
    }
    */
    return false;
}

void Game::unitMove(std::unordered_map<std::string, std::string> &values, glm::vec2 mouseInWorld)
{
    // Color provinceColor = map.ClickOnProvince(mouseInWorld.x /4, mouseInWorld.y /4);
    Color provinceColor = map.ClickOnProvince(mouseInWorld.x, mouseInWorld.y);
    // auto it = std::find(provinces.begin(), provinces.end(), provinceColor);
    auto it = std::find_if(provinces.begin(), provinces.end(), [provinceColor](std::unique_ptr<Province> &p) {
        return p->GetColor() == provinceColor;
    });
    if (it != provinces.end()) {
        auto idIt = values.find("id");
        if (idIt == values.end())
            return;
        sf::Packet packet;
        packet << "UnitMove";
        packet << std::stoi(idIt->second);
        /*
               for (auto & val : values)
                   if (val.first == "id") {
                       packet << std::stoi(val.second);
                       break;
                   }
       */
        packet << (*it)->GetId();

        toSend.push_back(packet);
    }
}

void Game::processGuiEvent()
{
    std::vector<sf::Packet> packets;
    // std::string evType = click.GetEventName();
    /*
    if (evType == "openCountry") {
        GuiAid::OpenCountry(gui, countries, myCountry, wars, click);
    }
    else if (evType == "newUnit") {
        GuiAid::NewUnit(click, provinces, packets);
    }
    else if (evType == "decreaseValue") {
        GuiAid::DecreaseValue(gui, click);
    }
    else if (evType == "increaseValue") {
        GuiAid::IncreaseValue(gui, click);
    }
    else if (evType == "openUnit") {
        GuiAid::OpenUnit(gui, click, units);
    }
    else if (evType == "eraseObj") {
        GuiAid::EraseObj(gui, click);
    }
    else if (evType == "mergeUnits") {
        GuiAid::MergeUnits(gui, click, packets);
    }
    else if (evType == "declareWar") {
        GuiAid::DeclareWar(click, countries, packets);
    }
    else if (evType == "offerPeace") {
        GuiAid::OfferPeace(gui, wars, click, myCountry->GetName());
    }
    else if (evType == "sendPeaceOffer") {
        GuiAid::SendPeaceOffer(gui, wars, countries, provinces, click, packets);
    }
    else if (evType == "dateSpeed") {
        GuiAid::DateSpeed(click, packets);
    }
    else if (evType == "startImprRel") {
        GuiAid::StartImprRel(click, myCountry, countries, packets);
    }
    else if (evType == "stopImprRel") {
        GuiAid::StopImprRel(click, myCountry, countries, packets);
    }
    else if (evType == "openWar") {
        GuiAid::OpenWar(gui, click, wars);
    }
    else if (evType == "botPeaceOffer") {
        GuiAid::BotPeaceOffer(click, gui, peaceOffers, countries, provinces);
    }
    else if (evType == "acceptPeace") {
        GuiAid::AcceptPeace(click, gui, peaceOffers, packets);
    }
    else if (evType == "declinePeace") {
        GuiAid::DeclinePeace(click, gui, peaceOffers, packets);
    }
*/
    if (packets.size())
        toSend.insert(toSend.end(), packets.begin(), packets.end());
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
    // for (auto &b : battles) {
    //     b->Update();
    // }
}

void Game::updateGui()
{
    /*
    SiegedProvWinD sprwD;
    try {
        sprwD = std::any_cast<SiegedProvWinD>(gui.GetWinData(WinType::sieged_prov_win));
        int id = sprwD.id;
        auto provIt = std::find_if(provinces.begin(), provinces.end(), [id](const Province & prov){
                return prov.GetId() == id;
            });
        if (provIt != provinces.end()) {
            GA_UpdateSiegedProv(gui, provIt);
        }
    }
    catch(std::exception e) {
    }
    */
}

//      ClickEventType cType =
//          Gui::Base::Click(glm::vec2{window.xMouse * resolution.x / window.GetSize().x,
//                                     (window.GetSize().y - window.yMouse) * resolution.y /
//                                     window.GetSize().y});
/*
        Log("uClick " << (int)cType);
        if (cType == ClickEventType::MISS) {
            Log("uClick");
            if (Gui::OfferPeace::IsOpened() >= 0) {
                Color provinceColor = map.ClickOnProvince(mouseInWorld.x, mouseInWorld.y);
                auto provIt = std::find_if(
                    provinces.begin(), provinces.end(),
                    [provinceColor](std::unique_ptr<Province> &p) { return p->GetColor() ==
   provinceColor;
   }); if (provIt != provinces.end()) { int provId = (*provIt)->GetId(); auto it =
   std::find_if(offerPeaceData.provs.begin(), offerPeaceData.provs.end(), [provId](const
   OfferPeaceData::pair p) { return p.provId == provId; }); if (it != offerPeaceData.provs.end()) {
                        Gui::OfferPeace::DeleteProvince(provId);
                        map.Unbright();
                        offerPeaceData.provs.erase(it);
                        for (auto p : offerPeaceData.provs) {
                            map.BrightenProv(provinces[p.provId]->GetColor());
                        }
                    }
                    else {
                        int receiverId = (*provIt)->GetSiegeCountryId();
                        if (receiverId == -1) {
                            receiverId = myCountry->GetId();
                        }
                        std::string receiver = countries[receiverId]->GetName();
                        Gui::OfferPeace::AddProvince((*provIt)->GetName(), receiver, provId,
   receiverId); map.BrightenProv((*provIt)->GetColor()); offerPeaceData.provs.push_back({provId,
   receiverId});
                    }
                }
            }
            else if (!unitClick(mouseInWorld)) {
                provClick(mouseInWorld);
            }
        }
        else if (cType == ClickEventType::PROV_SWITCH_TAB) {
            Log("switch tab");
            Gui::Prov::SwitchTab();
            Gui::Base::ResetClick();
        }
        else if (cType == ClickEventType::OPEN_COUNTRY) {
            int ctrId = Gui::Base::GetHiddenValue();
            assert(ctrId >= 0 && ctrId < (int)countries.size());
            if (ctrId != myCountry->GetId()) {
                bool atWarWith = false;
                for (auto &war : wars) {
                    if (war.ShouldTheyFight(countries[ctrId]->GetId(), myCountry->GetId())) {
                        atWarWith = true;
                        break;
                    }
                }
                countries[ctrId]->subject.AddObserver(Gui::Country::Open(resolution, ctrId,
   atWarWith)); countries[ctrId]->UpdateGuiWin();
            }
            else {
                myCountry->subjectOfMyCountry.AddObserver(Gui::MyCountry::Open(resolution));
                myCountry->UpdateMyCountryGuiWin();
            }
            Gui::Base::ResetClick();
        }
        else if (cType == ClickEventType::DECLARE_WAR) {
            sf::Packet packet;
            packet << "DeclareWar";
            packet << Gui::Country::GetId();
            toSend.push_back(packet);
        }
        else if (cType == ClickEventType::OPEN_OFFER_PEACE) {
            offerPeaceData.provs.clear();
            map.Unbright();
            int rivalId = Gui::Country::GetId();
            bool rivalIsDefender = false;
            int warscore = 0;
            int myId = myCountry->GetId();
            if (rivalId != -1) {
                bool ok = false;
                for (auto &w : wars) {
                    if (!w.ShouldTheyFight(myId, rivalId))
                        continue;
                    int wAtt = w.GetAttacker();
                    int wDef = w.GetDefender();
                    if (myId == wAtt || myId == wDef) {
                        offerPeaceData.warId = w.GetId();
                        warscore = w.GetAttackerWarScore();
                        rivalIsDefender = rivalId == wDef;
                        ok = true;
                        break;
                    }
                    if (rivalId == wAtt || rivalId == wDef) {
                        offerPeaceData.warId = w.GetId();
                        rivalIsDefender = rivalId == wDef;
                        warscore = w.GetAttackerWarScore();
                        ok = true;
                        break;
                    }
                }
                offerPeaceData.recipantId = rivalId;
                if (ok) {
                    assert(rivalId >= 0 && rivalId < (int)countries.size());
                    if (rivalIsDefender)
                        Gui::OfferPeace::Open(resolution, countries[rivalId]->GetName(),
   myCountry->GetName(), offerPeaceData.warId, warscore, myId, rivalId); else
                        Gui::OfferPeace::Open(resolution, myCountry->GetName(),
   countries[rivalId]->GetName(), offerPeaceData.warId, warscore, rivalId, myId);
                }
            }
            else if (Gui::War::IsOpen()) {
                int warId = Gui::War::GetId();
                int warscore = 0, rivalId = -1;
                int myId = myCountry->GetId();
                bool rivalIsDefender = false;
                for (auto &w : wars) {
                    if (w.GetId() != warId)
                        continue;
                    int wAtt = w.GetAttacker();
                    int wDef = w.GetDefender();
                    if (wDef == myId) {
                        rivalId = wAtt;
                        warscore = w.GetAttackerWarScore();
                        break;
                    }
                    if (wAtt == myId) {
                        rivalId = wDef;
                        rivalIsDefender = true;
                        warscore = w.GetAttackerWarScore();
                        break;
                    }
                }
                if (rivalId != -1) {
                    if (rivalIsDefender)
                        Gui::OfferPeace::Open(resolution, countries[rivalId]->GetName(),
   myCountry->GetName(), offerPeaceData.warId, warscore, myId, rivalId); else
                        Gui::OfferPeace::Open(resolution, myCountry->GetName(),
   countries[rivalId]->GetName(), offerPeaceData.warId, warscore, rivalId, myId);
                }
            }
        }
        else if (cType == ClickEventType::OPEN_WAR_WINDOW) {
            int warId = Gui::Base::GetHiddenValue();
            auto warIt =
                std::find_if(wars.begin(), wars.end(), [warId](const War &war) { return warId ==
   war.GetId(); }); assert(warIt != wars.end()); warIt->subject.AddObserver(Gui::War::Open(resolution,
   warIt->GetId())); auto attackers = warIt->GetAttackers(); for (auto &a : attackers)
   Gui::War::AddAttacker(a); auto defenders = warIt->GetDefenders(); for (auto &d : defenders)
   Gui::War::AddDefender(d);
        }
        else if (cType == ClickEventType::SEND_PEACE_OFFER) {
            Log("send peace offer");
            Log("war id=" << offerPeaceData.warId);
            for (auto pair : offerPeaceData.provs) {
                if (pair.provId >= 0 && pair.provId < (int)provinces.size())
                    Log(provinces[pair.provId]->GetName());
            }
            sf::Packet packet;
            packet << "PeaceOffer";
            packet << offerPeaceData.warId;
            packet << offerPeaceData.recipantId;
            packet << (int)offerPeaceData.provs.size();
            for (auto pair : offerPeaceData.provs) {
                packet << pair.provId;
                packet << pair.recipantId;
            }
            toSend.push_back(packet);
        }
        else if (cType == ClickEventType::CLOSE_WINDOW) {
            Gui::Base::CloseWindowFromClick();
        }
        else if (cType == ClickEventType::DELETE_PROV_FROM_PEACE) {
            int provId = Gui::Base::GetHiddenValue();
            Log("delete prov from peace " << provId);
            if (provId >= 0 && provId < (int)provinces.size()) {
                Log(provinces[provId]->GetName());
                for (auto it = offerPeaceData.provs.begin(); it != offerPeaceData.provs.end(); ++it) {
                    if (provId == it->provId) {
                        Gui::OfferPeace::DeleteProvince(provId);
                        map.Unbright();
                        offerPeaceData.provs.erase(it);
                        for (auto pair : offerPeaceData.provs) {
                            map.BrightenProv(provinces[pair.provId]->GetColor());
                        }
                        break;
                    }
                }
            }
        }
        else if (cType == ClickEventType::OPEN_PEACE_OFFERT) {
            int peaceId = Gui::Base::GetHiddenValue();
            for (auto it = peaceOffers.begin(); it != peaceOffers.end(); ++it) {
                if (it->peaceId == peaceId) {
                    it->recipant =
                        myCountry
                            ->GetId();  // it->recipant nie jest nigdzie indziej ustawiane xd; ale jest
   oczywiste assert(it->recipant >= 0 && it->recipant < (int)countries.size()); assert(it->offeredBy >=
   0
   && it->offeredBy < (int)countries.size()); Gui::PeaceOffert::Open(resolution, it->peaceId,
   countries[it->recipant]->GetName(), countries[it->offeredBy]->GetName(), it->offeredBy,
   it->recipant); for (auto &prov : it->lostProv) { int provId = std::get<0>(prov); int ctrId =
   std::get<1>(prov); assert(provId >= 0 && provId < (int)provinces.size()); assert(ctrId >= 0 && ctrId
   < (int)countries.size()); Gui::PeaceOffert::AddProv(provinces[provId]->GetName(),
   countries[ctrId]->GetName());
                    }
                    for (auto &prov : it->gainProv) {
                        int provId = std::get<0>(prov);
                        int ctrId = std::get<1>(prov);
                        assert(provId >= 0 && provId < (int)provinces.size());
                        assert(ctrId >= 0 && ctrId < (int)countries.size());
                        Gui::PeaceOffert::AddProv(provinces[provId]->GetName(),
   countries[ctrId]->GetName());
                    }
                    break;
                }
            }
        }
Gui::Base::ResetClick();
*/
//}
