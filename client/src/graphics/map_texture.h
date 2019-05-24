#pragma once
#include "texture.h"
#include "../color.h"


class MapTexture : public Texture
{
    unsigned char * countryPixels, * countryPixelsOrigin;

    void drawProvinceBorders();

public:
    MapTexture(std::string path, int w, int h);
    MapTexture(const MapTexture &) = delete;
    virtual ~MapTexture();
    
    void BrightenProvince(Color bef, int increase);
    void SwapColors(Color bef, Color aft);
    void DrawCountries(std::vector<std::pair<std::string, Color>> cColor, /* to much commas, sussages in my head */ std::vector<std::pair<Color, std::string>> pColor);
    
    const unsigned char * GetPixels() { return countryPixels; };
    
};
