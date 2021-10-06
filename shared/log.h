#pragma once
#include <iostream>

#define Log(s) std::cout << s << " - " << __FILE__ << " L:" << __LINE__ << "\n";
//#define LogFunkStart std::cout << "START " << __FUNCTION__ << " - " << __FILE__ << " L:" << __LINE__ << '\n'; 
//#define LogFunkEnd std::cout << "END " << __FUNCTION__ << " - " << __FILE__ << " L:" << __LINE__ << '\n'; 
#define LogFunkStart
#define LogFunkEnd
/*
inline void Log(std::string s)
{
    std::cout << s << " - " << __FILE__ << " L:" << __LINE__ << "\n";
}
*/
