#include "battle.h"
    
Battle::Battle(int id, int warId, glm::vec3 pos, std::string prov, int provId)
   : id(id), warId(warId), position(pos), provName(prov), provId(provId)
{
}

Battle::~Battle()
{
}

void Battle::Update()
{
    //subject.Notify(GetValues());
    /*
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
    */       
}
    /*
bool Battle::IsInFight(const std::shared_ptr<Unit> b)
{
    for (auto & a : attackers)
        if (a->GetId() == b->GetId())
            return true;

    for (auto & d : defenders)
        if (d->GetId() == b->GetId())
            return true;

    return false;
}

bool Battle::ShouldFight(const std::shared_ptr<Unit> b)
{
    for (auto & a : attackers)
        if (a->GetCountryId() == b->GetCountryId())
            return true;
        
    for (auto & d : defenders)
        if (d->GetCountry() == b->GetCountry())
            return true;

    return false;
}
*/
void Battle::AddAttacker(std::shared_ptr<Unit> p)
{
    if (!attackers.size()) {
        attackerId = p->GetCountryId();
        attacker = p->GetCountry();
    }
    updated = true;
    p->Battle(true, true);
    attackers.push_back(p);
    //subject.Notify(GetValues());
}
    
void Battle::AddDefender(std::shared_ptr<Unit> p)
{
    if (!defenders.size()) {
        defenderId = p->GetCountryId();
        defender = p->GetCountry();
    }
    updated = true;
    p->Battle(true, false);
    defenders.push_back(p);
    //subject.Notify(GetValues());
}

void Battle::AddUnit(std::shared_ptr<Unit> p)
{
    //updated = true;
    for (auto & a : attackers)
        if (a->GetCountryId() == p->GetCountryId()) {
            AddAttacker(p);
            //subject.Notify(GetValues());
            return;
            //attackers.push_back(p);
            //break;
        }
    for (auto & d : defenders)
        if (d->GetCountryId() == p->GetCountryId()) {
            AddDefender(p);
            //subject.Notify(GetValues());
            return;
            //defenders.push_back(p);
            //break;
        }
}

void Battle::Erase(int id)
{
    for (std::size_t i = 0; i < attackers.size(); i++)
        if (attackers[i]->GetId() == id) {
            attackers[i]->Battle(false);
            attackers.erase(attackers.begin() + i);
            updated = true;
            //subject.Notify(GetValues());
            return;
        }
        
    for (std::size_t i = 0; i < defenders.size(); i++)
        if (defenders[i]->GetId() == id) {
            defenders[i]->Battle(false);
            defenders.erase(defenders.begin() + i);
            updated = true;
            //subject.Notify(GetValues());
            return;
        }
}
    
std::string Battle::GetWinner()
{
    if (attackers.size() == 0) {
        EndBattle();
        return defender;
    }
    if (defenders.size() == 0) {
        EndBattle();
        return attacker;
    }
        
    return "none";
}

void Battle::EndBattle()
{
    for (auto & a : attackers)
        a->Battle(false);
    for (auto & d : defenders)
        d->Battle(false);
}

std::unordered_map<std::string, std::string> Battle::GetValues() const
{
    std::unordered_map<std::string, std::string> result;
    result["attackersSize"] = std::to_string(attackersSize);
    result["defendersSize"] = std::to_string(defendersSize);
    result["attackersMorale"] = std::to_string(attackersMorale);
    result["defendersMorale"] = std::to_string(defendersMorale);
    result["attacker"] = attacker;
    result["defender"] = defender;

    return result;
}
