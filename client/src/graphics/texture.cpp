#include "texture.h"

Texture::Texture(std::string path, int w, int h, GLint param)
: originW(w), originH(h)
{
    int bytes = 0;
    pixelsOrigin = loadTexture(path, originW, originH, bytes); // w and h by refr.
        
        
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    if (bytes == originW * originH * 4) {
        rgba = true;
        glTexImage2D(GL_TEXTURE_2D,0, GL_RGBA, originW, originH, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixelsOrigin);
    }
    else {
        rgba = false;
        glTexImage2D(GL_TEXTURE_2D,0, GL_RGB, originW, originH, 0, GL_RGB, GL_UNSIGNED_BYTE, pixelsOrigin);
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, param);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, param);
        
    Unbind();
}
    
Texture::Texture(const unsigned char * pixels, int w, int h)
: originW(w), originH(h)
{
    rgba = true;
    int l = w*h*4;
    pixelsOrigin = new unsigned char[l];

    std::copy(pixels, pixels + w * h * 4, pixelsOrigin);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    glTexImage2D(GL_TEXTURE_2D,0, GL_RGBA, originW, originH, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixelsOrigin);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    
        
    Unbind();
}

Texture::~Texture()
{
    glDeleteTextures(1, &texture);
    if (pixelsOrigin != nullptr) {
        delete [] pixelsOrigin;
    }
}

void Texture::ReloadPixels()
{
    Bind();
    if (rgba) {
        glTexImage2D(GL_TEXTURE_2D,0, GL_RGBA, originW, originH, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixelsOrigin);
    }
    else {
        glTexImage2D(GL_TEXTURE_2D,0, GL_RGB, originW, originH, 0, GL_RGB, GL_UNSIGNED_BYTE, pixelsOrigin);
    }
    Unbind();
}        
