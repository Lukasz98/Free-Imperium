#include "asset_manager.h"

FT_Library AM::ft;
FT_Face AM::face;
AM AM::am;
std::unique_ptr<Texture> AM::atlasTexture[3];
AM::AtlasInfo AM::atlasInfo[3][128];

std::unique_ptr<Texture> loadFontAtlas(int fSize, enum AM::FontSize fontSizeEnum);

void AM::Init()
{
    //FT_Library ft;
    if(FT_Init_FreeType(&ft)) {
        fprintf(stderr, "Could not init freetype library\n");
    }

    //FT_Face face;
    if(FT_New_Face(ft, "src/img/Roboto-Light.ttf", 0, &face)) {
    //if(FT_New_Face(ft, "src/img/Roboto-LightItalic.ttf", 0, &face)) {
    //if(FT_New_Face(ft, "src/img/Hanken-Book.ttf", 0, &face)) {
        fprintf(stderr, "Could not open font\n");
    }

    //Model3D model{"src/img/DudeDonePosefix.obj", glm::vec3{0.0, 0.0, 0.1}};
    am.model = std::make_unique<Model3D>("src/img/DudeDonePosefix.obj", glm::vec3{0.0, 0.0, 0.1});
    am.modelTexture = std::make_unique<Texture>("src/img/Done3.png");
    am.shader = std::make_unique<Shader>(
            "src/graphics/shaders/model_3d/vert.v",
            "src/graphics/shaders/model_3d/frag.f", "", "");


/*
    Texture * atlas = loadFontAtlas(16, FontSize::PX16);
    if (atlas == nullptr) {
        Log("Font atlas creation error");
    }
    else {
        atlasTexture[FontSize::PX16] = std::make_unique<Texture>(atlas);
    }

    atlas = loadFontAtlas(32, FontSize::PX32);
    if (atlas == nullptr) {
        Log("Font atlas creation error");
    }
    else {
        atlasTexture[FontSize::PX32] = std::make_unique<Texture>(atlas);
    }
    
    atlas = loadFontAtlas(64, FontSize::PX64);
    if (atlas == nullptr) {
    }
    else {
        atlasTexture[FontSize::PX64] = std::make_unique<Texture>(atlas);
    }
*/
    if (!(atlasTexture[FontSize::PX16] = loadFontAtlas(16, FontSize::PX16))) {
        Log("Font atlas creation error");
    }

    if (!(atlasTexture[FontSize::PX32] = loadFontAtlas(32, FontSize::PX32))) {
        Log("Font atlas creation error");
    }

    if (!(atlasTexture[FontSize::PX64] = loadFontAtlas(64, FontSize::PX64))) {
        Log("Font atlas creation error");
    }





}

void AM::Dispose()
{
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

}

std::unique_ptr<Texture> loadFontAtlas(int fSize, enum AM::FontSize fontSizeEnum)
{
    FT_Set_Char_Size(AM::face, 0, fSize*64, 0, 0); //face, width,height in 1/64pixel, hor dpi, vert dpi
    int maxRows = 0, widthSum = 0;
    for (char c = '!'; c <= '~'; ++c) {
        FT_UInt glyph_index = FT_Get_Char_Index(AM::face, c);
        if (glyph_index == 0)
            continue;
    
        if (FT_Load_Glyph(AM::face, glyph_index, FT_LOAD_DEFAULT))
            continue;
    
        FT_GlyphSlot gs = AM::face->glyph;
        if (maxRows < gs->bitmap.rows)
            maxRows = gs->bitmap.rows;
        widthSum += gs->bitmap.width + 1;
    }
    
    if (widthSum <= 0 || maxRows <= 0)
        return nullptr;

    //Log(widthSum << ", " << maxRows); 
    unsigned char * atlas = (unsigned char *)calloc(maxRows * widthSum * 4, sizeof(unsigned char));
    int lastX = 0;
    for (char c = '!'; c <= '~'; ++c) {
        FT_UInt glyph_index = FT_Get_Char_Index(AM::face, c);
        if (glyph_index == 0)
            continue;
    
        if (FT_Load_Glyph(AM::face, glyph_index, FT_LOAD_RENDER | FT_LOAD_LINEAR_DESIGN))
            continue;
    
        FT_GlyphSlot gs = AM::face->glyph;
    
    
        AM::atlasInfo[fontSizeEnum][c].set = true;
        AM::atlasInfo[fontSizeEnum][c].tcX = (float)lastX / widthSum;
        AM::atlasInfo[fontSizeEnum][c].tcY = 0.0f;
        AM::atlasInfo[fontSizeEnum][c].tcWidth = (float)gs->bitmap.width / widthSum;
        AM::atlasInfo[fontSizeEnum][c].tcHeight = (float)gs->bitmap.rows / maxRows;
        AM::atlasInfo[fontSizeEnum][c].width = gs->bitmap.width;
        AM::atlasInfo[fontSizeEnum][c].height = gs->bitmap.rows;
        AM::atlasInfo[fontSizeEnum][c].advanceX = ((float)gs->advance.x) * 26.6 - gs->bitmap_left + 0.5; //26.6i is fractional pixel format. no idea what it means
        AM::atlasInfo[fontSizeEnum][c].yPos = gs->bitmap_top - gs->bitmap.rows;//this should be negative // - gs->bitmap_top;

        for (int y = 0; y < gs->bitmap.rows; ++y) {
            for (int x = 0; x < gs->bitmap.width; ++x) {
                unsigned char glyphColor = gs->bitmap.buffer[x + gs->bitmap.width * y];
                int atlasIndex = ((gs->bitmap.rows - (y + 1)) * widthSum + lastX + x)*4;
                atlas[atlasIndex + 0] = 200;
                atlas[atlasIndex + 1] = 200;
                atlas[atlasIndex + 2] = 200;
                atlas[atlasIndex + 3] = glyphColor;
            }
        }
 
        lastX += gs->bitmap.width + 1;

  /*         
        //Log(gs->advance.x/ 26.6);
        //Log(gs->linearHoriAdvance);
        for (int y = gs->bitmap.rows - 1; y > 0; --y) {
            for (int x = 0; x < gs->bitmap.width; ++x) {
                unsigned char glyphColor = gs->bitmap.buffer[x + (gs->bitmap.rows - y) * gs->bitmap.width];
                int atlasIndex = (y * widthSum + lastX + x)*4;
                atlas[atlasIndex + 0] = 0;
                atlas[atlasIndex + 1] = 0;
                atlas[atlasIndex + 2] = 0;
                atlas[atlasIndex + 3] = glyphColor;
            }
        }
        lastX += gs->bitmap.width + 1;
 */ 
        //Log(c);
    }
    auto atlasT = std::make_unique<Texture>(atlas, widthSum, maxRows);
    free(atlas);
    return std::move(atlasT);
}


