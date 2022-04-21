#include "save_borders.h"
//#include <fstream>
#include <glm/glm.hpp>

Color currCol;
int provId;
int nodeHash(int x, int y)
{
    int r = x | (y << 16);
    return r;
}

int dirKind(int x1, int y1, int x2, int y2)
{
    if (x1 == x2)
        return 1;
    else if (y1 == y2)
        return 2;
    else if (x1 == x2 + 1 && y1 == y2 + 1)
        return 3;
    else if (x1 == x2 - 1 && y1 == y2 + 1)
        return 4;
    else if (x1 == x2 + 1 && y1 == y2 - 1)
        return 5;
    else if (x1 == x2 - 1 && y1 == y2 - 1)
        return 6;
    return 1;
}

int step(int x1, int y1, int x2, int y2, int x3, int y3)
{
    if (x1 + 1 == x2 && x2 == x3) {
        if (y1 == y2 && y2 + 1 == y3) {
            return 1;
        }
        if (y1 == y2 && y2 - 1 == y3) {
            return 1;
        }
    }
    else if (x1 - 1 == x2 && x2 == x3) {
        if (y1 == y2 && y2 + 1 == y3) {
            return 1;
        }
        if (y1 == y2 && y2 - 1 == y3) {
            return 1;
        }
    }

    return 0;
}

void bord2(const unsigned char* pix, int x, int y, int w, int h, std::map<int, Node>& nodes, int chunkId)
{
    std::map<int, int> tmp;
    std::queue<int> que;
    que.push(x | (y << 16));
    while (que.size()) {
        int key = que.front();
        que.pop();
        y = key >> 16;
        x = key ^ (y << 16);
        int nh = nodeHash(x, y);
        if (tmp.find(nh) != tmp.end())
            continue;
        tmp[nh] = nh;
        int lx = x - 1, ly1 = y, ly2 = y + 1;
        int tx1 = x, tx2 = x + 1, ty = y + 1;
        int rx = x + 1, ry1 = y + 1, ry2 = y;
        int bx1 = x + 1, bx2 = x, by = y - 1;


        int nextI = lx * 3 + y * w * 3;
        if (lx < 0)
            nextI = (w - 1) * 3 + ly1 * w * 3;

        Color nc{pix[nextI + 0], pix[nextI + 1], pix[nextI + 2]};
        bool goL = (currCol == nc);
        if (goL == false) {
            int thash = nodeHash(lx + 1, ly1);
            if (nodes.find(thash) == nodes.end()) {
                nodes[thash] = Node{.x = lx + 1, .y = ly1};
            }
            int thash2 = nodeHash(lx + 1, ly2);
            if (nodes.find(thash2) == nodes.end()) {
                nodes[thash2] = Node{.x = lx + 1, .y = ly2};
            }
            nodes[thash].provId.push_back(std::make_pair(provId, chunkId));
            nodes[thash2].provId.push_back(std::make_pair(provId, chunkId));
            nodes[thash].next.push_back(thash2);
            nodes[thash2].next.push_back(thash);
        }
        else {
            que.push(lx | (ly1 << 16));
        }

        bool goT = false;
        if (ty < h)  // {
            nextI = tx1 * 3 + ty * w * 3;

        nc = Color{pix[nextI + 0], pix[nextI + 1], pix[nextI + 2]};
        goT = (currCol == nc);
        if (goT == false || ty >= h) {
            int thash = nodeHash(tx1, ty);
            if (nodes.find(thash) == nodes.end()) {
                nodes[thash] = Node{.x = tx1, .y = ty};
            }
            int thash2 = nodeHash(tx2, ty);
            if (nodes.find(thash2) == nodes.end()) {
                nodes[thash2] = Node{.x = tx2, .y = ty};
            }
            nodes[thash].next.push_back(thash2);
            nodes[thash2].next.push_back(thash);
            nodes[thash].provId.push_back(std::make_pair(provId, chunkId));
            nodes[thash2].provId.push_back(std::make_pair(provId, chunkId));
        }
        else {
            que.push(tx1 | (ty << 16));
        }

        bool goR = false;
        if (rx < w) {
            nextI = rx * 3 + ry2 * w * 3;
            nc = Color{pix[nextI + 0], pix[nextI + 1], pix[nextI + 2]};
            goR = (currCol == nc);
            if (goR == false) {
                int thash = nodeHash(rx, ry2);
                if (nodes.find(thash) == nodes.end()) {
                    nodes[thash] = Node{.x = rx, .y = ry2};
                }
                int thash2 = nodeHash(rx, ry1);
                if (nodes.find(thash2) == nodes.end()) {
                    nodes[thash2] = Node{.x = rx, .y = ry1};
                }
                nodes[thash].provId.push_back(std::make_pair(provId, chunkId));
                nodes[thash2].provId.push_back(std::make_pair(provId, chunkId));
                nodes[thash].next.push_back(thash2);
                nodes[thash2].next.push_back(thash);
            }
            else {
                que.push(rx | (ry2 << 16));
            }
        }

        bool goB = false;
        if (by > 0) {
            nextI = bx2 * 3 + by * w * 3;
            nc = Color{pix[nextI + 0], pix[nextI + 1], pix[nextI + 2]};
            goB = (currCol == nc);
            if (goB == false) {
                int thash = nodeHash(bx2, by + 1);
                if (nodes.find(thash) == nodes.end())
                    nodes[thash] = Node{.x = bx2, .y = by + 1};
                int thash2 = nodeHash(bx1, by + 1);
                if (nodes.find(thash2) == nodes.end())
                    nodes[thash2] = Node{.x = bx1, .y = by + 1};
                nodes[thash].next.push_back(thash2);
                nodes[thash2].next.push_back(thash);
                nodes[thash].provId.push_back(std::make_pair(provId, chunkId));
                nodes[thash2].provId.push_back(std::make_pair(provId, chunkId));
            }
            else {
                que.push(bx2 | (by << 16));
            }
        }
    }
}

