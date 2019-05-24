#include "war.h"

War::War(int id, int idInGui)
{
    this->id = id;
    this->idInGui = idInGui;
}

void War::AddBattleWinner(std::string winner)
{
    for (auto & a : attackers) {
        if (a == winner) {
            attackerWarScore += 10;
            return;
        }
    }

    for (auto & d : defenders) {
        if (d == winner) {
            attackerWarScore -= 10;
            return;
        }
    }
}

void War::Update()
{
    attWarScFromSiege = 0;
}

void War::Erase(std::string name)
{
    for (auto i = attackers.begin(); i < attackers.end(); i++)
        if (*i == name) {
            attackers.erase(i);
            return;
        }
    for (auto i = defenders.begin(); i < defenders.end(); i++)
        if (*i == name) {
            defenders.erase(i);
            return;
        }
}

bool War::ShouldTheyFight(std::string b, std::string c) const
{
    if (b == c)
        return false;
    
    for (auto & a : attackers)
        if (a == b)
            for (auto & d : defenders)
                if (d == c)
                    return true;

    for (auto & a : attackers)
        if (a == c)
            for (auto & d : defenders)
                if (d == b)
                    return true;

    return false;
}
    
bool War::IsAttacker(std::string c) const
{
    for (auto & a : attackers)
        if (a == c)
            return true;
    return false;
}

bool War::IsDefender(std::string c) const
{
    for (auto & d : defenders)
        if (d == c)
            return true;
    return false;
}

