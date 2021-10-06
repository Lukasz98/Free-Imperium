#pragma once
#include <memory>
#include <utility>
#include <glm/vec3.hpp>
#include "unit.h"


class Battle
{
    std::vector<std::shared_ptr<Unit>> attackers;
    std::vector<std::shared_ptr<Unit>> defenders;

    int id, warId;
    glm::vec3 position;
    //std::string provName;
    int provId;
    bool updated = false;
    int attackersSize = 0;
    int defendersSize = 0;
    int attackersMorale = 0;
    int defendersMorale = 0;
    int attackerId;
    int defenderId;
    bool ended = false;
    
    void endBattle();
    
    void Erase(int id);
public:
    static int FreeId;
    Battle(int warId, glm::vec3 pos, int provId);
    
    void Update();

    bool IsCountryInFight(int countryId) const;
    bool IsInFight(const std::shared_ptr<Unit> & b) const;
    bool ShouldFight(const std::shared_ptr<Unit> & b) const;
    void AddAttacker(std::shared_ptr<Unit> & p);
    void AddDefender(std::shared_ptr<Unit> & p);
    void AddUnit(std::shared_ptr<Unit> & p);
    int GetWinnerId();

    bool IsAttacker(int unitId) const
    {
        for (auto & a : attackers) {
            if (a->GetId() == unitId) {
                return true;
            }
        }
        return false;
    }
    

    std::string GetAttackerName() const
    {
        if (attackers.size())
            return attackers[0]->GetCountry();
        return "Floating water";
    }

    std::string GetDefenderName() const
    {
        if (defenders.size())
            return defenders[0]->GetCountry();
        return "Floating fire";
    }

    int GetAttackersSize() const { return attackersSize; }
    int GetDefendersSize() const { return defendersSize; }
    int GetWarId() const { return warId; }
    int GetId() const { return id; }
    bool WasUpdated() { bool r = updated; updated = 0; return r; }
    bool IsEnded() const { return ended; }
    glm::vec3 GetPos() const { return position; }
    glm::vec3 GetPosition() const { return position; }
    int GetProvId() const { return provId; }

};
