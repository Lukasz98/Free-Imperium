#include "process_packet.h"

void ProcessPacket::EndBattle(sf::Packet & packet, std::vector<std::unique_ptr<Battle>> & battles)
{
    int id;
    packet >> id;
    for (int i = 0; i < battles.size(); i++) {
        if (battles[i]->GetId() == id) {
            battles[i]->EndBattle();
            battles.erase(battles.begin() + i);
            break;
        }
    }    
}
