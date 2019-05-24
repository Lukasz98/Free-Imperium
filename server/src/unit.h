#pragma once
#include <iostream>
#include <memory>
#include <vector>
#include <math.h>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include "log.h"

struct Move
{
    glm::vec3 destiny;
    int daysLeft;
};

class Unit
{
    int id;

    glm::vec3 fakePos; // this says on what province unit stays 
    glm::vec3 position; 
    std::string name, country, fromProvince;
    int morale = 100;
    float speed = 10;
    int soldiers = 1000;
    int killed = 0;
    
    int siegePower = 1;
    int attack = 1, defense = 50;
    bool isInBattle = false;
    bool sieging = false;
    
    std::vector<Move> moves;

    
public:
    static int FreeId;
    Unit(std::string name, glm::vec3 pos, int soldiers, std::string country, std::string fromProvince);
    
    //Unit(const Unit & u) = delete;
    
    ~Unit();

    void Update();

    void AddMove(std::vector<Move> ms);

    void Kill(int amount);

    void Battle(bool is, bool attacker = false);

    void Add(const std::shared_ptr<Unit> & u);

    void SetSieging(bool s) { sieging = s; }
    bool IsSieging() const { return sieging; }
    
    std::vector<Move> GetMoves() const { return moves; }
    std::string GetName() const { return name; }
    std::string GetCountry() const { return country; }
    int GetMorale() const { return morale; }
    //int GetProvinceId() const { return provinceId; }
    int GetId() const { return id; }
    int GetSoldiers() const { return soldiers; }
    int GetAttack() const { return attack * soldiers / 20; }
    int GetDefense() const { return defense; }
    glm::vec3 GetFakePos() const { return fakePos; }
    glm::vec3 GetPos() const { return position; }
    bool IsInBattle() const { return !(fakePos == position); }
    bool InBattle() const { return isInBattle; }
    
    bool IsMoving() const { return moves.size(); }
    
    int GetSiegePower() const { return siegePower; }

    int GetAndResetKilled() { int k = killed; killed = 0; return k; }

    std::string GetFromProvince() const { return fromProvince; }
};
