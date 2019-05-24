#include "process_packet.h"

void ProcessPacket::HourlyUpdate(sf::Packet & packet, Gui & gui, std::vector<std::shared_ptr<Unit>> & units, std::vector<std::unique_ptr<Battle>> & battles)
{
    int unitCount;
    packet >> unitCount;
    for (int i = 0; i < unitCount; i++) {
        int id, soldiersCount;
        glm::vec3 uPos;
        packet >> id;
        packet >> soldiersCount;
        packet >> uPos.x;
        packet >> uPos.y;
        packet >> uPos.z;

        int movesCount = 0;
        packet >> movesCount;

        std::vector<glm::vec3> moves;
        for (int i = 0; i < movesCount; i++) {
            glm::vec3 v;
            packet >> v.x;
            packet >> v.y;
            packet >> v.z;
            moves.push_back(v);
        }

        auto uIt = std::find_if(units.begin(), units.end(), [id](const std::shared_ptr<Unit> u) {
                return u->GetId() == id;
            });
        if (uIt != units.end()) {            
            (*uIt)->Update(uPos, moves, soldiersCount);
        }
    }

    int battlesCount;
    packet >> battlesCount;
    for (int i = 0; i < battlesCount; i++) {
        int id, attSize, defSize;
        packet >> id;
        packet >> attSize;
        packet >> defSize;
        for (auto & battle : battles) {
            if (battle->GetId() == id) {
                battle->SetAttackersSize(attSize);
                battle->SetDefendersSize(defSize);
                break;
            }
        }
    }
}
