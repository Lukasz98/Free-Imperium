#include "path_finder.h"

#include <queue>

Posibility PathFinder::Find(const std::vector<std::shared_ptr<Province>>& provs, int first, int goal)
{
    assert(first >= 0 && first < provs.size());
    auto prov = provs.at(first);
    std::vector<int> neighbours = prov->GetNeighbours();

    int visited[provs.size()];
    for (int i = 0; i < provs.size(); ++i) visited[i] = -1;
    std::queue<int> que;
    que.push(first);
    visited[first] = first;
    int last = first;

    while (que.size()) {
        int cur = que.front();
        que.pop();

        last = cur;
        if (cur == goal)
            break;

        std::vector<int> neighb = provs[cur]->GetNeighbours();
        for (auto n : neighb) {
            if (visited[n] == -1) {
                visited[n] = cur;
                que.push(n);
            }
        }
    }

    Posibility ps;
    while (visited[last] != last) {
        ps.path.push_back(last);
        last = visited[last];
    }
    std::reverse(ps.path.begin(), ps.path.end());
    posibilities.push_back(ps);

    return posibilities[0];
}

void PathFinder::recur(const std::vector<std::shared_ptr<Province>>& provs, int current, int goal, Posibility& ps)
{
    assert(current >= 0 && current < provs.size());
    auto prov = provs.at(current);
    std::vector<int> neighbours = prov->GetNeighbours();
    for (auto& n : neighbours) {
        bool was = false;
        for (auto& lastProv : ps.path)
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
}
