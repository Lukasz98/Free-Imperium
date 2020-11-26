#include "country.h"

Country::Country(DataObj * obj)
    : ownedByBoot(true), active(true)
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

MonthCash Country::MonthUpdate()
{
    for (auto & ir : imprRel) {
        ImprRel(ir);
    }
  
    MonthCash mc;
    for (auto & p : provinces) {

        if (p->GetSiegeLevel() < 100) {
            float tax = 0.3f;
            mc.income += p->MonthUpdate(tax);
            mc.development += p->GetDevelopment();
            mc.population += p->GetPopulation();
            mc.manpower += p->GetManpower();
            mc.populationGrowth += p->GetPopulationGrowth();
            mc.manpowerRecovery += p->GetManpowerRecovery();
        }
    }
    return mc;
}

void Country::AddProvince(std::shared_ptr<Province> & p)
{
    provinces.push_back(p);
}

void Country::RemoveProvince(std::shared_ptr<Province> & p)
{
    for (auto it = provinces.begin(); it < provinces.end(); it++) {
        if ((*it)->GetId() == p->GetId()) {
            provinces.erase(it);
            break;
        }
    }
}

void Country::AddWar(War * w)
{
    wars.push_back(w);
}

void Country::RemoveWar(War * w)
{
//    Log("RemovingWar " << name << " wid="<<w->GetId());
    LogFunkStart;
    auto tmp = w->GetAttackers();
    if (w->IsAttacker(id))
        tmp = w->GetDefenders();

    for (auto it = wars.begin(); it < wars.end(); it++) {
        if ((*it)->GetId() == w->GetId()) {
            for (auto & p : provinces) {
                for (auto s : tmp) {
                   // Log("tu " << s);
                    p->EraseSiegeUnitsCtr(s);
                }
            } 
            wars.erase(it);
            break;
        }
    }
    LogFunkEnd;
}

bool Country::IsWarIn(const War * w)
{
    auto wIt = std::find_if(wars.begin(), wars.end(), [id = w->GetId()](War * war) {
        return war->GetId() == id;
    });
    return wIt != wars.end();
}

void Country::UpdateSiegesForWar()
{
    LogFunkStart;
    for (auto & prov : provinces) {
        if (!prov->IsStateChanged())
            continue;
        prov->ResetStateChange();
        int sc = prov->GetSiegeCountryId();
        if (sc == -1) {
            for (auto w : wars) {
                w->DeleteProv(prov->GetId());
                Log("deleting prov\n\n")
            }
        }
        else {
            for (auto w : wars) {
                if (w->IsAttacker(sc) || w->IsDefender(sc)) {
                    w->AddProv(prov->GetId(), id);
                    Log("adding prov\n\n");
                }
            }
        }
    }
LogFunkEnd;
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

    bool accepted = false;
    if (offer.lostProv.size()) {
        int lostVal = 0;
        for (auto & lost : offer.lostProv)
            lostVal += std::get<2>(lost) * 10;
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

void Country::StartImprRel(int cId)
{
    auto it = std::find(imprRel.begin(), imprRel.end(), cId);
    if (it == imprRel.end())
        imprRel.emplace_back(cId);
}

void Country::StopImprRel(int cId)
{
    auto it = std::find(imprRel.begin(), imprRel.end(), cId);
    if (it != imprRel.end())
        imprRel.erase(it);
}

bool Country::IsImprRel(int cId) const
{
    auto it = std::find(imprRel.begin(), imprRel.end(), cId);
    return (it != imprRel.end());
}

void Country::ImprRel(int cId, int r)
{
    auto it = std::find_if(relations.begin(), relations.end(), [cId](std::pair<int,int> & p){ return cId == p.first; });
    if (it != relations.end()) {
        (*it).second += r;
        if ((*it).second > 50)
            (*it).second = 50;
    }
    else {
        relations.emplace_back(std::make_pair(cId, 0));
        relations.back().second += r;
    }
}

int Country::GetRel(int cId) const
{
    auto it = std::find_if(relations.begin(), relations.end(), [cId](const std::pair<int,int> & p){ return cId == p.first; });
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

bool Country::IsTruce(int cId) const
{
    auto tIt = std::find_if(truces.begin(), truces.end(), [cId](const Truce & t) { return t.enemy == cId; });
    return tIt != truces.end();
}


void Country::DeleteExpiredTruces(const Date & d)
{
    for (auto tIt = truces.end() - 1; truces.size() && tIt >= truces.begin(); tIt--) {
        if (d >= (Date)(*tIt))
            truces.erase(tIt);
    }
}

void Country::AddUnit(std::shared_ptr<Unit> & u)
{
    units.emplace_back(u);
}

void Country::EraseUnit(std::shared_ptr<Unit> & u)
{
    auto it = std::find_if(units.begin(), units.end(), [id = u->GetId()](std::shared_ptr<Unit> & uu) {
        return (id == uu->GetId());
    });
    if (it != units.end())
        units.erase(it);
}
