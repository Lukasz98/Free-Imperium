#include "menu.h"

Menu::Menu(Window & window, Settings & s, std::vector<std::shared_ptr<Country>> & countries)
    : Scene(window, s.resolution), settings(s), countries(countries)
{
    //gui.AddWin("src/gui/menu.txt");
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
    Log("tu");


std::vector<std::string> vals;
vals.emplace_back("Polska");
Gui::OpenTopBar(vals, settings.resolution);
Gui::OpenUnitsList();

//Rectangle atlasRect{{0.0, 0.0, 0.1}, {AM::widthSum, AM::maxRows}};

while (!window.ShouldClose()) {
        window.Refresh();

        if (window.keys['R']) {
            //gui.Reload();
            //gui.AddWin("src/gui/menu.txt");
        }

        if (window.mouseL && !window.mouseR) {
            //auto guiEvent = gui.Click(window.xMouse, window.GetSize().y - window.yMouse);
Gui::ClickData cdata = Gui::Click(
glm::vec2{window.xMouse * resolution.x / window.GetSize().x, ( window.GetSize().y - window.yMouse) * resolution.y / window.GetSize().y});
if (cdata.evType != Gui::ClickEventType::NONE) {
    Log("gui click");
}
std::cout << window.xMouse * resolution.x / window.GetSize().x << " "<<( window.GetSize().y - window.yMouse) * resolution.y / window.GetSize().y << '\n';           
//auto guiEvent = gui.Click(window.xMouse * resolution.x / window.GetSize().x,( window.GetSize().y - window.yMouse) * resolution.y / window.GetSize().y);
            
/*
            bool wasGuiClicked = guiEvent.values.size();
            if (wasGuiClicked) {
                processGuiEvent(guiEvent);
                if (localGame || guestGame || quit)
                    break;
            }
  */
        }

        window.mouseL = false;
        window.mouseR = false;
if (window.scrollOffset) {
    Gui::Scroll(window.scrollOffset, glm::vec2{window.xMouse * resolution.x / window.GetSize().x, ( window.GetSize().y - window.yMouse) * resolution.y / window.GetSize().y});
    window.scrollOffset = 0;
}


  //      AM::atlasTexture->Bind();
    //    atlasRect.Draw();
      //  AM::atlasTexture->Unbind();

Gui::Hover(glm::vec2{window.xMouse * resolution.x / window.GetSize().x, ( window.GetSize().y - window.yMouse) * resolution.y / window.GetSize().y}); 
Gui::Draw();
        DrawGui();
    }
}

void Menu::processGuiEvent()
{
 /*   //GuiAid * aid = nullptr;
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
   */
   /*
    if (aid != nullptr) {
        auto packets = aid->GetPackets();
        toSend.insert(toSend.end(), packets.begin(), packets.end());
        delete aid;
    }
    */
}
