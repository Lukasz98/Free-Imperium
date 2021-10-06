#include "war.h"


int War::id_num = 0;

War::War()
{
    this->id = id_num;
    id_num++;
}

void War::AddBattleWinner(int winnerId)
{
    for (auto & a : attackers) {
        if (a == winnerId) {
            attackerWarScore += 2;
            return;
        }
    }

    for (auto & d : defenders) {
        if (d == winnerId) {
            attackerWarScore -= 2;
            return;
        }
    }
}

void War::Update()
{
    attWarScFromSiege = 0;
}

void War::AddProv(int provId, const int countryId)
{
    if (IsAttacker(countryId)) {
        auto p = std::find(attackersProvs.begin(), attackersProvs.end(), provId);
        if (p == attackersProvs.end()) {
            attackersProvs.emplace_back(provId);
        }
    }
    else {
        auto p = std::find(defendersProvs.begin(), defendersProvs.end(), provId);
        if (p == defendersProvs.end())
            defendersProvs.emplace_back(provId);
    }
}

void War::DeleteProv(int provId) {
    for (auto it = attackersProvs.begin(); it < attackersProvs.end(); it++) {
    //for (int i = 0; i < attackersProvs.size(); i++)
        if (*it == provId) {
            attackersProvs.erase(it);
            return;
        }
    }
    for (auto it = defendersProvs.begin(); it < defendersProvs.end(); it++) {
        if (*it == provId) {
            defendersProvs.erase(it);
            return;
        }
    }
}

void War::Erase(int cId)
{
    for (auto it = attackers.begin(); it < attackers.end(); it++) {
        if (*it == cId) {
            attackers.erase(it);
            return;
        }
    }
    for (auto it = defenders.begin(); it < defenders.end(); it++) {
        if (*it == cId) {
            defenders.erase(it);
            return;
        }
    }
}

bool War::ShouldTheyFight(int cId1, int cId2) const
{
    if (cId1 == cId2)
        return false;
    
    for (auto a : attackers)
        if (cId1 == a)
            for (auto d : defenders)
                if (cId2 == d)
                    return true;

    for (auto a : attackers)
        if (cId2 == a)
            for (auto d : defenders)
                if (cId1 == d)
                    return true;

    return false;
}
    
bool War::IsAttacker(int cId) const
{
    for (auto & a : attackers)
        if (cId == a)
            return true;
    return false;
}

bool War::IsDefender(int cId) const
{
    for (auto & d : defenders)
        if (d == cId)
            return true;
    return false;
}

int War::GetAttackerWarScore()
{
    int fromProvs = 0;
    fromProvs = 10 * defendersProvs.size();
    fromProvs -= 10 * attackersProvs.size();
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
