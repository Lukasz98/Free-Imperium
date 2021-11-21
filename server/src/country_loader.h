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
    /*  static std::vector<std::shared_ptr<Country>> Load()
      {
          std::vector<std::shared_ptr<Country>> countries;
          //bool log = 0;
          namespace fs = std::experimental::filesystem;
          std::string path = "../shared/countries/";
          for(auto & p : fs::directory_iterator(path))
          {
              std::string s = p.path().string();
              std::size_t txt = s.find(".txt");
              if (txt != std::string::npos)
              {
                  std::size_t sl = s.find_last_of("/") + 1;
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
  */
    static std::vector<std::shared_ptr<Country>> Load()
    {
        std::vector<std::shared_ptr<Country>> countries;
        std::vector<CountryData> ctrD = LoadCountriesData();
        for (auto& cd : ctrD) countries.emplace_back(std::make_shared<Country>(cd.id, cd.r, cd.g, cd.b, std::string{"ctr "} + std::to_string(cd.id)));
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
    /*
    static std::vector<std::shared_ptr<Country>> Load()
    {
        std::vector<std::shared_ptr<Country>> countries;
        unsigned int lineCount = 0;
        lineCount = 0;
        std::string fname{"../shared/CountryDataTest.txt"};
        std::fstream f;
        f.open(fname, std::fstream::in);
        std::string line;

        struct TmpData { int id, r, g, b; };
        while (getline(f, line)) {
            ++lineCount;
            char* ptr = strtok(line.data(), " ");
            if (strcmp(ptr, "{") != 0) {
                Log("ERROR -> fname: " << fname << ", line: " << lineCount);
                break;
            }

            TmpData ctr;
            while (getline(f, line)) {
                ++lineCount;
                ptr = strtok(line.data(), " ");
                if (strcmp(ptr, "}") == 0) {
                    countries.emplace_back(std::make_shared<Country>(ctr.id, ctr.r, ctr.g, ctr.b));

                    //ctrsData.push_back(ctr);
                    break;
                }
                else if (strcmp(ptr, "id:") == 0) {
                    ctr.id = loadInt(ptr);
                }
                else if (strcmp(ptr, "color:") == 0) {
                    ctr.r = loadInt(ptr);
                    ctr.g = loadInt(ptr);
                    ctr.b = loadInt(ptr);
                }
                else {
                    Log("cos tu nie gra: file: " << fname << ", line: " << lineCount);
                    break;
                }
            }
        }
        f.close();
        return countries;
    }
*/
    /*
    void loadCountriesData(std::vector<CountryData>& ctrsData)
    {
        unsigned int lineCount = 0;
        lineCount = 0;
        std::string fname{"CountryDataTest.txt"};
        std::fstream f;
        f.open(fname, std::fstream::in);
        std::string line;
        while (getline(f, line)) {
            ++lineCount;
            char* ptr = strtok(line.data(), " ");
            if (strcmp(ptr, "{") != 0) {
                Log("ERROR -> fname: " << fname << ", line: " << lineCount);
                break;
            }

            CountryData ctr;
            while (getline(f, line)) {
                ++lineCount;
                ptr = strtok(line.data(), " ");
                if (strcmp(ptr, "}") == 0) {
                    ctrsData.push_back(ctr);
                    break;
                }
                else if (strcmp(ptr, "id:") == 0) {
                    ctr.id = loadInt(ptr);
                }
                else if (strcmp(ptr, "color:") == 0) {
                    ctr.r = loadInt(ptr);
                    ctr.g = loadInt(ptr);
                    ctr.b = loadInt(ptr);
                }
                else {
                    Log("cos tu nie gra: file: " << fname << ", line: " << lineCount);
                    break;
                }
            }
        }
        f.close();
    }
    */
};
