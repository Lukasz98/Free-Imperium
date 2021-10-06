#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "log.h"

class Window
{
public:
    GLFWwindow * window;
    float bgColor[4];
    glm::vec2 size;
    std::string title;
    
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void error_callback(int error, const char* description);
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

public:
    Window(const Window &) = delete;
    Window(glm::vec2 size, std::string title);
    ~Window();
    
    bool ShouldClose();
    void Refresh();
    void Update();

    glm::vec2 GetSize() { return size; }
    
    bool keys[1000];
    double scrollOffset;
    double xMouse = 0.0, yMouse = 0.0;
    bool mouseL = false;
    bool mouseR = false;
    bool mouseLClicked = false;
    bool mouseRClicked = false;


};
