#pragma once

#include "war.h"
#include "battle.h"
#include "unit.h"
#include "province.h"
#include "country.h"

//#include "path_finder.h"

#include "client.h"
#include "packet.h"

#include "do_the_thing.h"

namespace ProcessPacket {

    void NewUnit(sf::Packet & packet, std::shared_ptr<Client> & client, std::vector<std::shared_ptr<Country>> & countries, 
                    std::vector<std::shared_ptr<Province>> & provinces, std::vector<Packet> & toSend);

    void UnitMove(sf::Packet & packet, std::shared_ptr<Client> & client, std::vector<std::shared_ptr<Province>> & provinces);

    void DeclareWar(sf::Packet & packet, std::shared_ptr<Client> & client, std::vector<std::shared_ptr<Country>> & countries, 
                    std::vector<War*> & wars, std::vector<Packet> & toSend);

    void OfferPeace(sf::Packet & packet, std::shared_ptr<Client> & client, std::vector<War*> & wars, std::vector<std::shared_ptr<Province>> & provinces,
                     std::vector<std::shared_ptr<Country>> & countries, std::vector<Packet> & toSend, const Date & date, std::vector<PeaceOffer> & offersForHumans);

    void MergeUnits(sf::Packet & packet, std::shared_ptr<Client> & client, std::vector<std::shared_ptr<Province>> & provinces, std::vector<Packet> & toSend);
    
    void StartImprRel(sf::Packet & packet, std::vector<std::shared_ptr<Country>> & countries);
    void StopImprRel(sf::Packet & packet, std::vector<std::shared_ptr<Country>> & countries);
    void AcceptedPeace(sf::Packet & packet, std::vector<std::shared_ptr<Country>> & countries, std::vector<War*> & wars, 
                        std::vector<std::shared_ptr<Province>> & provinces, std::vector<PeaceOffer> & offersForHumans, std::vector<Packet> & toSend);

    void DeclinePeace(sf::Packet & packet, std::vector<PeaceOffer> & offersForHumans, std::vector<std::shared_ptr<Client>> & clients, std::vector<Packet> & toSend);

}

