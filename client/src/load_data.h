#pragma once

#include <algorithm>
#include <map>
#include <vector>

#include "ctr_data.h"
#include "load_values_from_txt.h"
#include "prov_data.h"

unsigned int getHash(unsigned char r, unsigned char g, unsigned char b);

void loadProvData(std::vector<ProvData>& provinces, std::map<unsigned int, int>& colorToId);

void loadCountriesData(std::vector<CountryData>& ctrsData);

void loadProvPointsData(std::vector<std::pair<int, int>>* points, int size);
