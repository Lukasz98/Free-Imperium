#pragma once

#include <memory>
#include <functional>
#include <SFML/Network.hpp>

#include "unit.h"
#include "war.h"
#include "battle.h"
#include "province.h"

#include "gui/gui_bridge.h"
#include "gui_aid.h"

#include "country.h"
#include "map.h"
#include "log.h"
#include "peace_offer.h"


namespace ProcessPacket {

    
    void DailyUpdate(sf::Packet & packet, std::vector<War> & wars, std::vector<std::unique_ptr<Province>> & provinces, 
                        std::vector<std::shared_ptr<Country>> & countries, Map & map);
    
    void HourlyUpdate(sf::Packet & packet, std::vector<std::shared_ptr<Unit>> & units, std::vector<std::unique_ptr<Battle>> & battles,
                      float mapChunkScale);
    
    void PeaceAccepted(sf::Packet & packet, std::vector<std::unique_ptr<Province>> & provinces, 
                       std::vector<std::shared_ptr<Country>> & countries, std::vector<War> & wars, Map & map);
    
    void NewWar(sf::Packet & packet, std::vector<War> & wars, int myCountryId, std::vector<std::shared_ptr<Country>> & countries);


    void NewUnit(sf::Packet & packet, std::vector<std::shared_ptr<Unit>> & units, std::vector<std::shared_ptr<Country>> & countries, const std::string & countryName);

    void NewBattle(sf::Packet & packet, std::vector<std::shared_ptr<Unit>> & units, std::vector<std::unique_ptr<Battle>> & battles,
                     std::vector<std::unique_ptr<Province>> & provinces);

    void EndBattle(sf::Packet & packet, std::vector<std::unique_ptr<Battle>> & battles);

    void DeleteUnit(sf::Packet & packet, std::vector<std::shared_ptr<Unit>> & units);

    void NewUnitInBattle(sf::Packet & packet, std::vector<std::shared_ptr<Unit>> & units, std::vector<std::unique_ptr<Battle>> & battles);

    void MergeUnits(sf::Packet & packet, std::vector<std::shared_ptr<Unit>> & units);

    void ProvincePopulation(sf::Packet & packet, std::vector<std::unique_ptr<Province>> & provinces); 

    void SetGold(sf::Packet & packet, std::shared_ptr<Country> & country);

    void SetSpeed(sf::Packet & packet);

    void MonthlyUpdate(sf::Packet & packet, const std::string & myCountry, std::vector<std::shared_ptr<Country>> & countries);

    void BotPeaceOffer(sf::Packet & packet, std::vector<PeaceOffer> & peaceOffers, const std::vector<std::shared_ptr<Country>> & countries);

    void PeaceDeclined(sf::Packet & packet);
}
