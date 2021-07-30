#include "new_map.h"

#include <set>

#include "graphics/texture.h"

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

void newMapTest(Window& window, glm::vec2 resolution, glm::vec2 windowSize)
{
    Shader shader("src/graphics/shaders/map_batch/a.vert", "src/graphics/shaders/map_batch/a.frag", "", "");
    Camera camera{window.GetSize()};
    glUseProgram(shader.GetProgram());

    Texture grassT{"../shared/grass1.png", 64, 64, GL_REPEAT};
    Texture stoneT{"../shared/smoothstone.png", 64, 64, GL_REPEAT};
    int texID[32];
    for (int i = 0; i < 32; i++) texID[i] = i;
    texID[0] = grassT.GetId();
    texID[1] = stoneT.GetId();
    glUniform1iv(glGetUniformLocation(shader.GetProgram(), "tex"), 32, texID);
    GLuint textures[] = {(GLuint)texID[0], (GLuint)texID[1]};
    glBindTextures(textures[0], 2, textures);

    // int mapWidth = 1920, mapHeight = 1088;
    int mapWidth = 5632, mapHeight = 2048;
    // Texture heightMap{"../shared/map_height.png", mapWidth, mapHeight};
    Texture heightMap{"/home/lukasz/Pobrane/Heightmap.png", mapWidth, mapHeight};
    // Texture provTexture{"../shared/map.png", mapWidth, mapHeight};
    Texture provTexture{"/home/lukasz/Pobrane/Provinces.png", mapWidth, mapHeight};
    const unsigned char* pix = provTexture.GetPixels();
    const unsigned char* h = heightMap.GetPixels();

    unsigned char* va = new unsigned char[mapWidth * mapHeight];

    std::vector<Vertexx> vertexes;
    Vec4 color{24.0f, 1.0f, 0.0f, 1.0f};
    float tid = 0.0f;

    for (int x = 0; x < mapWidth; ++x) {
        for (int y = 0; y < mapHeight; ++y) {
            if (pix[(x + y * mapWidth) * 4 + 0] == 255 && pix[(x + y * mapWidth) * 4 + 1] == 255 &&
                pix[(x + y * mapWidth) * 4 + 2] == 255)
            {
                va[x + y * mapWidth] = 0x00;
                continue;
            }
            // Log(x);
            // float z = (float)h[(x + y * mapWidth)*4 + 0] / 255.0f;
            va[x + y * mapWidth] = h[(x + y * mapWidth) * 4 + 0];
            // Log(z);
            // z = 0.0f;
            // vertexes.push_back(Vertex{Vec3{(float)x * 1.0f,        (float)y,        z}, color, tc1, tid});
            // vertexes.push_back(Vertex{Vec3{(float)x * 1.0f,        ((float)y + 1.0f)*1.0f, z}, color, tc2,
            // tid}); vertexes.push_back(Vertex{Vec3{((float)x + 1.0f) * 1.0f, ((float)y + 1.0f)*1.0f, z}, color,
            // tc3, tid}); vertexes.push_back(Vertex{Vec3{((float)x + 1.0f) * 1.0f, (float)y,        z}, color,
            // tc4, tid});
        }
    }
    int cacheTab1[mapWidth], cacheTab2[mapWidth];
    int* currLine = cacheTab1;
    int* lastLine = cacheTab2;
    float scale = 4.0f;
    float w = 1.0f * scale;
    float zScale = 0.5f;  // 0.25f;
    std::vector<Vec4> clrs;
    for (int y = 0; y < mapHeight; ++y) {
        if (y % 100 == 0)
            Log(y);
        for (int x = 0; x < mapWidth; ++x) {
            if (va[x + y * mapWidth] == 0) {
                currLine[x] = -1;
                continue;
            }
            color.x = (float)pix[(x + y * mapWidth) * 4 + 0] / 255.0f;
            color.y = (float)pix[(x + y * mapWidth) * 4 + 1] / 255.0f;
            color.z = (float)pix[(x + y * mapWidth) * 4 + 2] / 255.0f;
            color.w = (float)pix[(x + y * mapWidth) * 4 + 3] / 255.0f;
            bool b = true;
            for (int i = 0; i < clrs.size(); ++i) {
                if (clrs[i] == color) {
                    b = false;
                    break;
                }
            }
            if (b)
                clrs.push_back(color);
            // if (color.x >=150)// && color.y>=240 && color.z >= 240)
            //       Log( color.x << " " << color.y );
            Vec2 tc1{(x + 0) % 10, (y + 0) % 10};
            Vec2 tc2{(x + 1) % 10, (y + 1) % 10};
            Vec2 tc3{(x + 2) % 10, (y + 2) % 10};
            Vec2 tc4{(x + 3) % 10, (y + 3) % 10};
            float z = (float)va[x + y * mapWidth] * zScale;  /// 4.0f;
            currLine[x] = vertexes.size();
            vertexes.push_back(Vertexx{.pos = Vec3{(float)x * scale, (float)y * scale, z},
                                       .color = color,
                                       .textureId = tid,
                                       .tc = tc1});  //, .normal=Vec2{0.0f, 0.0f} });
            vertexes.push_back(Vertexx{.pos = Vec3{(float)x * scale, ((float)y * scale + w), z},
                                       .color = color,
                                       .textureId = tid,
                                       .tc = tc2});  //, .normal=Vec2{0.0f, 0.0f} });
            vertexes.push_back(Vertexx{.pos = Vec3{((float)x * scale + w), ((float)y * scale + w), z},
                                       .color = color,
                                       .textureId = tid,
                                       .tc = tc3});  //, .normal=Vec2{0.0f, 0.0f} });
            vertexes.push_back(Vertexx{.pos = Vec3{((float)x * scale + w), (float)y * scale, z},
                                       .color = color,
                                       .textureId = tid,
                                       .tc = tc4});  //, .normal=Vec2{0.0f, 0.0f} });
        }
        // przejscie po dolnych
        if (y - 1 >= 0) {
            // pierwszy od prawej ma lewy wierzcholek dolnego tylko dla siebie stad odzielnosc
            if (currLine[0] >= 0 && lastLine[0] >= 0) {
                float z = vertexes[currLine[0] + 0].pos.z;
                vertexes[lastLine[0] + 1].pos.z = z;
                // vertexes[lastTab[xx] + 1].pos.z = z;
            }

            for (int xx = 0; xx < mapWidth; ++xx) {
                // int yy = y - 1;
                if (currLine[xx] < 0)
                    continue;
                if (lastLine[xx] < 0)
                    continue;

                int vertCount = 0;
                float z = 0.0f;

                vertCount++;
                z += vertexes[currLine[xx] + 3].pos.z;

                if (lastLine[xx] >= 0) {
                    vertCount++;
                    z += vertexes[lastLine[xx] + 2].pos.z;
                }
                if (xx + 1 < mapWidth) {
                    if (lastLine[xx + 1] >= 0) {
                        vertCount++;
                        z += vertexes[lastLine[xx + 1] + 1].pos.z;
                    }
                    if (currLine[xx + 1] >= 0) {
                        vertCount++;
                        z += vertexes[currLine[xx + 1] + 0].pos.z;
                    }
                }

                z /= vertCount;
                if (lastLine[xx] >= 0) {
                    vertexes[lastLine[xx] + 2].pos.z = z;
                }
                if (xx + 1 < mapWidth) {
                    if (lastLine[xx + 1] >= 0) {
                        vertexes[lastLine[xx + 1] + 1].pos.z = z;
                    }
                    if (currLine[xx + 1] >= 0) {
                        vertexes[currLine[xx + 1] + 0].pos.z = z;
                    }
                }

                // vertexes[currLine[xx] + 0].pos.z = z;
                vertexes[currLine[xx] + 3].pos.z = z;
                // vertexes[(xx + yy * mapWidth) * 4 + 1].pos.z = z;
                // vertexes[(xx + yy * mapWidth) * 4 + 2].pos.z = z;
            }
        }
        // swap cache tabs
        int* tmp = lastLine;
        lastLine = currLine;
        currLine = tmp;
    }

    //  for (int i = 0; i < clrs.size(); ++i) {
//        Log(clrs[i].x << " " << clrs[i].y << " " << clrs[i].z << " " << clrs[i].w);
// }
#if 1
    for (int i = 0; i < vertexes.size(); i += 4) {
        float right = ((float)vertexes[i + 2].pos.z + (float)vertexes[i + 3].pos.z) * 0.5f;
        float left = ((float)vertexes[i + 0].pos.z + (float)vertexes[i + 1].pos.z) * 0.5f;
        float bottom = ((float)vertexes[i + 0].pos.z + (float)vertexes[i + 3].pos.z) * 0.5f;
        float top = ((float)vertexes[i + 2].pos.z + (float)vertexes[i + 1].pos.z) * 0.5f;
        Vec2 normal{left - right, bottom - top};
        // std::cout << normal.y << " ";
        // normal.x *= 0.05f;
        // normal.y *= 0.05f;
        // normal.x *= zScale;
        // normal.y *= zScale;
        vertexes[i + 0].normal = Vec2{normal.x * 0.02f, normal.y * 0.02f};
        vertexes[i + 1].normal = Vec2{normal.x * 0.01f, normal.y * 0.01f};
        vertexes[i + 2].normal = Vec2{normal.x * 0.03f, normal.y * 0.03f};
        vertexes[i + 3].normal = Vec2{normal.x * 0.01f, normal.y * 0.01f};
        // vertexes[i + 0].normal = normal;
        // vertexes[i + 1].normal = normal;
        // vertexes[i + 2].normal = normal;
        // vertexes[i + 3].normal = normal;
    }
#endif
    // vertexes.clear();

    Vec2 tc{66.0f, 666.0f};
    Vec2 normal{56.0f, 566.0f};
    tid = 5.0f;
    float x = 100.0f, y = 100.0f, z = 0.0f, ww = 10.0f;
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

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertexDataSize, vertexes.data(), GL_STATIC_DRAW);
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
    // delete[] indices;

    // glBindVertexArray(vao);
    // glBindBuffer(GL_ARRAY_BUFFER, vbo);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

    // MapBatch batch;
    // batch.Init();
    err = glGetError();
    if (err)
        Log("Opengl error: " << err);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    float dt = 0.0f;
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
        if (window.scrollOffset)
            camera.Scroll(window.scrollOffset);
        camera.Update(window.xMouse, windowSize.y - window.yMouse, pix);
        glm::mat4 matrix = camera.GetMat();
        glUniformMatrix4fv(glGetUniformLocation(shader.GetProgram(), "pr_matrix"), 1, GL_FALSE,
                           glm::value_ptr(matrix));

        if (window.keys['L'])
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        if (window.keys['K'])
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        //      batch.Begin();

        // for (int i = 0; i < vertexes.size(); i += 4) {
        //    batch.Push(&vertexes[i]);
        //}
        // int indicesCount = 6;
        // glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_SHORT, NULL);
        // glDrawElements(GL_TRIANGLES, vertexes.size() * 6, GL_UNSIGNED_SHORT, indices);
        glDrawElements(GL_TRIANGLES, indicesSize, GL_UNSIGNED_SHORT, indices);

        GLenum err = glGetError();
        if (err)
            Log("Opengl error: " << err);

        // float tt2 = glfwGetTime();
        // Log(glfwGetTime() - tt2);
        //        batch.Flush();
        // Log("flush " << tFlush);
        tFlush = 0.0f;
        window.Update();
        // Log(glfwGetTime() - tt2);
        dt = glfwGetTime() - time;
        time = glfwGetTime();
    }
}

