#pragma once
#include <memory>
#include <GL/glew.h>
#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

#include "texture.h"
#include "log.h"

class Rectangle
{
    GLuint vao, vbo;
    float vertices[54];

    void bind();
    void unbind();
    
protected:
    std::shared_ptr<Texture> texture;
    glm::vec3 position;
    glm::vec2 size;
    glm::vec4 color;

    bool left = true;    

    void initWithVerts(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4);

public:
    Rectangle(Rectangle &) = delete;
    Rectangle(Rectangle &&) = delete;
    Rectangle(glm::vec3 pos, glm::vec2 s, glm::vec4 color = glm::vec4(.0f, 0.0f, .0f, 1.0f));
    Rectangle(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4, glm::vec4 color = glm::vec4(.0f, 0.0f, .0f, 1.0f));
   
    Rectangle(glm::vec3 pos, glm::vec2 s, glm::vec2 texCoordLeftBottom, glm::vec2 texCoordLen);

    virtual ~Rectangle();
    
    void SetColor(glm::vec4 color);
    void SetTexture(std::shared_ptr<Texture> t);
    
    void Draw(bool patches);
    virtual void Draw() { Draw(false); };
    bool Click(int x, int y);

    void Init();
    void Left();
    void Right();
    
    void MoveRect(int x, int y) { position.x += x; position.y += y; Init(); }
    void SetPosition(glm::vec3 p) { position = p; Init(); }
    void SetSize(glm::vec2 s) { size = s; Init(); }
    glm::vec3 GetPosition() { return position; }
    glm::vec2 GetSize() { return size; }
};
