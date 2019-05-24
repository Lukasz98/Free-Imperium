#include "process_packet.h"

void ProcessPacket::NewBattle(sf::Packet & packet, std::vector<std::shared_ptr<Unit>> & units, std::vector<std::unique_ptr<Battle>> & battles)
{
    int id, warId, attackerUnitId, defenderUnitId;
    glm::vec3 pos;
    std::string provName;
          
    packet >> id;
    packet >> warId;
    packet >> pos.x;
    packet >> pos.y;
    packet >> pos.z;
    packet >> provName;
    packet >> attackerUnitId;
    packet >> defenderUnitId;
        
    Battle battle{id, warId, pos, provName};
    for (auto & u : units) {
        if (u->GetId() == attackerUnitId) {
            battle.AddAttacker(u);
            break;
        }
    }
    for (auto & u : units) {
        if (u->GetId() == defenderUnitId) {
            battle.AddDefender(u);
            break;
        }
    }
    battles.emplace_back(std::make_unique<Battle>(battle));    
}
