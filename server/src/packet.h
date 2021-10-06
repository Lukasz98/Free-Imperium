#pragma once

#include <SFML/Network.hpp>

class Packet : public sf::Packet
{
    bool all;        
    std::vector<int> recipants;
    
public:
    Packet(bool all = false)
    {
        this->all = all;
    }

    bool IsRecipant(int countryId) const
    {
        auto f = std::find(recipants.begin(), recipants.end(), countryId);
        return f != recipants.end() || all;
    }

    void PushRecipant(int countryId)
    {
        recipants.push_back(countryId);
    }
};
