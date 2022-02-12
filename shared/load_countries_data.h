#pragma once
#include <map>
#include <string.h>
#include <stdio.h>
#include "ctr_data.h"
#include "prov_data.h"
#include "load_values_from_txt.h"

inline unsigned int getHash(unsigned char r, unsigned char g, unsigned char b)
{
    unsigned int res = (unsigned int)r;
    res |= ((unsigned int)g << 8);
    res |= ((unsigned int)b << 16);
    return res;
}

inline std::vector<CountryData> LoadCountriesData()
{
    std::vector<CountryData> countries;
    unsigned int lineCount = 0;
    lineCount = 0;
    std::string fname{"../shared/CountryDataTest.txt"};
    //std::fstream f;
    //f.open(fname, std::fstream::in);
    //if (f.is_open() == false)
    //   Log("Cannot read countries file");
    
    FILE *f = fopen(fname.c_str(), "r");
    if (f == NULL)
       Log("Cannot read countries file");

    char *line = (char*)malloc(1024);
    char * lineptr = NULL;
    size_t n = 1024;

    while (getline(&line, &n, f) > 0) {
        lineptr = line;
        ++lineCount;
        char* ptr = strtok(lineptr, " \n");
        if (strcmp(ptr, "{") != 0) {
            Log("ERROR -> fname: " << fname << ", line: " << lineCount);
            break;
        }

        CountryData ctr;
        while (getline(&line, &n, f) > 0) {
            lineptr = line;
            ++lineCount;
            ptr = strtok(lineptr, " \n");
            if (strcmp(ptr, "}") == 0) {
                countries.emplace_back(CountryData{.id = ctr.id, .r = ctr.r, .g = ctr.g, .b = ctr.b});

                // ctrsData.push_back(ctr);
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
    free(line);
    //free(lineptr);
    fclose(f);
    //f.close();
    return countries;
}

inline std::vector<ProvData> LoadProvincesData(std::map<unsigned int, int>& colorToId)
{
    std::vector<ProvData> provinces;
    unsigned int lineCount = 0;
    lineCount = 0;
    std::string fname{"../shared/ProvDataTest.txt"};
    //std::fstream f;
    //f.open(fname, std::fstream::in);
    //std::string line;
    FILE *f = fopen(fname.c_str(), "r");
    if (f == NULL)
       Log("Cannot read countries file");

    char *line = (char*)malloc(1024);
    char *lineptr;
    size_t n = 1024;

    while (getline(&line, &n, f) > 0) {
        //line = lineptr;
        lineptr = line;
    //while (getline(f, line)) {
        ++lineCount;
        char* ptr = strtok(lineptr, " \n");
        if (strcmp(ptr, "{") != 0) {
            Log("ERROR -> fname: " << fname << ", line: " << lineCount);
            break;
        }

        ProvData pd;
        //while (getline(f, line)) {
        while (getline(&line, &n, f) > 0) {
            lineptr = line;
            ++lineCount;
            ptr = strtok(lineptr, " \n");
            if (strcmp(ptr, "}") == 0) {
                //provinces.push_back(pd);
                //provinces.emplace_back(std::make_shared<Province>(pd.id, pd.r, pd.g, pd.b, pd.x, pd.y, pd.name, pd.ctrId));
               // provinces.emplace_back(ProvData{.name = pd.name, .r = pd.r, .g = pd.g, .b = pd.b, .id = pd.id, .ctrId = pd.ctrId, .x = pd.x, .y = pd.y});
                provinces.emplace_back(pd);
                colorToId[getHash(pd.r, pd.g, pd.b)] = pd.id;
                break;
            }
            else if (strcmp(ptr, "id:") == 0) {
                pd.id = loadInt(ptr);
            }
            else if (strcmp(ptr, "name:") == 0) {
                ptr = strtok(NULL, " \n");
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
                while ((ptr = strtok(NULL, " \n")) != NULL) {
                    pd.neighb.insert(loadInt2(ptr));
                }
            }
            else if (strcmp(ptr, "neighbSea:") == 0) {
                while ((ptr = strtok(NULL, " \n")) != NULL) {
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
    free(line);
    fclose(f);
    //f.close();
    //std::sort(provinces.begin(), provinces.end(), [](ProvData a, ProvData b) { return a.id < b.id; });
    return provinces;
}
