#include "menu.h"

Menu::Menu(Window* window, GameData* gd)
    : window(window), gd(gd)
{
}

Menu::~Menu() {}

void Menu::Play()
{
    loop();
    if (startGame) {
        Room room(window, gd);
        room.Play();
    }
    else if (quit) {
    }
}

void Menu::loop()
{
    guiLast.init(window, gd->settings.resolution, window->GetSize());
    while (!window->ShouldClose() && !quit && !startGame) {
        window->Refresh();

        guiLast.start();
        glm::vec2 mp{window->xMouse * gd->settings.resolution.x / window->GetSize().x,
                     (window->GetSize().y - window->yMouse) * gd->settings.resolution.y / window->GetSize().y};

        GuiLast::GuiEv ctype;
        GuiLast::GuiEv tmpctype;
        tmpctype = guiLast.start_menu(mp.x, mp.y);
        if (tmpctype.ct != ClickEventType::MISS)
            ctype = tmpctype;

        guiLast.flush();
        if (window->mouseLClicked) {
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
        window->mouseLClicked = false;
        window->mouseRClicked = false;
        window->scrollOffset = 0;
        window->Update();
    }
}

