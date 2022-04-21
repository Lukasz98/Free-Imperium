#pragma once

#include <cstring>
#include <fstream>
#include <string>

#include "log.h"
#include "vec.h"

int loadInt2(char* ptr);     // assumes pointer is continuation of strtok
int loadInt(char* ptr);      // assumes pointer is continuation of strtok
float loadFloat(char* ptr);  // assumes pointer is continuation of strtok

Vec3 loadVec3(char* ptr);  // assumes pointer is continuation of strtok
Vec4 loadVec4(char* ptr);  // assumes pointer is continuation of strtok
Vec2 loadVec2(char* ptr);  // assumes pointer is continuation of strtok

