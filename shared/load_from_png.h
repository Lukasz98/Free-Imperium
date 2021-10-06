#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <png.h>

struct RawImg
{
    int width, height;
    unsigned char * data;
    ~RawImg() { delete [] data; }
};

#define TEXTURE_LOAD_ERROR nullptr

unsigned char *  loadTexture(const std::string filename, int & width, int & height, int & bytes);
