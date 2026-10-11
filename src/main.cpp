#include "popup.hpp"

class $modify(CiPauseLayer, EditorPauseLayer) {
    void customSetup() {
        EditorPauseLayer::customSetup();
        auto spr = ButtonSprite::create("Insight", "goldFont.fnt", "GJ_button_01.png", .8f);
        auto btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(CiPauseLayer::onInsight));
        auto menu = CCMenu::create();
        menu->setID("creatorinsight-menu"_spr);
        auto win = CCDirector::get()->getWinSize();
        menu->setPosition({win.width - 62.f, 26.f});
        menu->addChild(btn);
        this->addChild(menu, 50);
    }

    void onInsight(CCObject*) {
        if (!m_editorLayer) return;
        if (auto p = InsightPopup::create(m_editorLayer)) p->show();
    }
};
