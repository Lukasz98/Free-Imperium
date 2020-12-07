#pragma once
#include <memory>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "graphics/model_3d.h"
#include "graphics/shader.h"
#include "graphics/texture.h"

struct AM
{
    //static bool init = false;
    static FT_Library ft;
    static FT_Face face;
    std::unique_ptr<Model3D> model;
    std::unique_ptr<Shader> shader;
    std::unique_ptr<Texture> modelTexture;

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
        PX64 = 2
    };

    static AtlasInfo atlasInfo[3][128];
    static std::unique_ptr<Texture> atlasTexture[3];
    //static int maxRows, widthSum;



//    AM() : shader(std::make_unique<Shader>(
  //      "src/graphics/shaders/model_3d/vert.v",
    //    "src/graphics/shaders/model_3d/frag.f", "", "")) 
    AM()
    {

    }
    static AM am;

    static void Init();
    static void Dispose();


};
