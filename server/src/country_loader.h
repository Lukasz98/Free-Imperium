#pragma once
#include <string.h>

#include <experimental/filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <vector>

#include "../shared/load_countries_data.h"
#include "country.h"
#include "data_object_loader.h"

class CountryLoader {
   public:
    static std::vector<std::shared_ptr<Country>> Load()
    {
        std::vector<std::shared_ptr<Country>> countries;
        std::vector<CountryData> ctrD = LoadCountriesData();
        for (auto& cd : ctrD)
            countries.emplace_back(
                std::make_shared<Country>(cd.id, cd.r, cd.g, cd.b, std::string{"ctr "} + std::to_string(cd.id)));
        std::sort(countries.begin(), countries.end(),
                  [](const std::shared_ptr<Country>& c1, const std::shared_ptr<Country>& c2) {
                      return c1->GetId() < c2->GetId();
                  });

        {
            int id = 0;
            if (countries.size())
                id = countries.back()->GetId() + 1;
            countries.emplace_back(std::make_shared<Country>(id, 0, 0, 0, "Atlantyda"));
        }

        Log("ctrs size=" << countries.size());
        return countries;
    }
};
