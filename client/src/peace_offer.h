#pragma once
#include <vector>
#include <tuple>

struct PeaceOffer
{
    int peaceId;
    int idInGui;
    
    std::vector<std::tuple<int, int, int>> lostProv;
    //std::vector<std::tuple<std::string,std::string,int>> lostProv;
    std::vector<std::tuple<int,int,int>> gainProv;
    //std::vector<std::tuple<std::string,std::string,int>> gainProv;
    int warScore;
    int warId;
    std::vector<std::string> attackers, defenders;
    //std::string recipant, offeredBy;
    int recipant, offeredBy;
    bool recipantIsDefender;
};

