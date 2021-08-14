#include "new_map.h"

#include <set>
#include <fstream>
#include <map>
#include "asset_manager.h"


#include "graphics/texture.h"

void saveProvinceFromImg(const unsigned char * provs, const unsigned char * height, int w, int h);

void MapBatch::Init()
{
    int vertexSize = sizeof(Vertexx);
    int spriteSize = vertexSize * 4;
    int vertexDataSize = spriteSize * maxSprites;
    int indicesSize = maxSprites * 6;
    Log(vertexSize);
    // glGenVertexArrays(1, &vao);
    glCreateVertexArrays(1, &vao);
    glBindVertexArray(vao);
    // glGenBuffers(1, &vbo);
    glCreateBuffers(1, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertexDataSize, NULL, GL_DYNAMIC_DRAW);
    Log("vbo=" << vbo);
    // Vertex vx;
    // Log(&vx);
    // Log(&vx.color);
    // Log(&vx.textureId);
    // Log(&vx.tc);
    // Log(&vx.normal);
    glEnableVertexArrayAttrib(vao, 0);
    glEnableVertexArrayAttrib(vao, 1);
    glEnableVertexArrayAttrib(vao, 2);
    glEnableVertexArrayAttrib(vao, 3);
    glEnableVertexArrayAttrib(vao, 4);
    GLuint err = glGetError();
    if (err)
        Log("Opengl error: " << err);
    // glEnableVertexAttribArray(0);
    // glEnableVertexAttribArray(1);
    // glEnableVertexAttribArray(2);
    // glEnableVertexAttribArray(3);
    // glEnableVertexAttribArray(4);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexSize, NULL);  //(const GLvoid*)0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, vertexSize,
                          (const GLvoid*)(offsetof(Vertexx, Vertexx::color)));  //(const GLvoid*)(3 * GL_FLOAT));
    glVertexAttribPointer(
        2, 1, GL_FLOAT, GL_FALSE, vertexSize,
        (const GLvoid*)(offsetof(Vertexx, Vertexx::textureId)));  //(const GLvoid*)(7 * GL_FLOAT));
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, vertexSize,
                          (const GLvoid*)(offsetof(Vertexx, Vertexx::tc)));  //(const GLvoid*)(7 * GL_FLOAT));
    glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, vertexSize,
                          (const GLvoid*)(offsetof(Vertexx, Vertexx::normal)));  //(const GLvoid*)(7 * GL_FLOAT));

    err = glGetError();
    if (err)
        Log("Opengl error: " << err);
    // glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLushort* indices = new GLushort[indicesSize];
    int offset = 0;
    for (int i = 0; i < indicesSize; i += 6) {
        indices[i] = offset + 0;
        indices[i + 1] = offset + 1;
        indices[i + 2] = offset + 2;
        indices[i + 3] = offset + 2;
        indices[i + 4] = offset + 3;
        indices[i + 5] = offset + 0;
        offset += 4;
    }

    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize * sizeof(GLushort), indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    err = glGetError();
    if (err)
        Log("Opengl error: " << err);
    delete[] indices;

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
}

