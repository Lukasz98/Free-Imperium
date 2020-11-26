#pragma once
#include <GL/glew.h>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

#include "load_from_png.h"
#include "color.h"


class Texture
{
    GLuint texture;
    bool rgba;

protected:
    //std::vector<unsigned char> pixelsOrigin;
    unsigned char * pixelsOrigin = nullptr;
    int originW, originH;
    
public:
    Texture(const Texture &) = delete;
    Texture(const Texture &&) = delete;

    Texture(std::string path, int w = 0, int h = 0, GLint param = GL_CLAMP_TO_BORDER);
    Texture(const unsigned char * pixels, int w, int h);
    
    virtual ~Texture();
        
    void Bind()
    {
        glBindTexture(GL_TEXTURE_2D, texture);
    }

    void Unbind()
    {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void ReloadPixels();
    unsigned char * GetPixToEdit() { return pixelsOrigin; }

    GLuint GetId() { return texture; }
    virtual const unsigned char * GetPixels() { return pixelsOrigin; }
    //virtual const unsigned char * GetPixels() { return pixelsOrigin; }
    const unsigned char * GetOriginPixels() { return pixelsOrigin; }
    virtual void SwapColors(Color bef, Color aft) {}//= 0;
    virtual void BrightenProvince(Color bef, int increase) {}

};
