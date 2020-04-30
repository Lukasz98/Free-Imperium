#pragma once
#include "graphics/rectangle.h"
#include "graphics/map_texture.h"
#include "log.h"

class Map : public Rectangle
{
    public:
    int scale = 1;
    int originW = 1920, originH = 1088;
    std::shared_ptr<Texture> mapTexture;
    
public:
    Map();
    ~Map();

    Color ClickOnProvince(int x, int y);
    void DrawCountries(std::vector<std::pair<std::string, Color>> cColor, /* to much commas, sussages in my head */ std::vector<std::pair<Color, std::string>> pColor)
    {
        ((MapTexture*)(mapTexture.get()))->DrawCountries(cColor, pColor);
    }

    void SwapColors(Color bef, Color aft) { mapTexture->SwapColors(bef, aft); }

    void DrawSieged(const Color & prov, const Color & siege);

    const unsigned char * GetPixels() { return texture->GetOriginPixels(); } 
    int GetScale() { return scale; }
};
