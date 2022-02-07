#include "room.h"

Room::Room(GameData* gd) : gd(gd) {}

Room::~Room() {}

void Room::Play(bool localhost)
{
    if (localhost)
        status = socket.connect(gd->settings.localhost, gd->settings.localPort);
    else
        status = socket.connect(gd->settings.friendIp, gd->settings.friendPort);

    socket.setBlocking(false);

    if (status == sf::Socket::Done) {
        sf::Packet pack;
        pack << "witam";
        socket.send(pack);
        pack.clear();
        pack << "name";
        pack << gd->settings.name;
        socket.send(pack);
    }

    loop();

    if (play) {
        Game game(socket, myCountryName, gd);
        game.Play();
    }
}

void Room::receivePackets()
{
    sf::Packet packet;
    if (socket.receive(packet) == sf::Socket::Done) {
        std::string messg;
        packet >> messg;
        if (messg == "Start") {
            Log(messg);
            play = true;
            // break;
        }
        else if (messg == "Country") {
            packet >> messg;
            myCountryName = messg;
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
}

void Room::guiDraw()
{
    ctype.ct = ClickEventType::MISS;
    guiLast.start();
    glm::vec2 mp{
        gd->window->xMouse * gd->settings.resolution.x / gd->window->GetSize().x,
        (gd->window->GetSize().y - gd->window->yMouse) * gd->settings.resolution.y / gd->window->GetSize().y};

    GuiLast::GuiEv tmpctype;
    tmpctype = guiLast.room_playerList(gd->settings.name, plarr, mp.x, mp.y, gd->window->scrollOffset);
    if (tmpctype.ct != ClickEventType::MISS)
        ctype = tmpctype;

    // tmpctype = guiLast.room_countryList(ctrarr, mp.x, mp.y, gd->window->scrollOffset);
    // if (tmpctype.ct != ClickEventType::MISS)
    //     ctype = tmpctype;

    tmpctype = guiLast.room_startButton(mp.x, mp.y);
    if (tmpctype.ct != ClickEventType::MISS)
        ctype = tmpctype;

    guiLast.flush();
    if (gd->window->mouseLClicked) {
        std::vector<sf::Packet> packets;
        switch (ctype.ct) {
            case ClickEventType::START_AS_SPECTATOR: {
                sf::Packet packet;
                packet << "country";
                packet << "Atlantyda";
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

    if (ctype.ct == ClickEventType::NONE || ctype.ct == ClickEventType::MISS)
        glfwSetCursor(gd->window->window, AM::am.def_cursor);
    else
        glfwSetCursor(gd->window->window, AM::am.hover_cursor);
}

void Room::input()
{
    cameraMovement(gd, dt);
    if (gd->window->scrollOffset && ctype.ct == ClickEventType::MISS) {
        gd->camera.Scroll(gd->window->scrollOffset);
    }

    if (gd->window->mouseLClicked && ctype.ct == ClickEventType::MISS) {
        glm::vec2 mp{
            gd->window->xMouse * gd->settings.resolution.x / gd->window->GetSize().x,
            (gd->window->GetSize().y - gd->window->yMouse) * gd->settings.resolution.y / gd->window->GetSize().y};
        int id = (float)clickedProvId(gd, mp);
        if (id >= 0 && id < gd->provinces.size()) {
            sf::Packet packet;
            packet << "country";
            packet << gd->countries[gd->provinces[id].countryId].GetName();
            toSend.emplace_back(packet);
            markedCountryId = gd->provinces[id].countryId;

            gd->map->markedCtrPix.resize(gd->provinces.size() * 4);
            std::memset(&gd->map->markedCtrPix[0], 0, gd->map->markedCtrPix.size());
            Color col{200, 200, 200, 255};
            for (int i = 0; i < gd->ctrProvs[markedCountryId].size(); ++i) {
                int provid = gd->ctrProvs[markedCountryId][i];
                gd->map->markedCtrPix[provid * 4] = col.r;
                gd->map->markedCtrPix[provid * 4 + 1] = col.g;
                gd->map->markedCtrPix[provid * 4 + 2] = col.b;
                gd->map->markedCtrPix[provid * 4 + 3] = col.a;
            }
            if (gd->map->markedCtrText != nullptr)
                delete gd->map->markedCtrText;
            gd->map->markedCtrText = new Texture(&gd->map->markedCtrPix[0], gd->provinces.size(), 1);
        }
    }
}

void Room::loop()
{
    guiLast.init(gd->window, gd->settings.resolution, gd->window->GetSize());

    // ojojoj atlantyda bedzie 2 razy w tej tablicy
    ctrarr.push_back("Atlantyda");
    for (auto& c : gd->countries) {
        if (c.GetName() == "Atlantyda")
            continue;
        ctrarr.push_back(c.GetName());
    }

    float time = 0.0f;
    while (!gd->window->ShouldClose() && play == false) {
        gd->window->Refresh();

        glEnable(GL_DEPTH_TEST);  // Enable depth testing for z-culling

        receivePackets();
        input();

        // map
        glm::mat4x4 matrix = gd->camera.GetMat();
        gd->map->ActivateTextures();
        gd->map->DrawWater(matrix, gd->camera.eye);
        int markedProvId = -1;
        gd->map->DrawLand(matrix, gd->camera.eye, markedProvId, gd->provinces.size(), gd->map->MAPID_COUNTRY,
                          gd->waterTime);
        gd->map->DrawBorders(matrix);
        checkCountryNamesFade(gd, dt);
        if (((gd->camera.eye.z > gd->zPoint) || gd->ctrNamesFade < 1.0f)) {
            drawCountryNames(gd);
        }
        updateCountryNamesFade(gd, dt);

        GLuint ts[] = {0};
        glBindTextures(ts[0], 1, ts);
        glActiveTexture(GL_TEXTURE0);
        //~map

        // gui draw & clicks
        guiDraw();

        gd->window->mouseLClicked = false;
        gd->window->mouseRClicked = false;
        gd->window->scrollOffset = 0;

        for (auto& pct : toSend) {
            socket.send(pct);
        }

        toSend.clear();
        gd->window->Update();

        dt = glfwGetTime() - time;
        gd->updateTime(dt);
        time = glfwGetTime();

        if (play) {
            break;
        }
    }
}

