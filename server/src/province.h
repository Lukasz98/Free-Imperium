#pragma once
#include <iostream>
#include <vector>
#include <sstream>
#include <algorithm>

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

#include "color.h"
#include "log.h"
#include "data_object.h"
#include "unit.h"
#include "siege.h"
//#include "war.h"

class Province
{
    int id = -1, countryId = -1;
    Color color;
    std::vector<int> neighbours;
    std::string name;
    int development = 1;
    glm::vec3 unitPosition;
    std::string country = "Poland";
   
    Siege siege;

    bool firstUpdate = true;
    int population = 0;
    int manpower = 0;
    float treasury = 0.0f;
    float monthIncome = 0.0f;
    float totalMonthIncome = 0.0f;

    int populationGrowth = 0.0f;
    int manpowerRecovery = 0.0f;

    std::vector<std::shared_ptr<Unit>> units;

public:
    Province() = delete;
    Province(int id, DataObj * data);
    Province(int id, int r, int g, int b, int x, int y, const std::string& name, int ctrId);
    ~Province();


    float MonthUpdate(float tax);
    
    void SetName(std::string n) { name = n; }
    void SetCountry(std::string c) { country = c; }
    void SetColor(Color c) { color = c; }
    void SetDevelopment(int d) { development = d; }
    void SetPopulation(float p) { population = p; }
    void EraseManpower(int p) { if (manpower >= p) manpower -= p; }
    void SetUnitPos(glm::vec2 p) { unitPosition = glm::vec3(p, 0.1); }
    
    inline int GetCountryId() const { return countryId; }
    std::string GetName() const { return name; }
    std::string GetCountry() const { return country; }
    Color GetColor() const { return color; }
    int GetId() const { return id; }
    std::vector<int> GetNeighbours() const { return neighbours; }
    glm::vec3 GetUnitPos() const { return unitPosition; }

    int GetPopulation() const { return population; }
    int GetManpower() const { return manpower; }
    int GetDevelopment() const { return development; }
    float GetTreasury() const { return treasury; }
    float GetMonthIncome() const { return monthIncome; }
    float GetTotalMonthIncome() const { return totalMonthIncome; }

    int GetPopulationGrowth() const { return populationGrowth; }
    int GetManpowerRecovery() const { return manpowerRecovery; }
    
    void KillPopulation(int amount)
    {
        population -= amount;
        if (population < 0)
            population = 0;
    }
    
    void SetOwner(std::string owner, int ownId)
    {
        siege = Siege();
        country = owner;
        countryId = ownId;
    }
    
    void NewDay();
    void Sieging(std::shared_ptr<Unit> & u);
    void UpdateSiege();
    bool IsStateChanged() { return siege.IsStateChanged(); }
    void ResetStateChange() { return siege.ResetStateChande(); }
 
    int GetSiegeLevel() const { return siege.GetLevel(); }
    int GetSiegeCountryId() const { return siege.GetLeader(); };
    int GetSiegeSoldiersCount() const { return siege.GetSiegeSoldiersCount(); }
    
    void AddNeighbour(int  n_id);
    void Print();

    bool operator==(Color c)
    {
        return color == c;
    }

    void AddUnit(std::shared_ptr<Unit> & u);
    void EraseUnit(std::shared_ptr<Unit> & u);
    void EraseSiegeUnitsCtr(int ctrId);
    std::vector<std::shared_ptr<Unit>> GetUnits() { return units; }
};