void saveBorders(const unsigned char* pix, int ww, int hh, std::vector<ProvData>& provD,
                 std::map<int, Node>& nodes)
{
    std::vector<std::pair<int, int>>* provPoints = new std::vector<std::pair<int, int>>[provD.size()];
    loadProvPointsData(provPoints, provD.size());

    Log("tu");
    for (auto& pd : provD) {
        if (pd.water)
            continue;
        provId = pd.id;
        currCol = Color{(unsigned char)pd.r, (unsigned char)pd.g, (unsigned char)pd.b};
        for (std::size_t i = 0; i < provPoints[pd.id].size(); ++i) {
            bord2(pix, provPoints[pd.id][i].first, provPoints[pd.id][i].second, ww, hh, nodes, i);
        }
        if (provPoints[pd.id].size() > 1)
            Log(pd.id << ", chunks: " << provPoints[pd.id].size());
    }
    delete [] provPoints;
    Log("nodes: " << nodes.size());
    for (auto& alele : nodes) {
        auto& it = alele.second;
        for (int i = 0; i < it.next.size(); ++i) {
            for (int j = i + 1; j < it.next.size(); ++j) {
                if (it.next[i] == it.next[j]) {
                    it.next.erase(it.next.begin() + j);
                    --j;
                }
            }
        }
    }
    bool deleted = false;
    int deletedLines = 0;
    int endedat = deletedLines;
    bool onlyStep = 1;
label:
    do {
        endedat = deletedLines;
        deleted = 0;
        for (auto& alele : nodes) {
            auto& it = alele.second;
            for (auto n : it.next) {
                if (n == nodeHash(it.x, it.y))
                    continue;
                if (nodes[n].next.size() > 2)
                    continue;
                int kind = dirKind(it.x, it.y, nodes[n].x, nodes[n].y);
                if (kind == -1)
                    continue;
                int kind2 = -1;
                for (auto nn : nodes[n].next) {
                    if (n == nn || nn == nodeHash(it.x, it.y))
                        continue;
                    if (onlyStep) {
                        if (step(it.x, it.y, nodes[n].x, nodes[n].y, nodes[nn].x, nodes[nn].y) == 0) {
                            continue;
                            kind2 = kind;
                        }
                    }
                    else {
                        kind2 = dirKind(nodes[n].x, nodes[n].y, nodes[nn].x, nodes[nn].y);
                        if (kind != kind2) {
                            continue;
                        }
                    }
                    ++deletedLines;
                    it.next.push_back(nn);
                    nodes[nn].next.push_back(nodeHash(it.x, it.y));
                    for (int j = 0; j < nodes[nn].next.size(); ++j) {
                        if (nodes[nn].next[j] == n) {
                            nodes[nn].next.erase(nodes[nn].next.begin() + j);
                            --j;
                        }
                    }
                    for (int j = 0; j < nodes[n].next.size(); ++j) {
                        if (nodes[n].next[j] == nn) {
                            nodes[n].next.erase(nodes[n].next.begin() + j);
                            --j;
                        }
                    }
                    for (int j = 0; j < nodes[n].next.size(); ++j) {
                        if (nodes[n].next[j] == nodeHash(it.x, it.y)) {
                            nodes[n].next.erase(nodes[n].next.begin() + j);
                            --j;
                        }
                    }
                    for (int j = 0; j < it.next.size(); ++j) {
                        if (it.next[j] == n) {
                            it.next.erase(it.next.begin() + j);
                            --j;
                        }
                    }
                    deleted = true;
                    break;
                }
                if ((kind2 != -1 && kind2 == kind))
                    break;
            }
        }
        Log("deleted lines: " << deletedLines << ", endedat: " << endedat);
    } while ((deleted && deletedLines < 2000000) || deletedLines == endedat - 1);
    if (onlyStep) {
        onlyStep = false;
        goto label;
    }
    deleted = false;
    int deletedDuplicats = 0;
    do {
        deleted = false;
        for (auto& alele : nodes) {
            auto& it = alele.second;
            for (int i = 0; i < it.next.size(); ++i) {
                if (it.next[i] == nodeHash(it.x, it.y)) {
                    deletedDuplicats++;
                    deleted = true;
                    it.next.erase(it.next.begin() + i);
                    --i;
                }
            }
        }
        Log("deleted duplicats: " << deletedDuplicats);
    } while (deleted && 0);

    int deletedEmpty = 0;
    for (auto it = nodes.begin(); it != nodes.end();) {
        if (it->second.next.size() == 0) {
            it = nodes.erase(it);
            deletedEmpty++;
        }
        else {
            ++it;
        }
    }

    Log("deleted empty: " << deletedEmpty);
    Log("nodes size: " << nodes.size());

    // create polygons
    std::fstream f2;
    f2.open("generated_data/polygon.txt", std::fstream::out);
    for (auto& pd : provD) {
        if (pd.water)
            continue;

        f2 << "id: ";
        f2 << std::to_string(pd.id) << "\n";
        std::vector<std::pair<int, int>> chunks;
        std::map<int, bool> visited;
        // detect chunks
        for (auto it = nodes.begin(); it != nodes.end(); ++it) {
            if (visited.find(it->first) != visited.end() && visited[it->first])
                continue;
            if (std::find_if(it->second.provId.begin(), it->second.provId.end(),
                             [pdid = pd.id](std::pair<int, int> p) { return pdid == p.first; }) !=
                it->second.provId.end())
            {
                int key = it->first;
                chunks.push_back(std::make_pair(key, 0));
                visited[key] = true;
                int count = 1;
            label_2:
                for (int i : nodes[key].next) {
                    if (chunks.back().first == i)
                        continue;
                    if (visited.find(i) != visited.end() && visited[i])
                        continue;
                    if (std::find_if(nodes[i].provId.begin(), nodes[i].provId.end(),
                                     [pdid = pd.id](std::pair<int, int> p) { return pdid == p.first; }) ==
                        nodes[i].provId.end())
                        continue;
                    {
                        key = i;
                        visited[key] = true;
                        ++count;
                        goto label_2;
                    }
                }
                chunks.back().second = count;
            }
        }

        assert(chunks.size());
        for (auto chunk : chunks) {
            std::vector<int> verts;
            verts.push_back(chunk.first);
            int vert = chunk.first;
            for (int j = 0; j < nodes[vert].next.size(); ++j) {
                if (std::find(verts.begin(), verts.end(), nodes[vert].next[j]) != verts.end())
                    continue;
                if (std::find_if(nodes[nodes[vert].next[j]].provId.begin(),
                                 nodes[nodes[vert].next[j]].provId.end(), [pdid = pd.id](std::pair<int, int> p) {
                                     return pdid == p.first;
                                 }) == nodes[nodes[vert].next[j]].provId.end())
                    continue;
                verts.push_back(nodes[vert].next[j]);
                vert = nodes[vert].next[j];
                j = -1;
            }
            // check if clockwise
            int sum = 0;
            for (int j = 0; j < verts.size(); ++j) {
                int next = (j + 1) % verts.size();
                int val = (nodes[verts[next]].x - nodes[verts[j]].x) * (nodes[verts[next]].y + nodes[verts[j]].y);
                sum += val;
            }
            if (sum < 0) {  // counter clockwise
                std::reverse(verts.begin(), verts.end());
            }
            int dkd = -1;
            while (verts.size() > 3) {
                for (int j = 0; j < verts.size() && verts.size() > 3; ++j) {
                    int next = (j + 1) % verts.size();
                    int prev = (j - 1);
                    if (prev < 0)
                        prev = verts.size() - 1;
                    glm::vec2 vp{(float)nodes[verts[prev]].x, (float)nodes[verts[prev]].y};
                    glm::vec2 vj{(float)nodes[verts[j]].x, (float)nodes[verts[j]].y};
                    glm::vec2 vn{(float)nodes[verts[next]].x, (float)nodes[verts[next]].y};
                    glm::vec3 jToP{vp.x - vj.x, vp.y - vj.y, 0.0};
                    glm::vec3 jToN{vn.x - vj.x, vn.y - vj.y, 0.0};
                    glm::vec3 nToP{vp.x - vn.x, vp.y - vn.y, 0.0};
                    if (jToP.x * jToN.y - jToP.y * jToN.x < 0.0f) {  // cross product ?
                        continue;
                    }
                    bool ok = true;
                    // czy jest w trojkacie
                    for (int k = 0; k < verts.size(); ++k) {
                        if (k == j || k == prev || k == next)
                            continue;
                        glm::vec2 vk{(float)nodes[verts[k]].x, (float)nodes[verts[k]].y};
                        glm::vec2 ab = vj - vp;
                        glm::vec2 bc = vn - vj;
                        glm::vec2 ca = vp - vn;

                        glm::vec2 ap = vk - vp;
                        glm::vec2 bp = vk - vj;
                        glm::vec2 cp = vk - vn;

                        float cr1 = ab.x * ap.y - ab.y * ap.x;
                        float cr2 = bc.x * bp.y - bc.y * bp.x;
                        float cr3 = ca.x * cp.y - ca.y * cp.x;

                        if (cr1 <= 0.0f && cr2 <= 0.0f && cr3 <= 0.0f) {
                            ok = false;
                            break;
                        }
                    }
                    if (!ok)
                        continue;
                    f2 << vp.x << " " << vp.y << " " << vj.x << " " << vj.y << " " << vn.x << " " << vn.y << "\n";
                    verts.erase(verts.begin() + j);
                    --j;
                }
                if (dkd == verts.size()) {
                    break;
                }
                dkd = verts.size();
            }
            if (verts.size() != 3 && verts.size() != 0)
                Log("ERROR pd.id = " << pd.id);

            for (int j = 0; j < verts.size() && j < 3; ++j) {
                f2 << nodes[verts[j]].x << " " << nodes[verts[j]].y << " ";
            }
            f2 << "\n";
        }
    }
    f2.close();
    std::fstream f;
    f.open("generated_data/BordersData.txt", std::fstream::out);
    int dell = 0;
    for (auto& alel : nodes) {
        auto& it = alel.second;
        for (int i = 0; i < it.next.size(); ++i) {
            int n = it.next[i];
            f << it.x << " " << it.y << " " << nodes[n].x << " " << nodes[n].y << '\n';

            for (int j = 0; j < nodes[n].next.size(); ++j) {
                if (nodes[n].next[j] == nodeHash(it.x, it.y)) {
                    nodes[n].next.erase(nodes[n].next.begin() + j);
                    dell++;
                    --j;
                }
            }
        }
    }
    f.close();
    Log("dell = " << dell);
}
// sea