void MapBatch::Begin()
{
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    vertexData = (Vertexx*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
}

void MapBatch::Push(const Vertexx* verts)  // takes array of 4
{
    if (vertCount + 4 >= maxSprites) {
        // if (spriteCount >= maxSprites) {
        Flush();
        Begin();
    }

    spriteCount++;
    vertCount += 4;
    indicesCount += 6;
    for (int i = 0; i < 4; ++i, vertexData++) {
        *vertexData = verts[i];
        vertexData->pos = verts[i].pos;
        vertexData->color = verts[i].color;
        vertexData->tc = verts[i].tc;
        vertexData->textureId = verts[i].textureId;
        vertexData->normal = verts[i].normal;
    }
}
float tFlush = 0.0f;
void MapBatch::Flush()
{
    float tt2 = glfwGetTime();
    glDrawElements(GL_PATCHES, indicesCount, GL_UNSIGNED_SHORT, NULL);
    // glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_SHORT, NULL);
    // Log(vertCount);
    vertCount = 0;
    spriteCount = 0;
    indicesCount = 0;
    glUnmapBuffer(GL_ARRAY_BUFFER);
    tFlush += (glfwGetTime() - tt2);
}

struct TreeModel {
    GLuint vao, vbo;

    TreeModel()
    {
        glm::vec4 brown{43.0 / 255.0, 20.0 / 255.0, 20.0 / 255.0, 1.0};
        glm::vec4 green{10.0 / 255.0, 127.0 / 255.0, 18.0 / 255.0, 1.0};
        glm::vec4 greener{18.0 / 255.0, 74.0 / 255.0, 9.0 / 255.0, 1.0};
        //auto vertInit = {
        float vertInit[] = {
                            0.0f, 0.0f, 0.0f, //front
                            brown.x, brown.y, brown.z, brown.w,
                            1.0f, 0.0f, 0.0f,
                            brown.x, brown.y, brown.z, brown.w,
                            1.0f, 0.0f, 1.0f,
                            brown.x, brown.y, brown.z, brown.w,

                            0.0f, 0.0f, 0.0f, 
                            brown.x, brown.y, brown.z, brown.w,
                            0.0f, 0.0f, 1.0f,
                            brown.x, brown.y, brown.z, brown.w,
                            1.0f, 0.0f, 1.0f,
                            brown.x, brown.y, brown.z, brown.w,
                            

                            0.0f, 1.0f, 0.0f, //back
                            brown.x, brown.y, brown.z, brown.w,
                            1.0f, 1.0f, 0.0f,
                            brown.x, brown.y, brown.z, brown.w,
                            1.0f, 1.0f, 1.0f,
                            brown.x, brown.y, brown.z, brown.w,

                            0.0f, 1.0f, 0.0f,
                            brown.x, brown.y, brown.z, brown.w,
                            0.0f, 1.0f, 1.0f,
                            brown.x, brown.y, brown.z, brown.w,
                            1.0f, 1.0f, 1.0f,
                            brown.x, brown.y, brown.z, brown.w,
                            

                            0.0f, 0.0f, 0.0f, //left
                            brown.x, brown.y, brown.z, brown.w,
                            0.0f, 1.0f, 0.0f,
                            brown.x, brown.y, brown.z, brown.w,
                            0.0f, 1.0f, 1.0f,
                            brown.x, brown.y, brown.z, brown.w,
                            
                            0.0f, 0.0f, 0.0f,
                            brown.x, brown.y, brown.z, brown.w,
                            0.0f, 0.0f, 1.0f,
                            brown.x, brown.y, brown.z, brown.w,
                            0.0f, 1.0f, 1.0f,
                            brown.x, brown.y, brown.z, brown.w,
                            

                            1.0f, 0.0f, 0.0f, //right
                            brown.x, brown.y, brown.z, brown.w,
                            1.0f, 1.0f, 0.0f,
                            brown.x, brown.y, brown.z, brown.w,
                            1.0f, 1.0f, 1.0f,
                            brown.x, brown.y, brown.z, brown.w,
                            
                            1.0f, 0.0f, 0.0f,
                            brown.x, brown.y, brown.z, brown.w,
                            1.0f, 0.0f, 1.0f,
                            brown.x, brown.y, brown.z, brown.w,
                            1.0f, 1.0f, 1.0f,
                            brown.x, brown.y, brown.z, brown.w,


                            -0.5f, 0.5f, 1.0f, //vertical 1
                            green.x, green.y, green.z, green.w,
                            1.5f, 0.5f, 1.0f,
                            green.x, green.y, green.z, green.w,
                            0.5f, 0.5f, 2.0f,
                            green.x, green.y, green.z, green.w,
                            
                            0.5f, -0.5f, 1.0f, //vertical 2
                            greener.x, greener.y, greener.z, greener.w,
                            0.5f, 1.5f, 1.0f,
                            greener.x, greener.y, greener.z, greener.w,
                            0.5f, 0.5f, 2.0f,
                            greener.x, greener.y, greener.z, greener.w,
        };
        
        glCreateVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        glBufferData(GL_ARRAY_BUFFER, 5 * 2 * 3 * 7 * sizeof(float), vertInit, GL_STATIC_DRAW);
        //glBufferData(GL_ARRAY_BUFFER, 12 * 3 * 7 * sizeof(float), vertInit, GL_STATIC_DRAW);
        //glBufferData(GL_ARRAY_BUFFER, 21 * 10 * sizeof(float), vertInit, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 7, 0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 7, (void*)(3 * sizeof(float)));
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
};


struct ProvData {
    std::string name;
    int r, g, b;
    int id;
    bool water;
    float x, y;
};
unsigned int getHash(unsigned char r, unsigned char g, unsigned char b) {
    unsigned int res = (unsigned int)r;
    res |= ((unsigned int)g << 8);
    res |= ((unsigned int)b << 16);
    return res;
}

std::vector<ProvData> provinces;
std::map<unsigned int, int> colorToId;

void loadProvData()
{
    std::fstream f;
    f.open("ProvDataTest.txt", std::fstream::in);
    std::string w;
    while (f >> w) {
        if (w == "{") {
            ProvData pd;
            while (f >> w) {
                if (w == "}") {
                    provinces.push_back(pd);
                    colorToId[getHash(pd.r, pd.g, pd.b)] = pd.id;
                    break;
                }
                else if (w == "id:") {
                    f >> w;
                    pd.id = std::atoi(w.c_str());
                }
                else if (w == "name:") {
                    f >> pd.name;
                }
                else if (w == "pos:") {
                    f >> pd.x;
                    f >> pd.y;
                }
                else if (w == "water:") {
                    f >> pd.water;
                }
                else if (w == "color:") {
                    f >> w;
                    pd.r = std::atoi(w.c_str());
                    f >> w;
                    pd.g = std::atoi(w.c_str());
                    f >> w;
                    pd.b = std::atoi(w.c_str());
                }
                else {
                    Log("cos tu nie gra");
                }
            }
        }
        else {
            Log("cos tu nie gra");
            break;
        }
    }
    f.close();
    std::sort(provinces.begin(), provinces.end(), [](ProvData a, ProvData b){ return a.id < b.id; });
    Log(provinces[2793].r << ", "<<provinces[2793].r << ", "<<provinces[2793].r << ", ");
}

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
    int terrainMapWidth = 5632, terrainMapHeight = 2048;
    Texture terrainTexture{"src/img/terrain_map.png", terrainMapWidth, terrainMapHeight};
    Texture waterMap{"src/img/Blank_map.png", terrainMapWidth, terrainMapHeight};
    Texture heightMap{"/home/lukasz/Pobrane/Heightmap.png", mapWidth, mapHeight};
    Texture provTexture{"/home/lukasz/Pobrane/Provinces_org.png", mapWidth, mapHeight};
    //int terrainMapWidth = 1279, terrainMapHeight = 463;
    Texture grassT{"../shared/grass1.png", 64, 64, GL_REPEAT};
    Texture stoneT{"../shared/smoothstone.png", 64, 64, GL_REPEAT};
    Texture waterT{"../shared/water1.png", 64, 64, GL_REPEAT};
    Texture sandT{"src/img/Sand_1.png", 32, 32, GL_REPEAT};


    //saveProvinceFromImg(provTexture.GetPixels(), waterMap.GetPixels(), mapWidth, mapHeight);
    loadProvData(); 

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
    Log("sand: " << texID[6]);
    //Log("Prov kurtyzana " << texID[3]);
    glUniform1iv(glGetUniformLocation(shader.GetProgram(), "tex"), 32, texID);
    err = glGetError();
    if (err)
        Log("Opengl error: " << err);

    for (int i = 0; i < 8; ++i) {
        // glActiveTexture((GLuint)texID[i]);

        //glActiveTexture(GL_TEXTURE0 + i);
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

    // GLuint textures[] = {(GLuint)texID[0], (GLuint)texID[1], (GLuint)texID[2], (GLuint)texID[3]};
    // glBindTextures(textures[0], 4, textures);

    // GLuint ts[] = {0, provTexture.GetId(), heightMap.GetId(), grassT.GetId(), stoneT.GetId(),
    //              stoneT.GetId(), provTexture.GetId(), provTexture.GetId() };

    // glBindTextures(ts[0], 8, ts);

    const unsigned char* pix = provTexture.GetPixels();
    const unsigned char* h = heightMap.GetPixels();

    // initBorders(pix, h, mapWidth, mapHeight);

    Log("tu1");
    std::vector<Vertexx> vertexes;
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

            vertexes.push_back(Vertexx{.pos = Vec3{x, y, z},
                                       .color = color,
                                       .textureId = tid,
                                       .tc = tc1});  //, .normal=Vec2{0.0f, 0.0f} });
            vertexes.push_back(Vertexx{.pos = Vec3{x, y + ww, z},
                                       .color = color,
                                       .textureId = tid,
                                       .tc = tc2});  //, .normal=Vec2{0.0f, 0.0f} });
            vertexes.push_back(Vertexx{.pos = Vec3{x + ww, y + ww, z},
                                       .color = color,
                                       .textureId = tid,
                                       .tc = tc3});  //, .normal=Vec2{0.0f, 0.0f} });
            vertexes.push_back(Vertexx{.pos = Vec3{x + ww, y, z},
                                       .color = color,
                                       .textureId = tid,
                                       .tc = tc4});  //, .normal=Vec2{0.0f, 0.0f} });
            x += ww;
            texC.x += tCL.x;
        }
        x = 0;
        y += ww;
        texC.x = 0.0f;
        texC.y += tCL.y;
    }
    Log("tu");

    // Vec2 tc{66.0f, 666.0f};
    // Vec2 normal{56.0f, 566.0f};
    tid = 5.0f;
    // float x = 100.0f, y = 100.0f, z = 0.0f, ww = 10.0f;
    // Log(vertexes.back().pos.x << " " << vertexes.back().pos.y << " " <<vertexes.back().pos.z << " ");
    color = {10.0f, 20.0f, 30.0f, 1.0f};
    int maxSprites = vertexes.size() / 4;
    int vertexSize = sizeof(Vertexx);
    int spriteSize = vertexSize * 4;
    int vertexDataSize = spriteSize * maxSprites;
    int indicesSize = maxSprites * 6;

    Log(vertexes.size());
    GLuint vao, vbo, ibo;
    // glGenVertexArrays(1, &vao);
    glCreateVertexArrays(1, &vao);
    glBindVertexArray(vao);
    // glGenBuffers(1, &vbo);
    glCreateBuffers(1, &vbo);
    err = glGetError();
    if (err)
        Log("Opengl error: " << err);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertexDataSize, vertexes.data(), GL_STATIC_DRAW);
    Log("vbo=" << vbo);
    glEnableVertexArrayAttrib(vao, 0);
    Log("tu");
    glEnableVertexArrayAttrib(vao, 1);
    glEnableVertexArrayAttrib(vao, 2);
    glEnableVertexArrayAttrib(vao, 3);
    glEnableVertexArrayAttrib(vao, 4);
    err = glGetError();
    if (err)
        Log("Opengl error: " << err);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexSize, NULL);  //(const GLvoid*)0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, vertexSize,
                          (const GLvoid*)(offsetof(Vertexx, Vertexx::color)));  //(const GLvoid*)(3 * GL_FLOAT));
    glVertexAttribPointer(
        2, 1, GL_FLOAT, GL_FALSE, vertexSize,
        (const GLvoid*)(offsetof(Vertexx, Vertexx::textureId)));  //(const GLvoid*)(7 * GL_FLOAT));
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, vertexSize,
                          (const GLvoid*)(offsetof(Vertexx, Vertexx::tc)));  //(const GLvoid*)(7 * GL_FLOAT));
    glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, vertexSize,
                          (const GLvoid*)(offsetof(Vertexx, Vertexx::normal)));  //(const GLvoid*)(7 * GL_FLOAT));

    err = glGetError();
    if (err)
        Log("Opengl error: " << err);
    // glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLushort* indices = new GLushort[indicesSize];
    int offset = 0;
    for (int i = 0; i < indicesSize; i += 6) {
        indices[i] = offset + 0;
        indices[i + 1] = offset + 1;
        indices[i + 2] = offset + 2;
        indices[i + 3] = offset + 2;
        indices[i + 4] = offset + 3;
        indices[i + 5] = offset + 0;
        offset += 4;
    }

    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize * sizeof(GLushort), indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    err = glGetError();
    if (err)
        Log("Opengl error: " << err);
        // delete[] indices;

