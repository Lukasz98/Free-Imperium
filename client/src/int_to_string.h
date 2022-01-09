#pragma once

inline std::string int_to_string(int val)
{
    int aa = val % 1000;
    int bb = val / 1000;
    int cc = aa / 100;
    if (bb >= 1) {
        return std::to_string(bb) + "." + std::to_string(cc) + "k";
    }
    else {
        return std::to_string(val);
    }
}
