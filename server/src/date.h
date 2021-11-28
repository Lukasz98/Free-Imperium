#pragma once
#include "log.h"

struct Date
{
    int hour = 0;
    int day = 26;
    int month = 1;
    int year = 950;

    bool operator>=(const Date & d) const
    {
        if (year > d.year)
            return true;
        if (year < d.year)
            return false;
        
        if (month > d.month)
            return true;
        if (month < d.month)
            return false;
        
        if (day > d.day)
            return true;
        if (day < d.day)
            return false;

        if (hour > d.hour)
            return true;
        if (hour < d.hour)
            return false;

        return false;
    }
};

class TimeManager : public Date
{
    int speed = 5;
    double time = 0.0;
    bool pause = false;
    
    void increaseDate()
    {
        if (hour < 24) { /*std::cout << hour << "\n";*/ hour++; }
        else if (day < 30) { hour = 0; day++; }
        else if (month < 12) { hour = 0; day = 1; month++; }
        else { hour = 0; day = 1; month = 1; year++; }
    }

    std::string translateM(int m)
    {
        if (m == 1) return "Jan";
        if (m == 2) return "Feb";
        if (m == 3) return "Mar";
        if (m == 4) return "Apr";
        if (m == 5) return "May";
        if (m == 6) return "Jun";
        if (m == 7) return "Jul";
        if (m == 8) return "Aug";
        if (m == 9) return "Sep";
        if (m == 10) return "Oct";
        if (m == 11) return "Nov";
        else return "Dec";
    }
    
public:
    TimeManager()
    {
        
    }
    ~TimeManager() {}

    void Update(double dt)
    {
        if (pause)
            return;
        
        time += dt;
        double defaultDayLength = 3.0 / 24.0;
        switch (speed)
        {
            case 1:
                if (time > defaultDayLength / 1) { increaseDate(); time = 0.0; }
                break;
            case 2:
                if (time > defaultDayLength / 2) { increaseDate(); time = 0.0; }
                break;
            case 3:
                if (time > defaultDayLength / 3) { increaseDate(); time = 0.0; }
                break;
            case 4:
                if (time > defaultDayLength / 4) { increaseDate(); time = 0.0; }
                break;
            case 5:
                if (time > defaultDayLength / 15) { increaseDate(); time = 0.0; }
                break;
        }
    }

    void Speed(int val)
    {
        speed = val;
    }

    std::string GetDate()
    {
        //std::cout << "date = " << std::to_string(day) << "\n";
        //std::cout << "date = " << std::to_string(month) << "\n";
        //std::cout << "date = " << std::to_string(year) << "\n";
        std::string result = std::to_string(day) + " " + translateM(month) + " " + std::to_string(year);
        return result;
    }  

    int GetMonth() { return month; }
    int GetDay() { return day; }
    int GetHour() { return hour; }
    int GetSpeed() { return speed; }
    bool IsPaused() { return pause; }
    void SetPause(bool p) { pause = p; }
};
