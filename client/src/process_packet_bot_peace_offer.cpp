#include "process_packet.h"

void ProcessPacket::BotPeaceOffer(sf::Packet & packet, Gui & gui, std::vector<PeaceOffer> & peaceOffers)
{
    PeaceOffer peaceOffer;
    int lostProvCount, gainProvCount;
    
    packet >> peaceOffer.peaceId;
    packet >> peaceOffer.warId;
    packet >> peaceOffer.offeredBy;
    
    packet >> lostProvCount;
    for (int i = 0; i < lostProvCount; i++) {
        std::string s, ss;
        int d;
        packet >> s;
        packet >> ss;
        packet >> d;
        peaceOffer.lostProv.emplace_back(std::make_tuple(s, ss, d));
    }

    packet >> gainProvCount;
    for (int i = 0; i < gainProvCount; i++) {
        std::string s, ss;
        int d;
        packet >> s;
        packet >> ss;
        packet >> d;
        peaceOffer.gainProv.emplace_back(std::make_tuple(s, ss, d));
    }

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
    icon->values["clickEvent:"] = "botPeaceOffer";
    icon->values["size:"] = "50 50";
    icon->values["texturePath:"] = "src/img/peace.png";
    icon->values["impact:"] = std::to_string(peaceOffer.peaceId);
    //Log(peaceOffer.peaceId);
    //Log(std::to_string(peaceOffer.peaceId));
    //icon->values["valueName:"] = "warId";
        
    DataObj * hoverText = new DataObj{"text"};
    hoverText->values["position:"] = "0.0 0.0 0.1";
    hoverText->values["bgColor:"] = "0 0 0 0";
    //hoverText->values["valueName:"] = "countryName";
    hoverText->values["height:"] = "25";
    hoverText->values["content:"] = peaceOffer.offeredBy;
    icon->values["content:"] = peaceOffer.offeredBy;
        
    popUpLabel->objects.push_back(hoverText);
    hoverLabel->objects.push_back(popUpLabel);
    obj->objects.push_back(hoverLabel);
    obj->objects.push_back(icon);
        
    int idInGui = gui.AddToList(obj, "notifications", "notificationsList");

    peaceOffer.idInGui = idInGui;
    peaceOffers.push_back(peaceOffer);

    delete obj;

    
}
