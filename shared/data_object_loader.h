#pragma once
#include <fstream>
#include "data_object.h"

class DataLoader
{
public:
    DataObj * LoadDataObj(std::fstream & file, const std::string & mainObjName, std::string objectName = "", int iteration = 0)
    {
        DataObj * object = nullptr;
        if (objectName != mainObjName) {
            if (objectName == "")
                object = new DataObj(mainObjName);
            else
                object = new DataObj(objectName);
        }

        std::string word = "";
        std::string valueName = objectName, value = "";
        int braceCount = 0;

        if (objectName != mainObjName)
            braceCount++;

        bool inValue = false;
        
        while (!file.eof()) {
            file >> word;
            if (word == "{") {
                braceCount++;

                if (braceCount > 2 || iteration > 0) {
                    iteration++;
                    DataObj * newObj = LoadDataObj(file, mainObjName, valueName, iteration);
                    if (object != nullptr)
                        object->objects.push_back(newObj);
                    else
                        delete newObj;
                    braceCount--;
                }
                else {
                    if (valueName == mainObjName) {
                        if (object != nullptr)
                            delete object;
                        object = new DataObj(valueName);
                    }
                }
            }
            else if (word == "}")
                braceCount--;
            else if (word == "'") {
                inValue = !inValue;

                if (!inValue && object != nullptr) {
                    object->values[valueName] = value;
                }
            }
            else if (inValue) {
                if (value.size())
                    value += " " + word;
                else
                    value += word;
            }
            else {
                value = "";
                valueName = word;
            }

            if (braceCount == 0 && word != mainObjName)
                break;
        }
    
        return object;
    }
};
