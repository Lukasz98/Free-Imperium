#include "siege.h"

Siege::Siege()
{

}

Siege::~Siege()
{
    for (auto & u : units)
        u->SetSieging(false);
}

void Siege::Update()
{
    bool doneBefore = (level == 100);
    for (auto & u : units) {
        if (u->IsInBattle())
            continue;
        int power = u->GetSiegePower();
        level += power;
        if (level >= 100) {
            level = 100;
            done = true;
            break;
        }
    }
    if (done && !doneBefore)
        stateChanged = true;
    if (done) {
        for (auto & u : units)
            u->SetSieging(false);
    }
}

void Siege::Unsiege(int power)
{
    if (units.size() != 0)
        return;
    bool doneBefore = (level == 0);
    unsiege = true;
    level -= power;
    if (level <= 0) {
        //stateChanged = true;
        level = 0;
        leader = -1;
        done = false;
    }
    if (level == 0 && !doneBefore)
        stateChanged = true;
}

void Siege::StopUnsiege()
{
    unsiege = false;
    if (done) {
        level = 100;
    }
}

void Siege::AddUnit(std::shared_ptr<Unit> & u) 
{ 
    if (isUnitIn(u) != units.end())
        return;
    if (leader == -1)
        leader = u->GetCountryId();
    units.push_back(u);
}

void Siege::EraseUnit(std::shared_ptr<Unit> & u)
{
    auto it = isUnitIn(u);
    if (it == units.end())
        return;
   
    u->SetSieging(false);
    units.erase(it);
    if (units.size() && level != 100) {
        leader = units[0]->GetCountryId();
        // tu powinno sie ustawic w pierwszej kolejnosci kogos z tego samego obozu, co poprzedni lider
    }
    else if (units.size() == 0 && level != 100 && !unsiege) {
        leader = -1;
        level = 0;
    }
}

std::vector<std::shared_ptr<Unit>>::iterator Siege::isUnitIn(std::shared_ptr<Unit> & u)
{
    auto it = std::find_if(units.begin(), units.end(), [id = u->GetId()](std::shared_ptr<Unit> & unit) {
        return (unit->GetId() == id);
    });
    return it;
}

int Siege::GetSiegeSoldiersCount() const
{
    int count = 0;
    for (auto & u : units) {
        count += u->GetSoldiers();
    }
    return count;
}