#define batch_rend 1

#if batch_rend
    MapBatch batch;
    batch.Init();
#endif
    err = glGetError();
    if (err)
        Log("Opengl error: " << err);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


    Shader treeShader{"src/graphics/shaders/tree/vert.v", "src/graphics/shaders/tree/frag.f", "", ""};

    
    //glm::vec3 treePos = glm::vec3(mapWidth * 1.0 * 2.0, mapHeight * 1.4f * 2.0f, 0.0f);
    TreeModel treeModel;
    glBindVertexArray(treeModel.vao);
    
    //glm::vec3 treePos = glm::vec3(-10.0f, 0.0f, 0.0f);
    float treePos[] = { 
            -10.0f, 0.0f, 10.0f,
            -5.0f, 0.0f, 10.0f,
            -2.0f, 0.0f, 10.0f
    };
    /*
    int amount = 300;
    glm::mat4 * tMat = new glm::mat4[amount];
    float tx = 0.0f, ty = 0.0f;
    for (int i = 0; i < amount; ++i) {    
        auto treeMl = glm::mat4(1.0);
        treeMl = glm::translate(treeMl, glm::vec3{0.0 - tx, 0.0 + ty, 80.0});
        treeMl = glm::scale(treeMl, glm::vec3{10.0, 10.0, 10.0});
        tMat[i] = treeMl;

        tx += 15.0f;
        if (i % 20 == 0) {
            tx = 0;
            ty += 15.0f;
        }
    }
    */
    std::vector<glm::mat4> tMat;
    auto terrainPix = terrainTexture.GetPixels();
    auto heightPix = heightMap.GetPixels();
    for (int y = 0; y < terrainMapHeight; y += 2) {
        for (int x = 0; x < terrainMapWidth; ++x) {
            int index = y * terrainMapWidth * 4 + x * 4;
            int hIndex = y * terrainMapWidth * 3 + x * 3;
            //if (terrainPix[index] == 16 && terrainPix[index + 1] == 60 && terrainPix[index + 2] == 9) {
            //if ((terrainPix[index] >= 14 && terrainPix[index + 1] >= 58 && terrainPix[index + 2] >= 6) &&
            //    (terrainPix[index] <= 18 && terrainPix[index + 1] <= 62 && terrainPix[index + 2] <= 11)) {
            if (terrainPix[index] == 41 && terrainPix[index + 1] == 155 && terrainPix[index + 2] == 22 && sin(x * y) > 0.7 ) {
                auto treeMl = glm::mat4(1.0);
                treeMl = glm::translate(treeMl, glm::vec3{(double)x * scale, (double)y * scale, 1.0 * heightPix[hIndex]});
                //treeMl = glm::translate(treeMl, glm::vec3{ 0.0 + x * ((double)mapWidth / terrainMapWidth) * scale, 0.0 + y * ((double)mapHeight / terrainMapHeight) * scale, 130.0});
                treeMl = glm::scale(treeMl, glm::vec3{5.0, 5.0, 10.0});
                tMat.push_back(treeMl);
                //Log((int)heightPix[index]);
            }
            else if (terrainPix[index] == 18 && terrainPix[index + 1] == 74 && terrainPix[index + 2] == 9 && sin(x * y) > 0.5) {
                auto treeMl = glm::mat4(1.0);
                treeMl = glm::translate(treeMl, glm::vec3{(double)x * scale, (double)y * scale, 1.0 * heightPix[hIndex]});
                treeMl = glm::scale(treeMl, glm::vec3{5.0, 5.0, 10.0});
                tMat.push_back(treeMl);
            }
        }
    }
    int amount = tMat.size();
    Log("trees amount = " << amount);
    GLuint treePosBuffer;
    glGenBuffers(1, &treePosBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, treePosBuffer);
    //glBufferData(GL_ARRAY_BUFFER, amount * 3 * sizeof(float), treePos, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, amount * 1 * sizeof(glm::mat4), tMat.data(), GL_STATIC_DRAW);
   


    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));
    glVertexAttribDivisor(2, 1);
    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);
    glBindVertexArray(0);