struct Vertexxx {
    Vec3 pos;
    Vec4 color;
};
struct BorderDraw {
    GLuint vao, vbo, ibo;
    std::vector<Vertexxx> vertexes;
    GLushort* indices;
};
BorderDraw borderDraw;

void initBorders(const unsigned char* provs, const unsigned char* heights, int w, int h)
{
    GLuint err;
    Vec3 pos{3500.0f, 180.0f, 20.0f};
    Vec2 size{.5f, .5f};
    Vec4 color{1.0f, 0.0f, 0.0f, 1.0f};

    borderDraw.vertexes.push_back(Vertexxx{.pos = pos, .color = color});
    borderDraw.vertexes.push_back(Vertexxx{.pos = Vec3{pos.x, pos.y + size.y, pos.z}, .color = color});
    borderDraw.vertexes.push_back(Vertexxx{.pos = Vec3{pos.x + size.x, pos.y + size.y, pos.z}, .color = color});
    borderDraw.vertexes.push_back(Vertexxx{.pos = Vec3{pos.x + size.x, pos.y, pos.z}, .color = color});

    int maxSprites = borderDraw.vertexes.size() / 4;
    int vertexSize = sizeof(Vertexxx);
    int spriteSize = vertexSize * 4;
    int vertexDataSize = spriteSize * maxSprites;
    int indicesSize = maxSprites * 6;
    glCreateVertexArrays(1, &borderDraw.vao);
    glBindVertexArray(borderDraw.vao);
    // glGenBuffers(1, &borderDraw.vbo);
    glCreateBuffers(1, &borderDraw.vbo);
    err = glGetError();
    if (err)
        Log("Opengl error: " << err);

    glBindBuffer(GL_ARRAY_BUFFER, borderDraw.vbo);
    glBufferData(GL_ARRAY_BUFFER, vertexDataSize, borderDraw.vertexes.data(), GL_STATIC_DRAW);
    Log("vbo=" << borderDraw.vbo);
    glEnableVertexArrayAttrib(borderDraw.vao, 0);
    glEnableVertexArrayAttrib(borderDraw.vao, 1);
    glEnableVertexArrayAttrib(borderDraw.vao, 2);
    glEnableVertexArrayAttrib(borderDraw.vao, 3);
    glEnableVertexArrayAttrib(borderDraw.vao, 4);
    err = glGetError();
    if (err)
        Log("Opengl error: " << err);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexSize, NULL);  //(const GLvoid*)0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, vertexSize,
                          (const GLvoid*)(offsetof(Vertexxx, Vertexxx::color)));  //(const GLvoid*)(3 * GL_FLOAT));

    err = glGetError();
    if (err)
        Log("Opengl error: " << err);
    // glBindBuffer(GL_ARRAY_BUFFER, 0);

    borderDraw.indices = new GLushort[indicesSize];
    int offset = 0;
    for (int i = 0; i < indicesSize; i += 6) {
        borderDraw.indices[i] = offset + 0;
        borderDraw.indices[i + 1] = offset + 1;
        borderDraw.indices[i + 2] = offset + 2;
        borderDraw.indices[i + 3] = offset + 2;
        borderDraw.indices[i + 4] = offset + 3;
        borderDraw.indices[i + 5] = offset + 0;
        offset += 4;
    }

    glGenBuffers(1, &borderDraw.ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, borderDraw.ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize * sizeof(GLushort), borderDraw.indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    err = glGetError();
    if (err)
        Log("Opengl error: " << err);
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
    Camera camera{window.GetSize()};
    glUseProgram(shader.GetProgram());
    glPatchParameteri(GL_PATCH_VERTICES, 3);
    float trCount = 150;
    glUniform1f(glGetUniformLocation(shader.GetProgram(), "level"), trCount);
    err = glGetError();
    if (err)
        Log("Opengl error: " << err);

    int mapWidth = 5632, mapHeight = 2048;
    Texture heightMap{"/home/lukasz/Pobrane/Heightmap.png", mapWidth, mapHeight};
    Texture provTexture{"/home/lukasz/Pobrane/Provinces_org.png", mapWidth, mapHeight};
    Texture grassT{"../shared/grass1.png", 64, 64, GL_REPEAT};
    Texture stoneT{"../shared/smoothstone.png", 64, 64, GL_REPEAT};
    Texture waterT{"../shared/water1.png", 64, 64, GL_REPEAT};
    err = glGetError();
    if (err)
        Log("Opengl error: " << err);
    int texID[32];
    for (int i = 0; i < 32; i++) texID[i] = i;
    texID[0] = grassT.GetId();
    texID[1] = provTexture.GetId();
    Log("Prov kurtyzana " << texID[1]);
    texID[2] = stoneT.GetId();
    texID[3] = heightMap.GetId();
    texID[4] = waterT.GetId();
    glUniform1iv(glGetUniformLocation(shader.GetProgram(), "tex"), 32, texID);
    err = glGetError();
    if (err)
        Log("Opengl error: " << err);

    for (int i = 0; i < 10; ++i) {
        // glActiveTexture((GLuint)texID[i]);

        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, (GLuint)texID[i]);
        err = glGetError();
        if (err)
            Log("Opengl error: " << err << " " << i);
    }
        
    glUseProgram(waterShader.GetProgram());
    glUniform1f(glGetUniformLocation(waterShader.GetProgram(), "level"), 32);
    glUniform1iv(glGetUniformLocation(waterShader.GetProgram(), "tex"), 32, texID);

    // GLuint textures[] = {(GLuint)texID[0], (GLuint)texID[1], (GLuint)texID[2], (GLuint)texID[3]};
    // glBindTextures(textures[0], 4, textures);

    // GLuint ts[] = {0, provTexture.GetId(), heightMap.GetId(), grassT.GetId(), stoneT.GetId(),
    //              stoneT.GetId(), provTexture.GetId(), provTexture.GetId() };

    // glBindTextures(ts[0], 8, ts);

    const unsigned char* pix = provTexture.GetPixels();
    const unsigned char* h = heightMap.GetPixels();

    initBorders(pix, h, mapWidth, mapHeight);

    Log("tu1");
    std::vector<Vertexx> vertexes;
    float tid = 0.0f;
    Vec4 color{1.0f, 0.0f, 0.0f, 1.0f};
    float scale = 2.0f;
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
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    

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
        glUseProgram(shader.GetProgram());
        if (window.scrollOffset) {
            camera.Scroll(window.scrollOffset);
            window.scrollOffset = 0;
            // float tesLevel = camera.GetFovDelta() * 20.0f + 15.0f;
            float tesLevel = camera.GetScrollPerc() + 15.0f;
            // tesLevel = sin(time) * 64.0f;
            if (tesLevel < 0.0f)
                tesLevel *= -1.0f;
            Log(tesLevel);
            tesLevel = 32.0f;
            glUniform1f(glGetUniformLocation(shader.GetProgram(), "level"), tesLevel);
        }
        // window.keys['L'] = 0;
        camera.Update(window.xMouse, windowSize.y - window.yMouse, pix);
        glm::mat4 matrix = camera.GetMat();
        // glUseProgram(shader.GetProgram());
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
        glUniformMatrix4fv(glGetUniformLocation(waterShader.GetProgram(), "pr_matrix"), 1, GL_FALSE,
                           glm::value_ptr(matrix));

        glUniformMatrix4fv(glGetUniformLocation(waterShader.GetProgram(), "matrix"), 1, GL_FALSE,
                           glm::value_ptr(matrix));
        glUniform1f(glGetUniformLocation(waterShader.GetProgram(), "waterTime"), waterTime);

        batch.Begin();
        Vertexx vertsWater[8];
        vertsWater[0] = Vertexx{.pos = Vec3{0.0, 0.0, 0.0}, .color = Vec4{0.0, 0.0, 1.0, 1.0}, .textureId = 0, .tc = Vec2{0.0, 0.0}}; 
        vertsWater[1] = Vertexx{.pos = Vec3{mapWidth * scale * 0.5, 0.0, 0.0}, .color = Vec4{0.0, 0.0, 1.0, 1.0}, .textureId = 0, .tc = Vec2{0.5, 0.0}}; 
        vertsWater[2] = Vertexx{.pos = Vec3{mapWidth * scale * 0.5, mapHeight * scale, 0.0}, .color = Vec4{0.0, 0.0, 1.0, 1.0}, .textureId = 0, .tc = Vec2{.5, 1.0}}; 
        vertsWater[3] = Vertexx{.pos = Vec3{0.0, mapHeight * scale, 0.0}, .color = Vec4{0.0, 0.0, 1.0, 1.0}, .textureId = 0, .tc = Vec2{0.0, 1.0}}; 
        
        vertsWater[4] = Vertexx{.pos = Vec3{mapWidth * scale * 0.5, 0.0, 0.0}, .color = Vec4{0.0, 0.0, 1.0, 1.0}, .textureId = 0, .tc = Vec2{0.5, 0.0}}; 
        vertsWater[5] = Vertexx{.pos = Vec3{mapWidth * scale, 0.0, 0.0}, .color = Vec4{0.0, 0.0, 1.0, 1.0}, .textureId = 0, .tc = Vec2{1.0, 0.0}}; 
        vertsWater[6] = Vertexx{.pos = Vec3{mapWidth * scale, mapHeight * scale, 0.0}, .color = Vec4{0.0, 0.0, 1.0, 1.0}, .textureId = 0, .tc = Vec2{1.0, 1.0}}; 
        vertsWater[7] = Vertexx{.pos = Vec3{mapWidth * scale * 0.5, mapHeight * scale, 0.0}, .color = Vec4{0.0, 0.0, 1.0, 1.0}, .textureId = 0, .tc = Vec2{0.5, 1.0}}; 

        batch.Push(vertsWater);
        batch.Push(&vertsWater[4]);
        batch.Flush();
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
        

        if (window.keys['I']) {
            unsigned char pixel[4];
            int pixx = window.xMouse, pixy = windowSize.y - window.yMouse;
            Log(pixx << " - " << pixy);
            glReadPixels(pixx, pixy, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);
            std::cout << "R: " << (int)pixel[0] << "< ";
            std::cout << "G: " << (int)pixel[1] << "< ";
            std::cout << "B: " << (int)pixel[2] << "< \n";
        }


        window.Update();
        // Log("flush " << glfwGetTime() - tt2);
        // Log(glfwGetTime() - tt2);
        waterTime += dt;
        dt = glfwGetTime() - time;
        //Log(dt);
        time = glfwGetTime();
    }
}

