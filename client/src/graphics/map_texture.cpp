#include "map_texture.h"

void MapTexture::Update()
{
    if  (id != 0) {
        glDeleteTextures(1, &id);
    }
    assert(pix != nullptr);
GLint er = glGetError();
if (er)
    Log("OPENGL ERR: " << er);
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    
    glTexImage2D(GL_TEXTURE_2D,0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, pix);
    //glTexImage2D(GL_TEXTURE_2D,0, GL_RGBA, originW, originH, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixelsOrigin);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    
    glBindTexture(GL_TEXTURE_2D, 0);
er = glGetError();
if (er)
    Log("OPENGL ERR: " << er);
    
}

MapTexture::~MapTexture()
{
Log("~MapTexture");
    if (pix)
        delete [] pix;

    if (id != 0)
        glDeleteTextures(1, &id);
}




