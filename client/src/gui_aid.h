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

void DecreaseValue(Gui & gui, GuiClick & event);

void IncreaseValue(Gui & gui, GuiClick & event);

void OpenCountry(Gui & gui, const std::vector<std::shared_ptr<Country>> & countries, const std::shared_ptr<Country> & myCountry, 
                 const std::vector<War> & wars, const GuiClick & event);

void NewUnit(const GuiClick & event, std::vector<std::unique_ptr<Province>> & provinces, std::vector<sf::Packet> & packets);

void OpenUnit(Gui & gui, GuiClick & event, const std::vector<std::shared_ptr<Unit>> & units);

void EraseObj(Gui & gui, GuiClick & event);

void DeclareWar(GuiClick & event, std::vector<std::shared_ptr<Country>> & countries, std::vector<sf::Packet> & packets);

void OpenWar(Gui & gui, GuiClick & event, const std::vector<War> & wars);

void OpenBattle(Gui & gui, const std::vector<std::unique_ptr<Battle>>::iterator & battleIt, 
                const std::vector<std::unique_ptr<Province>>::iterator & provIt);

void OpenSiegedProv(Gui & gui, const std::vector<std::unique_ptr<Province>>::iterator & provIt, const std::vector<War> & wars);

void OfferPeace(Gui & gui, const std::vector<War> & wars, GuiClick & event, const std::string & myCountry);

void SendPeaceOffer(Gui & gui, const std::vector<War> & wars, std::vector<std::shared_ptr<Country>> & countries,
                    std::vector<std::unique_ptr<Province>> & provinces, GuiClick & event, std::vector<sf::Packet> & packets);

void MergeUnits(Gui & gui, const GuiClick & event, std::vector<sf::Packet> & packets);

void DateSpeed(GuiClick & event, std::vector<sf::Packet> & packets);

void StartGame(std::vector<sf::Packet> & packets);

void SetCountry(GuiClick & click, std::vector<sf::Packet> & packets);

void StartImprRel(GuiClick & event, std::shared_ptr<Country> & myCountry, std::vector<std::shared_ptr<Country>> & countries, 
                  std::vector<sf::Packet> & packets);

void StopImprRel(GuiClick & event, std::shared_ptr<Country> & myCountry, std::vector<std::shared_ptr<Country>> & countries,
                 std::vector<sf::Packet> & packets);

void BotPeaceOffer(GuiClick & event, Gui & gui, std::vector<PeaceOffer> & peaceOffers, const std::vector<std::shared_ptr<Country>> & countries,
                    const std::vector<std::unique_ptr<Province>> & provinces);
 
void AcceptPeace(GuiClick & event, Gui & gui, std::vector<PeaceOffer> &  peaceOffers, std::vector<sf::Packet> & packets);

void DeclinePeace(GuiClick & event, Gui & gui, std::vector<PeaceOffer> &  peaceOffers,
                  std::vector<sf::Packet> & packets);





} //namespace
