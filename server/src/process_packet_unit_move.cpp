#include "process_packet.h"

void ProcessPacket::UnitMove(sf::Packet & packet, std::shared_ptr<Client> & client, std::vector<Province> & provinces, std::vector<std::shared_ptr<Unit>> & units)
{
    int uId;
    std::string provName;
        
    packet >> uId;
    packet >> provName;

    auto unitIt = std::find_if(units.begin(), units.end(), [uId](const std::shared_ptr<Unit> & u){
            return u->GetId() == uId;
        });
    if (unitIt == units.end())
        return;

    if ((*unitIt)->GetCountry() != client->GetCountry()->GetName())
        return;
        
    auto provIt = std::find_if(provinces.begin(), provinces.end(), [provName](Province &p){
            return p.GetName() == provName;
        });
    if (provIt == provinces.end())
        return;

    glm::vec3 uPos = (*unitIt)->GetFakePos();
    auto unitProvIt = std::find_if(provinces.begin(), provinces.end(), [uPos](Province &p){
            return p.GetUnitPos() == uPos;
        });
    if (unitProvIt == provinces.end())
        return;

    int unitProv = (*unitProvIt).GetId();
    int endProv = (*provIt).GetId();

    DoTheThing::MoveUnit(*unitIt, provinces, unitProv, endProv);
}
