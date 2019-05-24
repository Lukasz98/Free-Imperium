#include "process_packet.h"


void ProcessPacket::DeleteUnit(sf::Packet & packet, std::vector<std::shared_ptr<Unit>> & units)
{
    int id;
    packet >> id;
    for (int i = 0; i < units.size(); i++) {
        if (units[i]->GetId() == id) {
            //Log("Erase " << units[i]->GetName());
            units.erase(units.begin() + i);
            break;
        }
    }
}
