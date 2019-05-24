#include "process_packet.h"

void ProcessPacket::SetGold(sf::Packet & packet, Gui & gui, std::shared_ptr<Country> & country)
{
    float gold = 0.0f;
    packet >> gold;

    country->SetGold(gold);

    std::unordered_map<std::string,std::string> values;
    values["countryGold"] = ftos(gold);
    gui.Update(values, "topBar");
}
