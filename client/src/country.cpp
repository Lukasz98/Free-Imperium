#include "country.h"

Country::Country(const CountryData& cd, const std::string& _name) : name(_name)
{
    id = cd.id;

    color.r = cd.r;
    color.g = cd.g;
    color.b = cd.b;
}
