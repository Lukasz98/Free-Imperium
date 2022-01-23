#include "country.h"

Country::Country(DataObj * obj)
{
    id = std::stoi(obj->values["id:"]);
    name = obj->values["name:"];
    culture = obj->values["culture:"];
    religion = obj->values["religion:"];

    std::stringstream ss;
    ss << obj->values["rgb:"];
    std::string col;
    ss >> col;
    color.r = (unsigned char)std::stoi(col);
    ss >> col;
    color.g = (unsigned char)std::stoi(col);
    ss >> col;
    color.b = (unsigned char)std::stoi(col);
}
Country::Country(const CountryData& cd, const std::string& name)
    : name(name)
{
    id = cd.id;
    culture = "some culture";
    religion = "some religion";

    color.r = cd.r;
    color.g = cd.g;
    color.b = cd.b;
}

Country::~Country()
{

}

void Country::SetRel(const std::string & c, int r)
{
    auto it = std::find_if(relations.begin(), relations.end(), [c](std::pair<std::string,int> & p){ return c == p.first; });
    if (it != relations.end())
        (*it).second = r;
    else {
        relations.emplace_back(std::make_pair(c, r));
    }
}

int Country::GetRel(const std::string & c)
{
    auto it = std::find_if(relations.begin(), relations.end(), [c](std::pair<std::string,int> & p){ return c == p.first; });
    if (it != relations.end())
        return (*it).second;
    return 0;
}

void Country::StartImprRel(const std::string & c)
{
    auto it = std::find(impRelWith.begin(), impRelWith.end(), c);
    if (it == impRelWith.end())
        impRelWith.emplace_back(c);
}

void Country::StopImprRel(const std::string & c)
{
    auto it = std::find(impRelWith.begin(), impRelWith.end(), c);
    if (it != impRelWith.end())
        impRelWith.erase(it);
}

bool Country::IsImprRel(const std::string & c)
{
    auto it = std::find(impRelWith.begin(), impRelWith.end(), c);
    return !(it == impRelWith.end());
}

void Country::SetImprRel(const std::vector<std::string> & ir)
{
    impRelWith = ir;
}

bool Country::Apply(/*PeaceOffer offer*/)
{
    return false;
}
    
    