void bord3(const unsigned char* pix, int x, int y, int w, int h, const std::vector<ProvData>& provinces,
           std::map<int, Node>& nodes, std::map<unsigned int, int>& colorToId, int chunkId)
{
    std::map<int, int> tmp;
    std::queue<int> que;
    que.push(x | (y << 16));
    while (que.size()) {
        int key = que.front();
        que.pop();
        y = key >> 16;
        x = key ^ (y << 16);
        int nh = nodeHash(x, y);
        if (tmp.find(nh) != tmp.end())
            continue;
        tmp[nh] = nh;
        int lx = x - 1, ly1 = y, ly2 = y + 1;
        int tx1 = x, tx2 = x + 1, ty = y + 1;
        int rx = x + 1, ry1 = y + 1, ry2 = y;
        int bx1 = x + 1, bx2 = x, by = y - 1;


        int nextI = lx * 3 + y * w * 3;
        if (lx < 0)
            nextI = (w - 1) * 3 + ly1 * w * 3;

        Color nc{pix[nextI + 0], pix[nextI + 1], pix[nextI + 2]};
        bool goL = (currCol == nc);
        bool isWater = provinces[colorToId[getHash(nc.r, nc.g, nc.b)]].water;
        if (goL == false && isWater) {
            int thash = nodeHash(lx + 1, ly1);
            if (nodes.find(thash) == nodes.end()) {
                nodes[thash] = Node{.x = lx + 1, .y = ly1};
            }
            int thash2 = nodeHash(lx + 1, ly2);
            if (nodes.find(thash2) == nodes.end()) {
                nodes[thash2] = Node{.x = lx + 1, .y = ly2};
            }
            nodes[thash].provId.push_back(std::make_pair(provId, chunkId));
            nodes[thash2].provId.push_back(std::make_pair(provId, chunkId));
            nodes[thash].next.push_back(thash2);
            nodes[thash2].next.push_back(thash);
        }
        else if (isWater) {
            que.push(lx | (ly1 << 16));
        }

        bool goT = false;
        if (ty < h)
            nextI = tx1 * 3 + ty * w * 3;

        nc = Color{pix[nextI + 0], pix[nextI + 1], pix[nextI + 2]};
        goT = (currCol == nc);
        isWater = provinces[colorToId[getHash(nc.r, nc.g, nc.b)]].water;
        if ((goT == false || ty >= h) && isWater) {
            int thash = nodeHash(tx1, ty);
            if (nodes.find(thash) == nodes.end()) {
                nodes[thash] = Node{.x = tx1, .y = ty};
            }
            int thash2 = nodeHash(tx2, ty);
            if (nodes.find(thash2) == nodes.end()) {
                nodes[thash2] = Node{.x = tx2, .y = ty};
            }
            nodes[thash].next.push_back(thash2);
            nodes[thash2].next.push_back(thash);
            nodes[thash].provId.push_back(std::make_pair(provId, chunkId));
            nodes[thash2].provId.push_back(std::make_pair(provId, chunkId));
        }
        else if (isWater) {
            que.push(tx1 | (ty << 16));
        }

        bool goR = false;
        if (rx < w) {
            nextI = rx * 3 + ry2 * w * 3;
            nc = Color{pix[nextI + 0], pix[nextI + 1], pix[nextI + 2]};
            goR = (currCol == nc);
            isWater = provinces[colorToId[getHash(nc.r, nc.g, nc.b)]].water;
            if (goR == false && isWater) {
                int thash = nodeHash(rx, ry2);
                if (nodes.find(thash) == nodes.end()) {
                    nodes[thash] = Node{.x = rx, .y = ry2};
                }
                int thash2 = nodeHash(rx, ry1);
                if (nodes.find(thash2) == nodes.end()) {
                    nodes[thash2] = Node{.x = rx, .y = ry1};
                }
                nodes[thash].provId.push_back(std::make_pair(provId, chunkId));
                nodes[thash2].provId.push_back(std::make_pair(provId, chunkId));
                nodes[thash].next.push_back(thash2);
                nodes[thash2].next.push_back(thash);
            }
            else if (isWater) {
                que.push(rx | (ry2 << 16));
            }
        }

        bool goB = false;
        if (by > 0) {
            nextI = bx2 * 3 + by * w * 3;
            nc = Color{pix[nextI + 0], pix[nextI + 1], pix[nextI + 2]};
            goB = (currCol == nc);
            isWater = provinces[colorToId[getHash(nc.r, nc.g, nc.b)]].water;
            if (goB == false && isWater) {
                int thash = nodeHash(bx2, by + 1);
                if (nodes.find(thash) == nodes.end())
                    nodes[thash] = Node{.x = bx2, .y = by + 1};
                int thash2 = nodeHash(bx1, by + 1);
                if (nodes.find(thash2) == nodes.end())
                    nodes[thash2] = Node{.x = bx1, .y = by + 1};
                nodes[thash].next.push_back(thash2);
                nodes[thash2].next.push_back(thash);
                nodes[thash].provId.push_back(std::make_pair(provId, chunkId));
                nodes[thash2].provId.push_back(std::make_pair(provId, chunkId));
            }
            else if (isWater) {
                que.push(bx2 | (by << 16));
            }
        }
    }
}

