#pragma once
#include <string>
#include <vector>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include "click_event_enum.h"
#include "log.h"

namespace Gui::Base {
    ClickEventType Click(const glm::vec2 & mousePos);
    void Hover(const glm::vec2 & mousePos);
    bool Scroll(int y, const glm::vec2 & mousePos);
    bool Drag(const glm::vec2 & mousePos, float dt);
    void Draw();
    void ResetClick();
}

namespace Gui::Menu {
    void Open(const glm::vec2 & resolution);
    void Close();
}

namespace Gui::Room {
    void Open(const glm::vec2 & resolution);
    void AddCountryToList(const std::string & countryName);
    void ClearPlayersList();
    void AddPlayerToList(const std::string & s);
    std::string GetPickedCountry();
    void Close();
}


namespace Gui::TopBar {
    void Open(const std::vector<std::string> & values, const glm::vec2 & resolution);
}

namespace Gui::Prov {
    void Open(const glm::vec2 & resolution);
    void Close();
    void SwitchTab();
}

namespace Gui::ProvSiege {
    void Open(const glm::vec2 & resolution);
    void Close();
}

namespace Gui::Unit {
    void Open(const glm::vec2 & resolution);
    void Close();
}

namespace Gui::UnitsList {
    void Open(const glm::vec2 & resolution);
    void Close();
}

