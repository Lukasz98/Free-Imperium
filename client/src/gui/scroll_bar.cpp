#include "scroll_bar.h"

ScrollBar::ScrollBar(glm::vec3 p, glm::vec2 s, int elements, Color boatColor)
    : Rectangle(p, s, glm::vec4{1.0,1.0,1.0,1.0}), elements(elements), boatColor(boatColor)
{
    scrollCount = 0;
    maxButtons = 2;
    init();
}

ScrollBar::~ScrollBar()
{
    
}

void ScrollBar::init()
{
    padding = {0.0f, 0.0f};
    float boatHMax = size.y - padding.y * 2;
    float boatH = 0.0f;

    if (elements > maxButtons)
        boatH = boatHMax / elements;
    else
        boatH = boatHMax;

    speed = (boatHMax - boatH) / scrollCount;    
    glm::vec2 boatSize = {size.x - padding.x* 2, boatH};
    if (boat){
        boat->SetPosition(glm::vec3{position.x + padding.x, position.y + size.y - padding.y - boatH, position.z});
        boat->SetSize(boatSize);
    }
    else {
        boat = std::make_unique<Rectangle>(glm::vec3{position.x + padding.x, position.y + size.y - padding.y - boatH, position.z},
                                           boatSize,
                                           glm::vec4{(double)boatColor.r / 255, (double)boatColor.g / 255, (double)boatColor.b / 255, (double)boatColor.a / 255}
                                          );
    }
}

void ScrollBar::Scroll(int s)
{
    glm::vec3 oldPos = boat->GetPosition();
    glm::vec3 newPos = glm::vec3{oldPos.x, oldPos.y + speed * s, oldPos.z};

    if (newPos.y < position.y + padding.y)
        newPos.y = position.y + padding.y;

    if (newPos.y + boat->GetSize().y > position.y + size.y - padding.y)
        newPos.y = position.y + size.y - padding.y - boat->GetSize().y;

    boat->SetPosition(newPos);
}

void ScrollBar::SetElements(int e, int scrCount)
{
    elements = e;
    scrollCount = scrCount;
    init();
}


void ScrollBar::MaxButtons(int mb)
{
    maxButtons = mb;
}

void ScrollBar::Draw()
{
    Rectangle::Draw();
    boat->Draw();
}
