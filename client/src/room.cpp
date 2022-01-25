#include "room.h"

Room::Room(Window* window, GameData* gd)
    : window(window), gd(gd)
{
}

Room::~Room() {}

void Room::Play(bool localhost)
{
    if (localhost)
        status = socket.connect(gd->settings.localhost, gd->settings.localPort);
    else
        status = socket.connect(gd->settings.friendIp, gd->settings.friendPort);

    socket.setBlocking(false);
    std::string country = "";

    if (status == sf::Socket::Done) {
        sf::Packet pack;
        pack << "witam";
        socket.send(pack);
        pack.clear();
        pack << "name";
        pack << gd->settings.name;
        socket.send(pack);
    }

    bool play = false;
    loop(play, country);


    if (play) {
        Game game(window, socket, country, gd);
        game.Play();
    }
}

void Room::loop(bool& play, std::string& country)
{
    guiLast.init(window, gd->settings.resolution, window->GetSize());
    std::vector<sf::Packet> toSend;
    std::vector<std::string> plarr;
    std::vector<std::string> ctrarr;


    // ojojoj atlantyda bedzie 2 razy w tej tablicy
    ctrarr.push_back("Atlantyda");
    for (auto& c : gd->countries) {
        if (c.GetName() == "Atlantyda")
            continue;
        ctrarr.push_back(c.GetName());
    }

    while (!window->ShouldClose()) {
        window->Refresh();

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
                plarr.clear();
                for (int i = 0; i < playersCount; i++) {
                    packet >> messg;
                    std::string text = messg;
                    packet >> messg;
                    text += " " + messg;
                    plarr.push_back(text);
                }
            }
        }

        // gui draw & clicks
        guiLast.start();
        glm::vec2 mp{window->xMouse * gd->settings.resolution.x / window->GetSize().x,
                     (window->GetSize().y - window->yMouse) * gd->settings.resolution.y / window->GetSize().y};

        GuiLast::GuiEv ctype;
        GuiLast::GuiEv tmpctype;
        tmpctype = guiLast.room_playerList(plarr, mp.x, mp.y, window->scrollOffset);
        if (tmpctype.ct != ClickEventType::MISS)
            ctype = tmpctype;
        
        tmpctype = guiLast.room_countryList(ctrarr, mp.x, mp.y, window->scrollOffset);
        if (tmpctype.ct != ClickEventType::MISS)
            ctype = tmpctype;
        
        tmpctype = guiLast.room_startButton(mp.x, mp.y);
        if (tmpctype.ct != ClickEventType::MISS)
            ctype = tmpctype;

        guiLast.flush();
        if (window->mouseLClicked) {
            std::vector<sf::Packet> packets;
            switch (ctype.ct) {
                case ClickEventType::ROOM_PICK_COUNTRY: {
                    auto ctrName = ctrarr[ctype.val];
                    sf::Packet packet;
                    packet << "country"; 
                    packet << ctrName; 
                    toSend.emplace_back(packet); 
                    break;
                }
                case ClickEventType::ROOM_START_GAME: {
                    sf::Packet packet;
                    packet << "Start";
                    toSend.emplace_back(packet);
                    break;
                }
            }
            if (packets.size())
                toSend.insert(toSend.end(), packets.begin(), packets.end());
        }
        window->mouseLClicked = false;
        window->mouseRClicked = false;
        window->scrollOffset = 0;

        for (auto& pct : toSend) {
            socket.send(pct);
        }

        toSend.clear();
        window->Update();

        if (play) {
            break;
        }
    }
}

