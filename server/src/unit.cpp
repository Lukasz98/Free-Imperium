#include "unit.h"

int Unit::FreeId = 0;

Unit::Unit(std::string name, glm::vec3 pos, int soldiers, std::string country, std::string fromProvince)
{
    this->name = name;
    this->soldiers = soldiers;
    this->id = FreeId;
    this->fromProvince = fromProvince;
    FreeId++;
    this->country = country;
    fakePos = pos;
    position = pos;
}
    
Unit::~Unit()
{

}

void Unit::Update()
{
    if (moves.size() && fakePos == position) {
        if (moves[0].daysLeft <= 0) {
            position.x = moves[0].destiny.x;
            position.y = moves[0].destiny.y;
            fakePos = position;
            moves.erase(moves.begin());
            return;
        }
        else
            moves[0].daysLeft--;
    }
}

void Unit::AddMove(std::vector<Move> ms)
{
    moves = ms;
    glm::vec2 lastPos = {position.x, position.y};
    for (auto & m : moves) {
        float x = m.destiny.x - lastPos.x;
        float y = m.destiny.y - lastPos.y;
        float e = sqrt(x*x + y*y);
        m.daysLeft = (int)(e / speed + 0.5f);
        lastPos = {m.destiny.x, m.destiny.y};
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
    killed += amount;
    
    if (soldiers < 0)
        soldiers = 0;

    if (morale < 0)
        morale = 0;
}

void Unit::Battle(bool is, bool attacker)
{
    isInBattle = is;
    if (is) {
        fakePos = position;
        if (attacker) { position.x -= 15; position.y -= 10; }
        else { position.x += 15; }
    }
    else {
        position = fakePos;
    }
}

void Unit::Add(const std::shared_ptr<Unit> & u)
{
    morale = (float)(soldiers * morale + u->GetSoldiers() * u->GetMorale()) / (soldiers + u->GetSoldiers()) + 0.5f;
    soldiers += u->GetSoldiers();
}

