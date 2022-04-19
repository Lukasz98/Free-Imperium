#pragma once
#include <algorithm>
#include <sstream>

#include "color.h"
#include "data_object.h"
#include "log.h"
#include "month_cash.h"
#include "ctr_data.h"

class Country {
   public:
    int id;
    Color color;
    std::string name;
    float gold = 0.0f, income = 0.0f;
    float armyMaintenance = 0.0f;
    int population = 0;
    int manpower = 0;

   public:
    Country(const CountryData& cd, const std::string& name);
};

