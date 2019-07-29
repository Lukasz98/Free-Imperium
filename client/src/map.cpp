#include "map.h"

Map::Map()
    : Rectangle(glm::vec3(0.0, 0, -.5), glm::vec2(1920, 1080))
{
    mapTexture = std::make_shared<MapTexture>("../shared/map.png", 1920, 1080);
    SetTexture(mapTexture);
}
Map::~Map()
{
}

Color Map::ClickOnProvince(int x, int y)
{
    //Log("mouse in world " << x << " " << y);
    Color provinceColor = { 1, 1, 1, 1 };
    
    const unsigned char * pixels = mapTexture->GetOriginPixels();
    int num = (int)(originW * (y - 1) + x);

    if (num * 4 + 3 > 1920 * 1080 * 4 + 3 || y - 1 < 0)
        return provinceColor;

    Color before{
                 pixels[num * 4 + 0],
                 pixels[num * 4 + 1],
                 pixels[num * 4 + 2],
                 pixels[num * 4 + 3]
                };
    Color after{0, 0, 0, 255};
    int increase = 60;
    
    ((MapTexture*)(mapTexture.get()))->BrightenProvince(before, increase);

    provinceColor = before;
    return provinceColor;
}

void Map::DrawSieged(const Color & prov, const Color & siege)
{
    ((MapTexture*)(mapTexture.get()))->DrawSieged(prov, siege);
}

