#if 0
 * #include "map.h"

Map::Map()
    : Rectangle(glm::vec3(0.0, 0, -.5), glm::vec2(1920*4, 1080*4)),
      provsMap("../shared/map.png", 1920, 1088),
      heightMap("../shared/map_height.png", 1920, 1088),
      grassT{"../shared/grass1.png", 64, 64, GL_REPEAT},
      stoneT{"../shared/smoothstone.png", 64, 64, GL_REPEAT},
      water{"../shared/water2.png", 64, 64, GL_REPEAT},
      shader{"src/graphics/shaders/map/vert.v", "src/graphics/shaders/map/frag.f",
               "src/graphics/shaders/map/tes_ster.ts",
               "src/graphics/shaders/map/tes_w.tw"},
      chunkScale{2.0f}
{
    setChunks();
    setProvsBorders();
    for (int i = 0; i < originW * originH * 4; i++) {
        countryMap.push_back(0);
    }
}

Map::~Map()
{
}

void Map::setChunks()
{
    float w = 32.0f;
    //float w = 16.0f;
    float x = 0.0f, y = 0.0f;
    glm::vec2 tCL{w / originW, w / originH};
    glm::vec2 texC{0.0f, 0.0f};
    for (int i = 0; i < originH; i += w) {
        for (int j = 0; j < originW; j += w) {
            chunks.emplace_back(std::make_unique<Chunk>(
                glm::vec3{(float)j * chunkScale, i * chunkScale, -1.0}, glm::vec2{w * chunkScale, w * chunkScale}, texC, tCL
            ));
            texC.x += tCL.x;
        }
        texC.x = 0.0f;
        texC.y += tCL.y;
    }
}

void Map::Draw(Camera & camera, float dt)
{
Log("draw");
    waterTime += dt;
    glUseProgram(shader.GetProgram());
    glPatchParameteri(GL_PATCH_VERTICES, 3);

    glm::mat4 matrix = camera.GetMat();

    glUniformMatrix4fv(glGetUniformLocation(shader.GetProgram(), "matrix"), 1, GL_FALSE, glm::value_ptr(matrix));
    {
        float fov = camera.GetFov();
        float fovDelta = camera.GetFovDelta();// - 0.5f;
        float d = 1.5f - fovDelta;
        float trCount = 30.0f;
        trCount += d * 44;
        //Log(trCount);
        glUniform1f(glGetUniformLocation(shader.GetProgram(), "level"), trCount);
    }
    glUniform1f(glGetUniformLocation(shader.GetProgram(), "waterTime"), waterTime);

    GLuint ts[] = {0, provsMap.GetId(), heightMap.GetId(), grassT.GetId(), stoneT.GetId(), 
                   water.GetId(), countryBorders->GetId(), provsBorders->GetId() };

    glBindTextures(ts[0], 8, ts);
    
    for (auto & chunk : chunks) {
        auto rpos = chunk->rect.GetPosition();
        auto rsize = chunk->rect.GetSize();
        if (camera.IsPointInFrustum(rpos) ||
            camera.IsPointInFrustum(rpos + glm::vec3{rsize.x, 0,0}) ||
            camera.IsPointInFrustum(rpos + glm::vec3{0.0, rsize.y,0}) ||
            camera.IsPointInFrustum(rpos + glm::vec3{rsize.x, rsize.y, 0}))
            chunk->rect.Draw(true);
    }
}

