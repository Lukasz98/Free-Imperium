#pragma once

#include <cassert>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <vector>

#include "../asset_manager.h"
#include "gui_batch.h"

#define TEXT_CENTER 2
#define TEXT_RIGHT 3
#define TEXT_LEFT 4
struct GuiCore {
    Batch batch;
    bool isInRect(const glm::vec3& pos, const glm::vec2& size, int mx, int my);
    void drawRect(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& col);
    void drawIcon(const glm::vec3& pos, const glm::vec2& size, int iconId);
    void guiBatchInit();
    void guiBatchBegin();
    void guiBatchFlush();

    void drawText(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& col, const std::string& text,
                  int flags, AM::FontSize fontSize = AM::FontSize::PX16);
};
