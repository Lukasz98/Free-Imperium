#pragma once
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <experimental/filesystem>
#include <map>

#include "province.h"
#include "data_object_loader.h"
class ProvinceLoader
{
    static void loadFromFile(std::vector<std::unique_ptr<Province>> & provinces);
    static void loadFromMap(std::vector<std::unique_ptr<Province>> & provinces, const unsigned char * pixels, int width, int height);
    static void loadNeighbours(std::vector<std::unique_ptr<Province>> & provinces, Color lastColor, Color currentColor, int index);    

    static int getProvinceId(const std::vector<std::unique_ptr<Province>> & provinces, Color color);

public:
    ProvinceLoader() = delete;

    //static std::vector<std::unique_ptr<Province>> Load(const unsigned char * pixels, int width, int height);
    static std::vector<std::unique_ptr<Province>> Load(std::map<unsigned int, int>& colorToId);
};
