#pragma once
#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>

#include "graphics/rectangle.h"
#include "gui/label.h"
#include "gui/float_to_string.h"
#include "log.h"
#include "observer.h"

class Arrow : public Rectangle
{
public:
    Arrow() : Rectangle(glm::vec3{}, glm::vec3{}, glm::vec3{}, glm::vec3{}) {}
    Arrow(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4)
        : Rectangle(p1, p2, p3, p4)
    {
        
    }
};

struct Move
{
    glm::vec3 destiny;
    int daysLeft;
    std::shared_ptr<Arrow> arrow;
};

class Unit : public Rectangle, public Subject
{
    std::unique_ptr<Label> bar;

    // if those three will have different sizes
    // code will brake
    std::shared_ptr<Texture> texture[2];
    std::shared_ptr<Texture> move_texture[2];
    std::shared_ptr<Texture> battle_texture[2];

    float dt;
    int activeFrame;
    
    bool visible;
    int id;
    
    glm::vec3 fakePos; // this say on what province unit stays 
    std::string name, country;
    int morale = 100;
    int speed = 100;
    int soldiers = 1000;

    int siegePower = 1;
    int attack = 4, defense = 50;
    bool isInBattle = false;
    
    std::vector<Move> moves;

public:
    Unit(int id, std::string name, glm::vec3 pos, int soldiers, std::string country);
    
    Unit(const Unit & u) = delete;
    
    ~Unit();

    void Update(glm::vec3 p, std::vector<glm::vec3> mvs, int soldiersCount);

    void AddMove(std::vector<Move> ms);

    void Draw(bool isSelected = false);
    
    void Kill(int amount);

    void Battle(bool is, bool attacker = false);

    void UpdateDt(float d);
    
    void Visible(bool v) { visible = v; }

    bool Click(int x, int y) { if (!visible) return false; else return Rectangle::Click(x, y); }

    std::string GetName() { return name; }
    std::string GetCountry() { return country; }
    int GetMorale() { return morale; }
    int GetId() { return id; }
    int GetSoldiers() { return soldiers; }
    int GetAttack() { return attack; }
    int GetDefense() { return defense; }
    glm::vec3 GetFakePos() { return fakePos; }
    bool IsInBattle() { return !(fakePos == position); }
    bool InBattle() const { return isInBattle; }

    int GetSiegePower() const { return siegePower; }

    std::unordered_map<std::string,std::string> GetValues() const;
};
