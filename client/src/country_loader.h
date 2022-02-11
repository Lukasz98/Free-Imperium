#pragma once
#include <experimental/filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <vector>

#include "country.h"
#include "data_object_loader.h"

class CountryLoader {
   public:
    static std::vector<Country> Load();
};
