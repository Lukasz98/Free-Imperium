#pragma once
#include <iostream>
#include <vector>

class War
{
    int id = 0, idInGui = 0;
    std::vector<std::string> attackers;
    std::vector<std::string> defenders;

    int attackerWarScore = 0;
    int attWarScFromSiege = 0;
    bool end = false;

public:
    War(int id, int idInGui);
    
    void AddBattleWinner(std::string winner);

    void Update();
    void Erase(std::string name);

    void AttackerSiege(int s) { attWarScFromSiege += s; }
    
    void AddAttacker(std::string a) { attackers.push_back(a); }
    void AddDefender(std::string d) { defenders.push_back(d); }

    bool ShouldTheyFight(std::string b, std::string c) const;
    
    int GetId() const { return id; }
    int GetIdInGui() const { return idInGui; }
    std::string GetAttackerName() const { if (attackers.size()) return attackers[0];  return "mydelko"; }
    std::string GetDefenderName() const { if (defenders.size()) return defenders[0]; return "kaszka"; }
    int GetAttackerWarScore() const { return attackerWarScore + attWarScFromSiege; }
    void SetWarScore(int score) { attackerWarScore = score; }
    
    std::vector<std::string> GetAttackers() const { return attackers; }
    std::vector<std::string> GetDefenders() const { return defenders; }
    
    bool IsAttacker(std::string c) const;
    bool IsDefender(std::string c) const;
    
    bool IsEnded() const { return end; }

};
        
