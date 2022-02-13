#include "game.h"

#include <unordered_set>

#include "ctr_data.h"
#include "font_batch.h"
#include "int_to_string.h"
#include "load_data.h"
#include "prepare_packet.h"
#include "prov_data.h"
#include "save_borders.h"

struct OfferPeaceData {
    struct pair {
        int provId = -1, recipantId = -1;
    };
    std::vector<pair> provs;
    int warId = -1;
    int recipantId = -1;
} offerPeaceData;  // it represents peace offer that i'm currently making, it
                   // resets with opening new peace offer making
                   // gd->window->(even if same war)

TopBarData topBarData;

int openedProvId = -1;

static void labelText(const glm::vec3 &pos, const glm::vec2 &size,
                      const glm::vec4 &col, const std::string &text,
                      AM::FontSize fontSize, std::vector<Vertex> &labelVerts);
static void genRectFromVert(Vertex fv, float width, float height,
                            std::vector<Vertex> &verts);

Game::Game(sf::TcpSocket &sock, std::string countryName, GameData *gd)
    : socket(sock),
      gd(gd),
      model3d("src/img/DudeDonePosefix.obj", glm::vec3{0.0, 0.0, 0.1})
{
    gd->resetMarkedCountry();

    Log("Ctr name:" << countryName);
    auto it = std::find_if(
        gd->countries.begin(), gd->countries.end(),
        [countryName](const Country &c) { return c.GetName() == countryName; });
    if (it != gd->countries.end()) {
        myCountryId = it->GetId();
        Log(countryName);
        for (auto &prov : gd->provinces) {
            if (prov.GetCountryId() == gd->countries[myCountryId].GetId()) {
                gd->camera.SetPos(prov.GetUnitPos().x * gd->settings.scale,
                                  prov.GetUnitPos().y * gd->settings.scale);
                break;
            }
        }
    }
    else
        Log("There is no country for you ;(");
}

Game::~Game() {}

