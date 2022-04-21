#include "menu.h"

Menu::Menu(GameData* gd) : gd(gd) {}

Menu::~Menu() {}

void Menu::Play()
{
    loop();
    if (startGame) {
        Room room(gd);
        room.Play();
    }
    else if (quit) {
    }
}

void Menu::loop()
{
    guiLast.init(gd->window, gd->settings.resolution, gd->window->GetSize());
    while (!gd->window->ShouldClose() && !quit && !startGame) {
        gd->window->Refresh();

        guiLast.start();
        glm::vec2 mp{
            gd->window->xMouse * gd->settings.resolution.x / gd->window->GetSize().x,
            (gd->window->GetSize().y - gd->window->yMouse) * gd->settings.resolution.y / gd->window->GetSize().y};

        GuiLast::GuiEv ctype;
        GuiLast::GuiEv tmpctype;
        tmpctype = guiLast.start_menu(mp.x, mp.y);
        if (tmpctype.ct != ClickEventType::MISS)
            ctype = tmpctype;

        guiLast.flush();
        if (gd->window->mouseLClicked) {
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
        gd->window->mouseLClicked = false;
        gd->window->mouseRClicked = false;
        gd->window->scrollOffset = 0;
        if (ctype.ct == ClickEventType::NONE || ctype.ct == ClickEventType::MISS)
            glfwSetCursor(gd->window->window, AM::am.def_cursor);
        else
            glfwSetCursor(gd->window->window, AM::am.hover_cursor);
        gd->window->Update();
    }
}

