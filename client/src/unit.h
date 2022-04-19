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
    
    int id, countryId, provId;

    glm::vec3 fakePos;
    std::string name, country;
    int soldiers = 1000;

    std::vector<Move> moves;
    std::vector<glm::vec3> movesVec;

    void AddMove(std::vector<Move> ms);

   public:
    Unit();
    ~Unit();

    void Update(glm::vec3 p, std::vector<glm::vec3> mvs, int soldiersCount);


    void Draw(glm::mat4 matrix, bool isSelected = false);
    
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
    int GetId() const
    {
        return id;
    }
    int GetSoldiers() const
    {
        return soldiers;
    }
    glm::vec3 GetFakePos()
    {
        return fakePos;
    }
};
