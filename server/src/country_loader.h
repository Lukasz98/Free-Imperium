#pragma once
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <experimental/filesystem>

#include <memory>

#include "country.h"
#include "data_object_loader.h"

class CountryLoader
{
public:
    static std::vector<std::shared_ptr<Country>> Load()
    {
        std::vector<std::shared_ptr<Country>> countries;
        bool log = 0;
        namespace fs = std::experimental::filesystem;
        std::string path = "../shared/countries/";
        for(auto & p : fs::directory_iterator(path))
        {
            std::string s = p.path().string();
            std::size_t txt = s.find(".txt");
            if (txt != std::string::npos)
            {
                std::size_t sl = s.find_last_of("/") + 1;
                std::size_t dot = s.find(".");
                std::string name = s.substr(sl, txt - sl);

                std::cout << "country name: " << name << std::endl;
                if (name.find("/") != std::string::npos)
                    continue;

                std::fstream file(path + name + ".txt", std::fstream::in);
                if (file.is_open())
                {
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
};
/*
                    Country country;//();
                    Color color;
                    std::string culture, religion;
                    
                    std::string line;
                    while (std::getline(file, line))
                    {
                        std::string variable;
                        if (log)
                            std::cout << "loaded line: " << line << "\n";
        
                        std::size_t hash = s.find("#");
                        if (hash != std::string::npos)
                        {
                            line = s.substr(0, hash);
                        }

                        std::istringstream stream(line);
                        while (!stream.eof())
                        {
                            std::string word;
                            stream >> word;
                            if (log)
                                std::cout << "variable: " << variable << ", word: " << word << "\n";
                            if (variable == "rgb")
                                color.Push(std::stoi(word));
                            else if (variable == "culture")
                                culture = word;
                            else if (variable == "religion")
                                religion = word;
                            else
                                variable = word;
                        }
                    }
                    country.SetName(name);
                    country.SetColor(color);
                    country.SetCulture(culture);
                    country.SetReligion(religion);

*/
