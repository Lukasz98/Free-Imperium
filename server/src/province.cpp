#include "province.h"

Province::Province(int id, DataObj * data)
    : unitPosition{0.0, 0.0, 0.1}
{
    this->id = id;
    name = data->values["name:"];
    development = std::stoi(data->values["development:"]);
    country = data->values["country:"];
    population = std::stoi(data->values["population:"]);
    {
        std::string val;
        std::stringstream ss;
        val = data->values["rgb:"];
        ss << val;
        ss >> val;
        color.r = std::stoi(val);
        ss >> val;
        color.g = std::stoi(val);
        ss >> val;
        color.b = std::stoi(val);
    }
    {
        std::string val;
        std::stringstream ss;
        val = data->values["unitPosition:"];
        ss << val;
        ss >> val;
        unitPosition.x = std::stoi(val); 
        ss >> val;
        unitPosition.y = std::stoi(val); 
        unitPosition.z = 0.1;
    }
}

Province::~Province()
{
    
}

float Province::MonthUpdate(float tax)
{
    if (firstUpdate) {
        manpower = population / 100.0f;
        firstUpdate = false;
    }

    //populationGrowth = (float)population * 0.01f / 12.0f * (prosperity / 100.0f) + 1.0f;
    //float monthPopulationGrowth = (0.01f / 12.0f) * (prosperity / 100.0f) + 1.0f;
    //manpowerRecovery = (float)populationGrowth * 0.01f + 1.0f;
    //manpower += manpowerRecovery;
    //manpower += (population * monthPopulationGrowth) / 100.0f * (administration / 100.0f) * ((100.0f - autonomy) / 100.0f);
    //population *= monthPopulationGrowth;
    //population += populationGrowth;
    
    //unrest = prosperity / 100.0f * ((100.0f - autonomy) / 100.0f);
    
    //totalMonthIncome = (float)development * (administration / 100.0f) * ((100.0f - autonomy) / 100.0f);
    //monthIncome = totalMonthIncome;

    manpowerRecovery = development * 50;
    if (manpower < population / 100.0f)
        manpower += manpowerRecovery;
    if (manpower > population / 100.0f)
        manpower = population / 100.0f;
    
    totalMonthIncome = development;
    monthIncome = totalMonthIncome;
    
    float returnTax = 0.0f;
    if (tax >= 0.0f && tax <= 1.0f) {
        returnTax = totalMonthIncome * tax;
        monthIncome = totalMonthIncome - returnTax;
    }
    treasury += monthIncome;
    
    return returnTax;
}

void Province::NewDay()
{
    if (siege == true && siegeCountry == "" && sieged < 100) {
        siege = false;
        sieged = 0;
        siegedBy = "";
    }
    
    if (siege == true && siegeCountry == "" && sieged == 100) {
        siege = false;
    }
    
    if (unsiege == true && siegeCountry == "" && sieged > 0) {
        unsiege = false;
        sieged = 100;
    }
}

bool Province::Siege(int s, const std::string & siegeCountry, int soldiers)
{
    if (unsiege == true)
        return false;
    siege = true;
    siegeSoldiers += soldiers;
    sieged += s;
        
    if (this->siegeCountry == "")
        this->siegeCountry = siegeCountry;

    if (sieged >= 100) {
        siegedBy = siegeCountry;
        this->siegeCountry.clear();
        sieged = 100;
        siege = false;
        return true;
    }
    else
        return false;
}

bool Province::Unsiege(int s, int soldiers, const std::string & c)
{
    if (siege == true)
        return false;
    unsiege = true;
        
    siegeCountry = c;
    siegeSoldiers += soldiers;
    sieged -= s;
    if (sieged <= 0) {
        siegeCountry = "";
        siegedBy = "";
        sieged = 0;
        unsiege = false;
        return true;
    }
    return false;
}

void Province::SiegeBreak()
{
    siegeCountry = "";
    siegeSoldiers = 0;
}

void Province::SiegeBy(std::string siegeCountry, int soldiers)
{
}

void Province::AddNeighbour(int n_id)
{
    for (auto & n : neighbours)
    {
        if (n == n_id) { return; }
    }
    neighbours.push_back(n_id);
}

std::string Province::GetSiegeCountry() const
{
    if (siegeCountry == "") {
        if (siegedBy == "") {
            return country;
        }
        else
            return siegedBy;
    }
    else
        return siegeCountry;
}

std::string Province::GetSiegedBy() const
{
    if (siegedBy == "")
        return country;
    else
        return siegedBy;
}

void Province::ResetSiege()
{
    sieged = 0;
    siegeSoldiers = 0;
    siegeCountry = "";
}

void Province::Print()
{
    std::cout << name << ", id: " << id << "\n";
    std::cout << "r=" << (int)color.r << ", g=" << (int)color.g << ", b=" << (int)color.b << ", a=" << (int)color.a << "\n";
    std::cout << "Neighbours\n";
    for (auto & n : neighbours)
        std::cout << "- " << "" << n << "\n";
    std::cout << "--------------------\n\n";
}
