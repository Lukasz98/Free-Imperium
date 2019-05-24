#pragma once

#include <SFML/Network.hpp>
#include "log.h"

namespace PreparePacket {

    static sf::Packet NewUnit(const std::string & country, const std::string & province, int size)
    {
        sf::Packet packet;
        packet << "NewUnit";
        packet << country;
        packet << province;
        packet << size;
        
        return packet;
    }

}   
