#include "process_packet.h"

void ProcessPacket::PeaceDeclined(sf::Packet & packet, Gui & gui)
{
    std::string recipant;
    packet >> recipant;

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
    icon->values["clickEvent:"] = "eraseObj";
    icon->values["size:"] = "50 50";
    icon->values["texturePath:"] = "src/img/peace_declined.png";
        
    DataObj * hoverText = new DataObj{"text"};
    hoverText->values["position:"] = "0.0 0.0 0.1";
    hoverText->values["bgColor:"] = "0 0 0 0";
    hoverText->values["height:"] = "25";
    hoverText->values["content:"] = recipant;
        
    popUpLabel->objects.push_back(hoverText);
    hoverLabel->objects.push_back(popUpLabel);
    obj->objects.push_back(hoverLabel);
    obj->objects.push_back(icon);

    int idInGui = gui.AddToList(obj, "notifications", "notificationsList");
    delete obj;
}
