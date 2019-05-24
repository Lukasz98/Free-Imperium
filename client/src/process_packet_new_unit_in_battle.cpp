#include "process_packet.h"


void ProcessPacket::NewUnitInBattle(sf::Packet & packet, std::vector<std::shared_ptr<Unit>> & units, std::vector<std::unique_ptr<Battle>> & battles)
{
    int battleId, unitId;
    bool attacker;
    packet >> battleId;
    packet >> unitId;
    packet >> attacker;
    for (auto & battle : battles) {
        if (battle->GetId() == battleId) {
            for (auto & unit : units) {
                if (unit->GetId() == unitId) {
                    if (attacker)
                        battle->AddAttacker(unit);
                    else
                        battle->AddDefender(unit);
                    break;
                }
            }
            break;
        }
    }
}
