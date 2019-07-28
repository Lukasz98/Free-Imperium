#pragma once
#include "log.h"
#include <fstream>
#include <glm/vec2.hpp>
#include "data_object_loader.h"


struct Settings
{
    std::string name = "Lukasz";
    std::string localhost = "127.0.0.1";
    std::string friendIp = "127.0.0.1";
    int localPort = 55555;
    int friendPort = 55555;
    glm::vec2 resolution;
    
    Settings()
    {
        resolution = { 1280, 720 };
        std::fstream file{"src/settings.txt"};
        if (!file.is_open())
            return;

        DataLoader dl;
        DataObj * data = dl.LoadDataObj(file, "settings");

        auto d = data->values.find("name:");
        if (d != data->values.end()) {
            name = d->second;
        }

        d = data->values.find("friendIp:");
        if (d != data->values.end()) {
            friendIp = d->second;
        }

        d = data->values.find("friendPort:");
        if (d != data->values.end()) {
            friendPort = std::stoi(d->second);
        }
        
        delete data;
        file.close();
    }
};
