#include "process_packet.h"

void ProcessPacket::SetSpeed(sf::Packet & packet, Gui & gui)
{
    int speed = 1;
    packet >> speed;
    std::unordered_map<std::string,std::string> values;
    values["dateSpeed"] = std::to_string(speed);
    gui.Update(values, "topBar");
}
