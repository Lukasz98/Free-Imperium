#pragma once
#include <algorithm>
#include <sstream>

#include "log.h"
#include "color.h"
#include "month_cash.h"
#include "data_object.h"
#include "observer.h"
#include "ctr_data.h"


class Country
{
    int id;
    Color color;
    std::string name, culture, religion;
    float gold = 0.0f, income = 0.0f;
    float armyMaintenance = 0.0f;
    int population = 0;
    int manpower = 0;
    int prestige = 0;
    int totalDevelopment = 0;
    
    std::vector<std::pair<std::string,int>> relations;
    std::vector<std::string> impRelWith;
    
public:


    Country(const CountryData& cd, const std::string& name);
    Country(DataObj * obj);
    ~Country();

    Subject subject, subjectOfMyCountry; // for gui windows

    void UpdateGuiWin();
    void UpdateMyCountryGuiWin();

    bool Apply(/*PeaceOffer offer*/);
    
    void SetName(std::string n) { name = n; }
    void SetCulture(std::string c) { culture = c; }
    void SetReligion(std::string r) { religion = r; }
    void SetColor(Color c) { color = c; }    
    void SetGold(float g) { gold = g; }
    void SetIncome(float i) { income = i; }

    void SetRel(const std::string & c, int r = 2);
    void StartImprRel(const std::string & c);
    void StopImprRel(const std::string & c);
    void SetImprRel(const std::vector<std::string> & ir);
    int GetRel(const std::string & c);
    bool IsImprRel(const std::string & c);

    float GetGold() { return gold; }
    float GetIncome() { return income; }
    float GetArmyMaintenance() { return armyMaintenance; }
    void SetArmyMaintenance(float m) { armyMaintenance = m; }

    inline int GetId() const { return id; }        
    const std::string & GetName() const { return name; }
    std::string GetCulture() const { return culture; }
    std::string GetReligion() const { return religion; }
    float GetGold() const { return gold; }
    Color GetColor() const { return color; }
    int GetTotalDevelopment() const { return totalDevelopment; }

    const std::vector<std::pair<std::string,int>> & GetRelations() const { return relations; }

};

