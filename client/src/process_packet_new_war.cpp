#include "process_packet.h"


void ProcessPacket::NewWar(sf::Packet & packet, Gui & gui, std::vector<War> & wars, const std::string myCountry)
{
    int id;
    std::string attacker, defender;

    packet >> id;
    packet >> attacker;
    packet >> defender;

    if (myCountry == attacker || myCountry == defender) {
        DataObj * obj = new DataObj{"label"};
        obj->values["position:"] = "0.0 0.0 0.1";
        obj->values["bgColor:"] = "0 0 0 0";
        obj->values["size:"] = "50 50";
        
        DataObj * hoverLabel = new DataObj{"label"};
        hoverLabel->values["position:"] = "0.0 0.0 -0.1";
        hoverLabel->values["bgColor:"] = "0 0 0 0";
        hoverLabel->values["size:"] = "50 50";
        hoverLabel->values["hover:"] = "true";
        
        DataObj * popUpLabel = new DataObj{"label"};
        popUpLabel->values["position:"] = "-200.0 3.0 0.1";
        popUpLabel->values["bgColor:"] = "50 15 15 200";
        popUpLabel->values["size:"] = "205 44";
        popUpLabel->values["contentAlign:"] = "center";

        DataObj * icon = new DataObj{"icon"};
        icon->values["position:"] = "0.0 0.0 0.1";
        icon->values["clickEvent:"] = "openWar";
        icon->values["size:"] = "50 50";
        icon->values["texturePath:"] = "src/img/war.png";
        icon->values["impact:"] = std::to_string(id);
        //icon->values["valueName:"] = "warId";
        
        DataObj * hoverText = new DataObj{"text"};
        hoverText->values["position:"] = "0.0 0.0 0.1";
        hoverText->values["bgColor:"] = "0 0 0 0";
        //hoverText->values["valueName:"] = "countryName";
        hoverText->values["height:"] = "25";
        if (attacker != myCountry) {
            hoverText->values["content:"] = attacker;
            icon->values["content:"] = attacker;
        }
        else {
            hoverText->values["content:"] = defender;
            icon->values["content:"] = defender;
        }
        
        popUpLabel->objects.push_back(hoverText);
        hoverLabel->objects.push_back(popUpLabel);
        obj->objects.push_back(hoverLabel);
        obj->objects.push_back(icon);
        
        int idInGui = gui.AddToList(obj, "notifications", "notificationsList");
        delete obj;

        War war{id, idInGui};
        war.AddAttacker(attacker);
        war.AddDefender(defender);
        wars.emplace_back(war);

    }
}
