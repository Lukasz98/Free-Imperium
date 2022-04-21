#include "save_data.h"

#include <GL/glew.h>
#include <png.h>
#include <queue>
#include <cassert>

#include "color.h"
#include "ctr_data.h"
#include "prov_data.h"

void savePng(unsigned char* pix, int w, int h);

void saveProvinceFromImg(const unsigned char* provs, const unsigned char* water,
                         int w, int h)
{
    std::fstream f;
    f.open("../shared/generated_data/ProvDataTest.txt", std::fstream::out);
    std::map<unsigned int, ProvData> pmap;
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            int windex = x * 3 + (w * y) * 3;
            int index = x * 3 + (w * y) * 3;
            if (water[windex + 0] == 0 && water[windex + 1] == 0 &&
                water[windex + 2] == 0)
                continue;
            unsigned int hash =
                getHash(provs[index + 0], provs[index + 1], provs[index + 2]);
            if (pmap.find(hash) == pmap.end()) {
                ProvData pd;
                pd.id = pmap.size();
                pd.name = "Province" + std::to_string(pmap.size());
                pd.r = provs[index + 0];
                pd.g = provs[index + 1];
                pd.b = provs[index + 2];
                pd.x = x;
                pd.y = y;
                if (water[windex + 0] == 68)
                    pd.water = true;
                else
                    pd.water = false;
                pmap[hash] = pd;
            }
        }
    }
    
    {  // find centered x, y (centered enough)
        bool * visited = (bool*)malloc(w * h);
        memset(visited, 0, w * h);////= {0};
        for (auto& pdd : pmap) {
            auto& pd = pdd.second;
            int index = pd.x * 3 + pd.y * w * 3;
            int index2 = pd.x + pd.y * w;
            long first = (long)pd.x | ((long)(pd.y) << 32);
            std::queue<long> que;
            que.push(first);
            int last = first;

            int bestsc = 0;

            while (que.size()) {
                long cur = que.front();
                que.pop();
                int xx = cur & 0xFFFFFFFF;
                int yy = cur >> 32;

                int index = xx * 3 + yy * w * 3;
                int index2 = xx + yy * w;
                if (visited[index2] != true && provs[index + 0] == pd.r &&
                    provs[index + 1] == pd.g && provs[index + 2] == pd.b)
                {
                    {  // add neighbours
                        // up
                        int nv = (yy + 1) % h;
                        long hash = (long)xx | ((long)(nv) << 32);
                        que.push(hash);
                        // down
                        nv = (yy - 1 >= 0 ? yy - 1 : h + (yy - 1));
                        hash = (long)xx | ((long)(nv) << 32);
                        que.push(hash);
                        // left
                        nv = (xx - 1 >= 0 ? xx - 1 : w + (xx - 1));
                        hash = (long)nv | ((long)(yy) << 32);
                        que.push(hash);
                        // right
                        nv = (xx + 1) % w;
                        hash = (long)nv | ((long)(yy) << 32);
                        que.push(hash);
                    }
                    //calc score
                    int leftsc = 0, rightsc = 0, upsc = 0, downsc = 0;
                    int xxx = xx, yyy = yy;
                    index = xx * 3 + yy * w * 3;
                    while (provs[index + 0] == pd.r && provs[index + 1] == pd.g && provs[index + 2] == pd.b) {
                        ++leftsc;
                        xxx = (xxx - 1 >= 0 ? xxx - 1 : w + (xxx - 1));
                        index = xxx * 3 + yyy * w * 3;
                    }
                    xxx = xx, yyy = yy;
                    index = xx * 3 + yy * w * 3;
                    while (provs[index + 0] == pd.r && provs[index + 1] == pd.g && provs[index + 2] == pd.b) {
                        ++rightsc;
                        xxx = (xxx + 1) % w;
                        index = xxx * 3 + yyy * w * 3;
                    }
                    xxx = xx, yyy = yy;
                    index = xx * 3 + yy * w * 3;
                    while (provs[index + 0] == pd.r && provs[index + 1] == pd.g && provs[index + 2] == pd.b) {
                        ++upsc;
                        yyy = (yyy + 1) % h;
                        index = xxx * 3 + yyy * w * 3;
                    }
                    xxx = xx, yyy = yy;
                    index = xx * 3 + yy * w * 3;
                    while (provs[index + 0] == pd.r && provs[index + 1] == pd.g && provs[index + 2] == pd.b) {
                        ++downsc;
                        yyy = (yyy - 1 >= 0 ? yyy - 1 : h + (yyy - 1));
                        index = xxx * 3 + yyy * w * 3;
                    }
                    
                    int sc = (downsc + upsc) * (leftsc + rightsc);
                    if (bestsc < sc) {
                        bestsc = sc;
                        pd.x = xx;
                        pd.y = yy;
                    }

                    visited[index2] = true;
                }
            }
        }
        free(visited);
    }
    
    Color lastC = {0, 0, 0, 255};
    unsigned int lastHash = 0;
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            int index = x * 3 + (w * y) * 3;
            Color col = {provs[index + 0], provs[index + 1], provs[index + 2],
                         255};
            unsigned int hash = getHash(col.r, col.g, col.b);
            if (col == lastC || x == 0) {
                lastC = col;
                lastHash = hash;
                continue;
            }
            if (pmap.find(hash) == pmap.end()) {
                Log("Provs map error at" << x << " " << y );
            }
            if (pmap[lastHash].water)
                pmap[hash].neighbSea.insert(pmap[lastHash].id);
            else
                pmap[hash].neighb.insert(pmap[lastHash].id);

            if (pmap[hash].water)
                pmap[lastHash].neighbSea.insert(pmap[hash].id);
            else
                pmap[lastHash].neighb.insert(pmap[hash].id);

            lastHash = hash;
            lastC = col;
        }
    }
    for (int x = 0; x < w; ++x) {
        for (int y = 0; y < h; ++y) {
            int index = x * 3 + (w * y) * 3;
            Color col = {provs[index + 0], provs[index + 1], provs[index + 2],
                         255};
            unsigned int hash = getHash(col.r, col.g, col.b);
            if (col == lastC || x == 0) { // shouldnt here be y == 0?
                lastC = col;
                lastHash = hash;
                continue;
            }
            if (pmap.find(hash) == pmap.end()) {
                Log("Provs map error at" << x << " " << y );
            }
            if (pmap[lastHash].water)
                pmap[hash].neighbSea.insert(pmap[lastHash].id);
            else
                pmap[hash].neighb.insert(pmap[lastHash].id);

            if (pmap[hash].water)
                pmap[lastHash].neighbSea.insert(pmap[hash].id);
            else
                pmap[lastHash].neighb.insert(pmap[hash].id);

            lastHash = hash;
            lastC = col;
        }
    }
    std::vector<CountryData> ctrs;

    bool visited[pmap.size()];
    memset(visited, 0, pmap.size());

    for (auto& pdd : pmap) {
        auto& pd = pdd.second;
        if (pd.water)
            continue;
        if (visited[pd.id])
            continue;
        visited[pd.id] = true;
        CountryData cd;
        cd.id = ctrs.size();
        int t = ((cd.id * pd.id + 1234) * 231) % 255;
        cd.r = t;
        cd.g = 255 - t;
        cd.b = t / 2;
        cd.r = pd.r;
        cd.g = pd.g;
        cd.b = pd.b;
        ctrs.push_back(cd);
        pd.ctrId = cd.id;
        for (auto i : pd.neighb) {
            if (visited[i])
                continue;
            visited[i] = true;
            for (auto it = pmap.begin(); it != pmap.end(); ++it) {
                if (it->second.id == i) {
                    it->second.ctrId = cd.id;
                    break;
                }
            }
        }
    }

    for (auto& pd : pmap) {
        f << "{\n";
        f << "id: " << pd.second.id << "\n";
        f << "name: " << pd.second.name << '\n';
        f << "pos: " << pd.second.x << " " << pd.second.y << '\n';
        f << "water: " << pd.second.water << '\n';
        f << "neighb: ";
        for (auto i : pd.second.neighb) f << i << " ";
        f << "\n";
        f << "neighbSea: ";
        for (auto i : pd.second.neighbSea) f << i << " ";
        f << "\n";
        f << "countryId: ";
        f << std::to_string(pd.second.ctrId);
        f << "\n";
        f << "color: ";
        f << std::to_string(pd.second.r) << " ";
        f << std::to_string(pd.second.g) << " ";
        f << std::to_string(pd.second.b) << "\n}\n";
    }
    f.close();

    f.open("../shared/generated_data/CountryDataTest.txt", std::fstream::out);
    for (auto& ctr : ctrs) {
        f << "{\n";
        f << "id: " << ctr.id << "\n";
        f << "color: ";
        f << std::to_string(ctr.r) << " ";
        f << std::to_string(ctr.g) << " ";
        f << std::to_string(ctr.b) << "\n}\n";
    }
    f.close();
}
