#include "process_packet.h"

void ProcessPacket::NewUnit(sf::Packet & packet, std::shared_ptr<Client> & client, std::vector<std::shared_ptr<Country>> & countries, std::vector<Province> & provinces, std::vector<std::shared_ptr<Unit>> & units, std::vector<Packet> & toSend)
{

    std::string countryName, provName;
    int unitSize;
    packet >> countryName >> provName >> unitSize;

    std::shared_ptr<Country> country;
    if (client->GetName() == "boot") {
        bool ok = false;
        for (auto & c : countries) {
            if (c->GetName() == countryName) {
                country = c;
                ok = true;
                break;
            }
        }
        if (!ok)
            return;
    }
    else {
        country = client->GetCountry();
        if (country->GetName() != countryName)
            return;
    }

    auto provIt = std::find_if(provinces.begin(), provinces.end(), [provName](Province &p){
            return p.GetName() == provName;
        });
    if (provIt == provinces.end())
        return;
        
    if (country->GetName() != (*provIt).GetCountry())
        return;

    if ((*provIt).GetManpower() < unitSize)
        return;


    float priceForHundred = 0.1f;
    float price = (float)unitSize / (priceForHundred * 1000.0f);
    if (country->GetGold() >= price) {
        country->EraseMoney(price);
        (*provIt).EraseManpower(unitSize);
        country->EraseManpower(unitSize);
        Unit unit(provName + " unit", (*provIt).GetUnitPos(), unitSize, countryName, provName);
        units.push_back(std::make_shared<Unit>(unit));

        DoTheThing::SendNewUnitInfo(units.back(), toSend);
        Packet pack{};
        pack.PushRecipant(countryName);
        pack << "SetGold";
        pack << country->GetGold();
        toSend.emplace_back(pack);
    }        

}
