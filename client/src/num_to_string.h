#pragma once
#include <string>

// namespace convert {

inline std::string itos(int f)
{
    bool negative = false;
    if (f < 0) {
        negative = true;
        f *= -1;
    }

    std::string result;

    if (f >= 1000) {
        int a = f / 1000;
        int b = f % 1000;
        result = std::to_string(a);
        if (b >= 100) {
            result += ".";
            std::string bb = std::to_string(b);
            result += bb.substr(0, 1);
        }
        else {
            result += ".";
            result += "0";
        }
        result += "k";
    }
    else
        result = std::to_string(f);

    if (negative)
        result.insert(0, "-");

    return result;
}

inline std::string ftos(float f)
{
    if (f < 1000.0f && f > -1000.0f) {
        std::string result = std::to_string(f);
        std::size_t dot = result.find('.');
        return result.substr(0, dot + 2);
    }
    else {
        return itos(f);
    }
}
