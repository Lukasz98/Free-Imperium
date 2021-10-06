#pragma once

#include <SFML/Network.hpp>

#include "country.h"


class Client : public sf::TcpSocket
{
    std::string name, countryName;
    std::shared_ptr<Country> country;
    bool isAi;
    
public:
    Client(bool isAi = false) : isAi(isAi)
    {

    }

    void SetName(const std::string & n) { name = n; }
    const std::string & GetName() { return name; }

    void SetCountryName(const std::string & n) { countryName = n; }
    const std::string & GetCountryName() { return countryName; }

    void SetCountry(std::shared_ptr<Country> c) { country = c; }
    std::shared_ptr<Country> GetCountry() { return country; }
};
