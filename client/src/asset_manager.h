#pragma once
#include <ft2build.h>
#include FT_FREETYPE_H

struct AM
{
    //static bool init = false;
    static FT_Library ft;
    static FT_Face face;

    static void Init();
    static void Dispose();
};
