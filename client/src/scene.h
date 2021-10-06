#pragma once
#include <chrono>
#include <thread>

#include <GL/glew.h>
#include <glm/vec2.hpp>

#include <SFML/System/Clock.hpp>

#include "graphics/window.h"
#include "graphics/shader.h"
#include "camera.h"
#include "gui/gui_bridge.h"

class Scene
{
protected:
    Window & window;
    Shader shader;
    Camera camera;
//    Gui gui;
    sf::Clock clock;
    float dt = 0.0f;

    glm::vec2 resolution;

    Scene(Window & win, const glm::vec2 & resolution);

    void DrawGui();
    
public:
    virtual ~Scene();

    //void Play();
    
};
