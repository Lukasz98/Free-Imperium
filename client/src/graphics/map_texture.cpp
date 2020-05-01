#include "map_texture.h"

MapTexture::MapTexture(std::string path, int w, int h)
    :Texture(path, w, h)
{
    countryPixels = new unsigned char[originW * originH * 4];
    countryPixelsOrigin = new unsigned char[originW * originH * 4];
    brightenProvPixels = new unsigned char[originW * originH * 4];
    std::copy(pixelsOrigin, pixelsOrigin + originW * originH * 4, countryPixels);
    std::copy(pixelsOrigin, pixelsOrigin + originW * originH * 4, brightenProvPixels);
    //drawProvinceBorders();
}

MapTexture::~MapTexture()
{
    delete [] countryPixels;
    delete [] countryPixelsOrigin;
    delete [] brightenProvPixels;
}

void MapTexture::drawProvinceBorders()
{
    Color white{ 255, 255, 255, 255 };
    Color black{ 0, 0, 0, 255 };
    int lastId = 0;
    for (int i = 0; i < originW * originH * 4 - 3; i += 4) {  // horizontaly 
        Color lastColor{ countryPixels[lastId + 0], countryPixels[lastId + 1], countryPixels[lastId + 2], countryPixels[lastId + 3] };
        Color currentColor{ countryPixels[i + 0], countryPixels[i + 1], countryPixels[i + 2], countryPixels[i + 3] };

        if (lastColor != currentColor && currentColor != black && lastColor != black) {
            countryPixels[lastId + 0] = black.r;
            countryPixels[lastId + 1] = black.g;
            countryPixels[lastId + 2] = black.b;
            countryPixels[lastId + 3] = black.a;                
            countryPixels[i + 0] = black.r;
            countryPixels[i + 1] = black.g;
            countryPixels[i + 2] = black.b;
            countryPixels[i + 3] = black.a;                
        }
        lastId = i;
    }
        
    lastId = 0;
    for (int x = 0; x < originW * 4 - 1; x += 4) {  // verticaly
        for (int y = 0; y < originH * 4 - 1; y += 4) {
            int index = y * originW + x;
            Color lastColor{ countryPixels[lastId + 0], countryPixels[lastId + 1], countryPixels[lastId + 2], countryPixels[lastId + 3] };
            Color currentColor{ countryPixels[index + 0], countryPixels[index + 1], countryPixels[index + 2], countryPixels[index + 3] };

            if (lastColor != currentColor && currentColor != black && lastColor != black) {
                countryPixels[lastId + 0] = black.r;
                countryPixels[lastId + 1] = black.g;
                countryPixels[lastId + 2] = black.b;
                countryPixels[lastId + 3] = black.a;                
                countryPixels[index + 0] = black.r;
                countryPixels[index + 1] = black.g;
                countryPixels[index + 2] = black.b;
                countryPixels[index + 3] = black.a;                
            }
            lastId = index;
        }            
    }
        
    std::copy(countryPixels, countryPixels + originW * originH * 4, countryPixelsOrigin);
    Bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, originW, originH, 0, GL_RGBA, GL_UNSIGNED_BYTE, countryPixels);
    Unbind();
}

void MapTexture::BrightenProvince(Color bef, int increase)
{
    std::copy(countryPixels, countryPixels + originW * originH * 4, brightenProvPixels);

    for (int i = 0; i < originW * originH * 4 - 4; i += 4) {
        Color current{ pixelsOrigin[i + 0], pixelsOrigin[i + 1], pixelsOrigin[i + 2], pixelsOrigin[i + 3] };
        if (current == bef) {
            Color after{
                      countryPixels[i + 0],
                      countryPixels[i + 1],
                      countryPixels[i + 2],
                      countryPixels[i + 3]
                    };
            
            if (increase + after.r > 255) after.r = 255;
            else after.r = after.r + increase;

            if (increase + after.g > 255) after.g = 255;
            else after.g = after.g + increase;

            if (increase + after.b > 255) after.b = 255;
            else after.b = after.b + increase;

            brightenProvPixels[i + 0] = after.r;
            brightenProvPixels[i + 1] = after.g;
            brightenProvPixels[i + 2] = after.b;
            brightenProvPixels[i + 3] = after.a;                  
        }
    }
    Bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D,0, GL_RGBA, originW, originH, 0, GL_RGBA, GL_UNSIGNED_BYTE, brightenProvPixels);
    Unbind();
}
    
