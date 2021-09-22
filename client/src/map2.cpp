#include "map2.h"

#include <glm/gtc/type_ptr.hpp>
#include "log.h"

GLuint err = 0;

LandBorders::LandBorders(const unsigned char* hpix, int mapWidth, int mapHeight, int scale)
{
    std::fstream file;
    file.open("BordersData2.txt", std::fstream::in);
    std::string ss;
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
            int h1 = hpix[index];
            verts.push_back(Vec3{x1, y1, hpix[index]});

            file >> x1 >> y1 >> z1;
            index = (int)x1 * 3 + (int)y1 * mapWidth * 3;
            if (y1 >= mapHeight)
                index = (int)x1 * 3 + (int)(mapHeight - 1) * mapWidth * 3;
            x1 *= scale;
            y1 *= scale;
            int h2 = hpix[index];
            verts.push_back(Vec3{x1, y1, hpix[index]});

            file >> x1 >> y1 >> z1;
            index = (int)x1 * 3 + (int)y1 * mapWidth * 3;
            if (y1 >= mapHeight)
                index = (int)x1 * 3 + (int)(mapHeight - 1) * mapWidth * 3;
            x1 *= scale;
            y1 *= scale;
            int h3 = hpix[index];
            verts.push_back(Vec3{x1, y1, hpix[index]});
            if (0) {
                if (h2 > h1 && h2 >= h3)
                    h1 = h2;
                else if (h3 > h1 && h3 >= h2)
                    h1 = h3;
            }
        }
    }
    file.close();
    glCreateVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glCreateBuffers(1, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3) * verts.size(), verts.data(), GL_STATIC_DRAW);
    glEnableVertexArrayAttrib(vao, 0);
    err = glGetError();
    if (err)
        Log("Opengl error: " << err);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3), NULL);
}

SeaBorders::SeaBorders(const unsigned char* hpix, int mapWidth, int mapHeight, int scale)
{
    std::fstream file;
    file.open("SeaBordersData.txt", std::fstream::in);
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
        int i1 = x1 * 3 + y1 * mapWidth * 3;
        if (i1 > mapHeight * mapWidth * 3 - 3)
            i1 = mapHeight * mapWidth * 3 - 3;
        int i2 = x2 * 3 + y2 * mapWidth * 3;
        if (i2 > mapHeight * mapWidth * 3 - 3)
            i2 = mapHeight * mapWidth * 3 - 3;
        verts.push_back(BorderVertex{.pos = Vec3{((float)x1) * scale, ((float)y1) * scale, hpix[i1]},
                                     .tc = Vec2{(float)x1 / mapWidth, (float)y1 / mapHeight}});
        verts.push_back(BorderVertex{.pos = Vec3{((float)x2) * scale, ((float)y2) * scale, hpix[i2]},
                                     .tc = Vec2{(float)x2 / mapWidth, (float)y2 / mapHeight}});
    }
    Log("Sea bor verts count: " << verts.size());
    file.close();
    glCreateVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glCreateBuffers(1, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(BorderVertex) * verts.size(), verts.data(), GL_STATIC_DRAW);
    glEnableVertexArrayAttrib(vao, 0);
    glEnableVertexArrayAttrib(vao, 1);
    GLuint err = glGetError();
    if (err)
        Log("Opengl error: " << err);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(BorderVertex), NULL);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(BorderVertex),
                          (const GLvoid*)(offsetof(BorderVertex, BorderVertex::tc)));
}

