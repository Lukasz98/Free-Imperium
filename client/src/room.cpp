#include "room.h"

Room::Room(Window & window, Settings & s, std::vector<std::shared_ptr<Country>> & countries)
    : Scene(window, s.resolution), settings(s), countries(countries)
{
    Gui::Room::Open(resolution);
//gui.AddWin("src/gui/room.txt");
    for (auto & c : countries) {
        Gui::Room::AddCountryToList(c->GetName());
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
    
    Gui::Room::Close();

    if (play) {
        Game game(window, socket, country, settings.resolution, countries);
        game.Play();
    }
}

void Room::loop(bool & play, std::string & country)
{
    //std::vector<std::string> players;
    std::vector<std::string> playersList;
    std::vector<sf::Packet> toSend;
    GuiLast::Gui gui;
    auto winSize = window.GetSize();
    gui.init(&window, resolution, winSize);
    double time = 0.0;
    while (!window.ShouldClose()) {
        glEnable(GL_DEPTH_TEST);  // Enable depth testing for z-culling
        glDepthFunc(GL_LESS);
        glEnable(GL_SCISSOR_TEST);
        glScissor(0, 0, winSize.x, winSize.y);
        window.Refresh();
        glm::vec2 mousePos{(float)window.xMouse * resolution.x / winSize.x,
                           (float)(winSize.y - window.yMouse) * (resolution.y / winSize.y)};

        if (window.keys['M']) {
            gui.reloadShader();
        }

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
                //Gui::Room::ClearPlayersList();
                playersList.clear();
                for (int i = 0; i < playersCount; i++) {
                    packet >> messg;
                    std::string text = messg;
                    packet >> messg;
                    text += " " + messg;
                    playersList.emplace_back(text);
                    //Gui::Room::AddPlayerToList(text);
                }
            }
        }

        gui.start();
        //gui.atchBegin();
        // batch.Begin();
        //glm::vec3 pos{10.0f, 135.0f, 0.0f};
        //glm::vec2 size{100.0f, 200.0f};

        //drawRect(pos, size, brownCol);
        //glm::vec3 tpos{10.0f, 10.0f, 0.1f};
        //g/lm::vec2 tsize{100.0f, 200.0f};

        //drawText(tpos, tsize, greenCol, "Lukasz Kadracki", TEXT_CENTER);

        //doTestList(window->scrollOffset, mousePos.x, mousePos.y, dt, window->mouseL, window->mouseLClicked);
        gui.room_playerListDraw(playersList);
        window.scrollOffset = 0;
        window.mouseLClicked = false;
        //drawLists();

        gui.flush();
        // batch.Flush();

        window.Update();
        dt = glfwGetTime() - time;
        time = glfwGetTime();
    }

/*
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
                Gui::Room::ClearPlayersList();
                for (int i = 0; i < playersCount; i++) {
                    packet >> messg;
                    std::string text = messg;
                    packet >> messg;
                    text += " " + messg;
                    Gui::Room::AddPlayerToList(text);
                }
            }
        }


        if (window.mouseLClicked && !window.mouseRClicked) {
            ClickEventType cType = Gui::Base::Click(glm::vec2{window.xMouse * resolution.x / window.GetSize().x, 
                                                   (window.GetSize().y - window.yMouse) * resolution.y / window.GetSize().y});
            if (cType == ClickEventType::PICK_COUNTRY) {
                auto ctrName = Gui::Room::GetPickedCountry();
                sf::Packet packet;
                packet << "country";
                packet << ctrName;
                toSend.emplace_back(packet);
                Gui::Base::ResetClick();
            }
            else if (cType == ClickEventType::START_GAME) {
                sf::Packet packet;
                packet << "Start";
                toSend.emplace_back(packet);
                Gui::Base::ResetClick();
            }
            //else if (cdata.evType == Gui::ClickEventType::DEL_FROM_UNITS_LIST) {
            //      Gui::DelFromUnitsList(cdata);
            //}
            //else if (cdata.evType != Gui::ClickEventType::NONE) {
            //    Log("gui click");
            //}
        }

        window.mouseLClicked = false;
        window.mouseRClicked = false;
        if (window.mouseL) {
            Gui::Base::Drag(glm::vec2{window.xMouse * resolution.x / window.GetSize().x, 
                                (window.GetSize().y - window.yMouse) * resolution.y / window.GetSize().y}, dt);
        }
        if (window.mouseR) {
        }
        if (window.scrollOffset) {
            Gui::Base::Scroll(window.scrollOffset, glm::vec2{window.xMouse * resolution.x / window.GetSize().x, 
                        (window.GetSize().y - window.yMouse) * resolution.y / window.GetSize().y});
            window.scrollOffset = 0;
        }

        Gui::Base::Hover(glm::vec2{window.xMouse * resolution.x / window.GetSize().x, 
                             (window.GetSize().y - window.yMouse) * resolution.y / window.GetSize().y}); 
 


        for (auto & pct : toSend) {
            socket.send(pct);
        }

        toSend.clear();
        Gui::Base::Draw();
        DrawGui();
        
        if (play) {
            break;
        }
    }
    */
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