void Map::DrawCountries(std::unordered_map<Color, Color, CCC::Hash> & provCToCountryC)
{
    const unsigned char * provPixels = provsMap.GetPixels();

    /*
        * in following 4 fors :
         * draw country borders
         * with offset coloring, so that border is blury
        * going from 4 directions (left, right, top, bottom)
    */
    int originW = 1920, originH = 1088;
    unsigned char countryBorderPixels[1920 * 1088 * 4];
    std::fill(countryBorderPixels, countryBorderPixels + 1920 * 1088 * 4, 0);
    Color lastColor{255,255,255,255};
    Color lastProvColor{255,255,255,255};
    Color white{255,255,255,255};
    int lastIndex = 0;
    for (int y = 0; y < originH; y++) {
        for (int x = 0; x < originW; x++) {
            int index = y * originW * 4 + x * 4;
            Color provColor{ provPixels[index + 0], provPixels[index + 1], provPixels[index + 2], provPixels[index + 3] };
    
            if (provColor == lastProvColor) {
                lastIndex = index;
                continue;
            }
            
            const auto ctryColIt = provCToCountryC.find(provColor);
            if (ctryColIt == provCToCountryC.end()) {
                lastIndex = index;
                continue;
            }

            if (ctryColIt->second == lastColor) {
                lastIndex = index;
                continue;
            }

            for (int off = 0; off < 3 && x - off >= 0; off++) {
                int offInd = lastIndex - off * 4;
                if (countryBorderPixels[offInd + 3] > 0)
                    continue;
                countryBorderPixels[offInd + 0] = lastColor.r;
                countryBorderPixels[offInd + 1] = lastColor.g;
                countryBorderPixels[offInd + 2] = lastColor.b;
                countryBorderPixels[offInd + 3] = 255 - (off + 1) * 60;
            }
            lastColor = ctryColIt->second;
            lastIndex = index;
        }
    }

    lastIndex = 0;
    lastColor = {255,255,255,255};
    for (int y = 0; y < originH; y++) {
        for (int x = originW - 1; x >= 0; x--) {
            int index = y * originW * 4 + x * 4;
 
            Color provColor{ provPixels[index + 0], provPixels[index + 1], provPixels[index + 2], provPixels[index + 3] };
            if (provColor == lastProvColor) {
                lastIndex = index;
                continue;
            }

            const auto ctryColIt = provCToCountryC.find(provColor);
            if (ctryColIt == provCToCountryC.end()) {
                lastIndex = index;
                continue;
            }

            if (ctryColIt->second == lastColor) {
                lastIndex = index;
                continue;
            }

            for (int off = 0; off < 3 && x + off < originW; off++) {
                int offInd = lastIndex + off * 4;
                if (countryBorderPixels[offInd + 3] > 0)
                    continue;
                countryBorderPixels[offInd + 0] = lastColor.r;
                countryBorderPixels[offInd + 1] = lastColor.g;
                countryBorderPixels[offInd + 2] = lastColor.b;
                countryBorderPixels[offInd + 3] = 255 - (off + 1) * 60;
            }
            lastColor = ctryColIt->second;
            lastIndex = index;
        }
    }
   
    lastIndex = 0;
    lastColor = {255,255,255,255};
    for (int x = 0; x < originW; x++) {
        for (int y = originH - 1; y >= 0; y--) {
            int index = y * originW * 4 + x * 4;
            Color provColor{ provPixels[index + 0], provPixels[index + 1], provPixels[index + 2], provPixels[index + 3] };
            
            if (provColor == lastProvColor) {
                lastIndex = index;
                continue;
            }

            const auto ctryColIt = provCToCountryC.find(provColor);
            if (ctryColIt == provCToCountryC.end()) {
                lastIndex = index;
                continue;
            }

            if (ctryColIt->second == lastColor) {
                lastIndex = index;
                continue;
            }

            for (int off = 0; off < 3 && y + off < originH; off++) {
                unsigned char myAlfa = 255 - (off + 1) * 60;
                unsigned char a = countryBorderPixels[lastIndex + off * originW * 4 + 3];
                if (a > myAlfa)
                //if (a > 0)
                    continue;
                countryBorderPixels[lastIndex + off * originW * 4 + 0] = lastColor.r;
                countryBorderPixels[lastIndex + off * originW * 4 + 1] = lastColor.g;
                countryBorderPixels[lastIndex + off * originW * 4 + 2] = lastColor.b;
                countryBorderPixels[lastIndex + off * originW * 4 + 3] = myAlfa;
            }
            lastColor = ctryColIt->second;
            lastIndex = index;
        }
    }

    lastIndex = 0;
    lastColor = {255,255,255,255};
    for (int x = 0; x < originW; x++) {
        for (int y = 0; y < originH; y++) {
            int index = y * originW * 4 + x * 4;
            Color provColor{ provPixels[index + 0], provPixels[index + 1], provPixels[index + 2], provPixels[index + 3] };
            
            if (provColor == lastProvColor) {
                lastIndex = index;
                continue;
            }

            const auto ctryColIt = provCToCountryC.find(provColor);
            if (ctryColIt == provCToCountryC.end()) {
                lastIndex = index;
                continue;
            }

            if (ctryColIt->second == lastColor) {
                lastIndex = index;
                continue;
            }
            
            for (int off = 0; off < 3 && y - off >= 0; off++) {
                unsigned char myAlfa = 255 - (off + 1) * 60;
                unsigned char a = countryBorderPixels[lastIndex - off * originW * 4 + 3];
                //if (a > 0)
                if (a > myAlfa)
                    continue;
                countryBorderPixels[lastIndex - off * originW * 4 + 0] = lastColor.r;
                countryBorderPixels[lastIndex - off * originW * 4 + 1] = lastColor.g;
                countryBorderPixels[lastIndex - off * originW * 4 + 2] = lastColor.b;
                countryBorderPixels[lastIndex - off * originW * 4 + 3] = myAlfa;
            }
            lastColor = ctryColIt->second;
            lastIndex = index;
        }
    }
    
    countryBorders = std::make_unique<Texture>(countryBorderPixels, 1920, 1088);
}

