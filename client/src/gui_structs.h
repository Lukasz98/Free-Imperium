#pragma once
#include <vector>

#include "num_to_string.h"

struct TopBarData {
    std::string ctrName, date;
    float gold = 0.0f, income = 0.0f, armyMaintenance = 0.0f;
    int manpower = 0, manpRecovery = 0, dateSpeed = 0;
};

struct SideBarData {
    enum class IType {
        WAR = 0,
        PEACE_OFFER = 1,
        PEACE_ACCEPTED = 2,
        PEACE_DECLINED = 3,
        PADDING_FOR_ERASE_ICON = 4,
    };
    struct Element {
        IType type;
        int val;
        std::string hoverText;
    };
    std::vector<Element> elements;
};
