#pragma once

#include <SFML/Network.hpp>
#include <functional>
#include <memory>

#include "battle.h"
#include "country.h"
#include "gui_structs.h"
#include "log.h"
#include "map2.h"
#include "peace_offer.h"
#include "province.h"
#include "unit.h"
#include "war.h"

namespace ProcessPacket {

void DailyUpdate(sf::Packet& packet, std::vector<War>& wars, std::vector<Province>& provinces,
                 std::vector<Country>& countries, TopBarData& topBarData, Map2* map2);

void HourlyUpdate(sf::Packet& packet, std::vector<Unit>& units, std::vector<Battle>& battles, float mapChunkScale,
                  const unsigned char* height, int mapWidth);

int PeaceAccepted(sf::Packet& packet, std::vector<Province>& provinces, std::vector<Country>& countries,
                  std::vector<War>& wars, SideBarData& sideBarData);

void NewWar(sf::Packet& packet, std::vector<War>& wars, int myCountryId, std::vector<Country>& countries,
            SideBarData& sideBarData);

void ProvincePopulation(sf::Packet& packet, std::vector<Province>& provinces);

void SetGold(sf::Packet& packet, Country* country);

void SetSpeed(sf::Packet& packet);

void MonthlyUpdate(sf::Packet& packet, const std::string& myCountry, std::vector<Country>& countries);

void BotPeaceOffer(sf::Packet& packet, std::vector<PeaceOffer>& peaceOffers, const std::vector<Country>& countries,
                   SideBarData& sideBarData, const Country* myCountry);

void PeaceDeclined(sf::Packet& packet, SideBarData& sideBarData, const std::vector<Country>& countries);

void Paused(sf::Packet& packet, bool& game_paused);
}  // namespace ProcessPacket