float tesLevel  = 32.0f;
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
                  "src/graphics/shaders/tes_new_map/tes_ster.ts", "src/graphics/shaders/tes_new_map/tes_w.tw");
            borderShader = Shader{"src/graphics/shaders/borders/vert.v", "src/graphics/shaders/borders/frag.f", "", ""};
            waterShader = Shader("src/graphics/shaders/water/vert.v", "src/graphics/shaders/water/frag.f",
                       "src/graphics/shaders/water/tes_ster.ts", "src/graphics/shaders/water/tes_w.tw");
            colorMapShader = Shader("src/graphics/shaders/map_pick/vert.v", "src/graphics/shaders/map_pick/frag.f",
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
            //Log(tesLevel);
            tesLevel = 32.0f;
            glUseProgram(shader.GetProgram());
            glUniform1f(glGetUniformLocation(shader.GetProgram(), "level"), tesLevel);
        }
        glm::vec3 provColor;
        // window.keys['L'] = 0;
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

            unsigned char pixel[4];
            int pixx = window.xMouse, pixy = windowSize.y - window.yMouse;
            //Log(pixx << " - " << pixy);
            glReadPixels(pixx, pixy, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);

            window.Refresh();
            // glUseProgram(shader.GetProgram());
            provColor = {(int)pixel[0] / 255.0, (int)pixel[1] / 255.0, (int)pixel[2] / 255.0};
            unsigned int phash = getHash(pixel[0], pixel[1], pixel[2]);
            if (colorToId.find(phash) != colorToId.end()) {
                int pid = colorToId[phash];
                Log(provinces[pid].id << ", water: " << provinces[pid].water << ", " << provinces[pid].name << ", col: " << provinces[pid].r << ", " << provinces[pid].g << ", " << provinces[pid].b); 
                unitPos.x = provinces[pid].x * scale;
                unitPos.y = provinces[pid].y * scale;
                unitPos.z = heightMap.GetPixels()[(int)(provinces[pid].x * 3 + provinces[pid].y * mapWidth * 3)];
                //unitPos.z = 120.0f;
            }
            std::cout << "R: " << (double)pixel[0] << "< ";
            std::cout << "G: " << (int)pixel[0] << "< ";
             std::cout << "B: " << (int)pixel[2] << "< \n";
            // std::cout << "R: " << provColor.x << "< ";
            // std::cout << "G: " << provColor.y << "< ";
            // std::cout << "B: " << provColor.z << "< \n";
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

