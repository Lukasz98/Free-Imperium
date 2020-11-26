#pragma once
#include <memory>
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
    std::size_t shortest = 100000; // represent number of provinces in shortest path

    void recur(const std::vector<std::shared_ptr<Province>> & provs, int current, int goal, Posibility & ps);
    
public:

    // first - start province id, goal - end procince id
    Posibility Find(const std::vector<std::shared_ptr<Province>> & provs, int first, int goal);

};
