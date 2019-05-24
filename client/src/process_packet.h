#pragma once

#include <memory>
#include <functional>
#include <SFML/Network.hpp>

#include "unit.h"
#include "war.h"
#include "battle.h"
#include "province.h"

#include "gui/gui.h"
#include "gui_aid.h"

#include "country.h"
#include "log.h"

namespace ProcessPacket {

    
    void DailyUpdate(sf::Packet & packet, Gui & gui, std::vector<War> & wars, std::vector<std::unique_ptr<Province>> & provinces);
    void HourlyUpdate(sf::Packet & packet, Gui & gui, std::vector<std::shared_ptr<Unit>> & units, std::vector<std::unique_ptr<Battle>> & battles);
    void PeaceAccepted(sf::Packet & packet, Gui & gui, std::vector<std::unique_ptr<Province>> & provinces, std::vector<War> & wars);
    void NewWar(sf::Packet & packet, Gui & gui, std::vector<War> & wars, const std::string myCountry);


    void NewUnit(sf::Packet & packet, std::vector<std::shared_ptr<Unit>> & units, const std::string & countryName);

    void NewBattle(sf::Packet & packet, std::vector<std::shared_ptr<Unit>> & units, std::vector<std::unique_ptr<Battle>> & battles);

    void EndBattle(sf::Packet & packet, std::vector<std::unique_ptr<Battle>> & battles);

    void DeleteUnit(sf::Packet & packet, std::vector<std::shared_ptr<Unit>> & units);

    void NewUnitInBattle(sf::Packet & packet, std::vector<std::shared_ptr<Unit>> & units, std::vector<std::unique_ptr<Battle>> & battles);

    void MergeUnits(sf::Packet & packet, std::vector<std::shared_ptr<Unit>> & units);

    void ProvincePopulation(sf::Packet & packet, std::vector<std::unique_ptr<Province>> & provinces); 

    void SetGold(sf::Packet & packet, Gui & gui, std::shared_ptr<Country> & country);

    void SetSpeed(sf::Packet & packet, Gui & gui);

    void MonthlyUpdate(sf::Packet & packet, const std::string & myCountry, std::vector<std::shared_ptr<Country>> & countries);

}