void MapTexture::SwapColors(Color bef, Color aft)
{
    for (int i = 0; i < originW * originH * 4 - 4; i += 4) {
        Color current{ pixelsOrigin[i + 0], pixelsOrigin[i + 1], pixelsOrigin[i + 2], pixelsOrigin[i + 3] };
        if (current == bef) {
            countryPixels[i + 0] = aft.r;
            countryPixels[i + 1] = aft.g;
            countryPixels[i + 2] = aft.b;
            countryPixels[i + 3] = aft.a;                  
        }
    }
    Bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D,0, GL_RGBA, originW, originH, 0, GL_RGBA, GL_UNSIGNED_BYTE, countryPixels);
    Unbind();
}

void MapTexture::DrawCountries(std::vector<std::pair<std::string, Color>> cColor, /* to much commas, sussages in my head */ std::vector<std::pair<Color, std::string>> pColor)
{
    std::vector<std::pair<Color, Color>> rainbow;
    for (auto & p : pColor) {
        for (auto & c : cColor) {
            if (p.second == c.first) {
                rainbow.push_back(std::make_pair(p.first, c.second));
                break;
            }
        }    
    }
    Color white{255,255,255,255};
    Color black{0,0,0,255};
    Color lastColor{255,255,255,255};
    Color lastUsedColor{255,255,255,255};
    for (int i = 0; i < originW * originH * 4 - 3; i += 4) {
        Color currentColor{ pixelsOrigin[i + 0], pixelsOrigin[i + 1], pixelsOrigin[i + 2], pixelsOrigin[i + 3] };
        Color borderMapCol{ countryPixels[i + 0], countryPixels[i + 1], countryPixels[i + 2], countryPixels[i + 3] };
            
        if (borderMapCol == white || borderMapCol == black)
            continue;
        if (currentColor == white || currentColor == black)
            continue;
        
        Color newColor = white;
        if (currentColor == lastColor)
            newColor = lastUsedColor;
        else {
            for (auto r : rainbow) {
                if (r.first == currentColor) {
                    newColor = r.second;
                    break;
                }
            }
        }

        if (newColor != white) {
            countryPixels[i + 0] = newColor.r;
            countryPixels[i + 1] = newColor.g;
            countryPixels[i + 2] = newColor.b;
            countryPixels[i + 3] = newColor.a;                
        }
        
        lastColor = currentColor;
        lastUsedColor = newColor;
    }
        
    std::copy(countryPixels, countryPixels + originW * originH * 4, countryPixelsOrigin);
    Bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, originW, originH, 0, GL_RGBA, GL_UNSIGNED_BYTE, countryPixels);
    Unbind();
}

void MapTexture::DrawSieged(const Color & prov, const Color & siege)
{
    Color black{0,0,0,255};
    int rowCount = 0;
    
    for (int i = 0; i < originW * originH * 4 - 4; i += 4) {
        Color current{ pixelsOrigin[i + 0], pixelsOrigin[i + 1], pixelsOrigin[i + 2], pixelsOrigin[i + 3] };
        Color wysiwyg{ countryPixels[i + 0], countryPixels[i + 1], countryPixels[i + 2], countryPixels[i + 3] };
        if (current == prov && rowCount % 3 == 0 && wysiwyg != black) {
            Color after = siege;
            
            countryPixels[i + 0] = after.r;
            countryPixels[i + 1] = after.g;
            countryPixels[i + 2] = after.b;
            countryPixels[i + 3] = after.a;                  
        }
        if (i % (originW * 4) == 0)
            rowCount++;
    }
    Bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D,0, GL_RGBA, originW, originH, 0, GL_RGBA, GL_UNSIGNED_BYTE, countryPixels);
    Unbind();    
}
