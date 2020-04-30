#pragma once
#include <memory>
#include <utility>

#include <glm/vec3.hpp>

#include "unit.h"
#include "observer.h"

class Battle : public Subject
{
    std::vector<std::shared_ptr<Unit>> attackers;
    std::vector<std::shared_ptr<Unit>> defenders;

    int id, warId, provId;
    glm::vec3 position;
    std::string provName;
    bool updated = false;
    int attackersSize = 0;
    int defendersSize = 0;
    int attackersMorale = 0;
    int defendersMorale = 0;
    std::string attacker = "";
    std::string defender = "";
    int attackerId, defenderId;

    
public:
    Battle(int id, int warId, glm::vec3 pos, std::string prov, int provId);
    ~Battle();
    
    void Update();
    bool IsInFight(const std::shared_ptr<Unit> b);
    bool ShouldFight(const std::shared_ptr<Unit> b);
    void AddAttacker(std::shared_ptr<Unit> p);
    void AddDefender(std::shared_ptr<Unit> p);
    void AddUnit(std::shared_ptr<Unit> p);
    void Erase(int id);

    void SetAttackersSize(int s) { attackersSize = s; }
    void SetDefendersSize(int s) { defendersSize = s; }

    std::string GetWinner();

    void EndBattle();

    glm::vec3 GetPosition() const
    {
        return position;
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

    std::unordered_map<std::string, std::string> GetValues() const;
    
    int GetAttackersSize() const { return attackersSize; }
    int GetDefendersSize() const { return defendersSize; }
    inline int GetProvId() const { return provId; }
    //std::string GetProvName() const { return provName; }
    bool WasUpdated() { bool r = updated; updated = 0; return r; }
    int GetWarId() const { return warId; }
    int GetId() const { return id; }
    glm::vec3 GetPos() const { return position; }
};
