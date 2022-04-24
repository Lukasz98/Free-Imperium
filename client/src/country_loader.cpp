#include "country_loader.h"

#include "../shared/load_countries_data.h"

std::vector<Country> CountryLoader::Load()
{
    std::vector<Country> countries;
    std::vector<CountryData> ctrData = LoadCountriesData();

    for (auto& cd : ctrData) countries.emplace_back(Country{cd, std::string{"ctr "} + std::to_string(cd.id)});

    CountryData cd = {.id = 0, .r = 0, .g = 0, .b = 0};
    if (countries.size())
        cd.id = countries.back().id + 1;

    countries.emplace_back(Country{cd, "Spectator"});

    std::sort(countries.begin(), countries.end(),
              [](const Country& c1, const Country& c2) { return c1.id < c2.id; });

    return countries;
}
