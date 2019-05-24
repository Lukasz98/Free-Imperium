#pragma once

#include <SFML/Network.hpp>

class Packet : public sf::Packet
{
    bool all;        
    std::vector<std::string> recipants;
    
public:
    Packet(bool all = false)
    {
        this->all = all;
    }

    bool IsRecipant(std::string r) const
    {
        auto f = std::find(recipants.begin(), recipants.end(), r);
        return f != recipants.end() || all;
    }

    void PushRecipant(std::string r)
    {
        recipants.push_back(r);
    }
};
