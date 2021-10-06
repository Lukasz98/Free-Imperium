#include "battle.h"
    
int Battle::FreeId = 0;

Battle::Battle(int warId, glm::vec3 pos, int prov)
    : warId(warId), position(pos), provId(prov)
{
    id = FreeId;
    FreeId++;
}

void Battle::Update()
{
    if (ended)
        return;
    attackersMorale = 0;
    defendersMorale = 0;

    attackersSize = 0;
    defendersSize = 0;

    int a_attack = 0;
    int d_attack = 0;
    int a_defense = 0;
    int d_defense = 0;
    
    std::vector<int> toDelete;
    for (auto & a : attackers) {
        if (position != a->GetFakePos() || a->GetSoldiers() <= 0)
            toDelete.push_back(a->GetId());
        else {
            attackersSize += a->GetSoldiers();
            attackersMorale += a->GetMorale();
            a_attack += a->GetAttack();
            a_defense += a->GetDefense();
        }
    }
    for (auto & d : defenders) {
        if (position != d->GetFakePos() || d->GetSoldiers() <= 0)
            toDelete.push_back(d->GetId());
        else {
            defendersSize += d->GetSoldiers();
            defendersMorale += d->GetMorale();
            d_attack += d->GetAttack();
            d_defense += d->GetDefense();
        }
    }
    
    for (auto td : toDelete)
        Erase(td);

    if (attackers.size() && defenders.size()) {
        attackersMorale /= attackers.size();
        defendersMorale /= defenders.size();
            
        int lost = (int)((float)d_attack / attackers.size() + 0.5f);
        for (auto & a : attackers) {
            a->Kill(lost);
        }

        lost = (int)((float)a_attack / defenders.size() + 0.5f);
        for (auto & d : defenders) {
            d->Kill(lost);
        }
    }
    else
        endBattle();
}

bool Battle::IsCountryInFight(int country) const
{
    for (auto & a : attackers)
        if (a->GetCountryId() == country) 
            return true;

    for (auto & d : defenders)
        if (d->GetCountryId() == country)
            return true;

    return false;    
}

bool Battle::IsInFight(const std::shared_ptr<Unit> & b) const
{
    for (auto & a : attackers)
        if (a->GetId() == b->GetId())
            return true;

    for (auto & d : defenders)
        if (d->GetId() == b->GetId())
            return true;

    return false;
}

bool Battle::ShouldFight(const std::shared_ptr<Unit> & b) const
{
    for (auto & a : attackers)
        if (a->GetCountry() == b->GetCountry())
            return true;
        
    for (auto & d : defenders)
        if (d->GetCountry() == b->GetCountry())
            return true;

    return false;
}

void Battle::AddAttacker(std::shared_ptr<Unit> & p)
{
    if (!attackers.size())
        attackerId = p->GetCountryId();
    updated = true;
    p->Battle(true, true);
    attackers.push_back(p);
}
    
void Battle::AddDefender(std::shared_ptr<Unit> & p)
{
    if (!defenders.size())
        defenderId = p->GetCountryId();
    updated = true;
    p->Battle(true, false);
    defenders.push_back(p);
}

void Battle::AddUnit(std::shared_ptr<Unit> & p)
{
    updated = true;
    for (auto & a : attackers)
        if (a->GetCountryId() == p->GetCountryId()) {
            AddAttacker(p);
            return;
        }
    for (auto & d : defenders)
        if (d->GetCountryId() == p->GetCountryId()) {
            AddDefender(p);
            return;
        }
}

void Battle::Erase(int id)
{
     for (auto it = attackers.begin(); it < attackers.end(); it++)
        if ((*it)->GetId() == id) {
            (*it)->Battle(false);
            attackers.erase(it);
            updated = true;
            return;
        }
        
    for (auto it = defenders.begin(); it < defenders.end(); it++)
        if ((*it)->GetId() == id) {
            (*it)->Battle(false);
            defenders.erase(it);
            updated = true;
            return;
        }
}

int Battle::GetWinnerId()
{
    if (attackers.size() == 0) {
        endBattle();
        return defenderId;
    }
    if (defenders.size() == 0) {
        endBattle();
        return attackerId;
    }
        
    return -1; //to sie moze zemscic
}

void Battle::endBattle()
{
    if (ended)
        return;
    ended = true;
    for (auto & a : attackers) {
        a->Battle(false);
    }
    for (auto & d : defenders) {
        d->Battle(false);
    }
}
