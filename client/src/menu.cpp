#include "menu.h"

Menu::Menu(Window & window, Settings & s, std::vector<std::shared_ptr<Country>> & countries)
    : Scene(window, s.resolution), settings(s), countries(countries)
{
    gui.AddWin("src/gui/menu.txt");
}

Menu::~Menu()
{
    
}

void Menu::Play()
{
    loop();
    if (localGame || guestGame) {
        Room room(window, settings, countries);
        room.Play(localGame);
    }
    else if (quit) {
    }
}

void Menu::loop()
{
    while (!window.ShouldClose()) {
        window.Refresh();

        if (window.keys['R']) {
            gui.Reload();
            gui.AddWin("src/gui/menu.txt");
        }

        if (window.mouseL && !window.mouseR) {
            //auto guiEvent = gui.Click(window.xMouse, window.GetSize().y - window.yMouse);
 std::cout << window.xMouse * resolution.x / window.GetSize().x << " "<<( window.GetSize().y - window.yMouse) * resolution.y / window.GetSize().y << '\n';           
auto guiEvent = gui.Click(window.xMouse * resolution.x / window.GetSize().x,( window.GetSize().y - window.yMouse) * resolution.y / window.GetSize().y);
            bool wasGuiClicked = guiEvent.values.size();
            if (wasGuiClicked) {
                processGuiEvent(guiEvent);
                if (localGame || guestGame || quit)
                    break;
            }
        }

        window.mouseL = false;
        window.mouseR = false;

        DrawGui();
        //gui.Draw();        
        //window.Update();
    }
}

void Menu::processGuiEvent(GuiClick & click)
{
    //GuiAid * aid = nullptr;
    std::string evType = click.GetEventName();

    if (evType == "localGame") {
        localGame = true;
        //aid = new GA_StartGame();
    }
    else if (evType == "guestGame") {
        guestGame = true;
        //aid = new GA_SetCountry(click);
    }
    else if (evType == "quit") {
        quit = true;
    }
    /*
    if (aid != nullptr) {
        auto packets = aid->GetPackets();
        toSend.insert(toSend.end(), packets.begin(), packets.end());
        delete aid;
    }
    */
}
