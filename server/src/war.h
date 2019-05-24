#pragma once
#include <iostream>
#include <vector>
#include <algorithm>

#include "country.h" // only for PeaceOffer


class War
{
    int id = 0;
    std::vector<std::string> attackers;
    std::vector<std::string> defenders;

    std::vector<std::string> attackersProvs;
    std::vector<std::string> defendersProvs;

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
    void AddBattleWinner(std::string winner);

    void Update();
    void Erase(std::string name);
    void AttackerSiege(int s) { attWarScFromSiege += s; }
    
    void AddAttacker(std::string a) { attackers.push_back(a); }
    void AddDefender(std::string d) { defenders.push_back(d); }

    void AddProv(const std::string & prov, const std::string & country);
    void DeleteProv(const std::string & prov);

    bool ShouldTheyFight(std::string b, std::string c) const;
    
    int GetId() const { return id; }
    std::string GetAttackerName() const { if (attackers.size()) return attackers[0];  return "mydelko"; }
    std::string GetDefenderName() const { if (defenders.size()) return defenders[0]; return "kaszka"; }
    int GetAttackerWarScore();

    std::vector<std::string> GetAttackers() const { return attackers; }
    std::vector<std::string> GetDefenders() const { return defenders; }
    
    bool IsAttacker(std::string c) const;
    bool IsDefender(std::string c) const;
    
    bool IsEnded() const { return end; }
    int GetMonths() const { return months; }
    void IncreaseMonth();

};
        
