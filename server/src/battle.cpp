#include "battle.h"
    
int Battle::FreeId = 0;

Battle::Battle(int warId, glm::vec3 pos, std::string prov)
    : warId(warId), position(pos), provName(prov)
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
        if (IsInFight(a)) {
            if (position != a->GetFakePos() || a->GetSoldiers() <= 0)
                toDelete.push_back(a->GetId());
            else {
                attackersSize += a->GetSoldiers();
                attackersMorale += a->GetMorale();
                a_attack += a->GetAttack();
                a_defense += a->GetDefense();
            }
        }
    }
    for (auto & d : defenders) {
        if (IsInFight(d)) {
            if (position != d->GetFakePos() || d->GetSoldiers() <= 0)
                toDelete.push_back(d->GetId());
            else {
                defendersSize += d->GetSoldiers();
                defendersMorale += d->GetMorale();
                d_attack += d->GetAttack();
                d_defense += d->GetDefense();
            }
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

bool Battle::IsCountryInFight(const std::string & country) const
{
    for (auto & a : attackers)
        if (a->GetCountry() == country) 
            return true;

    for (auto & d : defenders)
        if (d->GetCountry() == country)
            return true;

    return false;    
}

bool Battle::IsInFight(const std::shared_ptr<Unit> b) const
{
    for (auto & a : attackers)
        if (a->GetId() == b->GetId())
            return true;

    for (auto & d : defenders)
        if (d->GetId() == b->GetId())
            return true;

    return false;
}

bool Battle::ShouldFight(const std::shared_ptr<Unit> b) const
{
    for (auto & a : attackers)
        if (a->GetCountry() == b->GetCountry())
            return true;
        
    for (auto & d : defenders)
        if (d->GetCountry() == b->GetCountry())
            return true;

    return false;
}

void Battle::AddAttacker(std::shared_ptr<Unit> p)
{
    if (!attackers.size())
        attacker = p->GetCountry();
    updated = true;
    p->Battle(true, true);
    attackers.push_back(p);
}
    
void Battle::AddDefender(std::shared_ptr<Unit> p)
{
    if (!defenders.size())
        defender = p->GetCountry();
    updated = true;
    p->Battle(true, false);
    defenders.push_back(p);
}

void Battle::AddUnit(std::shared_ptr<Unit> p)
{
    updated = true;
    for (auto & a : attackers)
        if (a->GetCountry() == p->GetCountry()) {
            AddAttacker(p);
            return;
        }
    for (auto & d : defenders)
        if (d->GetCountry() == p->GetCountry()) {
            AddDefender(p);
            return;
        }
}

void Battle::Erase(int id)
{
    for (int i = 0; i < attackers.size(); i++)
        if (attackers[i]->GetId() == id) {
            attackers[i]->Battle(false);
            attackers.erase(attackers.begin() + i);
            updated = true;
            return;
        }
        
    for (int i = 0; i < defenders.size(); i++)
        if (defenders[i]->GetId() == id) {
            defenders[i]->Battle(false);
            defenders.erase(defenders.begin() + i);
            updated = true;
            return;
        }
}
    
std::string Battle::GetWinner()
{
    if (attackers.size() == 0) {
        endBattle();
        return defender;
    }
    if (defenders.size() == 0) {
        endBattle();
        return attacker;
    }
        
    return "none";
}

void Battle::endBattle()
{
    ended = true;
    for (auto & a : attackers)
        a->Battle(false);
    for (auto & d : defenders)
        d->Battle(false);
}
