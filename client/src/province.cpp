#include "province.h"
#include "num_to_string.h"

Province::Province(int id, DataObj * data)
{
    this->id = id;

    name = data->values["name:"];
    development = std::stoi(data->values["development:"]);
    country = data->values["country:"];
    population = std::stoi(data->values["population:"]);
    countryId = std::stoi(data->values["countryId:"]);

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
    }
}
Province::Province(const ProvData& pd)
{
    this->id = pd.id;

    name = pd.name;
    development = 1;
    country = std::string{"ctr "} + std::to_string(pd.ctrId);
    population = 100000;
    countryId = pd.ctrId;
    color.r = pd.r;
    color.g = pd.g;
    color.b = pd.b;
    unitPosition.x = pd.x;
    unitPosition.y = pd.y;
}

Province::~Province()
{
    
}

void Province::AddNeighbour(int n_id)
{
    for (auto & n : neighbours) {
        if (n == n_id) { return; }
    }
    neighbours.push_back(n_id);
}

void Province::SetOwner(std::string owner, int ctrId)
{
    sieged = 0;
    siegeSoldiers = 0;
    siegeCountryId = -1;
    siegeCountry = "";
    country = owner;
    countryId = ctrId;
//    notify(GetValues());
}

void Province::Sieging(std::string ctr, int ctrId, int sieg, int sold)
{
    sieged = sieg;
    siegeCountryId = ctrId;
    siegeCountry = ctr;
    siegeSoldiers = sold;
    siegeUpdated = true;
}

void Province::ResetSieging()
{
    sieged = 0;
    siegeCountryId = -1;
    siegeCountry = "";
    siegeSoldiers = 0;
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

