#pragma once

#include <SFML/Network.hpp>
#include "log.h"

namespace PreparePacket {

    static sf::Packet NewUnit(int ctrId, int provId, int size)//const std::string & country, const std::string & province, int size)
    {
        sf::Packet packet;
        packet << "NewUnit";
        packet << ctrId;
        packet << provId;
        packet << size;
        
        return packet;
    }

}   
