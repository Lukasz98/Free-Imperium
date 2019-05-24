#pragma once

#include "load_from_png.h"

class Map
{
    unsigned char * pixels;
    float originW = 1920;
    float originH = 1080;

    
public:
    Map()
    {
        std::string path = "../shared/map.png";
        int w = 0, h = 0, bytes;
        pixels = loadTexture(path, w, h, bytes);
    }

    ~Map()
    {
        delete [] pixels;
    }

    const unsigned char * GetPixels() { return pixels; }
    float GetW() { return originW; }
    float GetH() { return originH; }
    
};
