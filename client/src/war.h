#pragma once
#include <iostream>
#include <vector>
#include "num_to_string.h"

class War
{
public:
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
    std::string GetAttackerName() const { if (attackers.size()) return attackers[0];  return "mydelko"; }
    std::string GetDefenderName() const { if (defenders.size()) return defenders[0]; return "kaszka"; }
    int GetAttackerWarScore() const { return attackerWarScore + attWarScFromSiege; }
    void SetWarScore(int score) { attackerWarScore = score; }

    int GetAttacker() { if (attackersIds.size()) return attackersIds[0]; else return -1; }
    int GetDefender() { if (defendersIds.size()) return defendersIds[0]; else return -1; }

    std::vector<std::string> GetAttackers() const { return attackers; }
    std::vector<std::string> GetDefenders() const { return defenders; }
    
    bool IsAttacker(int ctrId) const;
    bool IsDefender(int ctrId) const;
    
    bool IsEnded() const { return end; }

};
        