void Game::Play()
{
    float fpsTime = 0.0f;
    int frames = 0;

    pickModelShader = Shader{"src/graphics/shaders/pick_model/vert.v",
                             "src/graphics/shaders/pick_model/frag.f", "", ""};
    labelShader = Shader{"src/graphics/shaders/label/vert.v",
                         "src/graphics/shaders/label/frag.f", "", ""};
    glPatchParameteri(GL_PATCH_VERTICES, 3);
    int err = glGetError();
    if (err)
        Log("Opengl error: " << err);

    std::map<int, Node> nodes;

    err = glGetError();
    if (err)
        Log("Opengl error: " << err);
    err = glGetError();
    if (err)
        Log("Opengl error: " << err);

    err = glGetError();
    if (err)
        Log("Opengl error: " << err);

    // to jest bindowane prz rysowaniu unitow trzeba to sprawdzic
    GLuint otherTexID[32];
    for (GLint i = 0; i < 32; ++i) otherTexID[i] = i;
    otherTexID[0] = AM::am.modelTexture->GetId();

    struct TempUnit {
        glm::vec3 pos;
        int provId;
    };
    std::vector<TempUnit> tempUnits;
    for (std::size_t i = 0; i < gd->provinces.size(); ++i) {
        if (gd->provinces[i].water)
            continue;

        float z = (float)gd->map->heightMap.GetPixels()[(
            int)(gd->provinces[i].unitPosition.x * 3 +
                 gd->provinces[i].unitPosition.y * gd->map->mapWidth * 3)];
        tempUnits.push_back(TempUnit{
            .pos =
                glm::vec3{gd->provinces[i].unitPosition.x * gd->settings.scale,
                          gd->provinces[i].unitPosition.y * gd->settings.scale,
                          z},
            .provId = gd->provinces[i].id});
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
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
                              (void *)0);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
                              (void *)(sizeof(glm::vec4)));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
                              (void *)(2 * sizeof(glm::vec4)));
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
                              (void *)(3 * sizeof(glm::vec4)));
        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);
        glBindVertexArray(0);
    }

    Shader shader("src/graphics/shaders/basic/vert.v",
                  "src/graphics/shaders/basic/frag.f", "", "");
    bool drawBorders = true;
    float time = glfwGetTime();
    guiLast.init(gd->window, gd->settings.resolution, gd->window->GetSize());
    while (!gd->window->ShouldClose()) {
        //
        receivePackets();
        sendPackets();
        updateBattles();

        glEnable(GL_DEPTH_TEST);  // Enable depth testing for z-culling
        glDepthFunc(GL_LESS);
        gd->window->Refresh();

        if (gd->window->keys['M']) {
            labelShader = Shader{"src/graphics/shaders/label/vert.v",
                                 "src/graphics/shaders/label/frag.f", "", ""};
            gd->fontShader = Shader{"src/graphics/shaders/fonts.vert",
                                    "src/graphics/shaders/fonts.frag", "", ""};
            pickModelShader =
                Shader{"src/graphics/shaders/pick_model/vert.v",
                       "src/graphics/shaders/pick_model/frag.f", "", ""};
        }

        glm::mat4 matrix = gd->camera.GetMat();

        gd->map->ActivateTextures();
        float rotateX = 60.0f * 3.1459265f / 180.0f, yScale = 10.0f;
        glm::mat4 rotate = glm::mat4{1.0f};
        rotate = glm::rotate(glm::mat4{1.0}, rotateX, glm::vec3{1.0, 0.0, 0.0});
        // for (auto &unit : gd->units) {
        for (int i = 0; i < gd->units.size(); ++i) {
            if (abs(gd->units[i].GetFakePos().x - gd->camera.eye.x) > 400)
                continue;
            if (abs(gd->units[i].GetFakePos().y - gd->camera.eye.y) > 200)
                continue;
            glm::mat4 unitModel = glm::mat4(1.0);
            unitModel = glm::translate(unitModel, gd->units[i].GetFakePos());

            unitModel = unitModel * rotate;
            unitModel = glm::scale(unitModel, glm::vec3{20.0, yScale, 20.0});
            pids.push_back(gd->units[i].GetProvId());
            uinds.push_back(i);

            // unitsAtPoint
            auto upos = gd->units[i].position;

            long point = (long)(upos.x + 0.5f);
            point |= ((long)(upos.y + 0.5f) << 32);
            Log("point: " << point);
            bool found = false;
            for (std::size_t j = 0; j < unitsAtPoint.size(); ++j) {
                if (unitsAtPoint[j].point == point) {
                    unitsAtPoint[j].uind.push_back(i);
                    unitsAtPoint[j].ctrIds.insert(gd->units[i].countryId);
                    unitsAtPoint[j].size += gd->units[i].soldiers;
                    if (myCountryId != gd->units[i].countryId &&
                        unitsAtPoint[j].status != 1) {
                        if (true ==
                            isAtWar(myCountryId, gd->units[i].countryId))
                            unitsAtPoint[j].status = 1;
                        else
                            unitsAtPoint[j].status = 2;
                    }
                    found = true;
                    break;
                }
            }
            if (false == found) {
                uMat.push_back(unitModel);
                unitsAtPoint.emplace_back();
                unitsAtPoint.back().point = point;
                unitsAtPoint.back().uind.push_back(i);
                unitsAtPoint.back().ctrIds.insert(gd->units[i].countryId);
                unitsAtPoint.back().size += gd->units[i].soldiers;
                if (myCountryId != gd->units[i].countryId) {
                    if (true == isAtWar(myCountryId, gd->units[i].countryId))
                        unitsAtPoint.back().status = 1;
                    else
                        unitsAtPoint.back().status = 2;
                }
            }
            //~unitsAtPoint
        }

        input();

        if (gd->window->keys['U']) {
            break;
        }

        if (gd->window->keys['L'])
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        if (gd->window->keys['K'])
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        if (gd->window->keys['B']) {
            drawBorders = !drawBorders;
        }

        {
            gd->map->DrawWater(matrix, gd->camera.eye);
            if (openPeaceOfferId == -1) {
                gd->map->DrawLand(matrix, gd->camera.eye, markedProvId,
                                  gd->provinces.size(), gd->map->MAPID_COUNTRY,
                                  gd->waterTime);
            }
            else {  // open offering peace mode
                    // Log("peace mode");
                unsigned char *nexpix =
                    new unsigned char[gd->map->mapWidth * 4];
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
                    for (int i = 0; i < gd->provinces.size(); ++i) {
                        if (std::find_if(
                                peaceOffers[peaceind].lostProv.begin(),
                                peaceOffers[peaceind].lostProv.end(),
                                [id = i](const std::tuple<int, int, int> &t) {
                                    return std::get<0>(t) == id;
                                }) != peaceOffers[peaceind].lostProv.end())
                        {
                            col = &red;
                        }
                        else if (std::find_if(
                                     peaceOffers[peaceind].gainProv.begin(),
                                     peaceOffers[peaceind].gainProv.end(),
                                     [id = i](
                                         const std::tuple<int, int, int> &t) {
                                         return std::get<0>(t) == id;
                                     }) != peaceOffers[peaceind].gainProv.end())
                        {
                            col = &green;
                        }
                        else if (peaceOffers[peaceind].offeredBy ==
                                     gd->provinces[i].GetCountryId() ||
                                 peaceOffers[peaceind].recipant ==
                                     gd->provinces[i].GetCountryId())
                        {
                            if (peaceOffers[peaceind].offeredBy ==
                                gd->provinces[i].GetCountryId())
                                col = &lightBlue;
                            else
                                col = &lightRed;
                        }
                        else {
                            col = &neutral;
                        }
                        if (i == 3018) {
                            // Log((int)col->r << " " << (int)col->g << " " <<
                            // (int)col->b);
                        }

                        nexpix[i * 4] = col->r;
                        nexpix[i * 4 + 1] = col->g;
                        nexpix[i * 4 + 2] = col->b;
                        nexpix[i * 4 + 3] = col->a;
                    }
                    Texture newtex = Texture(nexpix, gd->provinces.size(), 1);
                    delete[] nexpix;
                    gd->map->texID[(int)(gd->map->MAPID_PEACE_OFFER + 0.5f)] =
                        newtex.GetId();
                    gd->map->ActivateTextures();
                    gd->map->DrawLand(matrix, gd->camera.eye, markedProvId,
                                      gd->provinces.size(),
                                      gd->map->MAPID_PEACE_OFFER, 0.0f);
                }
            }
        }

        if (drawBorders) {
            gd->map->DrawBorders(matrix);
        }

        // drawingMoves = false;
        if (gd->camera.eye.z < gd->zPoint && !gd->window->keys['U']) {
            if (openUnitId != -1) {  // drawing moves arrows
                auto unit = std::find_if(gd->units.begin(), gd->units.end(),
                                         [id = openUnitId](const Unit &u) {
                                             return u.GetId() == id;
                                         });
                if (unit != gd->units.end()) {
                    drawingMoves = true;
                    std::vector<BorderVertex> verts;
                    if (unit->movesVec.size()) {
                        verts.push_back(BorderVertex{
                            .pos = Vec3{unit->fakePos.x, unit->fakePos.y,
                                        unit->fakePos.z},
                            .tc = Vec2{unit->fakePos.x / gd->map->mapWidth,
                                       unit->fakePos.y / gd->map->mapHeight}});
                    }
                    for (std::size_t i = 0; i < unit->movesVec.size(); ++i) {
                        glm::vec3 pos = unit->movesVec[i];
                        Log(pos.x << " " << pos.y << " " << pos.z);
                        int ind = pos.x * 3 + pos.y * gd->map->mapWidth * 3;
                        if (ind >
                            gd->map->mapHeight * gd->map->mapWidth * 3 - 3)
                            ind =
                                gd->map->mapHeight * gd->map->mapWidth * 3 - 3;
                        verts.push_back(BorderVertex{
                            .pos = Vec3{pos.x, pos.y, pos.z},
                            .tc = Vec2{pos.x / gd->map->mapWidth,
                                       pos.y / gd->map->mapHeight}});
                        if (i < unit->movesVec.size() - 1) {
                            verts.push_back(BorderVertex{
                                .pos = Vec3{pos.x, pos.y, pos.z},
                                .tc = Vec2{pos.x / gd->map->mapWidth,
                                           pos.y / gd->map->mapHeight}});
                        }
                    }
                    GLuint err = 0;
                    GLuint arrvao, arrvbo;
                    glCreateVertexArrays(1, &arrvao);
                    glBindVertexArray(arrvao);
                    glCreateBuffers(1, &arrvbo);
                    err = glGetError();
                    if (err)
                        Log("Opengl error: " << err);

                    glDisable(
                        GL_DEPTH_TEST);  // Enable depth testing for z-culling
                    glBindBuffer(GL_ARRAY_BUFFER, arrvbo);
                    glBufferData(GL_ARRAY_BUFFER,
                                 sizeof(BorderVertex) * verts.size(),
                                 verts.data(), GL_STATIC_DRAW);
                    err = glGetError();
                    if (err)
                        Log("Opengl error: " << err);
                    glEnableVertexArrayAttrib(arrvao, 0);
                    glEnableVertexArrayAttrib(arrvao, 1);
                    err = glGetError();
                    if (err)
                        Log("Opengl error: " << err);
                    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                                          sizeof(BorderVertex), NULL);
                    glVertexAttribPointer(
                        1, 2, GL_FLOAT, GL_FALSE, sizeof(BorderVertex),
                        (const GLvoid *)(offsetof(BorderVertex, tc)));
                    glUseProgram(gd->map->seaBorderShader.GetProgram());
                    glUniformMatrix4fv(
                        glGetUniformLocation(
                            gd->map->seaBorderShader.GetProgram(), "matrix"),
                        1, GL_FALSE, glm::value_ptr(gd->camera.GetMat()));
                    glBindVertexArray(arrvao);
                    glBindBuffer(GL_ARRAY_BUFFER, arrvbo);
                    glDrawArrays(GL_LINES, 0, verts.size());
                    err = glGetError();
                    if (err)
                        Log("Opengl error: " << err);
                    glEnable(
                        GL_DEPTH_TEST);  // Enable depth testing for z-culling
                    glDeleteVertexArrays(1, &arrvao);
                    glDeleteBuffers(1, &arrvbo);
                }
            }
            float rotateX = 60.0f * 3.1459265f / 180.0f, yScale = 10.0f;
            glm::mat4 rotate = glm::mat4{1.0f};
            rotate =
                glm::rotate(glm::mat4{1.0}, rotateX, glm::vec3{1.0, 0.0, 0.0});

            Log(unitsAtPoint.size());

            {                              // print unit labels;
                glDisable(GL_DEPTH_TEST);  // Enable depth testing for z-culling

                std::vector<Vertex> labelVerts;
                glm::vec4 owncol{0.0f, 0.0f, 1.0f, 1.0f};
                glm::vec4 ownbgcol{0.0f, 0.0f, 0.5f, 1.0f};
                glm::vec4 enemycol{1.0f, 0.0f, 0.0f, 1.0f};
                glm::vec4 enemybgcol{0.8f, 0.0f, 0.5f, 1.0f};
                glm::vec4 enenemycol{1.0f, 0.5f, 0.0f, 1.0f};
                glm::vec4 enenemybgcol{0.5f, 0.3f, 0.0f, 1.0f};
                glm::vec4 *currcol = &owncol;
                glm::vec4 *currbgcol = &ownbgcol;
                for (std::size_t j = 0; j < unitsAtPoint.size(); ++j) {
                    assert(unitsAtPoint[j].uind.size() > 0);
                    int i = unitsAtPoint[j].uind[0];
                    glm::mat4 unitModel = glm::mat4(1.0);
                    unitModel =
                        glm::translate(unitModel, gd->units[i].GetFakePos());
                    auto asd = gd->units[i].GetFakePos();
                    unitModel =
                        glm::scale(unitModel, glm::vec3{20.0, yScale, 20.0});

                    float ww = 70.0f, hh = 25.0f;

                    if (unitsAtPoint[j].status == 0) {
                        currcol = &owncol;
                        currbgcol = &ownbgcol;
                    }
                    else if (unitsAtPoint[j].status == 1) {
                        currcol = &enemycol;
                        currbgcol = &enemybgcol;
                    }
                    else {
                        currcol = &enenemycol;
                        currbgcol = &enenemybgcol;
                    }
                    Vertex fv{
                        .pos = glm::vec3{asd.x - ww * 0.6f, asd.y - hh - 8.0f,
                                         asd.z},
                        .color = *currcol,
                        .tc = glm::vec2{0.0f, 0.0f},
                        .textureId = (int)22,
                    };
                    Vertex fvbg = fv;
                    float wdiff = 2.0f;
                    fvbg.pos.x -= wdiff;
                    fvbg.pos.y -= wdiff;
                    fvbg.pos.z -= 0.5f;
                    fvbg.color = *currbgcol;
                    genRectFromVert(fvbg, ww + wdiff * 2.0f, hh + wdiff * 2.0f,
                                    labelVerts);
                    genRectFromVert(fv, ww, hh, labelVerts);

                    fv.pos.z += 1.0f;
                    std::string labtext = int_to_string(unitsAtPoint[j].size);
                    labelText(fv.pos, glm::vec2{ww, hh},
                              glm::vec4{1.0f, 1.0f, 1.0f, 1.0f}, labtext,
                              AM::FontSize::PX32, labelVerts);

                    // country etiquetes
                    float ewidth = ww * 0.1f, eheight = hh * 0.33f;
                    float offsetx = 1.0f, offsety = 1.0f;
                    float totalheight =
                        (eheight + offsety) * unitsAtPoint[j].ctrIds.size();
                    float starty = fv.pos.y + hh * 0.5f - totalheight * 0.5f;
                    fvbg.pos.x -= (offsetx + ewidth);
                    // for (int k = 0; k < unitsAtPoint[i].ctrIds.size(); ++k) {
                    for (auto ctrid : unitsAtPoint[j].ctrIds) {
                        fvbg.pos.y = starty;
                        fvbg.color = glm::vec4{
                            gd->countries[ctrid].color.r / 255.0f,
                            gd->countries[ctrid].color.g / 255.0f,
                            gd->countries[ctrid].color.b / 255.0f, 1.0f};
                        genRectFromVert(fvbg, ewidth, eheight, labelVerts);
                        starty += eheight + offsety;
                    }
                    //~ country etiquetes

                    // if unit is opened
                    //
                    //~ if unit is opened
                }

                glUseProgram(labelShader.GetProgram());
                for (int i = 0; i <= (int)AM::FontSize::PX160; ++i) {
                    glActiveTexture(GL_TEXTURE0 + i);
                    glBindTexture(GL_TEXTURE_2D, (GLuint)gd->fontTexID[i]);
                }
                glUniformMatrix4fv(
                    glGetUniformLocation(labelShader.GetProgram(), "matrix"), 1,
                    GL_FALSE, glm::value_ptr(matrix));
                glUniform1iv(
                    glGetUniformLocation(labelShader.GetProgram(), "tex"), 32,
                    gd->tex);
                GLuint labelVao, labelVbo;
                glCreateVertexArrays(1, &labelVao);
                glBindVertexArray(labelVao);
                glCreateBuffers(1, &labelVbo);

                glBindBuffer(GL_ARRAY_BUFFER, labelVbo);
                glBufferData(GL_ARRAY_BUFFER,
                             sizeof(Vertex) * labelVerts.size(),
                             labelVerts.data(), GL_STATIC_DRAW);
                glEnableVertexArrayAttrib(labelVao, 0);
                glEnableVertexArrayAttrib(labelVao, 1);
                glEnableVertexArrayAttrib(labelVao, 2);
                glEnableVertexArrayAttrib(labelVao, 3);
                GLuint err = glGetError();
                if (err)
                    Log("Opengl error: " << err);
                glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                                      NULL);  //(const GLvoid*)0);
                glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                                      (const GLvoid *)(offsetof(Vertex, tc)));
                glVertexAttribPointer(
                    2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                    (const GLvoid *)(offsetof(Vertex, color)));
                glVertexAttribPointer(
                    3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                    (const GLvoid *)(offsetof(Vertex, textureId)));

                glBindVertexArray(labelVao);
                glDrawArrays(GL_TRIANGLES, 0, labelVerts.size());

                glEnable(GL_DEPTH_TEST);  // Enable depth testing for z-culling
                glDeleteVertexArrays(1, &labelVao);
                glDeleteBuffers(1, &labelVbo);
            }
            for (int i = 0; i < 32; ++i) {
                glActiveTexture(GL_TEXTURE0 + i);
                glBindTexture(GL_TEXTURE_2D, otherTexID[i]);
            }
            for (auto i : uinds) {
                // if (abs(gd->units[i].GetFakePos().x - gd->camera.eye.x) >
                // 400)
                //     continue;
                // if (abs(gd->units[i].GetFakePos().y - gd->camera.eye.y) >
                // 200)
                //     continue;
                glm::mat4 unitModel = glm::mat4(1.0);
                unitModel =
                    glm::translate(unitModel, gd->units[i].GetFakePos());

                unitModel = unitModel * rotate;
                unitModel =
                    glm::scale(unitModel, glm::vec3{20.0, yScale, 20.0});

                glUseProgram(AM::am.shader->GetProgram());
                glUniformMatrix4fv(
                    glGetUniformLocation(AM::am.shader->GetProgram(), "matrix"),
                    1, GL_FALSE, glm::value_ptr(matrix));
                glUniformMatrix4fv(
                    glGetUniformLocation(AM::am.shader->GetProgram(), "ml"), 1,
                    GL_FALSE, glm::value_ptr(unitModel));
                glUniform1iv(
                    glGetUniformLocation(AM::am.shader->GetProgram(), "tex"),
                    32, gd->tex);
                model3d.Draw();
            }
        }
        // ctrnames
        for (int i = 0; i <= (int)AM::FontSize::PX160; ++i) {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, (GLuint)gd->fontTexID[i]);
        }

        checkCountryNamesFade(gd);
        if (((gd->camera.eye.z > gd->zPoint) || gd->ctrNamesFade < 1.0f) &&
            openPeaceOfferId == -1)
        {
            drawCountryNames(gd);
        }
        updateCountryNamesFade(gd, dt);
        //~ctrNames

        glEnable(GL_DEPTH_TEST);  // Enable depth testing for z-culling
        matrix = glm::ortho(0.0f, (float)gd->settings.resolution.x, 0.0f,
                            (float)gd->settings.resolution.y);
        glUseProgram(shader.GetProgram());
        glUniformMatrix4fv(glGetUniformLocation(shader.GetProgram(), "matrix"),
                           1, GL_FALSE, glm::value_ptr(matrix));

        GLuint ts[] = {0};
        glBindTextures(ts[0], 1, ts);
        glActiveTexture(GL_TEXTURE0);
        guiLast.start();

        guiDraw();
        guiLast.flush();

        gd->window->scrollOffset = 0.0f;
        gd->window->Update();
        pids.clear();
        uinds.clear();
        unitsAtPoint.clear();
        uMat.clear();
        dt = glfwGetTime() - time;
        gd->updateTime(dt);
        fpsTime += dt;
        ++frames;
        if (fpsTime > 1.0f) {
            fpsTime = 0.0f;
            frames = 0;
        }
        // gd->waterTime += dt;
        time = glfwGetTime();
    }
}

