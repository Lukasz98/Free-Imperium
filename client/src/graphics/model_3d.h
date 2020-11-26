#pragma once

#include <memory>
#include <GL/glew.h>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include "obj_loader.h"
#include "log.h"
#include "rectangle.h"

class Model3D
{
    glm::vec3 position;
    GLuint vao, vbo, ibo;

    objl::Loader Loader;

    std::unique_ptr<Rectangle> plane;

    void bind();
    void unbind();

public:
    Model3D(const std::string & path, glm::vec3 pos);
    ~Model3D();

    void Draw();
    void DrawRect(glm::mat4 ml);

    void Click(glm::mat4 ml,  glm::mat4 mlRot, glm::vec3 vv, glm::vec3 eye);
};