#include "scene.h"

Scene::Scene(Window & win, const glm::vec2 & resolution)
    : window(win), 
      shader("src/graphics/shaders/basic/vert.v", 
            "src/graphics/shaders/basic/frag.f", "", ""), 
      camera(window.GetSize())
{
    this->resolution = resolution;
    glUseProgram(shader.GetProgram());
    glm::mat4 matrix = glm::ortho(0.0f, (float)resolution.x, 0.0f, (float)resolution.y);
    glUniformMatrix4fv(glGetUniformLocation(shader.GetProgram(), "matrix"), 1, GL_FALSE, glm::value_ptr(matrix));
}

Scene::~Scene()
{
    
}

void Scene::DrawGui()
{
    dt = clock.getElapsedTime().asSeconds();        
    float frameGoal = 30.0f;
#if 1
    if (dt < 1.0f / frameGoal) {
        float timeForSleep = (1.0f / frameGoal - dt) * 1000.0f; 
        std::this_thread::sleep_for(std::chrono::milliseconds((int)timeForSleep));
        dt = clock.getElapsedTime().asSeconds();
    }
#endif
    clock.restart();
    //gui.Draw();
    window.Update();
}