PolyMap::PolyMap(const unsigned char* hpix, int mapWidth, int mapHeight, const std::vector<Color3>& provsCols,
                 int scale)
{
    std::fstream file;
    file.open("polygon.txt", std::fstream::in);
    std::string ss;
    int provId = -1;
    // int vertCount = 0;
    while (file >> ss) {
        if (ss == "id:") {
            file >> ss;
            // if (provId != -1)
            //    provinces[provId].vertCount = vertCount;

            provId = std::atoi(ss.data());
            file >> ss;
            // provinces[provId].firstVertId = verts.size();
            // vertCount = 0;
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
        // glm::vec3 col{(float)provinces[provId].r / 255.0f, (float)provinces[provId].g / 255.0f,
        //              (float)provinces[provId].b / 255.0f};
        glm::vec3 col{(float)provsCols[provId].r / 255.0f, (float)provsCols[provId].g / 255.0f,
                      (float)provsCols[provId].b / 255.0f};
        float tx = (float)provId;  // / 3773.0f;// + 0.5f;
        int index;

        index = x1 * 3 + y1 * mapWidth * 3;
        if (y1 >= mapHeight)
            index = x1 * 3 + (mapHeight - 1) * mapWidth * 3;
        z = hpix[index];
        verts.push_back(PolyVert{.pos = glm::vec3{x1 * scale, y1 * scale, z}, .col = col, .tx = tx});

        index = x2 * 3 + y2 * mapWidth * 3;
        if (y2 >= mapHeight)
            index = x2 * 3 + (mapHeight - 1) * mapWidth * 3;
        z = hpix[index];
        verts.push_back(PolyVert{.pos = glm::vec3{x2 * scale, y2 * scale, z}, .col = col, .tx = tx});

        index = x3 * 3 + y3 * mapWidth * 3;
        if (y3 >= mapHeight)
            index = x3 * 3 + (mapHeight - 1) * mapWidth * 3;
        z = hpix[index];
        verts.push_back(PolyVert{.pos = glm::vec3{x3 * scale, y3 * scale, z}, .col = col, .tx = tx});

        glm::vec3 w = verts[verts.size() - 2].pos - verts[verts.size() - 3].pos;
        glm::vec3 v = verts[verts.size() - 1].pos - verts[verts.size() - 3].pos;
        glm::vec3 normal;
        normal.x = (v.y * w.z) - (v.z * w.y);
        normal.y = (v.z * w.x) - (v.x * w.z);
        normal.z = (v.x * w.y) - (v.y * w.x);
        verts[verts.size() - 1].normal = normal;
        verts[verts.size() - 2].normal = normal;
        verts[verts.size() - 3].normal = normal;

        // vertCount += 3;
    }
    glCreateVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glCreateBuffers(1, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(PolyVert) * verts.size(), verts.data(), GL_STATIC_DRAW);
    glEnableVertexArrayAttrib(vao, 0);
    glEnableVertexArrayAttrib(vao, 1);
    glEnableVertexArrayAttrib(vao, 2);
    glEnableVertexArrayAttrib(vao, 3);
    GLuint err = glGetError();
    if (err)
        Log("Opengl error: " << err);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(PolyVert), NULL);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(PolyVert),
                          (const GLvoid*)(offsetof(PolyVert, PolyVert::col)));
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(PolyVert),
                          (const GLvoid*)(offsetof(PolyVert, PolyVert::tx)));
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(PolyVert),
                          (const GLvoid*)(offsetof(PolyVert, PolyVert::normal)));
}

SeaProvColor::SeaProvColor(int mapWidth, int mapHeight, int scale)
{
    verts.push_back(SeaProvVert{.pos = Vec3{0.0, 0.0, 0.0}, .tc = Vec2{0.0, 0.0}});
    verts.push_back(SeaProvVert{.pos = Vec3{0.0, mapHeight * scale, 0.0}, .tc = Vec2{0.0, 1.0}});
    verts.push_back(SeaProvVert{.pos = Vec3{mapWidth * scale, mapHeight * scale, 0.0}, .tc = Vec2{1.0, 1.0}});
    verts.push_back(SeaProvVert{.pos = Vec3{mapWidth * scale, mapHeight * scale, 0.0}, .tc = Vec2{1.0, 1.0}});
    verts.push_back(SeaProvVert{.pos = Vec3{mapWidth * scale, 0.0, 0.0}, .tc = Vec2{1.0, 0.0}});
    verts.push_back(SeaProvVert{.pos = Vec3{0.0, 0.0, 0.0}, .tc = Vec2{0.0, 0.0}});
    glCreateVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glCreateBuffers(1, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(SeaProvVert) * verts.size(), verts.data(), GL_STATIC_DRAW);
    glEnableVertexArrayAttrib(vao, 0);
    glEnableVertexArrayAttrib(vao, 1);
    err = glGetError();
    if (err)
        Log("Opengl error: " << err);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SeaProvVert), NULL);  //(const GLvoid*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(SeaProvVert),
                          (const GLvoid*)(offsetof(SeaProvVert, SeaProvVert::tc)));
}

