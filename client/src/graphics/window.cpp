#include "window.h"

Window::Window(glm::vec2 size, std::string title)
    : bgColor{ 0.5f, 0.5f, 0.5f, 1.0f }, title{title}
{
    //glewExperimental=GL_TRUE;
    int code = glfwInit();
    if (!code) {
        Log("GLFW Initialization failed, error code: " << code);
        return;
    }
    
    glfwSetErrorCallback(error_callback);
        
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    this->size = size;
    window = glfwCreateWindow(size.x, size.y, this->title.c_str(), NULL, NULL); // window mode
    //window = glfwCreateWindow(size.x, size.y, title.c_str(), glfwGetPrimaryMonitor(), NULL); // fullscrean

    if (!window) {
        Log("Window creation error");
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window);
    glfwSetWindowUserPointer(window, this);
    glfwSetKeyCallback(window, key_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    
    glEnable(GL_DEPTH_TEST);   // Enable depth testing for z-culling
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (int i = 0; i < 1000; i++)
        keys[i] = false;
    scrollOffset = 0.0;
    //glfwDestroyWindow(window);
}

Window::~Window()
{
    std::cout << "~Window\n";
    glfwDestroyWindow(window);
    glfwTerminate();
}
    
bool Window::ShouldClose()
{
    return glfwWindowShouldClose(window);
}

void Window::Refresh()
{
    glfwPollEvents();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearBufferfv(GL_COLOR, 0, bgColor);
}

void Window::Update()
{
    glfwSwapInterval(0);
    glfwSwapBuffers(window);
    glfwGetCursorPos(window, &xMouse, &yMouse);
}

void Window::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Window * w = (Window *)glfwGetWindowUserPointer(window);
	if (action == GLFW_PRESS) {
        w->keys[key] = true;
        //if (key == 'R') glfwSetCursorPos(w->window, 100.0, 100.0);
    }
	else if (action == GLFW_RELEASE)
		w->keys[key] = false;
}

void Window::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	Window * w = (Window *)glfwGetWindowUserPointer(window);
	w->scrollOffset = yoffset * -1.0;
}

void Window::error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

void Window::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    Window * w = (Window *)glfwGetWindowUserPointer(window);
    /*
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        w->mouseL = true;
    }
    */
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        w->mouseL = true;
    }
    /*
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        w->mouseR = true;
    }
    */
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
    {
        w->mouseR = true;
    }
}
