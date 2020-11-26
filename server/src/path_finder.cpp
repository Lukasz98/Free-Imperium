#include "path_finder.h"
    
Posibility PathFinder::Find(const std::vector<std::shared_ptr<Province>> & provs, int first, int goal)
{
    assert(first >= 0 && first < provs.size());
    auto prov = provs.at(first);
    //if (prov->GetId() == first) {
        std::vector<int> neighbours = prov->GetNeighbours();
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
                    recur(provs, n, goal, ps);
            }
        }
    //}
    std::size_t index = 0;
    for (std::size_t i = 0; i < posibilities.size(); i++) {
        if (posibilities[i].done)
            if (posibilities[i].path.size() < posibilities[index].path.size())
                index = i;
    }
    assert(posibilities.size() > 0);
    return posibilities[index];
}

void PathFinder::recur(const std::vector<std::shared_ptr<Province>> & provs, int current, int goal, Posibility & ps)
{
    assert(current >= 0 && current < provs.size());
    auto prov = provs.at(current);
    //if (prov->GetId() == current) {
        std::vector<int> neighbours = prov->GetNeighbours();
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
                    recur(provs, n, goal, newPs);
            }
        }
    //}
}