Water::Water(int mapWidth, int mapHeight, int scale)
{
    verts.push_back(WaterVert{.pos = Vec3{0.0f, 0.0f, 0.0f}, .tc = Vec2{0.0f, 0.0f}});
    verts.push_back(WaterVert{.pos = Vec3{mapWidth * scale * 0.5f, 0.0f, 0.0f}, .tc = Vec2{0.5f, 0.0f}});
    verts.push_back(
        WaterVert{.pos = Vec3{mapWidth * scale * 0.5f, mapHeight * scale, 0.0f}, .tc = Vec2{.5f, 1.0f}});

    verts.push_back(WaterVert{.pos = Vec3{0.0f, 0.0f, 0.0f}, .tc = Vec2{0.0f, 0.0f}});
    verts.push_back(
        WaterVert{.pos = Vec3{mapWidth * scale * 0.5f, mapHeight * scale * 0.5f, 0.0f}, .tc = Vec2{.5f, 1.0f}});
    verts.push_back(WaterVert{.pos = Vec3{0.0f, mapHeight * scale * 0.5f, 0.0f}, .tc = Vec2{0.0f, 1.0f}});

    verts.push_back(WaterVert{.pos = Vec3{0.0f, mapHeight * scale * 0.5f, 0.0f}, .tc = Vec2{.5f, 1.0f}});
    verts.push_back(
        WaterVert{.pos = Vec3{mapWidth * scale * 0.5f, mapHeight * scale * 0.5f, 0.0f}, .tc = Vec2{.5f, 1.0f}});
    verts.push_back(
        WaterVert{.pos = Vec3{mapWidth * scale * 0.5f, mapHeight * scale, 0.0f}, .tc = Vec2{.5f, 1.0f}});

    verts.push_back(WaterVert{.pos = Vec3{0.0f, mapHeight * scale * 0.5f, 0.0f}, .tc = Vec2{.5f, 1.0f}});
    verts.push_back(WaterVert{.pos = Vec3{0.0f, mapHeight * scale, 0.0f}, .tc = Vec2{.5f, 1.0f}});
    verts.push_back(
        WaterVert{.pos = Vec3{mapWidth * scale * 0.5f, mapHeight * scale, 0.0f}, .tc = Vec2{.5f, 1.0f}});

    verts.push_back(WaterVert{.pos = Vec3{mapWidth * scale * 0.5f, 0.0f, 0.0f}, .tc = Vec2{0.0f, 0.0f}});
    verts.push_back(WaterVert{.pos = Vec3{mapWidth * scale, 0.0f, 0.0f}, .tc = Vec2{0.5f, 0.0f}});
    verts.push_back(WaterVert{.pos = Vec3{mapWidth * scale, mapHeight * scale, 0.0f}, .tc = Vec2{.5f, 1.0f}});

    verts.push_back(WaterVert{.pos = Vec3{mapWidth * scale * 0.5f, 0.0f, 0.0f}, .tc = Vec2{0.0f, 0.0f}});
    verts.push_back(
        WaterVert{.pos = Vec3{mapWidth * scale, mapHeight * scale * 0.5f, 0.0f}, .tc = Vec2{.5f, 1.0f}});
    verts.push_back(
        WaterVert{.pos = Vec3{mapWidth * scale * 0.5f, mapHeight * scale * 0.5f, 0.0f}, .tc = Vec2{0.0f, 1.0f}});

    verts.push_back(
        WaterVert{.pos = Vec3{mapWidth * scale * 0.5f, mapHeight * scale * 0.5f, 0.0f}, .tc = Vec2{.5f, 1.0f}});
    verts.push_back(
        WaterVert{.pos = Vec3{mapWidth * scale, mapHeight * scale * 0.5f, 0.0f}, .tc = Vec2{.5f, 1.0f}});
    verts.push_back(WaterVert{.pos = Vec3{mapWidth * scale, mapHeight * scale, 0.0f}, .tc = Vec2{.5f, 1.0f}});

    verts.push_back(
        WaterVert{.pos = Vec3{mapWidth * scale * 0.5f, mapHeight * scale * 0.5f, 0.0f}, .tc = Vec2{.5f, 1.0f}});
    verts.push_back(
        WaterVert{.pos = Vec3{mapWidth * scale * 0.5f, mapHeight * scale, 0.0f}, .tc = Vec2{.5f, 1.0f}});
    verts.push_back(WaterVert{.pos = Vec3{mapWidth * scale, mapHeight * scale, 0.0f}, .tc = Vec2{.5f, 1.0f}});

    glCreateVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glCreateBuffers(1, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(WaterVert) * verts.size(), verts.data(), GL_STATIC_DRAW);
    glEnableVertexArrayAttrib(vao, 0);
    glEnableVertexArrayAttrib(vao, 1);
    err = glGetError();
    if (err)
        Log("Opengl error: " << err);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(WaterVert), NULL);  //(const GLvoid*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(WaterVert),
                          (const GLvoid*)(offsetof(WaterVert, WaterVert::tc)));
}

