#pragma once
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>

#include "asset_manager.h"
#include "graphics/rectangle.h"
#include "log.h"

class Arrow : public Rectangle {
   public:
    Arrow() : Rectangle(glm::vec3{}, glm::vec3{}, glm::vec3{}, glm::vec3{}) {}
    Arrow(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4) : Rectangle(p1, p2, p3, p4) {}
};

struct Move {
    glm::vec3 destiny;
    int daysLeft;
    std::shared_ptr<Arrow> arrow;
};

class Unit {
   public:
    glm::mat4 model, rotate;
    const float rotateX, yScale;
    glm::vec3 position;
    // if those three will have different sizes
    // code will brake
    std::shared_ptr<Texture> texture[2];
    std::shared_ptr<Texture> move_texture[2];
    std::shared_ptr<Texture> battle_texture[2];

    int labelXOffset = 28, labelYOffset = 28;
    float dt;
    int activeFrame;

    bool visible;
    int id, countryId, provId;

    glm::vec3 fakePos;  // this say on what province unit stays
    std::string name, country;
    int morale = 100, discipline = 100;
    int speed = 100;
    int soldiers = 1000;

    int siegePower = 1;
    int attack = 4, defense = 50;
    bool isInBattle = false;

    std::vector<Move> moves;
    std::vector<glm::vec3> movesVec;

   public:
    Unit();
    ~Unit();

    void Update(glm::vec3 p, std::vector<glm::vec3> mvs, int soldiersCount);

    void AddMove(std::vector<Move> ms);

    void Draw(glm::mat4 matrix, bool isSelected = false);
    void DrawGuiElements(bool isSelected = false);

    void Kill(int amount);

    void Battle(bool is, bool attacker = false);

    void UpdateDt(float d);

    void Visible(bool v)
    {
        visible = v;
    }

    bool Click(glm::vec3 vv, glm::vec3 eye);

    inline int GetCountryId() const
    {
        return countryId;
    }
    inline int GetProvId() const
    {
        return provId;
    }
    std::string GetName() const
    {
        return name;
    }
    std::string GetCountry() const
    {
        return country;
    }
    int GetMorale()
    {
        return morale;
    }
    int GetId() const
    {
        return id;
    }
    int GetSoldiers() const
    {
        return soldiers;
    }
    int GetAttack()
    {
        return attack;
    }
    int GetDefense()
    {
        return defense;
    }
    glm::vec3 GetFakePos()
    {
        return fakePos;
    }
    bool IsInBattle()
    {
        return !(fakePos == position);
    }
    bool InBattle() const
    {
        return isInBattle;
    }

    int GetSiegePower() const
    {
        return siegePower;
    }
};
