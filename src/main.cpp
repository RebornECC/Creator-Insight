// Creator Insight - level scan in the editor pause menu (see scan.hpp, popup.hpp)
#include "popup.hpp"

class $modify(CiPauseLayer, EditorPauseLayer) {
    void customSetup() {
        EditorPauseLayer::customSetup();
        auto btn = CCMenuItemExt::createSpriteExtra(ci::pill("Insight", ci::MUS, ci::INK, 88.f, 28.f), [this](auto) {
            if (auto p = InsightPopup::create(m_editorLayer)) p->show();
        });
        auto menu = CCMenu::create();
        menu->setID("creatorinsight-menu"_spr);
        auto win = CCDirector::get()->getWinSize();
        menu->setPosition({win.width - 62.f, 26.f});
        menu->addChild(btn);
        this->addChild(menu, 50);
    }
};