#if batch_rend
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
#else
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glDrawElements(GL_PATCHES, indicesSize, GL_UNSIGNED_SHORT, NULL);
#endif

        glUseProgram(waterShader.GetProgram());
    glUniform1f(glGetUniformLocation(waterShader.GetProgram(), "level"), 32);
    glUniform1iv(glGetUniformLocation(waterShader.GetProgram(), "tex"), 32, texID);
        glUniformMatrix4fv(glGetUniformLocation(waterShader.GetProgram(), "pr_matrix"), 1, GL_FALSE,
                           glm::value_ptr(matrix));

        glUniformMatrix4fv(glGetUniformLocation(waterShader.GetProgram(), "matrix"), 1, GL_FALSE,
                           glm::value_ptr(matrix));
        glUniform1f(glGetUniformLocation(waterShader.GetProgram(), "waterTime"), waterTime);

        batch.Begin();
        Vertexx vertsWater[8];
        vertsWater[0] = Vertexx{
            .pos = Vec3{0.0, 0.0, 0.0}, .color = Vec4{0.0, 0.0, 1.0, 1.0}, .textureId = 0, .tc = Vec2{0.0, 0.0}};
        vertsWater[1] = Vertexx{.pos = Vec3{mapWidth * scale * 0.5, 0.0, 0.0},
                                .color = Vec4{0.0, 0.0, 1.0, 1.0},
                                .textureId = 0,
                                .tc = Vec2{0.5, 0.0}};
        vertsWater[2] = Vertexx{.pos = Vec3{mapWidth * scale * 0.5, mapHeight * scale, 0.0},
                                .color = Vec4{0.0, 0.0, 1.0, 1.0},
                                .textureId = 0,
                                .tc = Vec2{.5, 1.0}};
        vertsWater[3] = Vertexx{.pos = Vec3{0.0, mapHeight * scale, 0.0},
                                .color = Vec4{0.0, 0.0, 1.0, 1.0},
                                .textureId = 0,
                                .tc = Vec2{0.0, 1.0}};

        vertsWater[4] = Vertexx{.pos = Vec3{mapWidth * scale * 0.5, 0.0, 0.0},
                                .color = Vec4{0.0, 0.0, 1.0, 1.0},
                                .textureId = 0,
                                .tc = Vec2{0.5, 0.0}};
        vertsWater[5] = Vertexx{.pos = Vec3{mapWidth * scale, 0.0, 0.0},
                                .color = Vec4{0.0, 0.0, 1.0, 1.0},
                                .textureId = 0,
                                .tc = Vec2{1.0, 0.0}};
        vertsWater[6] = Vertexx{.pos = Vec3{mapWidth * scale, mapHeight * scale, 0.0},
                                .color = Vec4{0.0, 0.0, 1.0, 1.0},
                                .textureId = 0,
                                .tc = Vec2{1.0, 1.0}};
        vertsWater[7] = Vertexx{.pos = Vec3{mapWidth * scale * 0.5, mapHeight * scale, 0.0},
                                .color = Vec4{0.0, 0.0, 1.0, 1.0},
                                .textureId = 0,
                                .tc = Vec2{0.5, 1.0}};

        batch.Push(vertsWater);
        batch.Push(&vertsWater[4]);
        batch.Flush();

