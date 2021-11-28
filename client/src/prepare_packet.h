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

    static sf::Packet SetSpeed(int val)
    {
        sf::Packet packet;
        packet << "Speed";
        packet << val;
        return packet;
    }
    
    static sf::Packet MoveUnit(int uid, int provid)
    {
        sf::Packet packet;
        packet << "UnitMove";
        packet << uid;
        packet << provid;
        return packet;
    }
}   
