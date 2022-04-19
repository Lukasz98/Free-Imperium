#pragma once
#include <experimental/filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

#include "data_object_loader.h"
#include "province.h"

class ProvinceLoader {
   public:
    ProvinceLoader() = delete;

    static std::vector<Province> Load(std::map<unsigned int, int>& colorToId);
};
