#pragma once
#include <iostream>
#include <vector>
#include <algorithm>
#include "log.h"
//#include "country.h" // only for PeaceOffer

class War
{
    int id = 0;
    std::vector<int> attackers;
    std::vector<int> defenders;

    std::vector<int> attackersProvs;
    std::vector<int> defendersProvs;

    int attackerWarScore = 0;
    int attWarScFromSiege = 0;
    int attWarScFromMonths = 0;
    bool end = false;
    int months = 0;

    void checkWarScFromMonths(int attScFromPr);
    
public:
    static int id_num;

    War();

    //void Peace(PeaceOffer offer);
    void AddBattleWinner(int winnerId);

    void Update();
    void Erase(int name);
    void AttackerSiege(int s) { attWarScFromSiege += s; }
    
    void AddAttacker(int cId) { attackers.push_back(cId); }
    void AddDefender(int cId) { defenders.push_back(cId); }

    void AddProv(int provId, int countryId);
    void DeleteProv(int provId);

    bool ShouldTheyFight(int cId1, int cId2) const;
    
    int GetId() const { return id; }
    int GetAttackerId() const { if (attackers.size()) return attackers[0]; return -1; } //hmm -1
    int GetDefenderId() const { if (defenders.size()) return defenders[0]; return -1; }
    int GetAttackerWarScore();

    std::vector<int> GetAttackers() const { return attackers; }
    std::vector<int> GetDefenders() const { return defenders; }
    
    bool IsAttacker(int cId) const;
    bool IsDefender(int cId) const;
    
    bool IsEnded() const { return end; }
    int GetMonths() const { return months; }
    void IncreaseMonth();

};
        