{

//glm::mat4 model = glm::mat4(1.0);
//    unitPos = glm::vec3(mapWidth * 1.0 * 2.0, mapHeight * 1.4f * 2.0f, 100.0f);
glm::mat4 unitModel = glm::mat4(1.0);
unitModel = glm::translate(unitModel, unitPos);

float rotateX = 60.0f*3.1459265f/180.0f, yScale = 10.0f;
glm::mat4 rotate = glm::mat4{1.0f};
rotate = glm::rotate(glm::mat4{1.0}, rotateX, glm::vec3{1.0, 0.0, 0.0}); 
unitModel = unitModel * rotate;
unitModel = glm::scale(unitModel, glm::vec3{20.0, yScale, 20.0});


glUseProgram(AM::am.shader->GetProgram());
glUniformMatrix4fv(glGetUniformLocation(AM::am.shader->GetProgram(), "matrix"), 1, GL_FALSE, glm::value_ptr(matrix));
glUniformMatrix4fv(glGetUniformLocation(AM::am.shader->GetProgram(), "ml"), 1, GL_FALSE, glm::value_ptr(unitModel));
glUniform1iv(glGetUniformLocation(AM::am.shader->GetProgram(), "tex"), 32, texID);

//AM::am.model->DrawRect(model);
AM::am.model->Draw();

}

        // tree
        glUseProgram(treeShader.GetProgram());
    
        //glUniform1iv(glGetUniformLocation(waterShader.GetProgram(), "tex"), 32, texID);

        glUniformMatrix4fv(glGetUniformLocation(treeShader.GetProgram(), "matrix"), 1, GL_FALSE,
                           glm::value_ptr(matrix));
