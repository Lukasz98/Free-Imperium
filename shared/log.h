#pragma once
#include <iostream>

#define Log(s) std::cout << s << " - " << __FILE__ << " L:" << __LINE__ << "\n";

/*
inline void Log(std::string s)
{
    std::cout << s << " - " << __FILE__ << " L:" << __LINE__ << "\n";
}
*/
