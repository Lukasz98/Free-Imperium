#include "unit.h"

#include "num_to_string.h"
Unit::Unit() : rotateX(60.0f * 3.1459265f / 180.0f), yScale(10.0f) {}

Unit::~Unit() {}

void Unit::Update(glm::vec3 p, std::vector<glm::vec3> mvs, int soldiersCount)
{
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

void Unit::AddMove(std::vector<Move> ms)
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
    model = glm::mat4(1.0);
    model = glm::translate(model, position);

    this->model = model;
    rotate = glm::rotate(glm::mat4{1.0}, rotateX, glm::vec3{1.0, 0.0, 0.0});
    model = model * rotate;
    model = glm::scale(model, glm::vec3{20.0, yScale, 10.0});
}