void Map::setProvsBorders()
{
    const int originW = 1920, originH = 1088;
    const unsigned char * pixelsOrigin = provsMap.GetPixels();
    unsigned char provsPixels[originW * originH * 4];
    std::fill(provsPixels, provsPixels + originW * originH * 4, 0);
    Color lastColor{255,255,255,255};
    Color bColor{50, 50, 50, 55};
    
    /*
        * drawing prov borders to provsBorders texture
        * in each of 4 fors going from four directions
        * (left, right, top, bottom)
    */
    for (int y = 0; y < originH; y++) {
        for (int x = 0; x < originW; x++) {
            int index = y * originW * 4 + x * 4;
            Color currentColor{ pixelsOrigin[index + 0], pixelsOrigin[index + 1], pixelsOrigin[index + 2], pixelsOrigin[index + 3] };
            if (currentColor == lastColor)
                continue;
            lastColor = currentColor;
            int off = 0;
            provsPixels[index + off * 4 + 0] = bColor.r;
            provsPixels[index + off * 4 + 1] = bColor.g;
            provsPixels[index + off * 4 + 2] = bColor.b;
            provsPixels[index + off * 4 + 3] = bColor.a;// - (off + 2) * 60;
        }
    }
    
    lastColor = {255,255,255,255};
    for (int y = 0; y < originH; y++) {
        for (int x = originW - 1; x >= 0; x--) {
            int index = y * originW * 4 + x * 4;
            Color currentColor{ pixelsOrigin[index + 0], pixelsOrigin[index + 1], pixelsOrigin[index + 2], pixelsOrigin[index + 3] };
            if (currentColor == lastColor)
                continue;
            lastColor = currentColor;

            int off = 0;
            provsPixels[index - off * 4 + 0] = bColor.r;
            provsPixels[index - off * 4 + 1] = bColor.g;
            provsPixels[index - off * 4 + 2] = bColor.b;
            provsPixels[index - off * 4 + 3] = bColor.a;//255 - (off + 2) * 60;
        }
    }
    
    lastColor = {255,255,255,255};
    for (int x = 0; x < originW; x++) {
        for (int y = originH - 1; y >= 0; y--) {
            int index = y * originW * 4 + x * 4;
            Color currentColor{ pixelsOrigin[index + 0], pixelsOrigin[index + 1], pixelsOrigin[index + 2], pixelsOrigin[index + 3] };
            if (currentColor == lastColor)
                continue;
            lastColor = currentColor;

            int off = 0;
            provsPixels[index - off * originW * 4 + 0] = bColor.r;
            provsPixels[index - off * originW * 4 + 1] = bColor.g;
            provsPixels[index - off * originW * 4 + 2] = bColor.b;
            provsPixels[index - off * originW * 4 + 3] = bColor.a;//myAlfa;
        }
    }

    lastColor = {255,255,255,255};
    for (int x = 0; x < originW; x++) {
        for (int y = 0; y < originH; y++) {
            int index = y * originW * 4 + x * 4;
            Color currentColor{ pixelsOrigin[index + 0], pixelsOrigin[index + 1], pixelsOrigin[index + 2], pixelsOrigin[index + 3] };
            if (currentColor == lastColor)
                continue;
            lastColor = currentColor;

            int off = 0;
            provsPixels[index + off * originW * 4 + 0] = bColor.r;
            provsPixels[index + off * originW * 4 + 1] = bColor.g;
            provsPixels[index + off * originW * 4 + 2] = bColor.b;
            provsPixels[index + off * originW * 4 + 3] = bColor.a;//myAlfa;
        }
    }
    
    provsBorders = std::make_unique<Texture>(provsPixels, 1920, 1088);
}

