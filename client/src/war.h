#pragma once
#include <iostream>
#include <vector>

class War
{
    int id = 0, idInGui = 0;
    std::vector<int> attackersIds;
    std::vector<int> defendersIds;
    std::vector<std::string> attackers;
    std::vector<std::string> defenders;

    int attackerWarScore = 0;
    int attWarScFromSiege = 0;
    bool end = false;

public:
    War(int id, int idInGui);
    
    void AddBattleWinner(std::string winner);

    void Update();
    void Erase(int ctrId);

    void AttackerSiege(int s) { attWarScFromSiege += s; }
    
    void AddAttacker(std::string a, int id) { attackers.push_back(a); attackersIds.push_back(id); }
    void AddDefender(std::string d, int id) { defenders.push_back(d); defendersIds.push_back(id); }

    bool ShouldTheyFight(std::string a, std::string b) const;
    bool ShouldTheyFight(int ctr1Id, int ctr2Id) const;
    
    int GetId() const { return id; }
    int GetIdInGui() const { return idInGui; }
    std::string GetAttackerName() const { if (attackers.size()) return attackers[0];  return "mydelko"; }
    std::string GetDefenderName() const { if (defenders.size()) return defenders[0]; return "kaszka"; }
    int GetAttackerWarScore() const { return attackerWarScore + attWarScFromSiege; }
    void SetWarScore(int score) { attackerWarScore = score; }
    
    std::vector<std::string> GetAttackers() const { return attackers; }
    std::vector<std::string> GetDefenders() const { return defenders; }
    
    bool IsAttacker(int ctrId) const;
    bool IsDefender(int ctrId) const;
    
    bool IsEnded() const { return end; }

};
        
