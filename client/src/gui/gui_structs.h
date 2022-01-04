#pragma once

#include "../observer.h"
#include "gui_update_struct.h"
#include "num_to_string.h"

struct TopBarData {
    Subject subject;
    std::string ctrName, date;  //, cash,
    float gold = 0.0f, income = 0.0f, armyMaintenance = 0.0f;
    int manpower = 0, manpRecovery = 0, dateSpeed = 0;
    void Update(std::string ctrN, std::string date, float gold, float inc, float armyMain, int manp, int manpRec,
                int dateSp, int ctrId)
    {
        std::vector<GuiStruct> vals;
        if (ctrName != ctrN) {
            ctrName = ctrN;
            vals.push_back({1, 0, ctrName, ctrId});
        }
        if (this->date != date) {
            this->date = date;
            vals.push_back({1, 4, date});
        }
        if (this->gold != gold) {
            this->gold = gold;
            vals.push_back({1, 1, ftos(gold)});
        }
        if (income != inc) {
            this->income = inc;
            vals.push_back({1, 3, ftos(income)});
        }
        if (armyMaintenance != armyMain) {
            armyMaintenance = armyMain;
            vals.push_back({2, 3, ftos(armyMaintenance)});
        }
        if (manpower != manp) {
            manpower = manp;
            // vals.push_back({0, 0, itos(manp)});
        }
        if (manpRecovery != manpRec) {
            manpRecovery = manpRec;
            // vals.push_back({0, 0, itos(manpRecovery)});
        }
        if (dateSpeed != dateSp) {
            dateSpeed = dateSp;
            vals.push_back({2, 4, itos(dateSpeed)});
        }
        // if (vals.size())
        //     subject.Notify(vals);
    }
};

struct SideBarData {
    enum class IType { WAR = 0, PEACE_OFFER = 1, PEACE_ACCEPTED = 2, PEACE_DECLINED = 3 };
    struct Element {
        IType type;
        int val;
        std::string hoverText;
    };
    std::vector<Element> elements;
};
