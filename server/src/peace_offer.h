#pragma once
#include <vector>
#include <tuple>

struct PeaceOffer
{
    static int FreeId;

    int peaceId;
    PeaceOffer();
    
    std::vector<std::tuple<int, int, int>> gainProv;
    std::vector<std::tuple<int, int, int>> lostProv;
    int warScore;
    int warId;
    std::vector<int> attackers, defenders;
    int recipant, offeredBy;
    bool recipantIsDefender;
};

