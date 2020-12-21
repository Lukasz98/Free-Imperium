#pragma once
#include <string>
#include <vector>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include "click_event_enum.h"
#include "log.h"
#include "../observer.h"

namespace Gui::Base {
    ClickEventType Click(const glm::vec2 & mousePos);
    void Hover(const glm::vec2 & mousePos);
    bool Scroll(int y, const glm::vec2 & mousePos);
    bool Drag(const glm::vec2 & mousePos, float dt);
    void Draw();
    void ResetClick();
    void UpdateVals();
    int GetHiddenValue();
    void CloseWindowFromClick();
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
    Observer * Open(const std::vector<std::string> & values, const glm::vec2 & resolution);
}

namespace Gui::Prov {
    Observer * Open(const glm::vec2 & resolution);
    void Close();
    void SwitchTab();
}

namespace Gui::ProvSiege {
    Observer * Open(const glm::vec2 & resolution);
    void Close();
}

namespace Gui::Unit {
    Observer * Open(const glm::vec2 & resolution);
    void Close();
}

namespace Gui::UnitsList {
    void Open(const glm::vec2 & resolution);
    void Close();
}

namespace Gui::Country{
    Observer * Open(const glm::vec2 & resolution, int ctrId, bool atWarWith);
    void Close();
    void SwitchTab();
    int GetId();
}

namespace Gui::MyCountry{
    Observer * Open(const glm::vec2 & resolution);
    void Close();
    void SwitchTab();
}

namespace Gui::SideBar{
    Observer * Open(const glm::vec2 & resolution);
    void Close();
    void AddWarIcon(int warId, const std::string & rival);
    void DeleteWarIcon(int warId);
    void AddPeaceOfferIcon(int warId, const std::string & rival);
    void DeletePeaceOfferIcon(int peaceOfferId);
    //void SwitchTab();
}

namespace Gui::War{
    Observer * Open(const glm::vec2 & resolution, int warId);
    void Close();
    void AddAttacker(const std::string & attacker);
    void AddDefender(const std::string & defender);
    bool IsOpen();
    int GetId();
}

namespace Gui::OfferPeace{
    Observer * Open(const glm::vec2 & resolution, const std::string & defender, const std::string & attacker, int warId, int warscore, int attackerId, int defenderId);
    void Close();
    void AddProvince(const std::string & province, const std::string & receiver, int provId, int receiverId);
    void DeleteProvince(int provId);
    bool IsOpened();
}




