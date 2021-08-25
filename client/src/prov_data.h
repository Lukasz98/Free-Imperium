#pragma once

#include <set>

struct ProvData {
    std::string name;
    int r, g, b;
    int id, ctrId = -1;
    int firstVertId, vertCount;
    float x, y;
    std::set<int> neighb, neighbSea;
    bool water;
};
