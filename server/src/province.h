#pragma once
#include <iostream>
#include <vector>
#include <sstream>

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

#include "color.h"
#include "log.h"
#include "data_object.h"

class Province
{
    int id;
    Color color;
    std::vector<int> neighbours;
    std::string name;
    int development = 1;
    glm::vec3 unitPosition;
    std::string country = "Poland";

    int sieged = 0;
    int siegeSoldiers = 0;
    std::string siegeCountry; // who is sieging
    std::string siegedBy; // who sieged
    bool siege = false, unsiege = false;
    
    bool firstUpdate = true;
    int population = 0;
    int manpower = 0;
    float autonomy = 0.0f;
    float unrest = 0.0f;
    float prosperity = 50.0f;
    float administration = 50.0f;
    float treasury = 0.0f;
    float monthIncome = 0.0f;
    float totalMonthIncome = 0.0f;

    int populationGrowth = 0.0f;
    int manpowerRecovery = 0.0f;
    
public:
    Province() = delete;
    Province(int id, DataObj * data);
    ~Province();

    float MonthUpdate(float tax);
    
    void SetName(std::string n) { name = n; }
    void SetCountry(std::string c) { country = c; }
    void SetColor(Color c) { color = c; }
    void SetDevelopment(int d) { development = d; }
    void SetPopulation(float p) { population = p; }
    void EraseManpower(int p) { if (manpower >= p) manpower -= p; }
    void SetUnitPos(glm::vec2 p) { unitPosition = glm::vec3(p, 0.1); }
    
    std::string GetName() const { return name; }
    std::string GetCountry() const { return country; }
    Color GetColor() const { return color; }
    int GetId() const { return id; }
    std::vector<int> GetNeighbours() const { return neighbours; }
    glm::vec3 GetUnitPos() const { return unitPosition; }

    int GetPopulation() const { return population; }
    int GetManpower() const { return manpower; }
    int GetDevelopment() const { return development; }
    float GetAutonomy() const { return autonomy; }
    float GetUnrest() const { return unrest; }
    float GetProsperity() const { return prosperity; }
    float GetAdministration() const { return administration; }
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
    
    void SetOwner(std::string owner)
    {
        sieged = 0;
        siegeSoldiers = 0;
        siegeCountry = "";
        country = owner;
    }
    
    void NewDay();
    
    bool Siege(int s, const std::string & siegeCountry, int soldiers);

    bool Unsiege(int s, int soldiers, const std::string & c);
    
    void SiegeBreak();
    
    void SiegeBy(std::string siegeCountry, int soldiers);
    
    int GetSieged() const { return sieged; }
    std::string GetSiegeCountry() const;
    std::string GetSiegedBy() const;
    void ResetSiege();
    int GetSiegeSoldiers() const { return siegeSoldiers; }
    
    void AddNeighbour(int  n_id);
    void Print();

    bool operator==(Color c)
    {
        return color == c;
    }
};
