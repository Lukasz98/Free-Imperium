#include "country_loader.h"

#include "../shared/load_countries_data.h"

std::vector<std::shared_ptr<Country>> CountryLoader::Load()
{
    std::vector<std::shared_ptr<Country>> countries;
    std::vector<CountryData> ctrData = LoadCountriesData();

    for (auto& cd : ctrData)
        countries.emplace_back(std::make_shared<Country>(cd, std::string{"ctr "} + std::to_string(cd.id)));
    

    {
        CountryData cd = {.id = 0, .r = 0, .g = 0, .b = 0};
        if (countries.size())
            cd.id = countries.back()->GetId() + 1;

        countries.emplace_back(std::make_shared<Country>(cd, "Atlantyda"));
    }

    return countries;
}
