#include "process_packet.h"

void ProcessPacket::AcceptedPeace(sf::Packet & packet, std::vector<Province> & provinces, std::vector<std::shared_ptr<Country>> & countries, std::vector<War> & wars, std::vector<PeaceOffer> & offersForHumans, Date & date, std::vector<Packet> & toSend)
{
    int peaceId;
    packet >> peaceId;

    auto offerIt = std::find_if(offersForHumans.begin(), offersForHumans.end(), [peaceId](PeaceOffer & p) { return p.peaceId == peaceId; });

    auto warIt = std::find_if(wars.begin(), wars.end(), [warId = offerIt->warId](War & w) { return w.GetId() == warId; });

    auto recipantIt = std::find_if(countries.begin(), countries.end(), [cname = offerIt->recipant](std::shared_ptr<Country> & c) { return c->GetName() == cname; });

    PeaceOffer peaceOffer = *offerIt;
    offersForHumans.erase(offerIt);
    DoTheThing::RealizeAcceptedPeace(provinces, peaceOffer, toSend, recipantIt, warIt, date, wars);
}

