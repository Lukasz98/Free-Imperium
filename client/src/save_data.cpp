#include "save_data.h"

#include <png.h>
#include <GL/glew.h>
#include "color.h"
#include <cassert>
#include "prov_data.h"
#include "ctr_data.h"

void savePng(unsigned char * pix, int w, int h);

void saveProvinceFromImg(const unsigned char* provs, const unsigned char* water, int w, int h)
{
    std::fstream f;
    f.open("ProvDataTest.txt", std::fstream::out);
    std::map<unsigned int, ProvData> pmap;
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            int windex = x * 3 + (w * y) * 3;
            int index = x * 3 + (w * y) * 3;
            if (water[windex + 0] == 0 && water[windex + 1] == 0 && water[windex + 2] == 0)
                continue;
            unsigned int hash = getHash(provs[index + 0], provs[index + 1], provs[index + 2]);
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
            else {  // sprawdz czy na pewno jest woda
                // if (water[index + 0] == 255)
                //    pmap[hash].water = false;
                // else if (water[index + 0] == 68)
                //    pmap[hash].water = true;
            }
        }
    }
    Color lastC = {0, 0, 0, 255};
    unsigned int lastHash = 0;
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            int index = x * 3 + (w * y) * 3;
            Color col = {provs[index + 0], provs[index + 1], provs[index + 2], 255};
            unsigned int hash = getHash(col.r, col.g, col.b);
            if (col == lastC || x == 0) {
                lastC = col;
                lastHash = hash;
                continue;
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
            Color col = {provs[index + 0], provs[index + 1], provs[index + 2], 255};
            unsigned int hash = getHash(col.r, col.g, col.b);
            if (col == lastC || x == 0) {
                lastC = col;
                lastHash = hash;
                continue;
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

    bool visited[pmap.size()] = {0};

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

    f.open("CountryDataTest.txt", std::fstream::out);
    for (auto& ctr : ctrs) {
        f << "{\n";
        f << "id: " << ctr.id << "\n";
        f << "color: ";
        f << std::to_string(ctr.r) << " ";
        f << std::to_string(ctr.g) << " ";
        f << std::to_string(ctr.b) << "\n}\n";
    }
    f.close();

    unsigned char * ctrPix = new unsigned char[w * h * 3];
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            int index = x * 3 + (w * y) * 3;
            unsigned int hash = getHash(provs[index + 0], provs[index + 1], provs[index + 2]);
            if (pmap.find(hash) == pmap.end())
                continue;
            if (pmap[hash].water) {
                ctrPix[index + 0] = 255;
                ctrPix[index + 1] = 255;
                ctrPix[index + 2] = 255;
            }
            else {
                int ctrId = pmap[hash].ctrId;
                assert(ctrId >= 0 && ctrId < ctrs.size());
                ctrPix[index + 0] = ctrs[ctrId].r;
                ctrPix[index + 1] = ctrs[ctrId].g;
                ctrPix[index + 2] = ctrs[ctrId].b;
            }
            
        }
    }

    savePng(ctrPix, w, h);

    delete [] ctrPix;
}

void savePng(unsigned char * pix, int w, int h)
{
    const char * path = "src/img/countries_map.png";
    FILE* file = fopen(path, "wb");

    if (!file) {
        printf("Cannot open file: %s\n", path);
        return;
    }

    png_structp pngPtr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (pngPtr == NULL) {
        printf("Error while creating png write struct\n");
        fclose(file);
        return;
    }

    png_infop infoPtr = png_create_info_struct(pngPtr);
    if (infoPtr == NULL) {
        printf("Error while creating png info struct\n");
        fclose(file);
        png_destroy_write_struct(&pngPtr, &infoPtr);
        return;
    }

    /* domyslna metoda obslugi bledow, zalecana przez biblioteke libpng */
    if (setjmp(png_jmpbuf(pngPtr))) {
        printf("Error");
        fclose(file);
        png_destroy_write_struct(&pngPtr, &infoPtr);
        return;
    }

    int pixelSize = 3;
    int depth = 8;

    /* ustawienie atrybutow obrazu */
    png_set_IHDR(pngPtr, infoPtr, w, h, depth, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    png_byte** rowPointers = (png_byte **)png_malloc(pngPtr, h * sizeof(png_byte*));
    for (size_t y = 0; y < h; ++y) {
        png_byte* row = (png_byte *)png_malloc(pngPtr, sizeof(uint8_t) * w * pixelSize);
        rowPointers[y] = row;
        for (size_t x = 0; x < w; ++x) {
            //unsigned char * pixel = pix + w * y + x;
            int index = w * (h - y - 1) * pixelSize + x * pixelSize;
            *row++ = pix[index + 0];
            *row++ = pix[index + 1];
            *row++ = pix[index + 2];
        }
    }

    /* Zapis do pliku */
    png_init_io(pngPtr, file);
    png_set_rows(pngPtr, infoPtr, rowPointers);
    png_write_png(pngPtr, infoPtr, PNG_TRANSFORM_IDENTITY, NULL);

    for (size_t y = 0; y < h; ++y) {
        png_free(pngPtr, rowPointers[y]);
    }
    png_free(pngPtr, rowPointers);

    png_destroy_write_struct(&pngPtr, &infoPtr);
    fclose(file);
}