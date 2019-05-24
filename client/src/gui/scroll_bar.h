#pragma once
#include <memory>

#include "../graphics/rectangle.h"

class ScrollBar : public Rectangle
{
    std::unique_ptr<Rectangle> boat;
    glm::vec2 padding;
    int elements, maxButtons, scrollCount;
    float speed;

    void init();

public:
    ScrollBar(glm::vec3 p, glm::vec2 s, int elements);
    ~ScrollBar() override;
    
    void Draw() override;
    void Scroll(int s);
    void SetElements(int e, int scrCount);
    void MaxButtons(int mb);
};

