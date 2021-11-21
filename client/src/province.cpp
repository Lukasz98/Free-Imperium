#include "province.h"
#include "gui/num_to_string.h"

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
    //pd.id, pd.r, pd.g, pd.b, pd.x, pd.y, pd.name, pd.ctrId
    this->id = pd.id;

    name = pd.name;//data->values["name:"];
    development = 1;//std::stoi(data->values["development:"]);
    country = std::string{"ctr "} + std::to_string(pd.ctrId);;//data->values["country:"];
    population = 100000;//std::stoi(data->values["population:"]);
    countryId = pd.ctrId;//std::stoi(data->values["countryId:"]);
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
  //  notify(GetValues());
    UpdateSiegeGuiWin();
}

void Province::ResetSieging()
{
    sieged = 0;
    siegeCountryId = -1;
    siegeCountry = "";
    siegeSoldiers = 0;
    //notify(GetValues());
}
    
std::unordered_map<std::string, std::string> Province::GetValues()
{
    std::unordered_map<std::string, std::string> values;
    
    values["provName"] = name;
    values["countryName"] = country;
    //values["population"] = itos(population);
    //values["manpower"] = itos(manpower);
    //values["monthIncome"] = ftos(monthIncome);
    values["sieged"] = std::to_string(sieged);
   // values["siegeSoldiers"] = itos(siegeSoldiers);
    values["siegeCountry"] = siegeCountry;

    // if delete this if, segmentation fault occurs while siege prov win is open and army retakes its prov
    if (siegeCountry == "")
        values["siegeCountry"] = "--";

    return values;
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

void Province::UpdateGuiWin()
{
    std::vector<GuiStruct> vals;
    vals.push_back({1, 10, name});
    vals.push_back({1, 11, itos(manpower)});
    vals.push_back({1, 12, country, countryId});

    subject.Notify(vals);
}

void Province::UpdateSiegeGuiWin()
{
    std::vector<GuiStruct> vals;
    vals.push_back({1, 10, name});
    vals.push_back({1, 11, country, countryId});
    vals.push_back({1, 12, siegeCountry, siegeCountryId});
    vals.push_back({2, 12, itos(siegeSoldiers)});
    vals.push_back({3, 12, itos(sieged)});
    
    siegeSubject.Notify(vals);
}


