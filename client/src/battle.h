#pragma once
#include <memory>
#include <utility>

#include <glm/vec3.hpp>

#include "unit.h"

class Battle
{
    public:

    std::vector<int> attackers;
    std::vector<int> defenders;

    int id, warId, provId;
    int attackersSize = 0;
    int defendersSize = 0;
    int attackersMorale = 0;
    int defendersMorale = 0;
    int attackerId, defenderId;


public:
    inline int GetProvId() const { return provId; }
};