void saveSeaBorders(const unsigned char* pix, int ww, int hh, const std::vector<ProvData>& provD,
                    std::map<int, Node>& nodes, std::map<unsigned int, int>& colorToId)
{
    std::vector<std::pair<int, int>>* provPoints = new std::vector<std::pair<int, int>>[provD.size()];
    loadProvPointsData(provPoints, provD.size());

    Log("HH = " << hh);
    int cnt = 0;
    for (auto& pd : provD) {
        if (0 == pd.water)
            continue;
        cnt++;

        provId = pd.id;
        currCol = Color{(unsigned char)pd.r, (unsigned char)pd.g, (unsigned char)pd.b};
        for (std::size_t i = 0; i < provPoints[pd.id].size(); ++i) {
            bord3(pix, provPoints[pd.id][i].first, provPoints[pd.id][i].second, ww, hh, provD, nodes, colorToId,
                  i);
        }
    }
    Log("wtr cnt="<<cnt);
    delete [] provPoints;
    Log("nodes: " << nodes.size());

    for (auto& alele : nodes) {
        auto& it = alele.second;
        for (int i = 0; i < it.next.size(); ++i) {
            for (int j = i + 1; j < it.next.size(); ++j) {
                if (it.next[i] == it.next[j]) {
                    it.next.erase(it.next.begin() + j);
                    --j;
                }
            }
        }
    }
    bool deleted = false;
    int deletedLines = 0;
    int endedat = deletedLines;
    bool onlyStep = 1;
label:
    do {
        endedat = deletedLines;
        deleted = 0;
        for (auto& alele : nodes) {
            auto& it = alele.second;
            for (auto n : it.next) {
                if (n == nodeHash(it.x, it.y))
                    continue;
                if (nodes[n].next.size() > 2)
                    continue;
                int kind = dirKind(it.x, it.y, nodes[n].x, nodes[n].y);
                if (kind == -1)
                    continue;
                int kind2 = -1;
                for (auto nn : nodes[n].next) {
                    if (n == nn || nn == nodeHash(it.x, it.y))
                        continue;
                    if (onlyStep) {
                        if (step(it.x, it.y, nodes[n].x, nodes[n].y, nodes[nn].x, nodes[nn].y) == 0) {
                            continue;
                            kind2 = kind;
                        }
                    }
                    else {
                        kind2 = dirKind(nodes[n].x, nodes[n].y, nodes[nn].x, nodes[nn].y);
                        if (kind != kind2) {
                            continue;
                        }
                    }
                    ++deletedLines;
                    it.next.push_back(nn);
                    nodes[nn].next.push_back(nodeHash(it.x, it.y));
                    for (int j = 0; j < nodes[nn].next.size(); ++j) {
                        if (nodes[nn].next[j] == n) {
                            nodes[nn].next.erase(nodes[nn].next.begin() + j);
                            --j;
                        }
                    }
                    for (int j = 0; j < nodes[n].next.size(); ++j) {
                        if (nodes[n].next[j] == nn) {
                            nodes[n].next.erase(nodes[n].next.begin() + j);
                            --j;
                        }
                    }
                    for (int j = 0; j < nodes[n].next.size(); ++j) {
                        if (nodes[n].next[j] == nodeHash(it.x, it.y)) {
                            nodes[n].next.erase(nodes[n].next.begin() + j);
                            --j;
                        }
                    }
                    for (int j = 0; j < it.next.size(); ++j) {
                        if (it.next[j] == n) {
                            it.next.erase(it.next.begin() + j);
                            --j;
                        }
                    }
                    deleted = true;
                    break;
                }
                if ((kind2 != -1 && kind2 == kind))
                    break;
            }
        }
        Log("deleted lines: " << deletedLines << ", endedat: " << endedat);
    } while ((deleted && deletedLines < 2000000) || deletedLines == endedat - 1);
    if (onlyStep) {
        onlyStep = false;
        goto label;
    }
    deleted = false;
    int deletedDuplicats = 0;
    do {
        deleted = false;
        for (auto& alele : nodes) {
            auto& it = alele.second;
            for (int i = 0; i < it.next.size(); ++i) {
                if (it.next[i] == nodeHash(it.x, it.y)) {
                    deletedDuplicats++;
                    deleted = true;
                    it.next.erase(it.next.begin() + i);
                    --i;
                }
            }
        }
        Log("deleted duplicats: " << deletedDuplicats);
    } while (deleted && 0);

    int deletedEmpty = 0;
    for (auto it = nodes.begin(); it != nodes.end();) {
        if (it->second.next.size() == 0) {
            it = nodes.erase(it);
            deletedEmpty++;
        }
        else {
            ++it;
        }
    }
    Log("deleted empty: " << deletedEmpty);
    Log("nodes size: " << nodes.size());
    std::fstream f;
    f.open("generated_data/SeaBordersData.txt", std::fstream::out);
    int dell = 0;
    for (auto& alel : nodes) {
        auto& it = alel.second;
        for (int i = 0; i < it.next.size(); ++i) {
            int n = it.next[i];
            f << it.x << " " << it.y << " " << nodes[n].x << " " << nodes[n].y << '\n';

            for (int j = 0; j < nodes[n].next.size(); ++j) {
                if (nodes[n].next[j] == nodeHash(it.x, it.y)) {
                    nodes[n].next.erase(nodes[n].next.begin() + j);
                    dell++;
                    --j;
                }
            }
        }
    }
    f.close();
    Log("dell = " << dell);

}

