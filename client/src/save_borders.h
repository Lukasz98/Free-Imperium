#pragma once
#include <map>
#include <queue>
#include <vector>

#include "color.h"
#include "load_data.h"
#include "log.h"
static int nodeHash(int x, int y);

struct Node {
    int x = 2, y = 2, chunk;
    std::vector<int> next;
    std::vector<std::pair<int,int>> provId; // and prov chunkId
    //bool visited = false;
};

void saveBorders(const unsigned char* pix, int ww, int hh, std::vector<ProvData>& provD,
                        std::map<int, Node>& nodes);
void saveSeaBorders(const unsigned char* pix, int ww, int hh, std::vector<ProvData>& provD,
                           std::map<int, Node>& nodes, std::map<unsigned int, int>& colorToId);

void createSaveProvPoints(const unsigned char* pix, int ww, int hh, std::vector<ProvData>& provD,
                                 std::map<unsigned int, int>& colorToId);
