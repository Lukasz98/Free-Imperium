#pragma once
#include <vector>
#include <tuple>

struct PeaceOffer
{
    std::vector<std::tuple<std::string,std::string,int>> lostProv;
    std::vector<std::tuple<std::string,std::string,int>> gainProv;
    int warScore;
    int warId;
    std::vector<std::string> attackers, defenders;
    std::string recipant, offeredBy;
    bool recipantIsDefender;
};
