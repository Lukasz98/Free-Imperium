#pragma once
#include <vector>
#include "province.h"


struct Posibility
{
    std::vector<int> path;
    bool done = false;
};


class PathFinder
{
    std::vector<Posibility> posibilities;
    int shortest = 100000; // represent number of provinces in shortest path

    void recur(const std::vector<Province> & prov, int current, int goal, Posibility & ps);
    
public:
    PathFinder();
    ~PathFinder();

    // first - start province id, goal - end procince id
    Posibility Find(const std::vector<Province> & prov, int first, int goal);

};