//        glUniformMatrix4fv(glGetUniformLocation(treeShader.GetProgram(), "ml"), 1, GL_FALSE,
//                           glm::value_ptr(treeMl));
        glBindVertexArray(treeModel.vao);
    //glBindBuffer(GL_ARRAY_BUFFER, treeModel.vbo);
    //glBindBuffer(GL_ARRAY_BUFFER, treePosBuffer);
        //glDrawElementsInstanced(GL_TRIANGLES, 0, GL_UNSIGNED_INT, 0, amount);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 3 * 2 * 5, amount);
        //glDrawArraysInstanced(GL_TRIANGLES, 0, 3 * 2 * 12, amount);

        // ~treee





        // int indicesCount = 6;
        // glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_SHORT, NULL);
        // glDrawElements(GL_TRIANGLES, vertexes.size() * 6, GL_UNSIGNED_SHORT, indices);
        // glDrawElements(GL_TRIANGLES, indicesSize, GL_UNSIGNED_SHORT, indices);
        float tt2 = glfwGetTime();
        // glDrawElements(GL_PATCHES, indicesSize, GL_UNSIGNED_SHORT, indices);

        /*
                glBindVertexArray(borderDraw.vao);
                glBindBuffer(GL_ARRAY_BUFFER, borderDraw.vbo);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, borderDraw.ibo);
                glUseProgram(borderShader.GetProgram());
                glUniformMatrix4fv(glGetUniformLocation(borderShader.GetProgram(), "matrix"), 1, GL_FALSE,
                                   glm::value_ptr(matrix));
                glDrawElements(GL_TRIANGLES, borderDraw.vertexes.size() * 6, GL_UNSIGNED_SHORT, NULL);
        */
        // GLenum err = glGetError();
        // if (err)
        //    Log("Opengl error: " << err);

        // Log(glfwGetTime() - tt2);
        //        batch.Flush();
        tFlush = 0.0f;

        window.Update();
        // Log("flush " << glfwGetTime() - tt2);
        // Log(glfwGetTime() - tt2);
        waterTime += dt;
        dt = glfwGetTime() - time;
        //Log(dt);
        time = glfwGetTime();
    }
}


