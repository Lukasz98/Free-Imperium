#pragma once
//#include <memory>
#include <vector>
#include <unordered_map>

struct DataObj
{
    std::string type;
    std::vector<DataObj*> objects;
    std::unordered_map<std::string, std::string> values;

    DataObj(std::string t) : type(t)
    {
        //std::cout << "Obj() " << type << "\n";
    }
    DataObj(const DataObj &) = delete;
    ~DataObj()
    {
        
        //std::cout << "~Obj() " << type << "\n";
        for (auto & o : objects)
            delete o;
    }

    virtual void Print(int indentCount = 0)
    {
        std::string indent = "";
        for (int i = 0; i < indentCount; i++)
            indent += "    ";

        std::cout << "\n" << indent  << "Type: " << type << "\n";

        indent += "    ";
        
        for (auto & pair : values) {
            std::cout << indent << pair.first << " " << pair.second << "\n";
        }
        for (auto & object : objects) {
            object->Print(indentCount + 1);
        }
    }

};

