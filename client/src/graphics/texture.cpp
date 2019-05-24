#include "texture.h"

Texture::Texture(std::string path, int w, int h)
: originW(w), originH(h)
{
    //std::cout << "1.Texture()\n";
    int bytes = 0;
        pixelsOrigin = loadTexture(path, originW, originH, bytes); // w and h by refr.
        
        
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    if (bytes == originW * originH * 4)
        glTexImage2D(GL_TEXTURE_2D,0, GL_RGBA, originW, originH, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixelsOrigin);
    else
        glTexImage2D(GL_TEXTURE_2D,0, GL_RGB, originW, originH, 0, GL_RGB, GL_UNSIGNED_BYTE, pixelsOrigin);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        
    Unbind();
}
    
Texture::Texture(const unsigned char * pixels, int w, int h)
: originW(w), originH(h)
{
    //std::cout << "2.Texture()\n";
    int l = w*h*4;
    //if (pixelsOrigin != nullptr)
    //  delete [] pixelsOrigin;
    pixelsOrigin = new unsigned char[l];

    std::copy(pixels, pixels + w * h * 4, pixelsOrigin);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    //if (sizeof(pixelsOrigin) / sizeof(pixelsOrigin[0]) == originW * originH * 4)
        glTexImage2D(GL_TEXTURE_2D,0, GL_RGBA, originW, originH, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixelsOrigin);
        //else
        //glTexImage2D(GL_TEXTURE_2D,0, GL_RGB, originW, originH, 0, GL_RGB, GL_UNSIGNED_BYTE, pixelsOrigin);

    //glTexImage2D(GL_TEXTURE_2D,0, GL_RGBA, originW, originH, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixelsOrigin);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    
        
    Unbind();
}

Texture::~Texture()
{
    //if (pixelsOrigin != nullptr)
    glDeleteTextures(1, &texture);
    if (pixelsOrigin != nullptr) {
        delete [] pixelsOrigin;
    }
    //std::cout << "~Texture\n";
}
        
