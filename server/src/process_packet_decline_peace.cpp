#include "process_packet.h"

void ProcessPacket::DeclinePeace(sf::Packet & packet, std::vector<PeaceOffer> & offersForHumans, std::vector<std::shared_ptr<Client>> & clients, std::vector<Packet> & toSend)
{
    int peaceId = 0;
    packet >> peaceId;

    auto peaceIt = std::find_if(offersForHumans.begin(), offersForHumans.end(), [peaceId](PeaceOffer & p) {
                     return p.peaceId == peaceId;
                   });
    if (peaceIt == offersForHumans.end())
        return;

    for (auto & c : clients) {
        if (peaceIt->offeredBy == c->GetCountryName()) {
            DoTheThing::NotifyOfDeclinedPeace(c->GetCountryName(), peaceIt->recipant, toSend);
        }
    }

    offersForHumans.erase(peaceIt);
    
}
