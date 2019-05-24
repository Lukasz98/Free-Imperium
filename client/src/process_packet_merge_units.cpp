#include "process_packet.h"


void ProcessPacket::MergeUnits(sf::Packet & packet, std::vector<std::shared_ptr<Unit>> & units)
{
    int baseUnitId, unitsCount;
    packet >> baseUnitId;
    packet >> unitsCount;

    auto unitBaseIt = std::find_if(units.begin(), units.end(), [baseUnitId](const std::shared_ptr<Unit> & unit) {
            return unit->GetId() == baseUnitId;
        });
    if (unitBaseIt != units.end()) {
        for (int i = 0; i < unitsCount; i++) {
            int unitId;
            packet >> unitId;
            auto unitIt = std::find_if(units.begin(), units.end(), [unitId](const std::shared_ptr<Unit> & unit) {
                    return unit->GetId() == unitId;
                });
            if (unitIt != units.end()) {
                units.erase(unitIt);
            }
        }
    }
}
