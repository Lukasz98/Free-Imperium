#include "process_packet.h"

void ProcessPacket::PeaceAccepted(sf::Packet & packet, Gui & gui, std::vector<std::unique_ptr<Province>> & provinces, std::vector<War> & wars)
{
    int provinceCount;
    packet >> provinceCount;
        
    for (int i = 0; i < provinceCount; i++) {
        std::string name, ctr;
        packet >> name;
        packet >> ctr;
        auto provIt = std::find_if(provinces.begin(), provinces.end(), [name](const std::unique_ptr<Province> & prov){
                return prov->GetName() == name;
            });
        if (provIt != provinces.end()) {
            (*provIt)->SetOwner(ctr);
            //setCountryMap();
        }
    }

    int warId;
    packet >> warId;

    auto warIt = std::find_if(wars.begin(), wars.end(), [warId](const War & war) {
            return war.GetId() == warId;
        });
    if (warIt == wars.end())
        return;

    std::string peaceType;
    packet >> peaceType;
    if (peaceType == "endWar") {
        gui.EraseObj("notifications", warIt->GetIdInGui());
        wars.erase(warIt);
    }
    else {            
        std::string ctr;
        packet >> ctr;
        (*warIt).Erase(ctr);
    }
}
