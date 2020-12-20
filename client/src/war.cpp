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

void War::Erase(int ctrId)
{
    for (std::size_t i = 0; i < attackers.size(); i++)
        if (attackersIds[i] == ctrId) {
            attackers.erase(attackers.begin() + i);
            attackersIds.erase(attackersIds.begin() + i);
            return;
        }
    for (std::size_t i = 0; i < defenders.size(); i++)
        if (defendersIds[i] == ctrId) {
            defenders.erase(defenders.begin() + i);
            defendersIds.erase(defendersIds.begin() + i);
            return;
        }
}

bool War::ShouldTheyFight(std::string b, std::string c) const
{
    if (b == c)
        return false;
    
    for (const auto & a : attackers)
        if (a == b)
            for (const auto & d : defenders)
                if (d == c)
                    return true;

    for (const auto & a : attackers)
        if (a == c)
            for (const auto & d : defenders)
                if (d == b)
                    return true;

    return false;

}

bool War::ShouldTheyFight(int b, int c) const
{
    if (b == c)
        return false;
    
    for (auto a : attackersIds)
        if (a == b)
            for (auto d : defendersIds)
                if (d == c)
                    return true;

    for (auto a : attackersIds)
        if (a == c)
            for (auto d : defendersIds)
                if (d == b)
                    return true;

    return false;
}
    
bool War::IsAttacker(int ctrId) const
{
    for (auto a : attackersIds)
        if (a == ctrId)
            return true;
    return false;
}

bool War::IsDefender(int ctrId) const
{
    for (auto d : defendersIds)
        if (d == ctrId)
            return true;
    return false;
}

void War::UpdateWinValues()
{
    std::vector<GuiStruct> vals;
    vals.push_back({1, 2, attackers[0]});
    vals.push_back({2, 2, defenders[0]});
    vals.push_back({3, 2, itos(attackerWarScore)});
    
    subject.Notify(vals);
}

