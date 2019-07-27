#include "process_packet.h"

void DoTheThing::NotifyOfDeclinedPeace(const std::string & clientCountryName, const std::string & recipant, std::vector<Packet> & toSend)
{
    Packet packet;
    packet.PushRecipant(clientCountryName);
    packet << "PeaceDeclined";
    packet << recipant;
    toSend.emplace_back(packet);
}
