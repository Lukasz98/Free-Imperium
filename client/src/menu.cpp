#include "menu.h"

Menu::Menu(Window& window, Settings& s, std::vector<std::shared_ptr<Country>>& countries)
    : Scene(window, s.resolution), settings(s), countries(countries)
{
    //    Gui::Menu::Open(resolution);
}

Menu::~Menu() {}

void Menu::Play()
{
    loop();
    if (startGame) {
        //      Gui::Menu::Close();
        Room room(window, settings, countries);
        room.Play();
    }
    else if (quit) {
    }
}

void Menu::loop()
{
    guiLast.init(&window, resolution, window.GetSize());
    while (!window.ShouldClose() && !quit && !startGame) {
        window.Refresh();

        guiLast.start();
        glm::vec2 mp{window.xMouse * resolution.x / window.GetSize().x,
                     (window.GetSize().y - window.yMouse) * resolution.y / window.GetSize().y};

        GuiLast::GuiEv ctype;
        GuiLast::GuiEv tmpctype;
        tmpctype = guiLast.start_menu(mp.x, mp.y);
        if (tmpctype.ct != ClickEventType::MISS)
            ctype = tmpctype;

        guiLast.flush();
        if (window.mouseLClicked) {
            std::vector<sf::Packet> packets;
            switch (ctype.ct) {
                case ClickEventType::QUIT_GAME: {
                    quit = true;
                    break;
                }
                case ClickEventType::START_ROOM: {
                    startGame = true;
                    break;
                }
            }
        }
        window.mouseLClicked = false;
        window.mouseRClicked = false;
        window.scrollOffset = 0;
        window.Update();
    }
}

