#include "font_batch.h"
#include "graphics/window.h"



void FontBatch::Init()
{
    int vertexSize = sizeof(FontVertex);
    int spriteSize = vertexSize * 6;
    int vertexDataSize = spriteSize * maxSprites;
    glCreateVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glCreateBuffers(1, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertexDataSize, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexArrayAttrib(vao, 0);
    glEnableVertexArrayAttrib(vao, 1);
    glEnableVertexArrayAttrib(vao, 2);
    glEnableVertexArrayAttrib(vao, 3);
    GLuint err = glGetError();
    if (err)
        Log("Opengl error: " << err);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexSize, NULL);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, vertexSize,
                            (const GLvoid*)(offsetof(FontVertex, FontVertex::tc)));
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, vertexSize,
                          (const GLvoid*)(offsetof(FontVertex, FontVertex::color)));
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, vertexSize,
                          (const GLvoid*)(offsetof(FontVertex, FontVertex::tid)));

    err = glGetError();
    if (err)
        Log("Opengl error: " << err);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
}
void FontBatch::Begin()
{
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    vertexData = (FontVertex*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
}

void FontBatch::Push(float x, float y, float z, float w, float h, float tcx, float tcy, float tcxLen, float tcyLen, float tid)
{
    if (vertCount + 6 >= maxSprites) {
        Flush();
        Begin();
    }

    spriteCount++;
    vertCount += 6;

    FontVertex fv = FontVertex{.pos = Vec3{x, y, z}, .tc = Vec2{tcx, tcy}, .tid = tid};
    *vertexData = fv;
    vertexData++;

    fv.pos.y += h;
    fv.tc.y += tcyLen;
    *vertexData = fv;
    vertexData++;

    
    fv.pos.x += w;
    fv.tc.x += tcxLen;
    *vertexData = fv;
    vertexData++;

    
    *vertexData = fv;
    vertexData++;

    
    fv.pos.y -= h;
    fv.tc.y -= tcyLen;
    *vertexData = fv;
    vertexData++;

    
    fv.pos.x -= w;
    fv.tc.x -= tcxLen;
    *vertexData = fv;
    vertexData++;

}
void FontBatch::Flush()
{
    glDrawArrays(GL_TRIANGLES, 0, vertCount);
    vertCount = 0;
    spriteCount = 0;
    glUnmapBuffer(GL_ARRAY_BUFFER);
}
