#pragma once

#include <SFML/Network.hpp>
#include <functional>
#include <memory>

#include "battle.h"
#include "country.h"
#include "gui/gui_structs.h"
#include "gui_aid.h"
#include "log.h"
#include "map.h"
#include "peace_offer.h"
#include "province.h"
#include "unit.h"
#include "war.h"

namespace ProcessPacket {

void DailyUpdate(sf::Packet& packet, std::vector<War>& wars, std::vector<std::unique_ptr<Province>>& provinces,
                 std::vector<std::shared_ptr<Country>>& countries, Map& map, TopBarData& topBarData);

void HourlyUpdate(sf::Packet& packet, std::vector<Unit>& units,
                  std::vector<Battle>& battles, float mapChunkScale, const unsigned char* height, int mapWidth);

int PeaceAccepted(sf::Packet& packet, std::vector<std::unique_ptr<Province>>& provinces,
                  std::vector<std::shared_ptr<Country>>& countries, std::vector<War>& wars, Map& map, SideBarData& sideBarData);

void NewWar(sf::Packet& packet, std::vector<War>& wars, int myCountryId,
            std::vector<std::shared_ptr<Country>>& countries, SideBarData& sideBarData);

//void NewUnit(sf::Packet& packet, std::vector<std::shared_ptr<Unit>>& units,
//             std::vector<std::shared_ptr<Country>>& countries, const std::string& countryName);
/*
void NewBattle(sf::Packet& packet, std::vector<Unit>& units,
               std::vector<std::unique_ptr<Battle>>& battles, std::vector<std::unique_ptr<Province>>& provinces);

void EndBattle(sf::Packet& packet, std::vector<std::unique_ptr<Battle>>& battles);
*/
//void DeleteUnit(sf::Packet& packet, std::vector<std::shared_ptr<Unit>>& units);

//void NewUnitInBattle(sf::Packet& packet, std::vector<Unit>& units,
//                     std::vector<std::unique_ptr<Battle>>& battles);

//void MergeUnits(sf::Packet& packet, std::vector<std::shared_ptr<Unit>>& units);

void ProvincePopulation(sf::Packet& packet, std::vector<std::unique_ptr<Province>>& provinces);

void SetGold(sf::Packet& packet, std::shared_ptr<Country>& country);

void SetSpeed(sf::Packet& packet);

void MonthlyUpdate(sf::Packet& packet, const std::string& myCountry,
                   std::vector<std::shared_ptr<Country>>& countries);

void BotPeaceOffer(sf::Packet& packet, std::vector<PeaceOffer>& peaceOffers,
                   const std::vector<std::shared_ptr<Country>>& countries, SideBarData& sideBarData, const std::shared_ptr<Country>& myCountry);

void PeaceDeclined(sf::Packet& packet, SideBarData& sideBarData);
}  // namespace ProcessPacket
