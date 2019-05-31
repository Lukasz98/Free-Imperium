#include "country.h"

Country::Country(DataObj * obj)
    : ownedByBoot(true), active(true)
{
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

Country::~Country()
{

}

void Country::Cash(MonthCash mc)
{
    income = mc.income;
    armyMaintenance = mc.armyMaintenance;
    gold += mc.income;
    gold -= mc.armyMaintenance;
    population = mc.population;
    manpower = mc.manpower;
    totalDevelopment = mc.development;
    if (totalDevelopment == 0)
        active = false;
    populationGrowth = mc.populationGrowth;
    manpowerRecovery = mc.manpowerRecovery;
}

bool Country::Apply(PeaceOffer offer, const Date & date)
{
    float myPower = this->GetPower();
    //Log(name << " MyPower: " << myPower);

    bool accepted = false;
    if (offer.lostProv.size()) {
        int lostVal = 0;
        for (auto & lost : offer.lostProv)
            lostVal += std::get<2>(lost) * 10;
        //Log("LostVal: " << lostVal << ", warScore=" << offer.warScore);
        if (lostVal > 100)
            return false;
        
        if (offer.recipantIsDefender && offer.warScore > 10) {
            if (offer.warScore >= lostVal)
                accepted = true;
        }
        else if (!offer.recipantIsDefender && offer.warScore < -10) {
            if (offer.warScore / -2 >= lostVal)
                accepted = true;
        }
    }
    else {
        int gainVal = 0;
        for (auto & gain : offer.gainProv)
            gainVal += std::get<2>(gain) * 10;
        //Log("gainVal: " << gainVal);
        if (offer.recipantIsDefender && offer.warScore < -10) {
            if (myPower <= gainVal || gainVal >= 100)
                accepted = true;
        }
        else if (!offer.recipantIsDefender && offer.warScore > 10) {
            if (myPower <= gainVal || gainVal >= 100)
                accepted = true;
        }
    }

    if (accepted) {
        truces.emplace_back(date);
        truces.back().year++;
        truces.back().enemy = offer.offeredBy;
    }
        
    return accepted;
}

void Country::AddPoI(int p)
{
    auto it = std::find(poi.begin(), poi.end(), p);
    if (it == poi.end())
        poi.push_back(p);
}

void Country::ErasePoI(int p)
{
    auto it = std::find(poi.begin(), poi.end(), p);
    if (it != poi.end())
        poi.erase(it);
}

bool Country::IsPoI(int p) const
{
    auto it = std::find(poi.begin(), poi.end(), p);
    return !(it == poi.end());
}

void Country::StartImprRel(const std::string & c)
{
    auto it = std::find(imprRel.begin(), imprRel.end(), c);
    if (it == imprRel.end())
        imprRel.emplace_back(c);
}

void Country::StopImprRel(const std::string & c)
{
    auto it = std::find(imprRel.begin(), imprRel.end(), c);
    if (it != imprRel.end())
        imprRel.erase(it);
}

bool Country::IsImprRel(const std::string & c) const
{
    auto it = std::find(imprRel.begin(), imprRel.end(), c);
    return (it != imprRel.end());
}

void Country::ImprRel(const std::string & c, int r)
{
    auto it = std::find_if(relations.begin(), relations.end(), [c](std::pair<std::string,int> & p){ return c == p.first; });
    if (it != relations.end()) {
        (*it).second += r;
        if ((*it).second > 50)
            (*it).second = 50;
    }
    else {
        relations.emplace_back(std::make_pair(c, 0));
        relations.back().second += r;
    }
}

int Country::GetRel(const std::string & c) const
{
    auto it = std::find_if(relations.begin(), relations.end(), [c](const std::pair<std::string,int> & p){ return c == p.first; });
    if (it != relations.end())
        return (*it).second;
    return 0;
}


void Country::AddTruce(Truce & t)
{
    auto tIt = std::find(truces.begin(), truces.end(), t);
    if (tIt == truces.end()) {
        truces.emplace_back(t);
    }
    else {
        if (t >= *tIt) {
            truces.erase(tIt);
            truces.emplace_back(t);
        }
    }
}

bool Country::IsTruce(const std::string & c, const Date & d) const
{
    auto tIt = std::find_if(truces.begin(), truces.end(), [c](const Truce & t) { return t.enemy == c; });
    return tIt != truces.end();
}


void Country::DeleteExpiredTruces(const Date & d)
{
    for (auto tIt = truces.end() - 1; truces.size() && tIt >= truces.begin(); tIt--) {
        if (d >= (Date)(*tIt))
            truces.erase(tIt);
    }
}
