#pragma once
#include <glm/vec2.hpp>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>

#include "color.h"
#include "data_object.h"
#include "log.h"
#include "prov_data.h"
class Province {
   public:
    int id;
    Color color;
    std::vector<int> neighbours;
    std::string name;
    int development = 1;
    glm::vec2 unitPosition;
    std::string country = "Poland";
    int countryId;
    int sieged = 0;
    int siegeSoldiers = 0;
    int siegeCountryId = -1;
    std::string siegeCountry;
    bool siegeUpdated = false;
    bool water = false;
    int population = 0, manpower = 0;
    float autonomy = 0.0f, unrest = 0.0f, prosperity = 0.0f, administration = 0.0f, treasury = 0.0f;
    float monthIncome = 0.0f, totalMonthIncome = 0.0f;

   public:
    Province() = delete;
    Province(const ProvData& pd);
    ~Province();

    void SetName(std::string n)
    {
        name = n;
    }
    void SetCountry(std::string c)
    {
        country = c;
    }
    void SetColor(Color c)
    {
        color = c;
    }
    void SetDevelopment(int d)
    {
        development = d;
    }
    void SetUnitPos(glm::vec2 p)
    {
        unitPosition = p;
    }

    void SetPopulation(int p)
    {
        population = p;
    }
    void SetManpower(int m)
    {
        manpower = m;
    }
    void SetTreasury(float t)
    {
        treasury = t;
    }
    void SetMonthIncome(float i)
    {
        monthIncome = i;
    }
    void SetTotalMonthIncome(float i)
    {
        totalMonthIncome = i;
    }

    inline int GetCountryId() const
    {
        return countryId;
    }
    std::string GetName() const
    {
        return name;
    }
    std::string GetCountry() const
    {
        return country;
    }
    Color GetColor() const
    {
        return color;
    }
    inline int GetId() const
    {
        return id;
    }
    std::vector<int> GetNeighbours() const
    {
        return neighbours;
    }
    glm::vec2 GetUnitPos() const
    {
        return unitPosition;
    }

    int GetPopulation() const
    {
        return population;
    }
    int GetManpower() const
    {
        return manpower;
    }
    int GetDevelopment() const
    {
        return development;
    }
    //float GetTreasury() const
    //{
    //    return treasury;
    //}
    float GetMonthIncome() const
    {
        return monthIncome;
    }
    float GetTotalMonthIncome() const
    {
        return totalMonthIncome;
    }

    void SetOwner(std::string owner, int ctrId);
    void Sieging(std::string ctr, int ctrId, int sieg, int sold);
    void ResetSieging();

    int GetSieged() const
    {
        return sieged;
    }
    inline int GetSiegeCountryId() const
    {
        return siegeCountryId;
    }
    int GetSiegeSoldiers() const
    {
        return siegeSoldiers;
    }
    bool WasSiegeUpdated()
    {
        bool tmp = siegeUpdated;
        siegeUpdated = false;
        return tmp;
    }

    void Print();

    bool operator==(Color c)
    {
        return color == c;
    }
};
