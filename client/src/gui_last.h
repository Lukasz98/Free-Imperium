#pragma once

#include <cassert>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <vector>

#include "camera.h"
#include "graphics/shader.h"
#include "graphics/window.h"

namespace GuiLast {

struct Gui {
    glm::vec4 brownCol{56.0f / 255.0f, 31.0f / 255.0f, 19.0f / 255.0f, 1.0f};
    glm::vec4 greenCol{0.0f / 255.0f, 255.0f / 255.0f, 0.0f / 255.0f, 1.0f};
    Window* window;
    Shader shader;
    glm::vec2 res, winSize;

    void init(Window* window, glm::vec2 resolution, glm::vec2 windowSize);
    void start();
    void flush();

    void reloadShader();
        



    void room_playerListDraw(const std::vector<std::string> & playersList);
};

};  // namespace GuiLast
