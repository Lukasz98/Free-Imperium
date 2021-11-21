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

    /*
    bool log = 0;
    namespace fs = std::experimental::filesystem;
    std::string path = "../shared/countries/";
    for(auto & p : fs::directory_iterator(path)) {
        std::string s = p.path().string();
        std::size_t txt = s.find(".txt");
        if (txt != std::string::npos) {
            std::size_t sl = s.find_last_of("/") + 1;
            std::size_t dot = s.find(".");
            std::string name = s.substr(sl, txt - sl);

            std::cout << "country name: " << name << std::endl;
            if (name.find("/") != std::string::npos)
                continue;

            std::fstream file(path + name + ".txt", std::fstream::in);
            if (file.is_open()) {
                DataLoader loader;
                DataObj * data = loader.LoadDataObj(file, "country");
                countries.emplace_back(std::make_shared<Country>(data));
                delete data;

                file.close();
            }
        }
    }
    */
    return countries;
}
