#include "country_loader.h"

std::vector<std::shared_ptr<Country>> CountryLoader::Load()
{
    std::vector<std::shared_ptr<Country>> countries;
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
    return countries;
}
