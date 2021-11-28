#include "province.h"

Province::Province(int id, DataObj * data)
    : unitPosition{0.0, 0.0, 0.1}
{
    this->id = id;
    name = data->values["name:"];
    development = std::stoi(data->values["development:"]);
    country = data->values["country:"];
    countryId = std::stoi(data->values["countryId:"]);
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

Province::Province(const ProvData& pd)
    : unitPosition{pd.x, pd.y, 0.1}
{
    this->id = pd.id;
    this->name = pd.name;
    development = 1;
    country = "ctr name";
    countryId = pd.ctrId;
    population = 100000;
    color.r = pd.r;
    color.g = pd.g;
    color.b = pd.b;
    for (auto n : pd.neighb)
        neighbours.push_back(n);
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
}

void Province::AddNeighbour(int n_id)
{
    for (auto & n : neighbours)
    {
        if (n == n_id) { return; }
    }
    neighbours.push_back(n_id);
}

void Province::Sieging(std::shared_ptr<Unit> & u)
{
    siege.AddUnit(u);
}

void Province::UpdateSiege()
{
    siege.Update();
    if (siege.IsDone() && siege.GetSiegeSoldiersCount() == 0) {
        int power = 0;
        for (auto & u : units) {
            if (u->GetCountryId() == countryId) {
                power += u->GetSiegePower();
                u->SetSieging(true);
            }
        }
        if (power > 0) {
            siege.Unsiege(power);
        }
        else {
            siege.StopUnsiege();
        }
        if (siege.GetLevel() == 0 && power > 0) {
            for (auto & u : units) {
                if (u->GetCountryId() == countryId) {
                    u->SetSieging(false);
                }
            }
        }
    } 
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

void Province::AddUnit(std::shared_ptr<Unit> & u)
{
    units.emplace_back(u);
}

void Province::EraseUnit(std::shared_ptr<Unit> & u)
{
    siege.EraseUnit(u);
    u->SetSieging(false); // important if unit was unsieging
    auto it = std::find_if(units.begin(), units.end(), [id = u->GetId()](std::shared_ptr<Unit> & uu) {
        return (id == uu->GetId());
    });
    if (it != units.end())
        units.erase(it);
}


void Province::EraseSiegeUnitsCtr(int ctr)
{
    auto leader = siege.GetLeader();
    std::vector<std::vector<std::shared_ptr<Unit>>::iterator>  a;
    for (auto it = units.begin(); it < units.end(); it++) {
        if ((*it)->GetCountryId() == ctr) {
            siege.EraseUnit((*it));
            a.push_back(it);
        }
    }
    if (leader == ctr) {
        siege = Siege();
    }
}
