#include "asset_manager.h"

FT_Library AM::ft;
FT_Face AM::face;

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
}

void AM::Dispose()
{
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

}
