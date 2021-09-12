#pragma once
#include "map_batch.h"
#include <glm/glm.hpp>

std::vector<BorderVertex> saveBordersTriangles(int mapWidth, int mapHeight, int scale, const unsigned char* h)
{
    std::vector<BorderVertex> borVerts;
    std::fstream file;
    file.open("BordersData.txt", std::fstream::in);
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
        //int chunkX = x1 / (int)w;
        //int chunkY = y1 / (int)w;
        //int chunkId = chunkX + chunkY * (mapWidth / w);
        int i1 = x1 * 3 + y1 * mapWidth * 3;
        if (i1 > mapHeight * mapWidth * 3 - 3)
            i1 = mapHeight * mapWidth * 3 - 3;
        int i2 = x2 * 3 + y2 * mapWidth * 3;
        if (i2 > mapHeight * mapWidth * 3 - 3)
            i2 = mapHeight * mapWidth * 3 - 3;
        borVerts.push_back(
            // BorderVertex{.pos = Vec3{((float)node.x + 0.5f) * scale, ((float)node.y + 0.5f) * scale, 200.0},
            BorderVertex{.pos = Vec3{((float)x1) * scale, ((float)y1) * scale, h[i1]},
                         .tc = Vec2{(float)x1 / mapWidth, (float)y1 / mapHeight}});
        borVerts.push_back(
            BorderVertex{.pos = Vec3{((float)x2) * scale, ((float)y2) * scale, h[i2]},
                         //.pos = Vec3{((float)x2 + 0.5f) * scale, ((float)y2 + 0.5f) * scale, 200.0},
                         .tc = Vec2{(float)x2 / mapWidth, (float)y2 / mapHeight}});
        //bordChunkId.push_back(chunkId);
    }
    file.close();
    std::vector<BorderVertex> allVerts;
    bool visited[borVerts.size() / 2] = {0};
    for (std::size_t i = 0; i < borVerts.size(); i += 2) {
        if (visited[i / 2])
            continue;
        visited[i / 2] = true;

        auto p1 = borVerts[i].pos;
        auto p2 = borVerts[i + 1].pos;
        Vec3 last1, last2;
        if (abs(p1.x - p2.x) == 0.0f) {
            float w = 0.5f;
            Vec3 pp1{p1.x - w, p1.y, p1.z};
            Vec3 pp2{p1.x + w, p1.y, p1.z};
            Vec3 pp3{p2.x - w, p2.y, p2.z};
            Vec3 pp4{p2.x + w, p2.y, p2.z};
            allVerts.push_back(BorderVertex{.pos = pp1});
            allVerts.push_back(BorderVertex{.pos = pp2});
            allVerts.push_back(BorderVertex{.pos = pp3});
            allVerts.push_back(BorderVertex{.pos = pp4});
            allVerts.push_back(BorderVertex{.pos = pp3});
            allVerts.push_back(BorderVertex{.pos = pp2});
            last1 = pp3;
            last2 = pp4;
        }
        else if (abs(p1.y - p2.y) == 0.0f) {
            float w = 0.5f;
            Vec3 pp1{p1.x, p1.y - w, p1.z};
            Vec3 pp2{p1.x, p1.y + w, p1.z};
            Vec3 pp3{p2.x, p2.y - w, p2.z};
            Vec3 pp4{p2.x, p2.y + w, p2.z};
            allVerts.push_back(BorderVertex{.pos = pp1});
            allVerts.push_back(BorderVertex{.pos = pp2});
            allVerts.push_back(BorderVertex{.pos = pp3});
            allVerts.push_back(BorderVertex{.pos = pp4});
            allVerts.push_back(BorderVertex{.pos = pp3});
            allVerts.push_back(BorderVertex{.pos = pp2});
            last1 = pp3;
            last2 = pp4;
        }
        else {
            float W = 0.5f;
            float width;

            float a = (p2.y - p1.y) / (p2.x - p1.x);
            float b = p2.y - p2.x * a;

            float a1 = -1.0f / a;
            width = W * a;
            float b1 = p2.y - (a1 * p2.x);
            float x1 = p2.x + width;
            float y1 = a1 * x1 + b1;
            Vec3 pp3{x1, y1, p2.z};
            x1 = p2.x - width;
            y1 = a1 * x1 + b1;
            Vec3 pp4{x1, y1, p2.z};

            a = (p1.y - p2.y) / (p1.x - p2.x);
            b = p1.y - p1.x * a;

            a1 = -1.0f / a;
            width = W * a;
            b1 = p1.y - (a1 * p1.x);
            x1 = p1.x + width;
            y1 = a1 * x1 + b1;
            Vec3 pp1{x1, y1, p1.z};
            x1 = p1.x - width;
            y1 = a1 * x1 + b1;
            Vec3 pp2{x1, y1, p1.z};

            allVerts.push_back(BorderVertex{.pos = pp1});
            allVerts.push_back(BorderVertex{.pos = pp2});
            allVerts.push_back(BorderVertex{.pos = pp3});
            allVerts.push_back(BorderVertex{.pos = pp4});
            allVerts.push_back(BorderVertex{.pos = pp3});
            allVerts.push_back(BorderVertex{.pos = pp2});
            last1 = pp3;
            last2 = pp4;
        }

        bool found = false;
        do {
            found = false;
            Vec3 findPos = borVerts[i + 1].pos;
            for (std::size_t j = i + 2; j < borVerts.size(); j += 2) {
                if (visited[j / 2])
                    continue;
                Vec3 p1, p2;
                if (borVerts[j].pos == findPos) {
                    p1 = borVerts[j].pos;
                    p2 = borVerts[j + 1].pos;
                }
                else if (borVerts[j + 1].pos == findPos) {
                    p1 = borVerts[j + 1].pos;
                    p2 = borVerts[j].pos;
                }
                else
                    continue;
                findPos = p2;
                visited[j / 2] = true;
                found = true;

                if (abs(p1.x - p2.x) == 0.0f) {
                    float w = 0.5f;
                    //Vec3 pp1 = last1;
                    //Vec3 pp2 = last2;
                    Vec3 pp1{p1.x - w, p1.y, p1.z};
                    Vec3 pp2{p1.x + w, p1.y, p1.z};
                    Vec3 pp3{p2.x - w, p2.y, p2.z};
                    Vec3 pp4{p2.x + w, p2.y, p2.z};
                    allVerts.push_back(BorderVertex{.pos = pp1});
                    allVerts.push_back(BorderVertex{.pos = pp2});
                    allVerts.push_back(BorderVertex{.pos = pp3});
                    allVerts.push_back(BorderVertex{.pos = pp4});
                    allVerts.push_back(BorderVertex{.pos = pp3});
                    allVerts.push_back(BorderVertex{.pos = pp2});
                    
                    allVerts.push_back(BorderVertex{.pos = pp1});
                    allVerts.push_back(BorderVertex{.pos = last1});
                    allVerts.push_back(BorderVertex{.pos = last2});
                    
                    allVerts.push_back(BorderVertex{.pos = pp2});
                    allVerts.push_back(BorderVertex{.pos = last1});
                    allVerts.push_back(BorderVertex{.pos = last2});
                    last1 = pp3;
                    last2 = pp4;
                }
                else if (abs(p1.y - p2.y) == 0.0f) {
                    float w = 0.5f;
                    //Vec3 pp1 = last2;
                    //Vec3 pp2 = last1;
                    Vec3 pp1{p1.x, p1.y - w, p1.z};
                    Vec3 pp2{p1.x, p1.y + w, p1.z};
                    Vec3 pp3{p2.x, p2.y - w, p2.z};
                    Vec3 pp4{p2.x, p2.y + w, p2.z};
                    allVerts.push_back(BorderVertex{.pos = pp1});
                    allVerts.push_back(BorderVertex{.pos = pp2});
                    allVerts.push_back(BorderVertex{.pos = pp3});
                    allVerts.push_back(BorderVertex{.pos = pp4});
                    allVerts.push_back(BorderVertex{.pos = pp3});
                    allVerts.push_back(BorderVertex{.pos = pp2});
                    
                    allVerts.push_back(BorderVertex{.pos = pp1});
                    allVerts.push_back(BorderVertex{.pos = last1});
                    allVerts.push_back(BorderVertex{.pos = last2});
                    
                    allVerts.push_back(BorderVertex{.pos = pp2});
                    allVerts.push_back(BorderVertex{.pos = last1});
                    allVerts.push_back(BorderVertex{.pos = last2});
                    
                    last1 = pp3;
                    last2 = pp4;
                }
                else {
                    float W = 0.5f;
                    float width;

                    float a = (p2.y - p1.y) / (p2.x - p1.x);
                    float b = p2.y - p2.x * a;

                    float a1 = -1.0f / a;
                    width = W * a;
                    float b1 = p2.y - (a1 * p2.x);
                    float x1 = p2.x - width;
                    float y1 = a1 * x1 + b1;
                    Vec3 pp3{x1, y1, p2.z};
                    x1 = p2.x + width;
                    y1 = a1 * x1 + b1;
                    Vec3 pp4{x1, y1, p2.z};

                    a = (p1.y - p2.y) / (p1.x - p2.x);
                    b = p1.y - p1.x * a;

                    a1 = -1.0f / a;
                    width = W * a;
                    b1 = p1.y - (a1 * p1.x);
                    x1 = p1.x - width;
                    y1 = a1 * x1 + b1;
                    Vec3 pp1{x1, y1, p1.z};
                    x1 = p1.x + width;
                    y1 = a1 * x1 + b1;
                    Vec3 pp2{x1, y1, p1.z};

                    //pp1 = last1;
                    //pp2 = last2;
                    allVerts.push_back(BorderVertex{.pos = pp1});
                    allVerts.push_back(BorderVertex{.pos = pp2});
                    allVerts.push_back(BorderVertex{.pos = pp3});
                    allVerts.push_back(BorderVertex{.pos = pp4});
                    allVerts.push_back(BorderVertex{.pos = pp3});
                    allVerts.push_back(BorderVertex{.pos = pp2});

                    allVerts.push_back(BorderVertex{.pos = pp1});
                    allVerts.push_back(BorderVertex{.pos = last1});
                    allVerts.push_back(BorderVertex{.pos = last2});
                    allVerts.push_back(BorderVertex{.pos = pp2});
                    allVerts.push_back(BorderVertex{.pos = last1});
                    allVerts.push_back(BorderVertex{.pos = last2});

                    last1 = pp3;
                    last2 = pp4;
                }

                break;
            }
        } while (found);
        if (i % 10000 == 0)
            std::cout << i << "\n";
    }

   
    file.open("BordersData2.txt", std::fstream::out);
    for (std::size_t i = 0; i < allVerts.size(); ++i) {
        file << allVerts[i + 0].pos.x << " " <<  allVerts[i + 0].pos.y << " " <<  allVerts[i + 0].pos.z << '\n'; 
    }
    return allVerts;
}
