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
    static std::vector<std::shared_ptr<Country>> Load();
    
};
