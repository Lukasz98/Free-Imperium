#pragma once
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <experimental/filesystem>

#include "province.h"
#include "data_object_loader.h"

class ProvinceLoader
{
    static void loadFromFile(std::vector<Province> & provinces);
    static void loadFromMap(std::vector<Province> & provinces, const unsigned char * pixels, int width, int height);
    static void loadNeighbours(std::vector<Province> & provinces, Color lastColor, Color currentColor, int index);    

    static int getProvinceId(const std::vector<Province> & provinces, Color color);

public:
    ProvinceLoader() = delete;

    static std::vector<Province> Load(const unsigned char * pixels, int width, int height);
};
