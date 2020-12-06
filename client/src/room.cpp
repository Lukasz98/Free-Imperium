#include "room.h"

Room::Room(Window & window, Settings & s, std::vector<std::shared_ptr<Country>> & countries)
    : Scene(window, s.resolution), settings(s), countries(countries)
{
    //gui.AddWin("src/gui/room.txt");
    for (auto & c : countries) {
        DataObj * dobj = new DataObj{"label"};
        dobj->objects.push_back(new DataObj{"text"});
        dobj->objects.back()->values["content:"] = c->GetName();
        dobj->objects.back()->values["clickEvent:"] = "setCountry";
//        gui.AddToList(dobj, "room", "countries");
        delete  dobj;
    }
}

Room::~Room()
{
    
}

void Room::Play(bool localhost)
{
    if (localhost)
        status = socket.connect(settings.localhost, settings.localPort);
    else
        status = socket.connect(settings.friendIp, settings.friendPort);
        
    socket.setBlocking(false);
    std::string country = "";

    if (status == sf::Socket::Done) {
        sf::Packet pack;
        pack << "witam";
        socket.send(pack);
        pack.clear();
        pack << "name";
        pack << settings.name;
        socket.send(pack);
    }

    bool play = false;
    loop(play, country);
    if (play) {
        Game game(window, socket, country, settings.resolution, countries);
        game.Play();
    }
}

void Room::loop(bool & play, std::string & country)
{
    //std::vector<std::string> players;
    std::vector<sf::Packet> toSend;
    
    while (!window.ShouldClose()) {
        window.Refresh();

        sf::Packet packet;
        if (socket.receive(packet) == sf::Socket::Done) {
            std::string messg;
            packet >> messg;
            if (messg == "Start") {
                Log(messg);
                play = true;
                break;
            }
            else if (messg == "Country") {
                packet >> messg;
                country = messg;
            }
            else if (messg == "Players") {
                int playersCount = 0;
                packet >> playersCount;
  //              gui.ClearList("room", "players");
                for (int i = 0; i < playersCount; i++) {
                    packet >> messg;
                    //players.emplace_back(messg);
                    DataObj * obj = new DataObj{"label"};
                    obj->objects.push_back(new DataObj{"text"});
                    obj->objects.back()->values["content:"] = messg;
                    packet >> messg;
                    obj->objects.back()->values["content:"] += " " + messg;
                    
    //                gui.AddToList(obj, "room", "players");

                    delete obj;
                }
            }
        }

        if (window.keys['R']) {
            //gui.Reload();
            //gui.AddWin("src/gui/room.txt");
        }

        if (window.mouseL && !window.mouseR) {
      //      //auto guiEvent = gui.Click(window.xMouse, window.GetSize().y - window.yMouse);
        //    auto guiEvent = gui.Click(window.xMouse *resolution.x/window.GetSize().x, (window.GetSize().y - window.yMouse) * resolution.y/window.GetSize().y);
        //    bool wasGuiClicked = guiEvent.values.size();
        //    if (wasGuiClicked) {
          //      processGuiEvent(guiEvent, toSend, play);
          //  }
        }

        window.mouseL = false;
        window.mouseR = false;
        
        for (auto & pct : toSend) {
            socket.send(pct);
        }

        toSend.clear();
        DrawGui();
        //gui.Draw();        
        //window.Update();
    }
}

void Room::processGuiEvent(std::vector<sf::Packet> & toSend, bool & play)
{
    std::vector<sf::Packet> packets;
    //std::string evType = click.GetEventName();

    //if (evType == "play") {
     //   GuiAid::StartGame(packets);
    //}
   // else if (evType == "setCountry") {
   //     GuiAid::SetCountry(click, packets);
    //}

    if (packets.size())
        toSend.insert(toSend.end(), packets.begin(), packets.end());
}
