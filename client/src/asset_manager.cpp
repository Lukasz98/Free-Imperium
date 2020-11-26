#include "asset_manager.h"

FT_Library AM::ft;
FT_Face AM::face;
AM AM::am;

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

}

void AM::Dispose()
{
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

}