bool Game::isAtWar(int ctr1, int ctr2)
{
    for (std::size_t i = 0; i < gd->wars.size(); ++i) {
        if (true == gd->wars[i].ShouldTheyFight(ctr1, ctr2))
            return true;
    }
    return false;
}

void Game::guiDraw()
{
    ctype.ct = ClickEventType::MISS;
    glm::vec2 mp{gd->window->xMouse * gd->settings.resolution.x /
                     gd->window->GetSize().x,
                 (gd->window->GetSize().y - gd->window->yMouse) *
                     gd->settings.resolution.y / gd->window->GetSize().y};

    GuiLast::GuiEv tmpctype;
    tmpctype = guiLast.game_topBar(topBarData, mp.x, mp.y);
    if (tmpctype.ct != ClickEventType::MISS)
        ctype = tmpctype;

    tmpctype =
        guiLast.game_SideBar(sideBarData, mp.x, mp.y, gd->window->mouseLClicked,
                             gd->window->scrollOffset);
    if (tmpctype.ct != ClickEventType::MISS)
        ctype = tmpctype;

    if (openMyCountry) {
        tmpctype =
            guiLast.game_myCountry(&gd->countries[myCountryId], mp.x, mp.y);
        if (gd->window->mouseLClicked &&
            tmpctype.ct == ClickEventType::CLOSE_WINDOW)
            openMyCountry = false;
        else if (tmpctype.ct != ClickEventType::MISS)
            ctype = tmpctype;
    }

    if (openProvId != -1) {
        tmpctype = guiLast.game_prov(gd->provinces[openProvId], mp.x, mp.y,
                                     gd->window->mouseLClicked,
                                     gd->provinces[openProvId].GetCountryId() ==
                                         gd->countries[myCountryId].GetId());
        if (gd->window->mouseLClicked &&
            tmpctype.ct == ClickEventType::CLOSE_WINDOW)
            openProvId = -1;
        else if (tmpctype.ct != ClickEventType::MISS)
            ctype = tmpctype;
    }

    if (openBattleId != -1) {
        assert(gd->battles[openBattleId].provId >= 0 &&
               gd->battles[openBattleId].provId < gd->provinces.size());
        tmpctype = guiLast.game_battle(
            gd->battles[openBattleId],
            gd->provinces[gd->battles[openBattleId].provId], mp.x, mp.y,
            gd->window->mouseLClicked, gd->countries, gd->units);
        if (gd->window->mouseLClicked &&
            tmpctype.ct == ClickEventType::CLOSE_WINDOW)
            openBattleId = -1;
        else if (tmpctype.ct != ClickEventType::MISS)
            ctype = tmpctype;
    }

    if (openCountryId != -1) {
        tmpctype = guiLast.game_country(&gd->countries[openCountryId],
                                        &gd->countries[myCountryId], gd->wars,
                                        mp.x, mp.y);
        if (gd->window->mouseLClicked &&
            tmpctype.ct == ClickEventType::CLOSE_WINDOW)
            openCountryId = -1;
        else if (tmpctype.ct != ClickEventType::MISS)
            ctype = tmpctype;
    }

    if (openUnitId != -1) {
        auto unit = std::find_if(
            gd->units.begin(), gd->units.end(),
            [id = openUnitId](const Unit &u) { return u.GetId() == id; });
        if (unit != gd->units.end()) {
            tmpctype = guiLast.game_unit(
                (*unit), mp.x, mp.y, gd->window->mouseLClicked, gd->countries);
            if (gd->window->mouseLClicked &&
                tmpctype.ct == ClickEventType::CLOSE_WINDOW)
                openUnitId = -1;
            else if (tmpctype.ct != ClickEventType::MISS)
                ctype = tmpctype;
        }
    }

    if (openWarId != -1) {
        int warind;
        for (std::size_t i = 0; i < gd->wars.size(); ++i) {
            if (gd->wars[i].id == openWarId) {
                warind = i;
                break;
            }
        }
        tmpctype = guiLast.game_war(&gd->wars[warind], mp.x, mp.y,
                                    gd->window->mouseLClicked);
        if (gd->window->mouseLClicked &&
            tmpctype.ct == ClickEventType::CLOSE_WINDOW)
            openWarId = -1;
        else if (tmpctype.ct != ClickEventType::MISS)
            ctype = tmpctype;
    }

    if (openUnitsList == true) {
        std::vector<Unit *> clickedUnits_ptr;
        for (auto uid : clickedUnits) {
            auto unit = std::find_if(
                gd->units.begin(), gd->units.end(),
                [id = uid](const Unit &u) { return u.GetId() == id; });
            if (unit != gd->units.end()) {
                clickedUnits_ptr.push_back(&(*unit));
            }
        }
        tmpctype = guiLast.game_unitsList(
            clickedUnits_ptr, mp.x, mp.y, gd->window->mouseLClicked,
            gd->window->scrollOffset, &gd->provinces[openUnitsListProvId],
            gd->countries);
        if (gd->window->mouseLClicked &&
            tmpctype.ct == ClickEventType::CLOSE_WINDOW)
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
        tmpctype = guiLast.game_peaceOffer(
            &peaceOffers[peaceind], mp.x, mp.y, myCountryId, gd->provinces,
            gd->countries, gd->window->mouseLClicked, gd->window->scrollOffset);
        if (gd->window->mouseLClicked &&
            tmpctype.ct == ClickEventType::CLOSE_WINDOW) {
            openPeaceOfferId = -1;
        }
        else if (tmpctype.ct != ClickEventType::MISS)
            ctype = tmpctype;
    }

    if (game_paused) {
        tmpctype = guiLast.game_drawPause();
    }

    if (gd->window->mouseLClicked) {
        std::vector<sf::Packet> packets;
        switch (ctype.ct) {
            case ClickEventType::OPEN_MY_COUNTRY: {
                resetGuiWindows();
                openMyCountry = true;
                break;
            }
            case ClickEventType::OPEN_COUNTRY: {
                resetGuiWindows();
                if (myCountryId == ctype.val)
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
                openCountryId = gd->provinces[openProvId].GetCountryId();
                break;
            }
            case ClickEventType::CREATE_UNIT: {
                packets.emplace_back(PreparePacket::NewUnit(
                    gd->provinces[openProvId].GetCountryId(), openProvId,
                    ctype.val));
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
                if (sideBarData.elements[ind].type ==
                    SideBarData::IType::PEACE_OFFER) {
                    resetGuiWindows();
                    openPeaceOfferId =
                        sideBarData.elements[ind].val;  // ctype.val;
                }
                else if (sideBarData.elements[ind].type ==
                         SideBarData::IType::WAR) {
                    resetGuiWindows();
                    openWarId = sideBarData.elements[ind].val;
                }
                else {
                    sideBarData.elements.erase(sideBarData.elements.begin() +
                                               ind);
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
                    for (std::size_t i = 0; i < gd->wars.size(); ++i) {
                        if ((openWarId != -1 && gd->wars[i].id == openWarId) ||
                            (openWarId == -1 && gd->wars[i].id == ctype.val))
                        {
                            warind = i;
                            break;
                        }
                    }
                    Log(warind);
                    int recipantId = gd->wars[warind].attackersIds[0];
                    if (recipantId == myCountryId)
                        recipantId = gd->wars[warind].defendersIds[0];
                    peaceOffers.push_back(PeaceOffer{
                        myCountryId, gd->wars[warind].id, recipantId});
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
                for (std::size_t ind = 0; ind < sideBarData.elements.size();
                     ++ind) {
                    if (sideBarData.elements[ind].type ==
                        SideBarData::IType::PEACE_OFFER) {
                        sideBarData.elements.erase(
                            sideBarData.elements.begin() + ind);
                        break;
                    }
                }
                if (peaceOffers[peaceind].peaceId == -2) {
                    sf::Packet packet;
                    packet << "PeaceOffer";
                    packet << peaceOffers[peaceind].warId;
                    packet << peaceOffers[peaceind].recipant;
                    packet << (unsigned int)(peaceOffers[peaceind]
                                                 .gainProv.size() +
                                             peaceOffers[peaceind]
                                                 .lostProv.size());
                    for (std::size_t i = 0;
                         i < peaceOffers[peaceind].lostProv.size(); ++i) {
                        packet
                            << std::get<0>(peaceOffers[peaceind].lostProv[i]);
                        packet
                            << std::get<1>(peaceOffers[peaceind].lostProv[i]);
                    }
                    for (std::size_t i = 0;
                         i < peaceOffers[peaceind].gainProv.size(); ++i) {
                        packet
                            << std::get<0>(peaceOffers[peaceind].gainProv[i]);
                        packet
                            << std::get<1>(peaceOffers[peaceind].gainProv[i]);
                    }
                    peaceOffers.erase(peaceOffers.begin() + peaceind);
                    packets.push_back(packet);
                }
                else if (peaceOffers[peaceind].peaceId != -1) {
                    sf::Packet packet;
                    packet << "AcceptPeace";
                    packet << peaceOffers[peaceind].peaceId;
                    packets.push_back(packet);
                    for (std::size_t ind = 0; ind < sideBarData.elements.size();
                         ++ind) {
                        if (sideBarData.elements[ind].type ==
                                SideBarData::IType::WAR &&
                            sideBarData.elements[ind].val ==
                                peaceOffers[peaceind].warId)
                        {
                            sideBarData.elements.erase(
                                sideBarData.elements.begin() + ind);
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

                for (std::size_t ind = 0; ind < sideBarData.elements.size();
                     ++ind) {
                    if (sideBarData.elements[ind].type ==
                        SideBarData::IType::PEACE_OFFER) {
                        sideBarData.elements.erase(
                            sideBarData.elements.begin() + ind);
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
                    auto unit = std::find_if(
                        gd->units.begin(), gd->units.end(),
                        [id = uid](const Unit &u) { return u.GetId() == id; });
                    if (unit != gd->units.end()) {
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

        gd->window->mouseLClicked = false;
        gd->window->mouseRClicked = false;
    }

    if (ctype.ct == ClickEventType::NONE || ctype.ct == ClickEventType::MISS)
        glfwSetCursor(gd->window->window, AM::am.def_cursor);
    else
        glfwSetCursor(gd->window->window, AM::am.hover_cursor);
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
        ProcessPacket::DailyUpdate(packet, gd->wars, gd->provinces,
                                   gd->countries, topBarData, gd->map);
    }
    else if (type == "hourly") {
        ProcessPacket::HourlyUpdate(
            packet, gd->units, gd->battles, gd->settings.scale,
            gd->map->heightMap.GetPixels(), gd->map->mapWidth);
    }
    else if (type == "PeaceAccepted") {
        ProcessPacket::PeaceAccepted(packet, gd->provinces, gd->countries,
                                     gd->wars, sideBarData);
        gd->editCountryMap();
        gd->makeCountryNames();
        if (openedProvId >= 0) {
            assert(openedProvId < (int)gd->provinces.size());
        }
    }
    else if (type == "ProvincePopulation") {
        ProcessPacket::ProvincePopulation(packet, gd->provinces);
    }
    else if (type == "NewWar") {
        ProcessPacket::NewWar(packet, gd->wars, myCountryId, gd->countries,
                              sideBarData);
    }
    else if (type == "SetGold") {
        ProcessPacket::SetGold(packet, &gd->countries[myCountryId]);
    }
    else if (type == "Speed") {
        ProcessPacket::SetSpeed(packet);
    }
    else if (type == "monthly") {
        ProcessPacket::MonthlyUpdate(
            packet, gd->countries[myCountryId].GetName(), gd->countries);
    }
    else if (type == "BotPeaceOffer") {
        ProcessPacket::BotPeaceOffer(packet, peaceOffers, gd->countries,
                                     sideBarData, &gd->countries[myCountryId]);
    }
    else if (type == "PeaceDeclined") {
        ProcessPacket::PeaceDeclined(packet, sideBarData, gd->countries);
    }
    else if (type == "Pause") {
        ProcessPacket::Paused(packet, game_paused);
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
    cameraMovement(gd, dt);
    if (gd->window->keys['P']) {
        gd->window->keys['P'] = false;
        sf::Packet packet;
        packet << "Stop";
        toSend.emplace_back(packet);
    }

    if (gd->window->scrollOffset && ctype.ct == ClickEventType::MISS) {
        gd->camera.Scroll(gd->window->scrollOffset);
    }

    if (gd->window->mouseLClicked && !gd->window->mouseRClicked) {
        if (ctype.ct == ClickEventType::MISS) {
            if (!unitClick()) {
                int pid = provClick();
                if (pid != -1) {
                    if (openPeaceOfferId == -1) {
                        resetGuiWindows();
                        bool foundBattle = false;
                        for (std::size_t i = 0; i < gd->battles.size(); ++i) {
                            if (gd->battles[i].GetProvId() == pid) {
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
                        if ((gd->provinces[pid].GetCountryId() ==
                                 peaceOffers[peaceind].offeredBy ||
                             gd->provinces[pid].GetCountryId() ==
                                 peaceOffers[peaceind].recipant) &&
                            peaceOffers[peaceind].offeredBy == myCountryId)
                        {
                            auto tmpit = std::find_if(
                                peaceOffers[peaceind].lostProv.begin(),
                                peaceOffers[peaceind].lostProv.end(),
                                [id = pid](const std::tuple<int, int, int> &t) {
                                    return std::get<0>(t) == id;
                                });
                            if (tmpit != peaceOffers[peaceind].lostProv.end()) {
                                peaceOffers[peaceind].lostProv.erase(tmpit);
                            }
                            else if (gd->provinces[pid].GetCountryId() ==
                                     myCountryId) {
                                peaceOffers[peaceind].lostProv.push_back(
                                    std::make_tuple(
                                        pid, peaceOffers[peaceind].recipant,
                                        gd->provinces[pid].GetDevelopment()));
                            }
                            tmpit = std::find_if(
                                peaceOffers[peaceind].gainProv.begin(),
                                peaceOffers[peaceind].gainProv.end(),
                                [id = pid](const std::tuple<int, int, int> &t) {
                                    return std::get<0>(t) == id;
                                });
                            if (tmpit != peaceOffers[peaceind].gainProv.end()) {
                                peaceOffers[peaceind].gainProv.erase(tmpit);
                            }
                            else if (gd->provinces[pid].GetCountryId() !=
                                     myCountryId) {
                                peaceOffers[peaceind].gainProv.push_back(
                                    std::make_tuple(
                                        pid, peaceOffers[peaceind].offeredBy,
                                        gd->provinces[pid].GetDevelopment()));
                            }
                        }
                    }
                }
            }
        }
    }
    else if (gd->window->mouseRClicked) {
        if (ctype.ct == ClickEventType::MISS && openUnitId != -1) {
            int pid = provClick();
            if (pid != -1) {
                toSend.emplace_back(PreparePacket::MoveUnit(openUnitId, pid));
                Log("Try unit move: " << openUnitId << " => topid: " << pid);
            }
        }
    }
    gd->window->mouseRClicked = false;
}

int Game::provClick()
{
    int id = (float)clickedProvId(gd);
    if (id >= 0 && id < gd->provinces.size()) {
        markedProvId = (float)id;
        openedProvId = markedProvId;
        return id;
    }
    return -1;
}

bool Game::unitClick()
{
    GLuint err;
    {  // check for unit clicks
        glUseProgram(pickModelShader.GetProgram());
        glUniformMatrix4fv(
            glGetUniformLocation(pickModelShader.GetProgram(), "matrix"), 1,
            GL_FALSE, glm::value_ptr(gd->camera.GetMat()));
        glBindVertexArray(model3d.rectVao);
        glBindBuffer(GL_ARRAY_BUFFER, model3d.rectVbo);
        // for (std::size_t i = 0; i < uMat.size(); ++i) {
        int r = 0, g = 0, b = 0, a = 255;
        int perm = 2;
        for (std::size_t i = 0; i < unitsAtPoint.size(); ++i) {
            glUniformMatrix4fv(
                glGetUniformLocation(pickModelShader.GetProgram(), "ml"), 1,
                GL_FALSE, glm::value_ptr(uMat[i]));
            // glm::vec4 pcol{(float)gd->provinces[pids[i]].GetColor().r /
            // 255.0f,
            //                gd->provinces[pids[i]].GetColor().g / 255.0f,
            //                gd->provinces[pids[i]].GetColor().b /
            //                255.0f, 1.0f};
            glm::vec4 pcol{(float)r / 255.0f, (float)g / 255.0f,
                           (float)b / 255.0f, 1.0f};
            glUniform4fv(
                glGetUniformLocation(pickModelShader.GetProgram(), "col"), 1,
                glm::value_ptr(pcol));

            glDrawArrays(GL_TRIANGLES, 0, model3d.rectVertsCount);
            err = glGetError();
            if (err)
                Log(err);
            unitsAtPoint[i].perm = (int)r | ((int)g << 8) | ((int)b << 16);

            // perm
            if (perm == 2) {
                if (b < 255)
                    b += 1;
                else {
                    b = 0;
                    g = 0;
                    perm = 1;
                }
            }
            else if (perm == 1) {
                if (b < 254)
                    b += 1;
                else if (g < 255) {
                    g += 1;
                    b = 0;
                }
                else {
                    b = 0;
                    r = 0;
                    perm = 0;
                }
            }
            else if (perm == 0) {
                if (b < 254)
                    b += 1;
                else if (g < 255) {
                    g += 1;
                    b = 0;
                }
                else if (r < 255) {
                    r += 1;
                    g = 0;
                    b = 0;
                }
            }
        }
    }

    unsigned char pixel[4];
    int pixx = gd->window->xMouse,
        pixy = gd->window->GetSize().y - gd->window->yMouse;
    glReadPixels(pixx, pixy, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);
    gd->window->Refresh();
    // unsigned int clickedUnitPhash = getHash(pixel[0], pixel[1], pixel[2]);
    int perm = (int)pixel[0] | ((int)pixel[1] << 8) | ((int)pixel[2] << 16);

    // if (gd->colorToId.find(clickedUnitPhash) != gd->colorToId.end()) {
    for (std::size_t i = 0; i < unitsAtPoint.size(); ++i) {
        if (unitsAtPoint[i].perm != perm)
            continue;
        Log("Unit click");
        clickedUnits.clear();
        for (int j = 0; j < unitsAtPoint[i].uind.size(); ++j) {
            //    if (u.GetProvId() == provId)
            clickedUnits.push_back(gd->units[unitsAtPoint[i].uind[j]].GetId());
        }
        if (unitsAtPoint[i].uind.size() == 1) {
            resetGuiWindows();
            openUnitId = clickedUnits[0];
            Log(openUnitId);
            // openUnitId = unitsAtPoint[i].uind[0];
        }
        else if (unitsAtPoint[i].uind.size() > 1) {
            resetGuiWindows();
            openUnitsList = true;
            openUnitsListProvId = gd->units[unitsAtPoint[i].uind[0]].GetProvId();
        }
        // auto provId = gd->colorToId[clickedUnitPhash];
        // clickedUnits.clear();
        // for (auto &u : gd->units) {
        //     if (u.GetProvId() == provId)
        //         clickedUnits.push_back(u.GetId());
        // }
        // if (clickedUnits.size() == 1) {
        //     resetGuiWindows();
        //     openUnitId = clickedUnits[0];
        // }
        // else if (clickedUnits.size() > 1) {
        //     resetGuiWindows();
        //     openUnitsList = true;
        // }
        return true;
    }
    // else {
    //     return false;
    // }
    return false;
}

// void Game::unitMove(std::unordered_map<std::string, std::string> &values,
// glm::vec2 mouseInWorld)
//{
//  Color provinceColor = map.ClickOnProvince(mouseInWorld.x, mouseInWorld.y);
//  auto it = std::find_if(provinces.begin(), provinces.end(),
//  [provinceColor](std::unique_ptr<Province> &p) {
//      return p->GetColor() == provinceColor;
//  });
//  if (it != provinces.end()) {
//      auto idIt = values.find("id");
//      if (idIt == values.end())
//          return;
//      sf::Packet packet;
//      packet << "UnitMove";
//      packet << std::stoi(idIt->second);
//      packet << (*it)->GetId();

//    toSend.push_back(packet);
//}
//}

void Game::processGuiEvent() {}

void Game::sendPackets()
{
    for (auto &p : toSend) {
        socket.send(p);
    }
    toSend.clear();
}

void Game::updateBattles() {}

void labelText(const glm::vec3 &pos, const glm::vec2 &size,
               const glm::vec4 &col, const std::string &text,
               AM::FontSize fontSize, std::vector<Vertex> &labelVerts)
{
    glm::vec3 textPos = pos;
    GLuint tid = (GLuint)fontSize;
    float spaceWidth = 10.0f;
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
        glm::vec3 rPos;
        glm::vec2 rSize, rLBTC, tcLen;
        if (AM::atlasInfo[fontSize][text[i]].set) {
            rPos = {textPos.x + off,
                    textPos.y + AM::atlasInfo[fontSize][text[i]].yPos,
                    textPos.z};
            // this 2 if's are walkaround around bug. If we draw char at x or y
            // with floating point it gets more blury the closer it gets to .5f,
            // at .0f it looks fine
            if (rPos.x > (((float)((int)rPos.x))))
                rPos.x = ((int)(rPos.x + 0.5f));
            if (rPos.y > (((float)((int)rPos.y))))
                rPos.y = ((int)(rPos.y + 0.5f));
            rSize = {AM::atlasInfo[fontSize][text[i]].width,
                     AM::atlasInfo[fontSize][text[i]].height};
            rLBTC = {AM::atlasInfo[fontSize][text[i]].tcX,
                     AM::atlasInfo[fontSize][text[i]].tcY};
            tcLen = {AM::atlasInfo[fontSize][text[i]].tcWidth,
                     AM::atlasInfo[fontSize][text[i]].tcHeight};
            off += rSize.x + betweenWidth;

            labelVerts.push_back(Vertex{.pos = rPos,
                                        .color = col,
                                        .tc = glm::vec2{rLBTC.x, rLBTC.y},
                                        .textureId = (float)tid});
            labelVerts.push_back(
                Vertex{.pos = glm::vec3{rPos.x, rPos.y + rSize.y, rPos.z},
                       .color = col,
                       .tc = glm::vec2{rLBTC.x, rLBTC.y + tcLen.y},
                       .textureId = (float)tid});
            labelVerts.push_back(Vertex{
                .pos = glm::vec3{rPos.x + rSize.x, rPos.y + rSize.y, rPos.z},
                .color = col,
                .tc = glm::vec2{rLBTC.x + tcLen.x, rLBTC.y + tcLen.y},
                .textureId = (float)tid});
            labelVerts.push_back(labelVerts.back());
            labelVerts.push_back(
                Vertex{.pos = glm::vec3{rPos.x + rSize.x, rPos.y, rPos.z},
                       .color = col,
                       .tc = glm::vec2{rLBTC.x + tcLen.x, rLBTC.y},
                       .textureId = (float)tid});
            labelVerts.push_back(Vertex{.pos = rPos,
                                        .color = col,
                                        .tc = glm::vec2{rLBTC.x, rLBTC.y},
                                        .textureId = (float)tid});
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

void genRectFromVert(Vertex fv, float width, float height,
                     std::vector<Vertex> &verts)
{
    verts.push_back(fv);
    fv.pos.y += height;
    verts.push_back(fv);
    fv.pos.x += width;
    verts.push_back(fv);
    verts.push_back(fv);
    fv.pos.y -= height;
    verts.push_back(fv);
    fv.pos.x -= width;
    verts.push_back(fv);
}