Map2::Map2(const unsigned char* hpix, int mapWidth, int mapHeight, const std::vector<Color3>& provsCols, int scale)
    : landBorders(hpix, mapWidth, mapHeight, scale),
      seaBorders(hpix, mapWidth, mapHeight, scale),
      polyMap(hpix, mapWidth, mapHeight, provsCols, scale),
      seaProvColor(mapWidth, mapHeight, scale),
      water(mapWidth, mapHeight, scale),
      provTexture("src/img/Provinces_org.png", mapWidth, mapHeight),
      grassT("../shared/grass1.png", 64, 64, GL_REPEAT)

{
    for (GLint i = 0; i < 32; ++i) {
        tex[i] = i;
    }
    for (GLint i = 0; i < 32; ++i) {
        texID[i] = i;
    }
    ReloadShaders();
}

void Map2::DrawForColorPick(glm::mat4 proj, float provCount)
{
    glUseProgram(waterColorShader.GetProgram());
    glUniformMatrix4fv(glGetUniformLocation(waterColorShader.GetProgram(), "matrix"), 1, GL_FALSE,
                       glm::value_ptr(proj));

    glUniform1iv(glGetUniformLocation(waterColorShader.GetProgram(), "tex"), 32, tex);
    glBindVertexArray(seaProvColor.vao);
    glBindBuffer(GL_ARRAY_BUFFER, seaProvColor.vbo);
    glDrawArrays(GL_TRIANGLES, 0, seaProvColor.verts.size());

    glUseProgram(landShader.GetProgram());
    glUniformMatrix4fv(glGetUniformLocation(landShader.GetProgram(), "matrix"), 1, GL_FALSE, glm::value_ptr(proj));
    glUniform1iv(glGetUniformLocation(landShader.GetProgram(), "tex"), 32, tex);
    glUniform1f(glGetUniformLocation(landShader.GetProgram(), "provCount"), provCount);
    glUniform1f(glGetUniformLocation(landShader.GetProgram(), "mapType"), MAPID_PROV_COLOR);
    glBindVertexArray(polyMap.vao);
    glBindBuffer(GL_ARRAY_BUFFER, polyMap.vbo);
    glDrawArrays(GL_TRIANGLES, 0, polyMap.verts.size());
}

