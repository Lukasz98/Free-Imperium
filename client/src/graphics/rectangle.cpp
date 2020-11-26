#include "rectangle.h"

Rectangle::Rectangle(glm::vec3 pos, glm::vec2 s, glm::vec4 color)
    : position(pos), size(s), color(color)
{
    //        return;
glCreateVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    Right();
    Init();

    bind();
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*9, 0);  
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float)*9, (void*)(3 * sizeof(float)));  
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(float)*9, (void*)(5 * sizeof(float)));  
    unbind();
}

Rectangle::Rectangle(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4, glm::vec4 color)
    : position(p1), color(color)
{
    glCreateVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    initWithVerts(p1, p2, p3, p4);

    bind();
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*9, 0);  
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float)*9, (void*)(3 * sizeof(float)));  
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(float)*9, (void*)(5 * sizeof(float)));  
    unbind();
}

Rectangle::Rectangle(glm::vec3 pos, glm::vec2 s, glm::vec2 texC, glm::vec2 tCL)
    :position{pos}, size{s}, color{0.0f,0.0f,0.0f,1.0f}
{
    glCreateVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    left = true;
    auto vertInit =
    {
        position.x, position.y, position.z,
        texC.x, texC.y,
        //texC.x + texCoordLen, texC.y,//1.0f, 0.0f,
        color.x, color.y, color.z, color.w,
      
        position.x, position.y + size.y, position.z,
        texC.x, texC.y + tCL.y,
        //texC.x + texCoordLen, texC.y + texCoordLen,//1.0f, 1.0f,
        color.x, color.y, color.z, color.w,

        position.x + size.x, position.y, position.z,
        texC.x + tCL.x, texC.y,
        //texC.x, texC.y,//0.0f, 0.0f,
        color.x, color.y, color.z, color.w,

        position.x, position.y + size.y, position.z,
        texC.x, texC.y + tCL.y,
        //texC.x + texCoordLen, texC.y + texCoordLen,//1.0f, 1.0f,
        color.x, color.y, color.z, color.w,

        position.x + size.x, position.y + size.y, position.z,
        texC.x + tCL.x, texC.y + tCL.y,
        //texC.x, texC.y + texCoordLen,//0.0f, 1.0f,
        color.x, color.y, color.z, color.w,

        position.x + size.x, position.y, position.z, 
        texC.x + tCL.x, texC.y,
        //texC.x, texC.y,//0.0f, 0.0f,
        color.x, color.y, color.z, color.w
    };
    std::copy(vertInit.begin(), vertInit.end(), vertices);
    bind();
    glBufferData(GL_ARRAY_BUFFER, 54 * sizeof(float), vertices, GL_STATIC_DRAW);  
    unbind();        


    bind();
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*9, 0);  
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float)*9, (void*)(3 * sizeof(float)));  
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(float)*9, (void*)(5 * sizeof(float)));  
    unbind();
}

Rectangle::~Rectangle()
{
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
}


void Rectangle::Init()
{
    if (left)
        Left();
    else
        Right();
    
    bind();
    glBufferData(GL_ARRAY_BUFFER, 54 * sizeof(float), vertices, GL_STATIC_DRAW);  
    unbind();
}

void Rectangle::initWithVerts(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4)
{
    //1 3
    //0 2
    auto vertInit =
    {
        p1.x, p1.y, p1.z,
        0.0f, 0.0f,
        color.x, color.y, color.z, color.w,
      
        p2.x, p2.y, p2.z,
        0.0f, 1.0f,
        color.x, color.y, color.z, color.w,

        p3.x, p3.y, p3.z,
        1.0f, 0.0f,
        color.x, color.y, color.z, color.w,

        p2.x, p2.y, p2.z,
        0.0f, 1.0f,
        color.x, color.y, color.z, color.w,

        p4.x, p4.y, p4.z,
        1.0f, 1.0f,
        color.x, color.y, color.z, color.w,

        p3.x, p3.y, p3.z,
        1.0f, 0.0f,
        color.x, color.y, color.z, color.w
    };
    std::copy(vertInit.begin(), vertInit.end(), vertices);

    bind();
    glBufferData(GL_ARRAY_BUFFER, 54 * sizeof(float), vertices, GL_STATIC_DRAW);  
    unbind();        
}

void Rectangle::SetColor(glm::vec4 color)
{
    this->color = color;
    Init();
}
    
void Rectangle::SetTexture(std::shared_ptr<Texture> t)
{
    texture = t;
}

void Rectangle::Draw(bool patches)
{
    bind();
    if (patches)
        glDrawArrays(GL_PATCHES, 0, 6);  
    else
        glDrawArrays(GL_TRIANGLES, 0, 6);  
    unbind();
}

void Rectangle::Left()
{
    left = true;
    auto vertInit =
    {
        position.x, position.y, position.z,
        1.0f, 0.0f,
        color.x, color.y, color.z, color.w,
      
        position.x, position.y + size.y, position.z,
        1.0f, 1.0f,
        color.x, color.y, color.z, color.w,

        position.x + size.x, position.y, position.z,
        0.0f, 0.0f,
        color.x, color.y, color.z, color.w,

        position.x, position.y + size.y, position.z,
        1.0f, 1.0f,
        color.x, color.y, color.z, color.w,

        position.x + size.x, position.y + size.y, position.z,
        0.0f, 1.0f,
        color.x, color.y, color.z, color.w,

        position.x + size.x, position.y, position.z, 
        0.0f, 0.0f,
        color.x, color.y, color.z, color.w
    };
    std::copy(vertInit.begin(), vertInit.end(), vertices);
}

void Rectangle::Right()
{
    left = false;
    auto vertInit =
    {
        position.x, position.y, position.z,
        0.0f, 0.0f,
        color.x, color.y, color.z, color.w,
      
        position.x, position.y + size.y, position.z,
        0.0f, 1.0f,
        color.x, color.y, color.z, color.w,

        position.x + size.x, position.y, position.z,
        1.0f, 0.0f,
        color.x, color.y, color.z, color.w,

        position.x, position.y + size.y, position.z,
        0.0f, 1.0f,
        color.x, color.y, color.z, color.w,

        position.x + size.x, position.y + size.y, position.z,
        1.0f, 1.0f,
        color.x, color.y, color.z, color.w,

        position.x + size.x, position.y, position.z, 
        1.0f, 0.0f,
        color.x, color.y, color.z, color.w
    };
    std::copy(vertInit.begin(), vertInit.end(), vertices);
}

bool Rectangle::Click(int x, int y)
{
    if (x > position.x && x < position.x + size.x && y > position.y && y < position.y + size.y) {
        return true;
    }
    return false;
}

void Rectangle::bind()
{
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    if (texture)
        texture->Bind();
}

void Rectangle::unbind()
{
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    if (texture)
        texture->Unbind();
}
    
