#pragma once
#include <memory>
#include <vector>
#include <algorithm>
#include "unit.h"

class Siege
{
    std::vector<std::shared_ptr<Unit>> units;
    bool done = false, unsiege = false;
    int level = 0;
    int leader = -1;
    bool stateChanged = false;

    std::vector<std::shared_ptr<Unit>>::iterator isUnitIn(std::shared_ptr<Unit> & u);
public:
    Siege();
    ~Siege();

    void Update(); //  should be called after whole cycle of adding units;
    void Unsiege(int power);
    void StopUnsiege();
    
    void AddUnit(std::shared_ptr<Unit> & u);
    void EraseUnit(std::shared_ptr<Unit> & u);

    bool IsStateChanged() { return stateChanged; }
    void ResetStateChande() { stateChanged = false; }
    int GetLevel() const { return level; }
    inline int GetLeader() const { return leader; }
    bool IsDone() const { return done; }
    int GetSiegeSoldiersCount() const;
};
