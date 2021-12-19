#pragma once


#include <memory>
#include <vector>
#include <cassert>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "gui_2/gui_batch.h"
#include "../asset_manager.h"

struct List2 {
    std::vector<Vertex> v;
    glm::vec3 pos;
    glm::vec2 size;
    float lastY, ypadding = 5.0f;
    int scr;
    List2(const glm::vec3& pos, const glm::vec2& size) : pos(pos), size(size)  //, scr(scr)
    {
        lastY = pos.y + size.y;
    }
};

struct ClickEvent
{
    int ev;
    int val;
};

#define TEXT_CENTER 2
#define TEXT_RIGHT 3
#define TEXT_LEFT 4
struct GuiCore {
Batch batch;
bool isInRect(const glm::vec3 & pos, const glm::vec2 & size, int mx, int my);
void drawRect(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& col);
void drawIcon(const glm::vec3& pos, const glm::vec2& size, int iconId);
void guiBatchInit();
void guiBatchBegin();
void guiBatchFlush();

bool isInRectList(std::size_t listid, std::size_t vid, int mx, int my);

int createList(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& col);

std::size_t drawRectToList(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& col, std::size_t listId);


void drawLists();


void drawTextToList(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& col, const std::string& text,
                    int flags, std::size_t listid);

void drawText(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& col, const std::string& text,
              int flags, AM::FontSize fontSize = AM::FontSize::PX16);


void listScroll(int listid, int& scr);

float getListLastY(std::size_t listid);
float getListYPadding(std::size_t listid);
void setListLastY(std::size_t listid, float lastY);
};

//ClickEventType drawTopBar(const TopBarData& td, const glm::vec2 & resolution);

