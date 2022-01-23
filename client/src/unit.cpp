#include "unit.h"

#include "num_to_string.h"
Unit::Unit() : rotateX(60.0f * 3.1459265f / 180.0f), yScale(10.0f) {}

Unit::~Unit() {}

void Unit::Update(glm::vec3 p, std::vector<glm::vec3> mvs, int soldiersCount)
{
    visible = true;
    if (soldiers != soldiersCount) {
        soldiers = soldiersCount;
    }
    if (p != position) {
        position = p;
        fakePos = p;
    }

    bool newMoves = false;
    for (auto& move : moves) {
        for (auto& nmv : mvs) {
            if (move.destiny != nmv) {
                newMoves = true;
                break;
            }
        }
    }

    if (moves.size() != mvs.size())
        newMoves = true;

    if (newMoves) {
        std::vector<Move> nMoves;
        for (auto& m : mvs) {
            Move move;
            move.destiny = m;
            nMoves.emplace_back(move);
        }
        AddMove(nMoves);
    }
}

void Unit::AddMove(std::vector<Move> ms)  // chyba moze byc prywatne
{
    for (std::size_t i = 0; i < ms.size(); i++) {
        float unitX = position.x;
        float unitY = position.y;
        if (i > 0) {
            unitX = ms[i - 1].destiny.x;
            unitY = ms[i - 1].destiny.y;
        }

        glm::vec3 ap1 = {unitX, unitY, 10.0};
        glm::vec3 ap2 = {ms[i].destiny.x, ms[i].destiny.y, 10.0};

        double rectWidth = 10.0;
        glm::vec3 ap3 = {ap1.x + rectWidth, ap1.y + rectWidth, 10.0};
        glm::vec3 ap4 = {ap2.x + rectWidth, ap2.y + rectWidth, 10.0};

        ms[i].arrow = std::make_shared<Arrow>(ap1, ap2, ap3, ap4);
    }
    moves = ms;
}

void Unit::Draw(glm::mat4 matrix, bool isSelected)
{
    if (!visible)
        return;

    model = glm::mat4(1.0);
    model = glm::translate(model, position);

    this->model = model;
    rotate = glm::rotate(glm::mat4{1.0}, rotateX, glm::vec3{1.0, 0.0, 0.0});
    model = model * rotate;
    model = glm::scale(model, glm::vec3{20.0, yScale, 10.0});
}

void Unit::DrawGuiElements(bool isSelected)
{
    if (isSelected) {
        for (auto& m : moves) {
            m.arrow->Draw(false);
        }
    }
}

bool Unit::Click(glm::vec3 vv, glm::vec3 eye)
{
    if (!visible)
        return false;
    else {
        return false;
    }
}

void Unit::Kill(int amount)
{
    if (soldiers == 0)
        return;
    int m = amount / soldiers * 1000;
    if (m < 1 && amount > 0)
        m = 1;

    morale -= m;
    soldiers -= amount;

    if (soldiers < 0)
        soldiers = 0;

    if (morale < 0)
        morale = 0;
}

void Unit::Battle(bool is, bool attacker)
{
    isInBattle = is;
    if (is) {
    }
}

void Unit::UpdateDt(float d)
{
    dt += d;

    if (dt > 0.5f) {
        dt = 0.0f;
        activeFrame++;
        if (activeFrame > 1)
            activeFrame = 0;
    }
}

