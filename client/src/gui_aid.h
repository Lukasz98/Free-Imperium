#pragma once

#include <vector>
#include <algorithm>
#include <cassert>

#include "gui/gui.h"
#include "country.h"
#include "battle.h"
#include "war.h"
#include "province.h"
#include "prepare_packet.h"
#include "peace_offer.h"

namespace GuiAid {

void DecreaseValue();

void IncreaseValue();

void OpenCountry(const std::vector<std::shared_ptr<Country>> & countries, const std::shared_ptr<Country> & myCountry, 
                 const std::vector<War> & wars);

void NewUnit( std::vector<std::unique_ptr<Province>> & provinces, std::vector<sf::Packet> & packets);

void OpenUnit(const std::vector<std::shared_ptr<Unit>> & units);

void EraseObj();

void DeclareWar(std::vector<std::shared_ptr<Country>> & countries, std::vector<sf::Packet> & packets);

void OpenWar(const std::vector<War> & wars);

void OpenBattle(const std::vector<std::unique_ptr<Battle>>::iterator & battleIt, 
                const std::vector<std::unique_ptr<Province>>::iterator & provIt);

void OpenSiegedProv(const std::vector<std::unique_ptr<Province>>::iterator & provIt, const std::vector<War> & wars);

void OfferPeace(const std::vector<War> & wars, const std::string & myCountry);

void SendPeaceOffer(const std::vector<War> & wars, std::vector<std::shared_ptr<Country>> & countries,
                    std::vector<std::unique_ptr<Province>> & provinces, std::vector<sf::Packet> & packets);

void MergeUnits( std::vector<sf::Packet> & packets);

void DateSpeed(std::vector<sf::Packet> & packets);

void StartGame(std::vector<sf::Packet> & packets);

void SetCountry(std::vector<sf::Packet> & packets);

void StartImprRel(std::shared_ptr<Country> & myCountry, std::vector<std::shared_ptr<Country>> & countries, 
                  std::vector<sf::Packet> & packets);

void StopImprRel(std::shared_ptr<Country> & myCountry, std::vector<std::shared_ptr<Country>> & countries,
                 std::vector<sf::Packet> & packets);

void BotPeaceOffer(std::vector<PeaceOffer> & peaceOffers, const std::vector<std::shared_ptr<Country>> & countries,
                    const std::vector<std::unique_ptr<Province>> & provinces);
 
void AcceptPeace(std::vector<PeaceOffer> &  peaceOffers, std::vector<sf::Packet> & packets);

void DeclinePeace(std::vector<PeaceOffer> &  peaceOffers,
                  std::vector<sf::Packet> & packets);





} 
