#include "war.h"


int War::id_num = 0;

War::War()
{
    this->id = id_num;
    id_num++;
}

void War::AddBattleWinner(std::string winner)
{
    for (auto & a : attackers) {
        if (a == winner) {
            attackerWarScore += 2;
            return;
        }
    }

    for (auto & d : defenders) {
        if (d == winner) {
            attackerWarScore -= 2;
            return;
        }
    }
}

void War::Update()
{
    attWarScFromSiege = 0;
}

void War::AddProv(const std::string & prov, const std::string & country)
{
    if (IsAttacker(country)) {
        auto p = std::find(attackersProvs.begin(), attackersProvs.end(), prov);
        if (p == attackersProvs.end())
            attackersProvs.emplace_back(prov);
    }
    else {
        auto p = std::find(defendersProvs.begin(), defendersProvs.end(), prov);
        if (p == defendersProvs.end())
            defendersProvs.emplace_back(prov);
    }
}

void War::DeleteProv(const std::string & prov)
{
    for (int i = 0; i < attackersProvs.size(); i++)
        if (attackersProvs[i] == prov) {
            attackersProvs.erase(attackersProvs.begin() + i);
            return;
        }
    for (int i = 0; i < defendersProvs.size(); i++)
        if (defendersProvs[i] == prov) {
            defendersProvs.erase(defendersProvs.begin() + i);
            return;
        }
}

void War::Erase(std::string name)
{
    for (int i = 0; i < attackers.size(); i++)
        if (attackers[i] == name) {
            attackers.erase(attackers.begin() + i);
            return;
        }
    for (int i = 0; i < defenders.size(); i++)
        if (defenders[i] == name) {
            defenders.erase(defenders.begin() + i);
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

int War::GetAttackerWarScore()
{
    int fromProvs = 0;
    for (auto & a : attackersProvs)
        fromProvs -= 10;
    for (auto & d : defendersProvs)
        fromProvs += 10;

    checkWarScFromMonths(fromProvs);
    
    return attackerWarScore + fromProvs + attWarScFromMonths;
}

void War::checkWarScFromMonths(int attScFromPr)
{
    if (attWarScFromMonths > 0 && attackerWarScore + attScFromPr < 0)
        attWarScFromMonths = 0;
    else if (attWarScFromMonths < 0 && attackerWarScore + attScFromPr > 0)
        attWarScFromMonths = 0;
}

void War::IncreaseMonth()
{
    months++;
    if (GetAttackerWarScore() < 0)
        attWarScFromMonths--;
    else if (GetAttackerWarScore() > 0)
        attWarScFromMonths++;
}
