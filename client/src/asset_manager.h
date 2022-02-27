#pragma once
#include <memory>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "graphics/model_3d.h"
#include "graphics/shader.h"
#include "graphics/texture.h"

struct AM
{
    static FT_Library ft;
    static FT_Face face;
    std::unique_ptr<Shader> shader;
    std::unique_ptr<Shader> modelInstancedShader;
    std::unique_ptr<Shader> circleShader;
    std::unique_ptr<Texture> modelTexture;
    std::unique_ptr<Texture> iconsTexture;
    std::unique_ptr<Texture> def_cursorTexture;
    std::unique_ptr<Texture> hover_cursorTexture;
    GLFWcursor* def_cursor; 
    GLFWcursor* hover_cursor; 
    
    GLuint fontAtlas;
    struct AtlasInfo
    {
        bool set = false;
        float tcX, tcY, tcWidth, tcHeight;
        int width, height, advanceX, yPos;
    };

    enum FontSize {
        PX16 = 0,
        PX32 = 1,
        PX64 = 2,
        PX80 = 3,
        PX128 = 4,
        PX160 = 5
    };

    static AtlasInfo atlasInfo[6][128];
    static std::unique_ptr<Texture> atlasTexture[6];

    AM()
    {

    }
    static AM am;

    static void Init();
    static void Dispose();

    void reloadShaders();
};
