#pragma once
#include <string>
#include <string.h>
#include <cstdio>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "log.h"
    
class Shader
{
 
    GLuint program;
    std::string readFile(const char* filepath);    
    bool CheckShaderError(GLuint shader, GLuint flag, bool isProgram, const std::string &errorMessage);

public:
    Shader(std::string vertS, std::string fragS);
    GLuint GetProgram() { return program; }
};
