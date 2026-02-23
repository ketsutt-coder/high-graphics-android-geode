#include <Geode/modify/MenuLayer.hpp>
#include "../classes/HighTexturesPopup.hpp"
#include "../HighGraphics.hpp"

using namespace geode::prelude;

class $modify(MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        auto manager = HighGraphics::get();
        // Ahora el compilador sí reconocerá setID(), show() y m_isHidden
        // porque HighTexturesPopup hereda correctamente de Popup en el .hpp
        if (manager->m_popup) {
            manager->m_popup->setID("HighTexturesPopup");
            manager->m_popup->m_isHidden = false;
            manager->m_popup->setVisible(true);
        }

        return true;
    }
};