void createSaveProvPoints(const unsigned char* pix, int ww, int hh, std::vector<ProvData>& provD,
                          std::map<unsigned int, int>& colorToId)
{
    unsigned char* pix2 = new unsigned char[ww * hh * 3];
    for (int i = 0; i < ww * hh * 3; ++i) {
        pix2[i] = pix[i];
    }

    std::vector<std::pair<int, int>> points[provD.size()];

    Color black{0, 0, 0, 255};
    for (int y = 0; y < hh; ++y) {
        for (int x = 0; x < ww; ++x) {
            int ind = x * 3 + y * ww * 3;
            Color col{pix2[ind + 0], pix2[ind + 1], pix2[ind + 2], 255};
            if (col == black)
                continue;

            int phash = getHash(col.r, col.g, col.b);
            if (colorToId.find(phash) != colorToId.end()) {
                int id = colorToId[phash];
                points[id].push_back(std::make_pair(x, y));
                Log(id);
            }

            std::queue<std::pair<int, int>> que;
            que.push(std::make_pair(x, y));
            while (que.size()) {
                auto pair = que.front();
                que.pop();
                int xx = pair.first;
                int yy = pair.second;
                if (xx >= ww)
                    xx = 0;
                if (yy >= hh)
                    yy = 0;
                ind = xx * 3 + yy * ww * 3;
                Color col2{pix2[ind + 0], pix2[ind + 1], pix2[ind + 2], 255};
                if (col2 != col || col2 == black)
                    continue;
                pix2[ind + 0] = 0;
                pix2[ind + 1] = 0;
                pix2[ind + 2] = 0;

                int xxx = xx + 1;
                int yyy = yy;
                if (xxx >= ww)
                    xxx = 0;
                if (yyy >= hh)
                    yyy = 0;
                ind = xxx * 3 + yyy * ww * 3;
                col2 = Color{pix2[ind + 0], pix2[ind + 1], pix2[ind + 2], 255};
                if (col2 == col)
                    que.push(std::make_pair(xxx, yyy));

                xxx = xx - 1;
                yyy = yy;
                if (xxx >= ww)
                    xxx = 0;
                if (xxx < 0)
                    xxx = ww - 1;
                if (yyy >= hh)
                    yyy = 0;
                ind = xxx * 3 + yyy * ww * 3;
                col2 = Color{pix2[ind + 0], pix2[ind + 1], pix2[ind + 2], 255};
                if (col2 == col)
                    que.push(std::make_pair(xxx, yyy));

                xxx = xx;
                yyy = yy + 1;
                if (xxx >= ww)
                    xxx = 0;
                if (xxx < 0)
                    xxx = ww - 1;
                if (yyy >= hh)
                    yyy = 0;
                ind = xxx * 3 + yyy * ww * 3;
                col2 = Color{pix2[ind + 0], pix2[ind + 1], pix2[ind + 2], 255};
                if (col2 == col)
                    que.push(std::make_pair(xxx, yyy));

                xxx = xx;
                yyy = yy - 1;
                if (xxx >= ww)
                    xxx = 0;
                if (xxx < 0)
                    xxx = ww - 1;
                if (yyy >= hh)
                    yyy = 0;
                if (yyy < 0)
                    yyy = y - 1;
                ind = xxx * 3 + yyy * ww * 3;
                col2 = Color{pix2[ind + 0], pix2[ind + 1], pix2[ind + 2], 255};
                if (col2 == col)
                    que.push(std::make_pair(xxx, yyy));
            }
        }
    }

    delete [] pix2;

    std::fstream f;
    f.open("generated_data/ProvPointsData.txt", std::fstream::out);
    for (std::size_t i = 0; i < provD.size(); ++i) {
        f << "id: " << i << "\n";
        for (std::size_t j = 0; j < points[i].size(); ++j) {
            f << std::to_string(points[i][j].first) << " " << std::to_string(points[i][j].second) << " ";
        }
        f << "\n";
    }
    f.close();
}
