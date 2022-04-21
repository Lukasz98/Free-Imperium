#include "load_data.h"

#include <cassert>

unsigned int getHash(unsigned char r, unsigned char g, unsigned char b)
{
    unsigned int res = (unsigned int)r;
    res |= ((unsigned int)g << 8);
    res |= ((unsigned int)b << 16);
    return res;
}

void loadProvData(std::vector<ProvData>& provinces, std::map<unsigned int, int>& colorToId)
{
    unsigned int lineCount = 0;
    lineCount = 0;
    std::string fname{"../shared/generated_data/ProvDataTest.txt"};
    std::fstream f;
    f.open(fname, std::fstream::in);
    std::string line;
    while (getline(f, line)) {
        ++lineCount;
        char* ptr = strtok(line.data(), " ");
        if (strcmp(ptr, "{") != 0) {
            Log("ERROR -> fname: " << fname << ", line: " << lineCount);
            break;
        }

        ProvData pd;
        while (getline(f, line)) {
            ++lineCount;
            ptr = strtok(line.data(), " ");
            if (strcmp(ptr, "}") == 0) {
                provinces.push_back(pd);
                colorToId[getHash(pd.r, pd.g, pd.b)] = pd.id;
                break;
            }
            else if (strcmp(ptr, "id:") == 0) {
                pd.id = loadInt(ptr);
            }
            else if (strcmp(ptr, "name:") == 0) {
                ptr = strtok(NULL, " ");
                pd.name = std::string{ptr};
            }
            else if (strcmp(ptr, "pos:") == 0) {
                Vec2 v2 = loadVec2(ptr);
                pd.x = v2.x;
                pd.y = v2.y;
            }
            else if (strcmp(ptr, "water:") == 0) {
                pd.water = loadInt(ptr);
            }
            else if (strcmp(ptr, "color:") == 0) {
                pd.r = loadInt(ptr);
                pd.g = loadInt(ptr);
                pd.b = loadInt(ptr);
            }
            else if (strcmp(ptr, "neighb:") == 0) {
                while ((ptr = strtok(NULL, " ")) != NULL) {
                    pd.neighb.insert(loadInt2(ptr));
                }
            }
            else if (strcmp(ptr, "neighbSea:") == 0) {
                while ((ptr = strtok(NULL, " ")) != NULL) {
                    pd.neighbSea.insert(loadInt2(ptr));
                }
            }
            else if (strcmp(ptr, "countryId:") == 0) {
                pd.ctrId = loadInt(ptr);
            }
            else {
                Log("cos tu nie gra: file: " << fname << ", line: " << lineCount);
                break;
            }
        }
    }
    f.close();
    std::sort(provinces.begin(), provinces.end(), [](ProvData a, ProvData b) { return a.id < b.id; });
}

void loadCountriesData(std::vector<CountryData>& ctrsData)
{
    unsigned int lineCount = 0;
    lineCount = 0;
    std::string fname{"../shared/generated_data/CountryDataTest.txt"};
    std::fstream f;
    f.open(fname, std::fstream::in);
    std::string line;
    while (getline(f, line)) {
        ++lineCount;
        char* ptr = strtok(line.data(), " ");
        if (strcmp(ptr, "{") != 0) {
            Log("ERROR -> fname: " << fname << ", line: " << lineCount);
            break;
        }

        CountryData ctr;
        while (getline(f, line)) {
            ++lineCount;
            ptr = strtok(line.data(), " ");
            if (strcmp(ptr, "}") == 0) {
                ctrsData.push_back(ctr);
                break;
            }
            else if (strcmp(ptr, "id:") == 0) {
                ctr.id = loadInt(ptr);
            }
            else if (strcmp(ptr, "color:") == 0) {
                ctr.r = loadInt(ptr);
                ctr.g = loadInt(ptr);
                ctr.b = loadInt(ptr);
            }
            else {
                Log("cos tu nie gra: file: " << fname << ", line: " << lineCount);
                break;
            }
        }
    }
    f.close();
}

void loadProvPointsData(std::vector<std::pair<int, int>>* points, int size)
{
    unsigned int lineCount = 0;
    lineCount = 0;
    std::string fname{"generated_data/ProvPointsData.txt"};
    std::fstream f;
    f.open(fname, std::fstream::in);
    std::string line;
    while (getline(f, line)) {
        ++lineCount;
        char* ptr = strtok(line.data(), " ");
        if (strcmp(ptr, "id:") != 0) {
            Log("ERROR -> fname: " << fname << ", line: " << lineCount);
            break;
        }
        int id = -1;
        ptr = strtok(NULL, " ");
        if (ptr == NULL) {
            Log("cos tu nie gra: file: " << fname << ", line: " << lineCount);
            break;
        }
        id = atoi(ptr);
        assert(id >= 0 && id < size);

        getline(f, line);
        ptr = strtok(line.data(), " ");

        while (ptr != NULL) {
            int x = atoi(ptr);
            ptr = strtok(NULL, " ");
            if (ptr == NULL) {
                Log("cos tu nie gra: file: " << fname << ", line: " << lineCount);
                break;
            }
            int y = atoi(ptr);
            points[id].push_back(std::make_pair(x, y));
            ptr = strtok(NULL, " ");
        }
    }
    f.close();
}
