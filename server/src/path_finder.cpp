#include "path_finder.h"
    
PathFinder::PathFinder() {}
PathFinder::~PathFinder() {}

Posibility PathFinder::Find(const std::vector<Province> & prov, int first, int goal)
{
    for (auto & p : prov) {
        if (p.GetId() == first) {
            std::vector<int> neighbours = p.GetNeighbours();
            for (auto & n : neighbours) {
                Posibility ps;
                if (n == goal) {
                    ps.path.push_back(n);
                    ps.done = true;
                    posibilities.push_back(ps);

                    if (ps.path.size() < shortest)
                        shortest = ps.path.size();
                     
                    break;
                }
                else {
                    ps.path.push_back(n);
                    ps.done = false;

                    if (!posibilities.size() || ps.path.size() < shortest)
                        recur(prov, n, goal, ps);
                }
            }
            break;
        }
    }
    std::size_t index = 0;
    for (std::size_t i = 0; i < posibilities.size(); i++) {
        if (posibilities[i].done)
            if (posibilities[i].path.size() < posibilities[index].path.size())
                index = i;
    }

    return posibilities[index];
}

void PathFinder::recur(const std::vector<Province> & prov, int current, int goal, Posibility & ps)
{
    for (auto & p : prov) {
        if (p.GetId() == current) {
            std::vector<int> neighbours = p.GetNeighbours();
            for (auto & n : neighbours) {
                bool was = false;
                for (auto & lastProv : ps.path)
                    if (lastProv == n)
                        was = true;

                if (was)
                    continue;

                if (n == goal) {
                    ps.path.push_back(n);
                    ps.done = true;
                    posibilities.push_back(ps);

                    if (ps.path.size() < shortest)
                        shortest = ps.path.size();
                    
                    break;
                }
                else {
                    Posibility newPs = ps;
                    newPs.path.push_back(n);
                    newPs.done = false;

                    if (!posibilities.size() || newPs.path.size() < shortest)
                        recur(prov, n, goal, newPs);
                }
            }
            break;
        }
    }    
}
