#include "process_packet.h"

void ProcessPacket::MonthlyUpdate(sf::Packet & packet, const std::string & myCountry, std::vector<std::shared_ptr<Country>> & countries)
{
    int countC = 0;
    packet >> countC;

    for (int i = 0; i < countC; i++) {
        std::string cName;
        packet >> cName;

        auto cIt = std::find_if(countries.begin(), countries.end(), [cName](std::shared_ptr<Country> & c) { return c->GetName() == cName; });
        
        int relC = 0;
        packet >> relC;
        for (int r = 0; r < relC; r++) {
            std::string rcn;
            packet >> rcn;
            int rcr = 0;
            packet >> rcr;

            if (cIt != countries.end())
                (*cIt)->SetRel(rcn, rcr);
        }
    }

    int countImprRel = 0;
    packet >> countImprRel;
    std::vector<std::string> imprRel;
    for (int i = 0; i < countImprRel; i++) {
        std::string s;
        packet >> s;
        imprRel.emplace_back(s);
    }

    auto cIt = std::find_if(countries.begin(), countries.end(), [myCountry](std::shared_ptr<Country> & c) { return c->GetName() == myCountry; });
    if (cIt != countries.end()) {
        (*cIt)->SetImprRel(imprRel);
    }
}
