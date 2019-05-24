#include "process_packet.h"

void ProcessPacket::StartImprRel(sf::Packet & packet, std::vector<std::shared_ptr<Country>> & countries)
{
    std::string c1, c2;
    packet >> c1;
    packet >> c2;

    auto it = std::find_if(countries.begin(), countries.end(), [c1](std::shared_ptr<Country> & c) { return c->GetName() == c1; });
    if (it != countries.end()) {
        (*it)->StartImprRel(c2);
    }
}

void ProcessPacket::StopImprRel(sf::Packet & packet, std::vector<std::shared_ptr<Country>> & countries)
{
    std::string c1, c2;
    packet >> c1;
    packet >> c2;

    auto it = std::find_if(countries.begin(), countries.end(), [c1](std::shared_ptr<Country> & c) { return c->GetName() == c1; });
    if (it != countries.end()) {
        (*it)->StopImprRel(c2);
    }
}
