#include "load_values_from_txt.h"


int loadInt2(char * ptr) // assumes pointer is continuation of strtok
{
    int v;
    try {
        v = std::stoi(ptr);
    } catch (std::exception& e) {
  //      Log(lineCount);
        Log(ptr);// << "; Line: " << lineCount);
        Log(e.what());// << "; Line: " << lineCount);
    }
    return v;
}
int loadInt(char * ptr) // assumes pointer is continuation of strtok
{
    int v;
    try {
        ptr = strtok(NULL, " ");
        v = std::stoi(ptr);
    } catch (std::exception& e) {
  //      Log(lineCount);
        Log(e.what());// << "; Line: " << lineCount);
    }
    return v;
}
float loadFloat(char * ptr) // assumes pointer is continuation of strtok
{
    float v;
    try {
        ptr = strtok(NULL, " ");
        v = std::stof(ptr);
    } catch (std::exception& e) {
        Log(ptr);// << "; Line: " << lineCount);
        Log(e.what());// << "; Line: " << lineCount);
    }
    return v;
}

Vec3 loadVec3(char * ptr) // assumes pointer is continuation of strtok
{
    Vec3 v; 
    v.x = loadFloat(ptr);
    v.y = loadFloat(ptr);
    v.z = loadFloat(ptr);
    //ptr = strtok(NULL, " ");
    //v.x = loadFloat(ptrstd::stof(ptr);
    //ptr = strtok(NULL, " ");
    //v.y = std::stof(ptr);
    //ptr = strtok(NULL, " ");
    //v.z = std::stof(ptr);
    return v;
}
Vec4 loadVec4(char * ptr) // assumes pointer is continuation of strtok
{
    Vec4 v; 
    v.x = loadFloat(ptr);
    v.y = loadFloat(ptr);
    v.z = loadFloat(ptr);
    v.w = loadFloat(ptr);
    //ptr = strtok(NULL, " ");
    //v.x = std::stof(ptr);
    //ptr = strtok(NULL, " ");
    //v.y = std::stof(ptr);
    //ptr = strtok(NULL, " ");
    //v.z = std::stof(ptr);
    //ptr = strtok(NULL, " ");
    //v.w = std::stof(ptr);
    return v;
}
Vec2 loadVec2(char * ptr) // assumes pointer is continuation of strtok
{
    Vec2 v; 
    v.x = loadFloat(ptr);
    v.y = loadFloat(ptr);
    //ptr = strtok(NULL, " ");
    //v.x = std::stof(ptr);
    //ptr = strtok(NULL, " ");
    //v.y = std::stof(ptr);
    return v;
}