void saveProvinceFromImg(const unsigned char * provs, const unsigned char * water, int w, int h)
{
    std::fstream f;
    f.open("ProvDataTest.txt", std::fstream::out);
    if (f.is_open())
    Log("open");
    std::map<unsigned int, ProvData> pmap;
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            int windex = x * 3 + (w * y) * 3;
            int index = x * 3 + (w * y) * 3;
            if (water[windex + 0] == 0 && water[windex + 1] == 0 && water[windex + 2] == 0)
                continue;
            unsigned int hash = getHash(provs[index + 0], provs[index + 1], provs[index + 2]); 
            if (pmap.find(hash) == pmap.end()) {
                ProvData pd;
                pd.id = pmap.size();
                pd.name = "Province" + std::to_string(pmap.size());
                pd.r = provs[index + 0];
                pd.g = provs[index + 1];
                pd.b = provs[index + 2];
                pd.x = x;
                pd.y = y;
                ///if (x == 2787 && y == 2048 - 647) {
                if (water[windex + 0] == 68)
                    pd.water = true;
                else
                    pd.water = false;
                if (pd.r == 152 && pd.g == 118 && pd.b == 64) {
                Log("!!!!!!id:" << pd.id << " -> " << (int)water[windex + 0] << " -> provColR: " << (int)provs[index + 0] << ", " << x << ", " << y << ", water: " << pd.water);
                }
                pmap[hash] = pd;

            }
            else { // sprawdz czy na pewno jest woda
                //if (water[index + 0] == 255)
                //    pmap[hash].water = false;
                //else if (water[index + 0] == 68)
                //    pmap[hash].water = true;
            }
        }
    }
    for (auto & pd: pmap) {
                f << "{\n";
                f << "id: " << pd.second.id << "\n";
                f << "name: " << pd.second.name << '\n';
                f << "pos: " << pd.second.x << " " << pd.second.y << '\n';
                f << "water: " << pd.second.water << '\n';
                f << "color: ";
                f << std::to_string(pd.second.r) << " ";
                f << std::to_string(pd.second.g) << " ";
                f << std::to_string(pd.second.b) << "\n}\n";
    }
Log("pmap size = " << pmap.size());
    f.close();
}