void Map2::DrawWater(glm::mat4 proj, glm::vec3 eye)
{
    glUseProgram(waterShader.GetProgram());
    glUniform1f(glGetUniformLocation(waterShader.GetProgram(), "level"), 32);
    glUniform3fv(glGetUniformLocation(waterShader.GetProgram(), "eyeLight"), 1, glm::value_ptr(eye));
    glUniformMatrix4fv(glGetUniformLocation(waterShader.GetProgram(), "matrix"), 1, GL_FALSE,
                       glm::value_ptr(proj));

    glBindVertexArray(water.vao);
    glBindBuffer(GL_ARRAY_BUFFER, water.vbo);
    glDrawArrays(GL_PATCHES, 0, water.verts.size());
}

void Map2::DrawLand(glm::mat4 proj, glm::vec3 eye, float provId, float provCount, int MAPID)
{
    glUseProgram(landShader.GetProgram());
    glUniformMatrix4fv(glGetUniformLocation(landShader.GetProgram(), "matrix"), 1, GL_FALSE,
                       glm::value_ptr(proj));
    glUniform1iv(glGetUniformLocation(landShader.GetProgram(), "tex"), 32, tex);
    glUniform3fv(glGetUniformLocation(landShader.GetProgram(), "eyeLight"), 1, glm::value_ptr(eye));
    glUniform1f(glGetUniformLocation(landShader.GetProgram(), "provId"), provId);
    glUniform1f(glGetUniformLocation(landShader.GetProgram(), "provCount"), provCount);
    glUniform1f(glGetUniformLocation(landShader.GetProgram(), "mapType"), MAPID);

    glBindVertexArray(polyMap.vao);
    glBindBuffer(GL_ARRAY_BUFFER, polyMap.vbo);
    glDrawArrays(GL_TRIANGLES, 0, polyMap.verts.size());
}

void Map2::DrawBorders(glm::mat4 proj)
{
    glUseProgram(borderShader.GetProgram());
    glUniformMatrix4fv(glGetUniformLocation(borderShader.GetProgram(), "matrix"), 1, GL_FALSE,
                       glm::value_ptr(proj));
    glBindVertexArray(landBorders.vao);
    glBindBuffer(GL_ARRAY_BUFFER, landBorders.vbo);
    glDrawArrays(GL_TRIANGLES, 0, landBorders.verts.size());

    glUseProgram(seaBorderShader.GetProgram());
    glUniformMatrix4fv(glGetUniformLocation(seaBorderShader.GetProgram(), "matrix"), 1, GL_FALSE,
                       glm::value_ptr(proj));
    glBindVertexArray(seaBorders.vao);
    glBindBuffer(GL_ARRAY_BUFFER, seaBorders.vbo);
    glDrawArrays(GL_LINES, 0, seaBorders.verts.size());
}

void Map2::ReloadShaders()
{
    borderShader = Shader{"src/graphics/shaders/borders2/vert.v", "src/graphics/shaders/borders2/frag.f", "", ""};
    seaBorderShader = Shader{"src/graphics/shaders/sea_borders/vert.v", "src/graphics/shaders/sea_borders/frag.f",
                             "", "", "src/graphics/shaders/sea_borders/geom.g"};
    waterShader = Shader("src/graphics/shaders/water/vert.v", "src/graphics/shaders/water/frag.f",
                         "src/graphics/shaders/water/tes_ster.ts", "src/graphics/shaders/water/tes_w.tw");
    landShader = Shader{"src/graphics/shaders/poly/vert.v", "src/graphics/shaders/poly/frag.f", "", ""};
    waterColorShader =
        Shader{"src/graphics/shaders/water_color/vert.v", "src/graphics/shaders/water_color/frag.f", "", ""};
}

void Map2::ActivateTextures()
{
    texID[0] = grassT.GetId();
    texID[1] = provTexture.GetId();
    texID[20] = mapTextures.province.id;
    texID[21] = mapTextures.country.id;
    for (int i = 0; i < 32; ++i) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, texID[i]);
    }
}

