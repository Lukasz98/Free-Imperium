#include "process_packet.h"

void ProcessPacket::NewUnit(sf::Packet & packet, std::vector<std::shared_ptr<Unit>> & units, const std::string & countryName)
{
    int id, unitSize, morale;
    std::string name = "cyce", country;
    glm::vec3 pos;
    packet >> id;
    packet >> name;
    packet >> country;
    packet >> morale;
    packet >> unitSize;
    packet >> pos.x;
    packet >> pos.y;
    packet >> pos.z;
    pos.x*=4;
    pos.y*=4;

    units.push_back(std::make_shared<Unit>(id, name, pos, unitSize, country));

    if (country == countryName)
        units.back()->Visible(true);
}
