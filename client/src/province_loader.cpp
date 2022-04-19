#include "province_loader.h"

#include "../shared/load_countries_data.h"
#include "load_values_from_txt.h"

std::vector<Province> ProvinceLoader::Load(std::map<unsigned int, int>& colorToId)
{
    std::vector<Province> provinces;
    std::vector<ProvData> provData = LoadProvincesData(colorToId);
    for (auto& pd : provData) provinces.emplace_back(Province{pd});
    std::sort(provinces.begin(), provinces.end(),
              [](const Province& p1, const Province& p2) { return p1.id < p2.id; });
    return provinces;
}
