#pragma once
#include "date.h"

struct Truce : public Date
{
    Truce() {}
    Truce(const Date & d)
    {
        hour = d.hour;
        month = d.month;
        day = d.day;
        year = d.year;
    }
            
    std::string enemy;
    bool operator>=(const Truce & t) const
    {
        return (enemy == t.enemy) && Date::operator>=(t);
    }
    
    bool operator==(const Truce & d) const
    {
        if (enemy != d.enemy)
            return false;
        if (hour != d.hour)
            return false;
        if (day != d.day)
            return false;
        if (month != d.month)
            return false;
        if (year != d.year)
            return false;
        
        return true;
    }
};
