#pragma once
#include <unordered_map>

struct GuiClick
{
    int parentId = 0;
    std::unordered_map<std::string,std::string> values;

    std::string GetEventName() const
    {
        auto valIt = values.find("name:");
        if (valIt != values.end())
            return valIt->second;
        return "";
    }
};


