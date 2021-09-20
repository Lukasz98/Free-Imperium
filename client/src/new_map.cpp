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
#include "font_batch.h"
#include "graphics/texture.h"
#include "load_data.h"
#include "map_batch.h"
#include "save_borders.h"
#include "save_borders_triangles.h"
#include "save_data.h"

// int nodeHash(unsigned short x, unsigned short y)
struct Line {
    int x, y;
};

void newTesMapTest(Window& window, glm::vec2 resolution, glm::vec2 windowSize)
{
    GLuint err = glGetError();
    if (err)
        Log("Opengl error: " << err);
    Shader shader("src/graphics/shaders/tes_new_map/vert.v", "src/graphics/shaders/tes_new_map/frag.f",
                  "src/graphics/shaders/tes_new_map/tes_ster.ts", "src/graphics/shaders/tes_new_map/tes_w.tw");
    Shader borderShader2{"src/graphics/shaders/borders2/vert.v", "src/graphics/shaders/borders2/frag.f", "", ""};
    Shader borderShader{"src/graphics/shaders/borders/vert.v", "src/graphics/shaders/borders/frag.f", "", "",
                        "src/graphics/shaders/borders/geom.g"};
    Shader seaBorderShader{"src/graphics/shaders/sea_borders/vert.v", "src/graphics/shaders/sea_borders/frag.f",
                           "", "", "src/graphics/shaders/sea_borders/geom.g"};
    Shader waterShader("src/graphics/shaders/water/vert.v", "src/graphics/shaders/water/frag.f",
                       "src/graphics/shaders/water/tes_ster.ts", "src/graphics/shaders/water/tes_w.tw");
    Shader colorMapShader("src/graphics/shaders/map_pick/vert.v", "src/graphics/shaders/map_pick/frag.f",
                          "src/graphics/shaders/map_pick/tes_ster.ts", "src/graphics/shaders/map_pick/tes_w.tw");
    Shader polyShader{"src/graphics/shaders/poly/vert.v", "src/graphics/shaders/poly/frag.f", "", ""};
    Shader polyProvShader{"src/graphics/shaders/polyProv/vert.v", "src/graphics/shaders/polyProv/frag.f", "", ""};
    Shader waterColorShader{"src/graphics/shaders/water_color/vert.v", "src/graphics/shaders/water_color/frag.f",
                            "", ""};
    Shader fontShader{"src/graphics/shaders/fonts.vert", "src/graphics/shaders/fonts.frag", "", ""};
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

    GLint tex[32];
    for (int i = 0; i < 32; ++i) {
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
    {
        for (std::size_t i = 0; i < ctrsData.size(); ++i) ctrProvs.push_back(std::vector<int>{});
        for (auto& pd : provinces) {
            if (pd.ctrId == -1)
                continue;
            ctrProvs[pd.ctrId].push_back(pd.id);
        }
    }
    // saveBorders(provTexture.GetPixels(), mapWidth, mapHeight, provinces, nodes);
    // saveSeaBorders(provTexture.GetPixels(), mapWidth, mapHeight, provinces, nodes, colorToId);
    // createSaveProvPoints(provTexture.GetPixels(), mapWidth, mapHeight, provinces, colorToId);
    float scale = 4.0f;
    // saveBordersTriangles(mapWidth, mapHeight, scale, heightMap.GetPixels());
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

    int fontTexID[32];
    for (int i = 0; i <= (int)AM::FontSize::PX160; ++i) {
        fontTexID[i] = AM::atlasTexture[i]->GetId();
    }

    glUseProgram(waterShader.GetProgram());
    glUniform1f(glGetUniformLocation(waterShader.GetProgram(), "level"), 32);
    glUniform1iv(glGetUniformLocation(waterShader.GetProgram(), "tex"), 32, tex);

    glUseProgram(colorMapShader.GetProgram());
    glUniform1f(glGetUniformLocation(colorMapShader.GetProgram(), "level"), 32);
    glUniform1iv(glGetUniformLocation(colorMapShader.GetProgram(), "tex"), 32, tex);

    const unsigned char* pix = provTexture.GetPixels();
    const unsigned char* h = heightMap.GetPixels();

    Log("tu1");
    std::vector<MapVertex> vertexes;
    float tid = 0.0f;
    Vec4 color{1.0f, 0.0f, 0.0f, 1.0f};
    // float w = 64.0f;  // * scale;
    float w = mapHeight * 0.25f;
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
    std::vector<int> bordChunkId;
    std::fstream file;
    file.open("BordersData2.txt", std::fstream::in);
    std::string ss;
    std::vector<Vec3> borVerts;
    // std::vector<BorderVertex> borVerts;
    {
        float x1;
        while (file >> x1) {
            float y1, z1;
            file >> y1 >> z1;
            int index;
            index = (int)x1 * 3 + (int)y1 * mapWidth * 3;
            if (y1 >= mapHeight)
                index = (int)x1 * 3 + (int)(mapHeight - 1) * mapWidth * 3;
            x1 *= scale;
            y1 *= scale;
            int h1 = h[index];
            borVerts.push_back(Vec3{x1, y1, h[index]});

            file >> x1 >> y1 >> z1;
            index = (int)x1 * 3 + (int)y1 * mapWidth * 3;
            if (y1 >= mapHeight)
                index = (int)x1 * 3 + (int)(mapHeight - 1) * mapWidth * 3;
            x1 *= scale;
            y1 *= scale;
            int h2 = h[index];
            borVerts.push_back(Vec3{x1, y1, h[index]});

            file >> x1 >> y1 >> z1;
            index = (int)x1 * 3 + (int)y1 * mapWidth * 3;
            if (y1 >= mapHeight)
                index = (int)x1 * 3 + (int)(mapHeight - 1) * mapWidth * 3;
            x1 *= scale;
            y1 *= scale;
            int h3 = h[index];
            borVerts.push_back(Vec3{x1, y1, h[index]});
            if (0) {
                if (h2 > h1 && h2 >= h3)
                    h1 = h2;
                else if (h3 > h1 && h3 >= h2)
                    h1 = h3;
            }
            // borVerts[borVerts.size() - 1].z = h1;
            // borVerts[borVerts.size() - 2].z = h1;
            // borVerts[borVerts.size() - 3].z = h1;
        }
    }
    file.close();
    GLuint borVao, borVbo;
    glCreateVertexArrays(1, &borVao);
    glBindVertexArray(borVao);
    glCreateBuffers(1, &borVbo);

    glBindBuffer(GL_ARRAY_BUFFER, borVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3) * borVerts.size(), borVerts.data(), GL_STATIC_DRAW);
    glEnableVertexArrayAttrib(borVao, 0);
    err = glGetError();
    if (err)
        Log("Opengl error: " << err);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3), NULL);  //(const GLvoid*)0);
    Log("nodes.size: " << nodes.size());
    Log("borVerts.size: " << borVerts.size());

    std::vector<BorderVertex> seaVerts;
    file.open("SeaBordersData.txt", std::fstream::in);
    while (file >> ss) {
        int x1 = std::atoi(ss.data());
        file >> ss;
        int y1 = std::atoi(ss.data());
        file >> ss;
        int x2 = std::atoi(ss.data());
        file >> ss;
        int y2 = std::atoi(ss.data());
        // file >> ss;
        int i1 = x1 * 3 + y1 * mapWidth * 3;
        if (i1 > mapHeight * mapWidth * 3 - 3)
            i1 = mapHeight * mapWidth * 3 - 3;
        int i2 = x2 * 3 + y2 * mapWidth * 3;
        if (i2 > mapHeight * mapWidth * 3 - 3)
            i2 = mapHeight * mapWidth * 3 - 3;
        seaVerts.push_back(BorderVertex{.pos = Vec3{((float)x1) * scale, ((float)y1) * scale, h[i1]},
                                        .tc = Vec2{(float)x1 / mapWidth, (float)y1 / mapHeight}});
        seaVerts.push_back(BorderVertex{.pos = Vec3{((float)x2) * scale, ((float)y2) * scale, h[i2]},
                                        .tc = Vec2{(float)x2 / mapWidth, (float)y2 / mapHeight}});
    }
    Log("Sea bor verts count: " << seaVerts.size());
    file.close();
    GLuint seaVao, seaVbo;
    {  // sea map
        glCreateVertexArrays(1, &seaVao);
        glBindVertexArray(seaVao);
        glCreateBuffers(1, &seaVbo);

        glBindBuffer(GL_ARRAY_BUFFER, seaVbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(BorderVertex) * seaVerts.size(), seaVerts.data(), GL_STATIC_DRAW);
        glEnableVertexArrayAttrib(seaVao, 0);
        glEnableVertexArrayAttrib(seaVao, 1);
        GLuint err = glGetError();
        if (err)
            Log("Opengl error: " << err);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(BorderVertex), NULL);  //(const GLvoid*)0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(BorderVertex),
                              (const GLvoid*)(offsetof(BorderVertex, BorderVertex::tc)));
    }

    std::vector<BorderVertex> waterColorVerts;
    waterColorVerts.push_back(BorderVertex{.pos = Vec3{0.0, 0.0, 0.0}, .tc = Vec2{0.0, 0.0}});
    waterColorVerts.push_back(BorderVertex{.pos = Vec3{0.0, mapHeight * scale, 0.0}, .tc = Vec2{0.0, 1.0}});
    waterColorVerts.push_back(
        BorderVertex{.pos = Vec3{mapWidth * scale, mapHeight * scale, 0.0}, .tc = Vec2{1.0, 1.0}});
    waterColorVerts.push_back(
        BorderVertex{.pos = Vec3{mapWidth * scale, mapHeight * scale, 0.0}, .tc = Vec2{1.0, 1.0}});
    waterColorVerts.push_back(BorderVertex{.pos = Vec3{mapWidth * scale, 0.0, 0.0}, .tc = Vec2{1.0, 0.0}});
    waterColorVerts.push_back(BorderVertex{.pos = Vec3{0.0, 0.0, 0.0}, .tc = Vec2{0.0, 0.0}});
    GLuint waterColorVao, waterColorVbo;
    {  // sea color map
        glCreateVertexArrays(1, &waterColorVao);
        glBindVertexArray(waterColorVao);
        glCreateBuffers(1, &waterColorVbo);

        glBindBuffer(GL_ARRAY_BUFFER, waterColorVbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(BorderVertex) * waterColorVerts.size(), waterColorVerts.data(),
                     GL_STATIC_DRAW);
        glEnableVertexArrayAttrib(waterColorVao, 0);
        glEnableVertexArrayAttrib(waterColorVao, 1);
        err = glGetError();
        if (err)
            Log("Opengl error: " << err);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(BorderVertex), NULL);  //(const GLvoid*)0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(BorderVertex),
                              (const GLvoid*)(offsetof(BorderVertex, BorderVertex::tc)));
    }

    struct PolyVert {
        float x, y, z;
        // float r = 1.0f, g = 0, b = 0, a = 1.0;
        glm::vec3 col{1.0f, 0.0f, 0.0f};
        float tx;
    };
    // int chunk_x_count = 4, chunk_y_count = 2;
    // float chunk_w = (float)(mapWidth * scale) / chunk_x_count;
    // float chunk_h = (float)(mapHeight * scale) / chunk_y_count;
    // std::vector<PolyVert> detailVerts[(int)(chunk_x_count * chunk_y_count)];
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
            int index;

            // int chunk_id;
            //{
            //    float xxx = x1 * scale, yyy = y1 * scale;
            //    int ch_x = xxx / (chunk_w + 2.0f);
            //    int ch_y = yyy / (chunk_h + 2.0f);
            //    chunk_id = ch_x + ch_y * chunk_x_count;
            //    assert(chunk_id < chunk_x_count * chunk_y_count);
            //}

            index = x1 * 3 + y1 * mapWidth * 3;
            if (y1 >= mapHeight)
                index = x1 * 3 + (mapHeight - 1) * mapWidth * 3;
            z = h[index];
            initMap.pverts.push_back(PolyVert{.x = x1 * scale, .y = y1 * scale, .z = z, .col = col, .tx = tx});
            // detailVerts[chunk_id].push_back(
            //    PolyVert{.x = x1 * scale, .y = y1 * scale, .z = z, .col = col, .tx = tx});

            index = x2 * 3 + y2 * mapWidth * 3;
            if (y2 >= mapHeight)
                index = x2 * 3 + (mapHeight - 1) * mapWidth * 3;
            z = h[index];
            initMap.pverts.push_back(PolyVert{.x = x2 * scale, .y = y2 * scale, .z = z, .col = col, .tx = tx});
            // detailVerts[chunk_id].push_back(
            //    PolyVert{.x = x2 * scale, .y = y2 * scale, .z = z, .col = col, .tx = tx});

            index = x3 * 3 + y3 * mapWidth * 3;
            if (y3 >= mapHeight)
                index = x3 * 3 + (mapHeight - 1) * mapWidth * 3;
            z = h[index];
            initMap.pverts.push_back(PolyVert{.x = x3 * scale, .y = y3 * scale, .z = z, .col = col, .tx = tx});
            // detailVerts[chunk_id].push_back(
            //    PolyVert{.x = x3 * scale, .y = y3 * scale, .z = z, .col = col, .tx = tx});

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
    //    int chunk_count = chunk_x_count * chunk_y_count;
    // GLuint detailPolyVao[chunk_count];
    //{  // detailed map
    //    for (int i = 0; i < chunk_count; ++i) {
    //        glCreateVertexArrays(1, &detailPolyVao[i]);
    //        glBindVertexArray(detailPolyVao[i]);
    //        GLuint pvbo;
    //        glCreateBuffers(1, &pvbo);

    //        glBindBuffer(GL_ARRAY_BUFFER, pvbo);
    //        glBufferData(GL_ARRAY_BUFFER, sizeof(PolyVert) * detailVerts[i].size(), detailVerts[i].data(),
    //                     GL_STATIC_DRAW);
    //        glEnableVertexArrayAttrib(detailPolyVao[i], 0);
    //        glEnableVertexArrayAttrib(detailPolyVao[i], 1);
    //        glEnableVertexArrayAttrib(detailPolyVao[i], 2);
    //        GLuint err = glGetError();
    //        if (err)
    //            Log("Opengl error: " << err);
    //        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(PolyVert), NULL);  //(const GLvoid*)0);
    //        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(PolyVert),
    //                              (const GLvoid*)(offsetof(PolyVert, PolyVert::col)));
    //        glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(PolyVert),
    //                              (const GLvoid*)(offsetof(PolyVert, PolyVert::tx)));
    //    }
    //}
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

    FontBatch fontBatch;
    fontBatch.Init();

    glUseProgram(borderShader.GetProgram());
    glUniform1iv(glGetUniformLocation(borderShader.GetProgram(), "tex"), 32, texID);
    bool drawBorders = true;
    glm::vec3 provColor;
    float markedProvId = -1.0f;
    float tesLevel = 32.0f;
    glm::vec3 unitPos;
    float dt = 0.0f, waterTime = 0.0f;
    float time = glfwGetTime();
    float rotateText;
    unsigned int clickedProviPhash;
    while (!window.ShouldClose()) {
        for (int i = 0; i < 9; ++i) {
            // glActiveTexture((GLuint)texID[i]);
            glActiveTexture(GL_TEXTURE0 + i);
            // glActiveTexture(GL_TEXTURE0 + texID[i]);
            glBindTexture(GL_TEXTURE_2D, (GLuint)texID[i]);
            err = glGetError();
            // if (err)
            //    Log("Opengl error: " << err << " " << i);
            // Log("Opengl error: " << err << " " << (GLuint)texID[i]);
        }
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
        if (window.keys['M']) {
            polyShader = Shader{"src/graphics/shaders/poly/vert.v", "src/graphics/shaders/poly/frag.f", "", ""};
            polyProvShader =
                Shader{"src/graphics/shaders/polyProv/vert.v", "src/graphics/shaders/polyProv/frag.f", "", ""};
            shader = Shader("src/graphics/shaders/tes_new_map/vert.v", "src/graphics/shaders/tes_new_map/frag.f",
                            "src/graphics/shaders/tes_new_map/tes_ster.ts",
                            "src/graphics/shaders/tes_new_map/tes_w.tw");
            borderShader = Shader{"src/graphics/shaders/borders/vert.v", "src/graphics/shaders/borders/frag.f", "",
                                  "", "src/graphics/shaders/borders/geom.g"};
            waterShader = Shader("src/graphics/shaders/water/vert.v", "src/graphics/shaders/water/frag.f",
                                 "src/graphics/shaders/water/tes_ster.ts", "src/graphics/shaders/water/tes_w.tw");
            colorMapShader =
                Shader("src/graphics/shaders/map_pick/vert.v", "src/graphics/shaders/map_pick/frag.f",
                       "src/graphics/shaders/map_pick/tes_ster.ts", "src/graphics/shaders/map_pick/tes_w.tw");

            seaBorderShader =
                Shader{"src/graphics/shaders/sea_borders/vert.v", "src/graphics/shaders/sea_borders/frag.f", "",
                       "", "src/graphics/shaders/sea_borders/geom.g"};
            borderShader2 =
                Shader{"src/graphics/shaders/borders2/vert.v", "src/graphics/shaders/borders2/frag.f", "", ""};
            waterColorShader = Shader{"src/graphics/shaders/water_color/vert.v",
                                      "src/graphics/shaders/water_color/frag.f", "", ""};
            fontShader = Shader{"src/graphics/shaders/fonts.vert", "src/graphics/shaders/fonts.frag", "", ""};
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
            glUseProgram(waterColorShader.GetProgram());
            glUniformMatrix4fv(glGetUniformLocation(waterColorShader.GetProgram(), "matrix"), 1, GL_FALSE,
                               glm::value_ptr(matrix));
            glUniform1iv(glGetUniformLocation(waterColorShader.GetProgram(), "tex"), 32, tex);
            glBindVertexArray(waterColorVao);
            glBindBuffer(GL_ARRAY_BUFFER, waterColorVbo);
            glDrawArrays(GL_TRIANGLES, 0, waterColorVerts.size());

            glUseProgram(polyProvShader.GetProgram());
            glUniformMatrix4fv(glGetUniformLocation(polyProvShader.GetProgram(), "matrix"), 1, GL_FALSE,
                               glm::value_ptr(matrix));
            glUniform1iv(glGetUniformLocation(polyProvShader.GetProgram(), "tex"), 32, tex);

            glBindVertexArray(initMap.polyVao);
            glBindBuffer(GL_ARRAY_BUFFER, initMap.polyVbo);
            glDrawArrays(GL_TRIANGLES, 0, polyCount);

            unsigned char pixel[4];
            int pixx = window.xMouse, pixy = windowSize.y - window.yMouse;
            glReadPixels(pixx, pixy, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);

            window.Refresh();
            provColor = {(int)pixel[0] / 255.0, (int)pixel[1] / 255.0, (int)pixel[2] / 255.0};
            clickedProviPhash = getHash(pixel[0], pixel[1], pixel[2]);
            if (colorToId.find(clickedProviPhash) != colorToId.end()) {
                int pid = colorToId[clickedProviPhash];
                markedProvId = (float)pid + 0.5f;
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
            std::cout << "R: " << (int)pixel[0] << "< ";
            std::cout << "G: " << (int)pixel[1] << "< ";
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

        if (window.keys['B']) {
            drawBorders = !drawBorders;
        }

        //    glUniform1f(glGetUniformLocation(shader.GetProgram(), "waterTime"), waterTime);

        glUniform1f(glGetUniformLocation(shader.GetProgram(), "borderTime"), waterTime);
        glUniform1iv(glGetUniformLocation(shader.GetProgram(), "tex"), 32, tex);

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

        if (!window.keys['P']) {
            glUseProgram(waterShader.GetProgram());
            glUniform1f(glGetUniformLocation(waterShader.GetProgram(), "level"), 32);
            glUniform1iv(glGetUniformLocation(waterShader.GetProgram(), "tex"), 32, tex);
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
        }

        {  // poly
            glUseProgram(polyShader.GetProgram());
            glUniformMatrix4fv(glGetUniformLocation(polyShader.GetProgram(), "matrix"), 1, GL_FALSE,
                               glm::value_ptr(matrix));
            glUniform1iv(glGetUniformLocation(polyShader.GetProgram(), "tex"), 32, tex);
            glUniform3fv(glGetUniformLocation(polyShader.GetProgram(), "provColor"), 1, glm::value_ptr(provColor));
            glUniform1f(glGetUniformLocation(polyShader.GetProgram(), "provId"), markedProvId);

            if (window.keys['P']) {
                glBindVertexArray(initMap.polyVao);
                glBindBuffer(GL_ARRAY_BUFFER, initMap.polyVbo);
                glDrawArrays(GL_TRIANGLES, 0, polyCount);

                glUseProgram(waterColorShader.GetProgram());
                glUniformMatrix4fv(glGetUniformLocation(waterColorShader.GetProgram(), "matrix"), 1, GL_FALSE,
                                   glm::value_ptr(matrix));
                glUniform1iv(glGetUniformLocation(waterColorShader.GetProgram(), "tex"), 32, tex);
                glBindVertexArray(waterColorVao);
                glBindBuffer(GL_ARRAY_BUFFER, waterColorVbo);
                glDrawArrays(GL_TRIANGLES, 0, waterColorVerts.size());
            }
            else {
                // float eye_x = camera.eye.x, eye_y = camera.eye.y;
                // for (int i = 0; i < chunk_count; ++i) {
                // int chx = i % chunk_x_count;
                // if (chunk_w * (chx) >= eye_x && chunk_w * (chx) <= eye_x + chunk_w)
                //    continue;
                // glBindVertexArray(detailPolyVao[i]);
                ////glBindBuffer(GL_ARRAY_BUFFER, countryMap.polyVbo);
                // glDrawArrays(GL_TRIANGLES, 0, detailVerts[i].size());
                //}
                glBindVertexArray(countryMap.polyVao);
                glBindBuffer(GL_ARRAY_BUFFER, countryMap.polyVbo);
                glDrawArrays(GL_TRIANGLES, 0, polyCount);
            }
        }

        glUseProgram(borderShader2.GetProgram());
        glUniformMatrix4fv(glGetUniformLocation(borderShader2.GetProgram(), "matrix"), 1, GL_FALSE,
                           glm::value_ptr(matrix));
        if (drawBorders) {
            // glDisable(GL_DEPTH_TEST);   // Enable depth testing for z-culling
            // glDepthFunc(GL_GEQUAL);//QUAL);
            // glUseProgram(borderShader2.GetProgram());
            glBindVertexArray(borVao);
            glBindBuffer(GL_ARRAY_BUFFER, borVbo);
            glDrawArrays(GL_TRIANGLES, 0, borVerts.size());
            double dxd = glfwGetTime();
            borderBatch.flushtime = 0.0;
            borderBatch.pushtime = 0.0;

            glUseProgram(seaBorderShader.GetProgram());
            glUniformMatrix4fv(glGetUniformLocation(seaBorderShader.GetProgram(), "matrix"), 1, GL_FALSE,
                               glm::value_ptr(matrix));
            glUniform1iv(glGetUniformLocation(seaBorderShader.GetProgram(), "tex"), 32, tex);
            glBindVertexArray(seaVao);
            glBindBuffer(GL_ARRAY_BUFFER, seaVbo);
            glDrawArrays(GL_LINES, 0, seaVerts.size());
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
            glUniform1iv(glGetUniformLocation(AM::am.shader->GetProgram(), "tex"), 32, tex);

            // AM::am.model->DrawRect(model);
            AM::am.model->Draw();
        }

        for (int i = 0; i <= (int)AM::FontSize::PX160; ++i) {
            // glActiveTexture(GL_TEXTURE0 + fontTexID[i]);
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, (GLuint)fontTexID[i]);
            err = glGetError();
            // if (err)
            //    Log("Opengl error: " << err << " " << i);
            // Log("Opengl error: " << err << " " << (GLuint)fontTexID[i]);
        }

        glDisable(GL_DEPTH_TEST);  // Enable depth testing for z-culling
        // glDepthFunc(GL_LESS);

        glUseProgram(fontShader.GetProgram());
        glUniformMatrix4fv(glGetUniformLocation(fontShader.GetProgram(), "matrix"), 1, GL_FALSE,
                           glm::value_ptr(matrix));
        glUniform1iv(glGetUniformLocation(fontShader.GetProgram(), "tex"), 32, tex);
        fontBatch.Begin();
        {  // chars
            // unsigned int phash = getHash(119, 212, 150);
            unsigned int phash = clickedProviPhash;
            int pid;
            if ((colorToId.find(phash) != colorToId.end()) &&
                ((pid = colorToId[phash]) && provinces[pid].ctrId != -1)) {
                int maxxid, minxid, maxyid, minyid;
                glm::vec2 maxx{-1.0f, 0.0f}, minx{10000.0f, 0.0f};
                glm::vec2 maxy{0.0f, -1.0f}, miny{0.0f, 10000.0f};
                int ctrId = provinces[pid].ctrId;
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

                // float distX = pow(maxx.x - minx.x, 2) + pow(maxx.y - minx.y, 2);
                // float distY = pow(maxy.x - miny.x, 2) + pow(maxy.y - miny.y, 2);
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
                rotateText = atan((maxx.y - minx.y) / abs(maxx.x - minx.x));  // * 180.0f * 3.1456f;
                if (minx.x - maxx.x >= 0.0f) {
                    rotateText *= -1.0f;
                }

                std::string text = "ctr " + std::to_string(ctrId);
                float px = provinces[pid].x, py = provinces[pid].y;
                AM::FontSize fontSize = AM::FontSize::PX16;

                int len = 0, hei = 0;
                while (len < dist * scale && (fontSize) <= AM::FontSize::PX160) {
                    len = 0; hei = 0;
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
               Log("fontSize="<<(int)fontSize);
                if (fontSize > AM::FontSize::PX160)
                    fontSize = AM::FontSize::PX160;
                if (len > dist * scale && fontSize > 0)
                    fontSize = (AM::FontSize)(fontSize - 1);
                    len = 0; hei = 0;
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
               Log("fontSize="<<(int)fontSize);
                hei = hei / text.size();
                // int cx = px * scale, cy = py * scale, cz = 300.0f;
                int cx = -len / 2, cy = -hei / 2;
                // textml = glm::rotate(textml, abs(sin(waterTime)) * 90.0f, glm::vec3{0.0f, 1.0f, 0.0f});
                int cz = 80.0f;
                {
                    int ind = (cx + maxx.x) * 3 + (cy + maxx.y) * mapWidth * 3;
                    assert(ind >= 0 && ind < mapWidth * mapHeight * 3);
                    cz = h[ind];
                    // Log("cz="<<cz);
                }

                // if (window.keys['Y'])
                //    rotateText = sin(waterTime) * 90.0f;
                textml = glm::rotate(textml, rotateText, glm::vec3{0.0f, 0.0f, 1.0f});
                glUniformMatrix4fv(glGetUniformLocation(fontShader.GetProgram(), "ml"), 1, GL_FALSE,
                                   glm::value_ptr(textml));
                int off = 0.0f;
                for (auto c : text) {
                    glm::vec3 rPos;
                    glm::vec2 rSize, rLBTC, tcLen;
                    if (AM::atlasInfo[fontSize][c].set) {
                        rPos = {cx + off, cy + AM::atlasInfo[fontSize][c].yPos, cz};
                        // rPos = textml * glm::vec4{rPos, 1.0f};
                        rSize = {(float)AM::atlasInfo[fontSize][c].width,
                                 (float)AM::atlasInfo[fontSize][c].height};
                        rLBTC = {AM::atlasInfo[fontSize][c].tcX, AM::atlasInfo[fontSize][c].tcY};
                        tcLen = {AM::atlasInfo[fontSize][c].tcWidth, AM::atlasInfo[fontSize][c].tcHeight};
                        off += rSize.x + 1;

                        fontBatch.Push(rPos.x, rPos.y, rPos.z, rSize.x, rSize.y, rLBTC.x, rLBTC.y, tcLen.x,
                                       tcLen.y, (float)fontSize);  //(float)AM::atlasTexture[fontSize]->GetId());
                        // void push(float x, float y, float z, float w, float h, float tcx, float tcy, float
                        // tcxlen, float tcylen); text.rects.push_back(std::make_unique<Rectangle>(rPos, rSize,
                        // rLBTC, tcLen)); if (content == "Quit game") { rSize.s += 0.1f;
                        //  Log("tu:"<<text.rects.back()->GetPosition().x <<", "<<text.rects.back()->GetSize().x
                        //         << ":: "<<text.rects.back()->GetPosition().x + text.rects.back()->GetSize().x );
                        // Log("tu2"<<":"<<AM::atlasInfo[this->text->fontSize][c].width);
                        //}
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
            }
        }
        fontBatch.Flush();
        float tt2 = glfwGetTime();

        window.Update();
        waterTime += dt;
        dt = glfwGetTime() - time;
        // Log(dt);
        time = glfwGetTime();
    }
}
/*
struct HashPair {
    int left, right;
};

Color currCol;
int provId;
const unsigned char* pix;
int wW, hH;
std::vector<int> left, right;
bool ok = false;
*/

/*
provs error on map
id:
430, 710, 825, 1120, 1892, 2445, 2994, 3020, 3321, 3554, 3590, 3663 3723, 3764, 3765
*/
