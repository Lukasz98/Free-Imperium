#include "process_packet.h"

void ProcessPacket::DeclareWar(sf::Packet & packet, std::shared_ptr<Client> & client, std::vector<std::shared_ptr<Country>> & countries, std::vector<War> & wars, std::vector<Packet> & toSend, const Date & date)
{
    std::string declaringTo;
    packet >> declaringTo;

    auto declaredBy = client->GetCountry()->GetName();

    DoTheThing::DeclareWar(countries, declaredBy, declaringTo, wars, toSend, date);
}
