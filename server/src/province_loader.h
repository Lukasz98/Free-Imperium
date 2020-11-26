#pragma once
#include <iostream>
#include <memory>
#include <vector>
#include <fstream>
#include <sstream>
#include <experimental/filesystem>

#include "province.h"
#include "data_object_loader.h"

class ProvinceLoader
{
    using vec = std::vector<std::shared_ptr<Province>>;
    static void loadFromFile(vec & provinces);
    static void loadFromMap(vec & provinces, const unsigned char * pixels, int width, int height);
    static void loadNeighbours(vec & provinces, Color lastColor, Color currentColor);    

    static int getProvinceId(const vec & provinces, Color color);

public:
    ProvinceLoader() = delete;

    static vec Load(const unsigned char * pixels, int width, int height);
};
