#pragma once
#include <sstream>
#include <algorithm>
#include <memory>

#include "log.h"
#include "color.h"
#include "peace_offer.h"
#include "data_object.h"
#include "truce.h"

#include "unit.h"
#include "province.h"
#include "war.h"

struct MonthCash
{
    float income = 0.0f;
    float armyMaintenance = 0.0f;
    int development = 0;
    int population = 0, manpower = 0;
    int populationGrowth = 0;
    int manpowerRecovery = 0;
};

class Country
{
    int id = -1;
    Color color;
    bool ownedByBoot, active;
    std::string name, culture, religion;

    float gold = 10.65f;
    float income = 0.0f;
    float armyMaintenance = 0.0f;
    
    int population = 0;
    int manpower = 0;
    int populationGrowth = 0;
    int manpowerRecovery = 0;
    
    int prestige = 0;
    int totalDevelopment = 0;

    MonthCash monthUpdate;

    std::vector<std::pair<int,int>> relations;
    std::vector<int> imprRel;

    std::vector<int> poi; //provinceOfIntrest;
    std::vector<Truce> truces; //provinceOfIntrest;

    std::vector<std::shared_ptr<Unit>> units;
    std::vector<std::shared_ptr<Province>> provinces;
    std::vector<War*> wars;

public:

    Country(DataObj * obj);
    Country(int id, int r, int g, int b, const std::string& name);

    MonthCash MonthUpdate();

    void AddProvince(std::shared_ptr<Province> p);
    void RemoveProvince(std::shared_ptr<Province> p);

    void AddWar(War * w);
    void RemoveWar(War * w);
    bool IsWarIn(const War * w);
    void UpdateSiegesForWar();

    std::vector<std::shared_ptr<Province>> GetProvs() { return provinces; }

    void Cash(MonthCash mc);

    float GetPower() const
    {
        return manpower / 1000 + gold + (-1 * armyMaintenance);
    }

    bool Apply(PeaceOffer offer, const Date & date);
    
    void SetName(std::string n) { name = n; }
    void SetCulture(std::string c) { culture = c; }
    void SetReligion(std::string r) { religion = r; }
    void SetColor(Color c) { color = c; }
    void EraseMoney(float m) { gold -= m; }
    void EraseManpower(int m) { manpower -= m; }

    void SetArmyMaintenance(int am) { armyMaintenance = am; }
    
    inline int GetId() const { return id; }
    std::string GetName() { return name; }
    std::string GetCulture() { return culture; }
    std::string GetReligion() { return religion; }

    float GetGold() { return gold; }
    float GetIncome() { return income; }
    float GetArmyMaintenance() { return armyMaintenance; }

    Color GetColor() { return color; }
    int GetTotalDevelopment() { return totalDevelopment; }
    int GetPopulation() { return population; }
    int GetManpower() { return manpower; }

    int GetPopulationGrowth() { return populationGrowth; }
    int GetManpowerRecovery() { return manpowerRecovery; }

    void OwnByHuman(bool h) { ownedByBoot = !h; }
    bool IsOwnedByBoot() { return ownedByBoot; }
    bool IsActive() const { return active; }


    void AddPoI(int p);
    void ErasePoI(int p);
    bool IsPoI(int p) const;
    std::vector<int> GetPoI() const { return poi; }
    int GetPoISize() const { return poi.size(); }

    void StartImprRel(int cId);
    void StopImprRel(int cOd);
    bool IsImprRel(int cId) const;
    void ImprRel(int cId, int r = 2);
    int GetRel(int cId) const;
    const std::vector<std::pair<int,int>> & GetRelations() const { return relations; }
    const std::vector<int> & GetImprRel() const { return imprRel; }

    void AddTruce(Truce & t);
    bool IsTruce(int cId) const;
    void DeleteExpiredTruces(const Date & d);

    void AddUnit(std::shared_ptr<Unit> & u);
    void EraseUnit(std::shared_ptr<Unit> & u);
    std::vector<std::shared_ptr<Unit>> GetUnits() { return units; }
};
