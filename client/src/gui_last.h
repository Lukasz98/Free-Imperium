#pragma once


#include <memory>
#include <cassert>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>



#include "graphics/window.h"
#include "graphics/shader.h"
#include "camera.h"

namespace GuiLast {

void init(Window * window, glm::vec2 resolution, glm::vec2 windowSize);

void start();

};
