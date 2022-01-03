#pragma once
#include <vector>
#include <tuple>

struct PeaceOffer
{
    int peaceId = -1;
    //int idInGui;
                           //prov,country,development
    std::vector<std::tuple<int, int, int>> lostProv;
    std::vector<std::tuple<int,int,int>> gainProv;
    int warId;
    int recipant, offeredBy;
    bool recipantIsDefender;

    PeaceOffer(int offeredBy, int warid, int recipant) : offeredBy(offeredBy), warId(warid), recipant(recipant), peaceId(-2) {}
    PeaceOffer() {}
};

