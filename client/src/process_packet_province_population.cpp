#include "process_packet.h"

void ProcessPacket::ProvincePopulation(sf::Packet & packet, std::vector<std::unique_ptr<Province>> & provinces)
{
    int id, population;
    packet >> id;
    packet >> population;
    auto provIt = std::find_if(provinces.begin(), provinces.end(), [id](const std::unique_ptr<Province> & prov){
            return id == prov->GetId();
        });
    if (provIt != provinces.end()) {
        (*provIt)->SetPopulation(population);
    }
}
