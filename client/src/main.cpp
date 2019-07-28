//#include <memory>
#include <SFML/Network.hpp>
#include "log.h"
#include "graphics/window.h"
#include "graphics/shader.h"

#include "game.h"
#include "asset_manager.h"
#include "settings.h"

void processGuiEvent(GuiClick & click, std::vector<sf::Packet> & toSend);


int main()
{
    //glm::vec2 windowSize = {800, 600};
    glm::vec2 windowSize = {1280, 720};
    //glm::vec2 windowSize = {1366, 768};
    //glm::vec2 windowSize = {1600, 900};
    //glm::vec2 windowSize = {1920, 1080};

    glm::vec2 resolution = windowSize;
    //glm::vec2 resolution = {windowSize.x * 2, windowSize.y * 2};
    //glm::vec2 resolution = {1280, 720};
    //glm::vec2 resolution = {1920, 1080};
    std::string title = "Word";

    Window window(windowSize, title);
    if (glewInit() != GLEW_OK) {
        std::cout << "glew failed\n";
		return -1;
    }

    AM::Init();

    Gui gui{resolution};
    gui.AddWin("src/gui/menu.txt");    

    auto countries = CountryLoader::Load();
    for (auto & c : countries) {
        DataObj * dobj = new DataObj{"label"};
        dobj->objects.push_back(new DataObj{"text"});
        dobj->objects.back()->values["content:"] = c->GetName();
        dobj->objects.back()->values["clickEvent:"] = "setCountry";
        gui.AddToList(dobj, "menu", "countries");
        delete  dobj;
    }

    Shader shader("src/graphics/shaders/vert.v", "src/graphics/shaders/frag.f");
    glUseProgram(shader.GetProgram());

    glm::mat4 matrix = glm::ortho(0.0f, (float)resolution.x, 0.0f, (float)resolution.y);
    glUniformMatrix4fv(glGetUniformLocation(shader.GetProgram(), "matrix"), 1, GL_FALSE, glm::value_ptr(matrix));

    sf::TcpSocket socket;
    sf::Socket::Status status = socket.connect("127.0.0.1", 55555);

    socket.setBlocking(false);
    std::string country = "";
    std::vector<std::string> players;

    Settings settings;
    
    if (status == sf::Socket::Done) {
        sf::Packet pack;
        pack << "witam";
        socket.send(pack);
        pack.clear();
        pack << "name";
        pack << settings.name;
        //pack << "Lukasz";
        socket.send(pack);
    }

    std::vector<sf::Packet> toSend;
    bool play = false;
    
    while (!window.ShouldClose()) {
        window.Refresh();

        sf::Packet packet;
        if (socket.receive(packet) == sf::Socket::Done) {
            std::string messg;
            packet >> messg;
            //if (messg != "Players")
            //Log(messg);
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
                gui.ClearList("menu", "players");
                for (int i = 0; i < playersCount; i++) {
                    packet >> messg;
                    players.emplace_back(messg);
                    DataObj * obj = new DataObj{"label"};
                    obj->objects.push_back(new DataObj{"text"});
                    obj->objects.back()->values["content:"] = messg;
                    packet >> messg;
                    obj->objects.back()->values["content:"] += " " + messg;
                    
                    gui.AddToList(obj, "menu", "players");

                    delete obj;
                }
            }
        }

        if (window.keys['R']) {
            gui.Reload();
            gui.AddWin("src/gui/menu.txt");
        }

        if (window.mouseL && !window.mouseR) {
            auto guiEvent = gui.Click(window.xMouse, windowSize.y - window.yMouse);
            bool wasGuiClicked = guiEvent.values.size();
            if (wasGuiClicked) {
                processGuiEvent(guiEvent, toSend);
            }
        }

        window.mouseL = false;
        window.mouseR = false;
        
        for (auto & pct : toSend) {
            socket.send(pct);
        }

        toSend.clear();

        /*        
                  socket.receive(packet);
                  std::string messg;
                  packet >> messg;
                  if (messg == "Start") {
                  socket.setBlocking(false);
                  Game game(window, socket, country, resolution);
                  game.Play();
                  break;
                  }
        */
        gui.Draw();
        
        window.Update();
    }

    if (play) {
        Game game(window, socket, country, resolution, countries);
        game.Play();
    }

    AM::Dispose();
    //system("./../server/build/Word_and_Sword");
    socket.disconnect();
    
}

void processGuiEvent(GuiClick & click, std::vector<sf::Packet> & toSend)
{
    GuiAid * aid = nullptr;
    std::string evType = click.GetEventName();

    if (evType == "play") {
        aid = new GA_StartGame();
    }
    else if (evType == "setCountry") {
        aid = new GA_SetCountry(click);
    }

    if (aid != nullptr) {
        auto packets = aid->GetPackets();
        //Log(std::to_string(packets.size()));
        toSend.insert(toSend.end(), packets.begin(), packets.end());
        delete aid;
    }
}
