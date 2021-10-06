#pragma once
#include <glm/glm.hpp>
#include <map>

#include "map_batch.h"

int hashI(int x, int y)
{
    int h = x | (y << 16);
    return h;
}
struct FPair {
    float x, y;
};
struct Point {
    int x, y;
    std::vector<FPair> pairs;
};

struct BorderVertexLoc
{
    Vec3 pos;
    Vec2 tc;
};

void addToPoint(float x, float y, Point& p)
{
    for (std::size_t i = 0; i < p.pairs.size(); ++i) {
        if (p.pairs[i].x == x && p.pairs[i].y == y)
            return;
    }
    p.pairs.push_back(FPair{x, y});
}
std::vector<BorderVertexLoc> saveBordersTriangles(int mapWidth, int mapHeight, int scale, const unsigned char* h)
{
    std::map<int, Point> points;
    std::vector<BorderVertexLoc> borVerts;
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
        int i1 = x1 * 3 + y1 * mapWidth * 3;
        if (i1 > mapHeight * mapWidth * 3 - 3)
            i1 = mapHeight * mapWidth * 3 - 3;
        int i2 = x2 * 3 + y2 * mapWidth * 3;
        if (i2 > mapHeight * mapWidth * 3 - 3)
            i2 = mapHeight * mapWidth * 3 - 3;
        points[hashI(x1, y1)] = Point{};
        points[hashI(x2, y2)] = Point{};
        borVerts.push_back(BorderVertexLoc{.pos = Vec3{((float)x1), ((float)y1), h[i1]},
                                        .tc = Vec2{(float)x1 / mapWidth, (float)y1 / mapHeight}});
        borVerts.push_back(BorderVertexLoc{.pos = Vec3{((float)x2), ((float)y2), h[i2]},
                                        .tc = Vec2{(float)x2 / mapWidth, (float)y2 / mapHeight}});
    }
    file.close();
    float W = 0.2f;
    float w = 0.2f; 
    std::vector<BorderVertexLoc> allVerts;
    bool visited[borVerts.size() / 2] = {0};
    for (std::size_t i = 0; i < borVerts.size(); i += 2) {
        if (visited[i / 2])
            continue;
        visited[i / 2] = true;

        auto p1 = borVerts[i].pos;
        auto p2 = borVerts[i + 1].pos;
        Vec3 last1, last2;
        if (abs(p1.x - p2.x) == 0.0f) {
            Vec3 pp1{p1.x - w, p1.y, p1.z};
            Vec3 pp2{p1.x + w, p1.y, p1.z};
            Vec3 pp3{p2.x - w, p2.y, p2.z};
            Vec3 pp4{p2.x + w, p2.y, p2.z};
            allVerts.push_back(BorderVertexLoc{.pos = pp1});
            allVerts.push_back(BorderVertexLoc{.pos = pp2});
            allVerts.push_back(BorderVertexLoc{.pos = pp3});
            allVerts.push_back(BorderVertexLoc{.pos = pp4});
            allVerts.push_back(BorderVertexLoc{.pos = pp3});
            allVerts.push_back(BorderVertexLoc{.pos = pp2});
            last1 = pp3;
            last2 = pp4;
            addToPoint(pp1.x, pp1.y, points[hashI(p1.x, p1.y)]);
            addToPoint(pp2.x, pp2.y, points[hashI(p1.x, p1.y)]);
            addToPoint(pp3.x, pp3.y, points[hashI(p2.x, p2.y)]);
            addToPoint(pp4.x, pp4.y, points[hashI(p2.x, p2.y)]);
        }
        else if (abs(p1.y - p2.y) == 0.0f) {
            Vec3 pp1{p1.x, p1.y - w, p1.z};
            Vec3 pp2{p1.x, p1.y + w, p1.z};
            Vec3 pp3{p2.x, p2.y - w, p2.z};
            Vec3 pp4{p2.x, p2.y + w, p2.z};
            allVerts.push_back(BorderVertexLoc{.pos = pp1});
            allVerts.push_back(BorderVertexLoc{.pos = pp2});
            allVerts.push_back(BorderVertexLoc{.pos = pp3});
            allVerts.push_back(BorderVertexLoc{.pos = pp4});
            allVerts.push_back(BorderVertexLoc{.pos = pp3});
            allVerts.push_back(BorderVertexLoc{.pos = pp2});
            last1 = pp3;
            last2 = pp4;
            addToPoint(pp1.x, pp1.y, points[hashI(p1.x, p1.y)]);
            addToPoint(pp2.x, pp2.y, points[hashI(p1.x, p1.y)]);
            addToPoint(pp3.x, pp3.y, points[hashI(p2.x, p2.y)]);
            addToPoint(pp4.x, pp4.y, points[hashI(p2.x, p2.y)]);
        }
        else {
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

            allVerts.push_back(BorderVertexLoc{.pos = pp1});
            allVerts.push_back(BorderVertexLoc{.pos = pp2});
            allVerts.push_back(BorderVertexLoc{.pos = pp3});
            allVerts.push_back(BorderVertexLoc{.pos = pp4});
            allVerts.push_back(BorderVertexLoc{.pos = pp3});
            allVerts.push_back(BorderVertexLoc{.pos = pp2});
            last1 = pp3;
            last2 = pp4;
            addToPoint(pp1.x, pp1.y, points[hashI(p1.x, p1.y)]);
            addToPoint(pp2.x, pp2.y, points[hashI(p1.x, p1.y)]);
            addToPoint(pp3.x, pp3.y, points[hashI(p2.x, p2.y)]);
            addToPoint(pp4.x, pp4.y, points[hashI(p2.x, p2.y)]);
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
                    // Vec3 pp1 = last1;
                    // Vec3 pp2 = last2;
                    Vec3 pp1{p1.x - w, p1.y, p1.z};
                    Vec3 pp2{p1.x + w, p1.y, p1.z};
                    Vec3 pp3{p2.x - w, p2.y, p2.z};
                    Vec3 pp4{p2.x + w, p2.y, p2.z};
                    allVerts.push_back(BorderVertexLoc{.pos = pp1});
                    allVerts.push_back(BorderVertexLoc{.pos = pp2});
                    allVerts.push_back(BorderVertexLoc{.pos = pp3});
                    allVerts.push_back(BorderVertexLoc{.pos = pp4});
                    allVerts.push_back(BorderVertexLoc{.pos = pp3});
                    allVerts.push_back(BorderVertexLoc{.pos = pp2});

                    // allVerts.push_back(BorderVertexLoc{.pos = pp1});
                    // allVerts.push_back(BorderVertexLoc{.pos = last1});
                    // allVerts.push_back(BorderVertexLoc{.pos = last2});

                    // allVerts.push_back(BorderVertexLoc{.pos = pp2});
                    // allVerts.push_back(BorderVertexLoc{.pos = last1});
                    // allVerts.push_back(BorderVertexLoc{.pos = last2});
                    addToPoint(pp1.x, pp1.y, points[hashI(p1.x, p1.y)]);
                    addToPoint(pp2.x, pp2.y, points[hashI(p1.x, p1.y)]);
                    addToPoint(pp3.x, pp3.y, points[hashI(p2.x, p2.y)]);
                    addToPoint(pp4.x, pp4.y, points[hashI(p2.x, p2.y)]);
                    last1 = pp3;
                    last2 = pp4;
                }
                else if (abs(p1.y - p2.y) == 0.0f) {
                    // Vec3 pp1 = last2;
                    // Vec3 pp2 = last1;
                    Vec3 pp1{p1.x, p1.y - w, p1.z};
                    Vec3 pp2{p1.x, p1.y + w, p1.z};
                    Vec3 pp3{p2.x, p2.y - w, p2.z};
                    Vec3 pp4{p2.x, p2.y + w, p2.z};
                    allVerts.push_back(BorderVertexLoc{.pos = pp1});
                    allVerts.push_back(BorderVertexLoc{.pos = pp2});
                    allVerts.push_back(BorderVertexLoc{.pos = pp3});
                    allVerts.push_back(BorderVertexLoc{.pos = pp4});
                    allVerts.push_back(BorderVertexLoc{.pos = pp3});
                    allVerts.push_back(BorderVertexLoc{.pos = pp2});

                    // allVerts.push_back(BorderVertexLoc{.pos = pp1});
                    // allVerts.push_back(BorderVertexLoc{.pos = last1});
                    // allVerts.push_back(BorderVertexLoc{.pos = last2});
                    //
                    // allVerts.push_back(BorderVertexLoc{.pos = pp2});
                    // allVerts.push_back(BorderVertexLoc{.pos = last1});
                    // allVerts.push_back(BorderVertexLoc{.pos = last2});
                    addToPoint(pp1.x, pp1.y, points[hashI(p1.x, p1.y)]);
                    addToPoint(pp2.x, pp2.y, points[hashI(p1.x, p1.y)]);
                    addToPoint(pp3.x, pp3.y, points[hashI(p2.x, p2.y)]);
                    addToPoint(pp4.x, pp4.y, points[hashI(p2.x, p2.y)]);

                    last1 = pp3;
                    last2 = pp4;
                }
                else {
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

                    // pp1 = last1;
                    // pp2 = last2;
                    allVerts.push_back(BorderVertexLoc{.pos = pp1});
                    allVerts.push_back(BorderVertexLoc{.pos = pp2});
                    allVerts.push_back(BorderVertexLoc{.pos = pp3});
                    allVerts.push_back(BorderVertexLoc{.pos = pp4});
                    allVerts.push_back(BorderVertexLoc{.pos = pp3});
                    allVerts.push_back(BorderVertexLoc{.pos = pp2});

                    // allVerts.push_back(BorderVertexLoc{.pos = pp1});
                    // allVerts.push_back(BorderVertexLoc{.pos = last1});
                    // allVerts.push_back(BorderVertexLoc{.pos = last2});
                    // allVerts.push_back(BorderVertexLoc{.pos = pp2});
                    // allVerts.push_back(BorderVertexLoc{.pos = last1});
                    // allVerts.push_back(BorderVertexLoc{.pos = last2});

                    addToPoint(pp1.x, pp1.y, points[hashI(p1.x, p1.y)]);
                    addToPoint(pp2.x, pp2.y, points[hashI(p1.x, p1.y)]);
                    addToPoint(pp3.x, pp3.y, points[hashI(p2.x, p2.y)]);
                    addToPoint(pp4.x, pp4.y, points[hashI(p2.x, p2.y)]);
                    last1 = pp3;
                    last2 = pp4;
                }

                break;
            }
        } while (found);
        if (i % 10000 == 0)
            std::cout << i << "\n";
    }

    for (auto& pm : points) {
        auto& p = pm.second;
        for (std::size_t i = 0; i < p.pairs.size() && i + 3 < p.pairs.size(); i += 4) {
            allVerts.push_back(BorderVertexLoc{.pos = Vec3{p.pairs[i].x, p.pairs[i].y, 0.0f}});
            allVerts.push_back(BorderVertexLoc{.pos = Vec3{p.pairs[i + 2].x, p.pairs[i + 2].y, 0.0f}});
            allVerts.push_back(BorderVertexLoc{.pos = Vec3{p.pairs[i + 1].x, p.pairs[i + 1].y, 0.0f}});

            allVerts.push_back(BorderVertexLoc{.pos = Vec3{p.pairs[i + 1].x, p.pairs[i + 1].y, 0.0f}});
            allVerts.push_back(BorderVertexLoc{.pos = Vec3{p.pairs[i + 3].x, p.pairs[i + 3].y, 0.0f}});
            allVerts.push_back(BorderVertexLoc{.pos = Vec3{p.pairs[i].x, p.pairs[i].y, 0.0f}});
            
/*
            float d1 = pow(p.pairs[i].x - p.pairs[i + 2].x, 2) + pow(p.pairs[i].y - p.pairs[i + 2].y, 2); 
            float d2 = pow(p.pairs[i].x - p.pairs[i + 1].x, 2) + pow(p.pairs[i].y - p.pairs[i + 1].y, 2); 
            float d3 = pow(p.pairs[i + 2].x - p.pairs[i + 1].x, 2) + pow(p.pairs[i + 2].y - p.pairs[i + 1].y, 2); 

            if (d1 >= d2 + d3)
                allVerts.push_back(BorderVertexLoc{.pos = Vec3{p.pairs[i + 0].x, p.pairs[i + 0].y, 0.0f}});
            else
                allVerts.push_back(BorderVertexLoc{.pos = Vec3{p.pairs[i + 1].x, p.pairs[i + 1].y, 0.0f}});
*/



        }
    }

    file.open("BordersData2.txt", std::fstream::out);
    for (std::size_t i = 0; i < allVerts.size(); ++i) {
        file << allVerts[i + 0].pos.x << " " << allVerts[i + 0].pos.y << " " << allVerts[i + 0].pos.z << '\n';
    }
    return allVerts;
}
