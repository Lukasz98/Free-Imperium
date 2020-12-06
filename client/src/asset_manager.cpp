#include "asset_manager.h"

FT_Library AM::ft;
FT_Face AM::face;
AM AM::am;
std::unique_ptr<Texture> AM::atlasTexture;
int AM::maxRows;
int AM::widthSum;
AM::AtlasInfo AM::atlasInfo[128];

void AM::Init()
{
    //FT_Library ft;
    if(FT_Init_FreeType(&ft)) {
        fprintf(stderr, "Could not init freetype library\n");
    }

    //FT_Face face;
    if(FT_New_Face(ft, "src/img/Hanken-Book.ttf", 0, &face)) {
        fprintf(stderr, "Could not open font\n");
    }

//Model3D model{"src/img/DudeDonePosefix.obj", glm::vec3{0.0, 0.0, 0.1}};
am.model = std::make_unique<Model3D>("src/img/DudeDonePosefix.obj", glm::vec3{0.0, 0.0, 0.1});
am.modelTexture = std::make_unique<Texture>("src/img/Done3.png");
am.shader = std::make_unique<Shader>(
        "src/graphics/shaders/model_3d/vert.v",
        "src/graphics/shaders/model_3d/frag.f", "", "");




// font atlas
// fontAtlas, AtlasInfo: atlasInfo[128]
//int charSize = 300;
//FT_Set_Pixel_Sizes(AM::face, 0, charSize);
FT_Set_Char_Size(AM::face, 0, 64*64, 0, 0); //face, width,height in 1/64pixel, hor dpi, vert dpi
maxRows = 0, widthSum = 0;
for (char c = '!'; c <= '~'; ++c) {
    FT_UInt glyph_index = FT_Get_Char_Index( face, c);
    if (glyph_index == 0)
        continue;

    if (FT_Load_Glyph(AM::face, glyph_index, FT_LOAD_DEFAULT))
        continue;

    FT_GlyphSlot gs = face->glyph;
    if (maxRows < gs->bitmap.rows)
        maxRows = gs->bitmap.rows;
    widthSum += gs->bitmap.width;
}

   Log(widthSum << ", " << maxRows); 
unsigned char * atlas = (unsigned char *)calloc(maxRows * widthSum * 4, sizeof(unsigned char));
int lastX = 0;
for (char c = '!'; c <= '~'; ++c) {
    FT_UInt glyph_index = FT_Get_Char_Index( face, c);
    if (glyph_index == 0)
        continue;

    if (FT_Load_Glyph(AM::face, glyph_index, FT_LOAD_RENDER | FT_LOAD_LINEAR_DESIGN))
        continue;

    FT_GlyphSlot gs = face->glyph;


atlasInfo[c].set = true;
atlasInfo[c].tcX = (float)lastX / widthSum;
atlasInfo[c].tcY = 0.0f;
atlasInfo[c].tcWidth = (float)gs->bitmap.width / widthSum;
atlasInfo[c].tcHeight = (float)gs->bitmap.rows / maxRows;
atlasInfo[c].width = gs->bitmap.width;
atlasInfo[c].height = gs->bitmap.rows;
atlasInfo[c].advanceX = ((float)gs->advance.x / 26.6) - gs->bitmap_left + 0.5; //26.6i is fractional pixel format. no idea what it means
atlasInfo[c].yPos = gs->bitmap_top - gs->bitmap.rows;//this should be negative // - gs->bitmap_top;

   
Log(gs->advance.x/ 26.6);
Log(gs->linearHoriAdvance);
    for (int y = gs->bitmap.rows - 1; y > 0; --y) {
        for (int x = 0; x < gs->bitmap.width; ++x) {
            unsigned char glyphColor = gs->bitmap.buffer[x + (gs->bitmap.rows - y) * gs->bitmap.width];
            int atlasIndex = (y * widthSum + lastX + x)*4;
            atlas[atlasIndex + 0] = 0;
            atlas[atlasIndex + 1] = 0;
            atlas[atlasIndex + 2] = 0;
            //atlas[atlasIndex + 0] = glyphColor;
            //atlas[atlasIndex + 1] = glyphColor;
            //atlas[atlasIndex + 2] = glyphColor;
            atlas[atlasIndex + 3] = glyphColor;
        }
    }
    lastX += gs->bitmap.width;
            Log(c);
    
}

    atlasTexture = std::make_unique<Texture>(atlas, widthSum, maxRows);
free(atlas);












}

void AM::Dispose()
{
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

}
