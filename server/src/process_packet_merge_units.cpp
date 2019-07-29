#include "process_packet.h"


void ProcessPacket::MergeUnits(sf::Packet & packet, std::shared_ptr<Client> & client, std::vector<std::shared_ptr<Unit>> & units, std::vector<Province> & provinces, std::vector<Packet> & toSend)
{
    int countUnits;
    std::vector<int> unitsIds;
    packet >> countUnits;
    for (int i = 0; i < countUnits; i++) {
        int id;
        packet >> id;
        unitsIds.push_back(id);
    }

    bool possible = true;
    std::vector<int> indexesToErase;
    std::string whatProvinceTheyAre = "";
    for (int i = 0; i < unitsIds.size(); i++) {
        int id = unitsIds[i];
        auto unitIt = std::find_if(units.begin(), units.end(), [id](const std::shared_ptr<Unit> & unit) {
                return unit->GetId() == id;
            });
        if (unitIt == units.end()) {
            indexesToErase.push_back(i);
            continue;
        }
        if ((*unitIt)->GetCountry() != client->GetCountry()->GetName()) {
            indexesToErase.push_back(i);
            continue;
        }
        glm::vec3 unitFakePos = (*unitIt)->GetFakePos();
        auto provIt = std::find_if(provinces.begin(), provinces.end(), [unitFakePos](const Province & p) {
                return p.GetUnitPos() == unitFakePos;
            });
        if (provIt == provinces.end()) {
            possible = false;
        }
        else if (whatProvinceTheyAre == "")
            whatProvinceTheyAre = (*provIt).GetName();
        else if (whatProvinceTheyAre != (*provIt).GetName()) {
            possible = false;
            break;
        }

        if ((*unitIt)->IsInBattle()) {
            possible = false;
            break;
        }
    }

    for (int i = indexesToErase.size() - 1; i >= 0; i--) {
        unitsIds.erase(unitsIds.begin() + indexesToErase[i]);
    }
        
    if (unitsIds.size() > 1 && possible) {
        // merging

        int unitBaseId = unitsIds[0];
        unitsIds.erase(unitsIds.begin());
        DoTheThing::MergeUnits(units, unitBaseId, unitsIds, toSend);
    }

}
