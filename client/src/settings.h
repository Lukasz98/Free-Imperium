#pragma once
#include "log.h"
#include <fstream>
#include "data_object_loader.h"

struct Settings
{
    std::string name;

    Settings()
    {
        std::fstream file{"src/settings.txt"};
        if (file.is_open()) {
            DataLoader dl;
            DataObj * data = dl.LoadDataObj(file, "settings");

            auto d = data->values.find("name:");
            if (d != data->values.end()) {
                name = d->second;
            }
            
            delete data;
            file.close();
        }
        else {
            name = "Lukasz";
        }
    }
};