Color Map::ClickOnProvince(int x, int y)
{
    Log("mouse in world " << x << " " << y);
    const unsigned char * pixels = provsMap.GetPixels();

    x = (float)(x) / chunkScale + 0.5f;
    y = (float)(y) / chunkScale + 0.5f;
    int num = (int)(originW * (y - 1) + x);

    if (num * 4 + 3 > originW * originH * 4 + 3 || 0.5f * y - 1 < 0) 
        return Color{1, 1, 1, 1};

    Color provinceColor = {
                 pixels[num * 4 + 0],
                 pixels[num * 4 + 1],
                 pixels[num * 4 + 2],
                 pixels[num * 4 + 3]
                };
    
    //Log("prov num="<<num);
    //Log("prov col:"<<(int)before.r);
    
    BrightenProv(provinceColor);

    return provinceColor;
}

void Map::DrawSieged(const Color & prov, const Color & siege)
{
    auto provPix = provsMap.GetPixels();
    auto countryPix = countryBorders->GetPixToEdit();
    for (int y = 0; y < originH; y += 2) { // change in this loop has to be reviewed in Map::ScrapSiegeColor()
        for (int x = 0; x < originW; ++x) {
            int index = ( y * originW + x) * 4;
            Color currProvC = { provPix[index + 0], provPix[index + 1], provPix[index + 2], provPix[index + 3] };
            if (currProvC != prov)
                continue;

            if (countryPix[index + 3] > 0)  // if > 0 there is something drawn
                continue;

            countryPix[index + 0] = siege.r;
            countryPix[index + 1] = siege.g;
            countryPix[index + 2] = siege.b;
            countryPix[index + 3] = 254; //trick for scraping siege color
        }
    }
    countryBorders->ReloadPixels();
}

void Map::ScrapSiegeColor(const Color & prov)
{
    auto provPix = provsMap.GetPixels();
    auto countryPix = countryBorders->GetPixToEdit();
    for (int y = 0; y < originH; y += 2) {
        for (int x = 0; x < originW; ++x) {
            int index = ( y * originW + x) * 4;
            Color currProvC = { provPix[index + 0], provPix[index + 1], provPix[index + 2], provPix[index + 3] };
            if (currProvC != prov)
                continue;

            if (countryPix[index + 3] != 254)  // if == 254 there is old siege drawn
                continue;

            //pix[index + 0] = 0;
            //pix[index + 1] = 0;
            //pix[index + 2] = 0;
            countryPix[index + 3] = 0;
        }
    }
    countryBorders->ReloadPixels();
}


void Map::BrightenProv(Color c)
{
    auto provPix = provsMap.GetPixels();
    unsigned char * pix = provsBorders->GetPixToEdit();
    
    for (int x = 0; x < 1920; x++) {
        for (int y = 0; y < 1088; y++) {
            int index = (y * 1920 + x) * 4;
            Color curr = { provPix[index + 0], provPix[index + 1], provPix[index + 2], provPix[index + 3] };
            if (curr == c && pix[index + 0] == 0) {
                pix[index + 0] = brightCol.r;
                pix[index + 1] = brightCol.g;
                pix[index + 2] = brightCol.b;
                pix[index + 3] = brightCol.a;
            }
        }
    }
    
    provsBorders->ReloadPixels();
    brightenedProv = true;
}

void Map::Unbright()
{
    if (!brightenedProv)
        return;
    brightenedProv = false;

    //auto provPix = provsMap.GetPixels();
    unsigned char * pix = provsBorders->GetPixToEdit();
    for (int x = 0; x < 1920; x++) {
        for (int y = 0; y < 1088; y++) {
            int index = (y * 1920 + x) * 4;
            Color curr = { pix[index + 0], pix[index + 1], pix[index + 2], pix[index + 3] };
            if (curr == brightCol) {
                pix[index + 0] = 0;
                pix[index + 1] = 0;
                pix[index + 2] = 0;
                pix[index + 3] = 0;
            }
        }
    }
    provsBorders->ReloadPixels();
}


#endif
