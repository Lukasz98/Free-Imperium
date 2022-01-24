#pragma once

#include <SFML/Network.hpp>
#include <functional>
#include <memory>

#include "battle.h"
#include "country.h"
#include "gui_structs.h"
#include "log.h"
#include "peace_offer.h"
#include "province.h"
#include "unit.h"
#include "war.h"
#include "map2.h"

namespace ProcessPacket {

void DailyUpdate(sf::Packet& packet, std::vector<War>& wars, std::vector<std::unique_ptr<Province>>& provinces,
                 std::vector<std::shared_ptr<Country>>& countries, TopBarData& topBarData, Map2* map2);

void HourlyUpdate(sf::Packet& packet, std::vector<Unit>& units,
                  std::vector<Battle>& battles, float mapChunkScale, const unsigned char* height, int mapWidth);

int PeaceAccepted(sf::Packet& packet, std::vector<std::unique_ptr<Province>>& provinces,
                  std::vector<std::shared_ptr<Country>>& countries, std::vector<War>& wars, SideBarData& sideBarData, std::vector<ProvData>& provsData);

void NewWar(sf::Packet& packet, std::vector<War>& wars, int myCountryId,
            std::vector<std::shared_ptr<Country>>& countries, SideBarData& sideBarData);


void ProvincePopulation(sf::Packet& packet, std::vector<std::unique_ptr<Province>>& provinces);

void SetGold(sf::Packet& packet, std::shared_ptr<Country>& country);

void SetSpeed(sf::Packet& packet);

void MonthlyUpdate(sf::Packet& packet, const std::string& myCountry,
                   std::vector<std::shared_ptr<Country>>& countries);

void BotPeaceOffer(sf::Packet& packet, std::vector<PeaceOffer>& peaceOffers,
                   const std::vector<std::shared_ptr<Country>>& countries, SideBarData& sideBarData, const std::shared_ptr<Country>& myCountry);

void PeaceDeclined(sf::Packet& packet, SideBarData& sideBarData, const std::vector<std::shared_ptr<Country>>& countries);

void Paused(sf::Packet& packet, bool& game_paused);
}  // namespace ProcessPacket
