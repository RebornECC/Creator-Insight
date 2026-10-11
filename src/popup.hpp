#pragma once
#include "scan.hpp"

namespace ci {
    constexpr ccColor3B MUS = {212, 166, 10};
    constexpr ccColor3B GRY = {201, 203, 204};
    constexpr ccColor3B SLT = {107, 118, 133};
    constexpr ccColor3B INK = {42, 49, 61};
    constexpr ccColor3B SNOW = {247, 247, 245};

    inline ccColor4F f4(ccColor3B c, float a = 1.f) { return {c.r / 255.f, c.g / 255.f, c.b / 255.f, a}; }

    inline void rect(CCDrawNode* d, float x, float y, float w, float h, ccColor4F c) {
        CCPoint v[4] = {{x, y}, {x + w, y}, {x + w, y + h}, {x, y + h}};
        d->drawPolygon(v, 4, c, 0.f, c);
    }

    inline void roundRect(CCDrawNode* d, float x, float y, float w, float h, float r, ccColor4F c) {
        r = std::min(r, std::min(w, h) / 2.f);
        rect(d, x + r, y, w - 2 * r, h, c);
        rect(d, x, y + r, w, h - 2 * r, c);
        d->drawDot({x + r, y + r}, r, c);
        d->drawDot({x + w - r, y + r}, r, c);
        d->drawDot({x + r, y + h - r}, r, c);
        d->drawDot({x + w - r, y + h - r}, r, c);
    }

    inline void tri(CCDrawNode* d, CCPoint a, CCPoint b, CCPoint c, ccColor3B col) {
        CCPoint v[3] = {a, b, c};
        d->drawPolygon(v, 3, f4(col), 0.f, f4(col));
    }

    inline void quad(CCDrawNode* d, CCPoint a, CCPoint b, CCPoint c, CCPoint e, ccColor3B col) {
        CCPoint v[4] = {a, b, c, e};
        d->drawPolygon(v, 4, f4(col), 0.f, f4(col));
    }

    inline CCNode* pill(char const* text, ccColor3B fill, ccColor3B ink, float w = 84.f, float h = 26.f) {
        auto n = CCNode::create();
        n->setContentSize({w, h});
        n->setAnchorPoint({0.5f, 0.5f});
        auto d = CCDrawNode::create();
        n->addChild(d);
        roundRect(d, 0.f, -2.f, w, h, 8.f, f4(INK, 0.35f));
        roundRect(d, 0.f, 0.f, w, h, 8.f, f4(fill));
        auto l = CCLabelBMFont::create(text, "bigFont.fnt");
        l->setTag(7);
        l->setColor(ink);
        l->setScale(.38f);
        l->setPosition({w / 2, h / 2 + 1.f});
        n->addChild(l, 2);
        return n;
    }

    inline CCNode* logoButton(float size) {
        auto n = CCNode::create();
        n->setContentSize({size, size});
        n->setAnchorPoint({0.5f, 0.5f});
        auto d = CCDrawNode::create();
        n->addChild(d);
        roundRect(d, 0.f, -2.f, size, size, 4.f, f4(INK, 0.4f));
        roundRect(d, -1.5f, -1.5f, size + 3.f, size + 3.f, 5.f, f4(INK));
        roundRect(d, 0.f, 0.f, size, size, 4.f, f4(SNOW));
        float k = size / 1000.f;
        auto P = [&](float x, float y) { return CCPoint{x * k, (1000.f - y) * k}; };
        tri(d, P(40, 40), P(420, 40), P(40, 340), MUS);
        quad(d, P(470, 40), P(960, 40), P(960, 210), P(270, 410), GRY);
        quad(d, P(130, 520), P(540, 390), P(810, 670), P(430, 900), SLT);
        quad(d, P(860, 250), P(960, 250), P(960, 610), P(850, 520), MUS);
        tri(d, P(40, 640), P(320, 940), P(40, 940), MUS);
        quad(d, P(480, 940), P(960, 940), P(960, 720), P(850, 720), GRY);
        tri(d, P(40, 380), P(250, 430), P(40, 520), GRY);
        auto c = P(470, 655);
        for (int i = 0; i < 2; i++) {
            auto l = CCLabelBMFont::create("CI", "bigFont.fnt");
            l->setScale(size * 0.0085f);
            l->setColor(i == 0 ? INK : SNOW);
            l->setPosition({c.x + (i == 0 ? 1.f : 0.f), c.y + (i == 0 ? -1.f : 0.f)});
            n->addChild(l, 2 - i);
        }
        return n;
    }

    inline void setPill(CCNode* n, char const* text) {
        if (auto l = typeinfo_cast<CCLabelBMFont*>(n->getChildByTag(7))) {
            l->setString(text);
            l->setScale(.38f);
            float maxW = n->getContentSize().width - 14.f;
            if (l->getContentSize().width * l->getScale() > maxW) l->setScale(maxW / l->getContentSize().width);
        }
    }
}

class InsightPopup : public geode::Popup {
public:
    static inline int s_open = 0;
    InsightPopup() { s_open++; }
    ~InsightPopup() override { s_open--; }

protected:
    LevelEditorLayer* m_lel = nullptr;
    CCNode* m_content = nullptr;
    CCNode* m_gBtn = nullptr;
    CCNode* m_iBtn = nullptr;
    ci::Scan m_scan;

    CCLabelBMFont* text(CCNode* p, char const* s, char const* font, float scale, CCPoint pos, ccColor3B c) {
        auto l = CCLabelBMFont::create(s, font);
        l->setScale(scale);
        l->setPosition(pos);
        l->setColor(c);
        p->addChild(l);
        return l;
    }

    void stat(float x, float y, int value, char const* cap, ccColor3B c) {
        text(m_content, fmt::format("{}", value).c_str(), "bigFont.fnt", .55f, {x, y}, c);
        text(m_content, cap, "chatFont.fnt", .5f, {x, y - 14.f}, {200, 205, 213});
    }

    void drawWall(CCNode* parent) {
        using namespace ci;
        auto d = CCDrawNode::create();
        parent->addChild(d, 0);
        roundRect(d, -2.f, -2.f, 364.f, 284.f, 12.f, f4(INK, 0.9f));
        roundRect(d, 0.f, 0.f, 360.f, 280.f, 10.f, f4(SNOW));
        tri(d, {8.f, 272.f}, {96.f, 272.f}, {8.f, 214.f}, MUS);
        quad(d, {250.f, 272.f}, {352.f, 272.f}, {352.f, 232.f}, {290.f, 232.f}, GRY);
        quad(d, {338.f, 224.f}, {352.f, 224.f}, {352.f, 150.f}, {338.f, 160.f}, MUS);
        tri(d, {8.f, 8.f}, {40.f, 8.f}, {8.f, 52.f}, MUS);
        quad(d, {352.f, 8.f}, {300.f, 8.f}, {320.f, 54.f}, {352.f, 54.f}, GRY);
        roundRect(d, 16.f, 58.f, 328.f, 182.f, 6.f, f4(SNOW));
        roundRect(d, 20.f, 62.f, 320.f, 174.f, 4.f, f4(SLT));
    }

    void rebuild() {
        using namespace ci;
        m_scan = ci::run(m_lel);
        m_content->removeAllChildren();
        auto& s = m_scan;
        bool heavy = s.total > (int)Mod::get()->getSettingValue<int64_t>("warn-objects");
        ccColor3B warn = {255, 140, 100};
        stat(80.f, 216.f, s.total, "objects", heavy ? warn : ccColor3B{235, 188, 40});
        stat(180.f, 216.f, s.triggers, "triggers", SNOW);
        stat(280.f, 216.f, s.solids, "solid blocks", SNOW);
        stat(80.f, 184.f, s.hazards, "hazards", SNOW);
        stat(180.f, 184.f, s.decor, "decoration", SNOW);
        stat(280.f, 184.f, s.other, "other", SNOW);

        std::string line;
        if (heavy) line = "Heavy level, it may lag on weak phones";
        else if (s.top.empty()) line = "No groups used yet";
        else {
            line = "Busiest groups: ";
            for (size_t i = 0; i < s.top.size(); i++) {
                if (i) line += "   ";
                line += fmt::format("{} (x{})", s.top[i].first, s.top[i].second);
            }
        }
        text(m_content, line.c_str(), "chatFont.fnt", .55f, {180.f, 154.f}, heavy ? warn : ccColor3B{225, 229, 235});

        auto d = CCDrawNode::create();
        m_content->addChild(d);
        float cell = 10.f, step = 12.f, x0 = 30.f, top = 146.f;
        for (int r = 0; r < 6; r++) {
            for (int c = 0; c < 25; c++) {
                int g = r * 25 + c + 1;
                ccColor3B col = {84, 94, 108};
                if (s.groups.count(g)) col = MUS;
                if (g == s.freeGroup) col = SNOW;
                ci::rect(d, x0 + c * step, top - (r + 1) * step, cell, cell, f4(col));
            }
        }
        text(m_content, fmt::format("Groups 1-150    yellow: used    white: next free    highest used: {}", s.maxGroup).c_str(),
             "chatFont.fnt", .5f, {180.f, 47.f}, INK);
        setPill(m_gBtn, fmt::format("Group {}", s.freeGroup).c_str());
        setPill(m_iBtn, fmt::format("Item {}", s.freeItem).c_str());
    }

    bool init(LevelEditorLayer* lel) {
        using namespace ci;
        if (!Popup::init(360.f, 280.f, "GJ_square05.png")) return false;
        m_lel = lel;
        m_bgSprite->setVisible(false);
        drawWall(m_mainLayer);
        text(m_mainLayer, "Creator Insight", "bigFont.fnt", .62f, {178.f, 256.f}, INK);
        m_content = CCNode::create();
        m_mainLayer->addChild(m_content, 1);

        auto menu = CCMenu::create();
        menu->setPosition({0.f, 0.f});
        m_mainLayer->addChild(menu, 10);

        m_gBtn = pill("Group", MUS, INK, 92.f);
        auto gb = CCMenuItemExt::createSpriteExtra(m_gBtn, [this](auto) {
            geode::utils::clipboard::write(fmt::format("{}", m_scan.freeGroup));
            Notification::create(fmt::format("Copied group {}", m_scan.freeGroup), NotificationIcon::Success)->show();
        });
        gb->setPosition({100.f, 24.f});
        menu->addChild(gb);

        m_iBtn = pill("Item", GRY, INK, 92.f);
        auto ib = CCMenuItemExt::createSpriteExtra(m_iBtn, [this](auto) {
            geode::utils::clipboard::write(fmt::format("{}", m_scan.freeItem));
            Notification::create(fmt::format("Copied item {}", m_scan.freeItem), NotificationIcon::Success)->show();
        });
        ib->setPosition({200.f, 24.f});
        menu->addChild(ib);

        auto rs = pill("Rescan", SLT, SNOW, 80.f);
        auto rb = CCMenuItemExt::createSpriteExtra(rs, [this](auto) { this->rebuild(); });
        rb->setPosition({290.f, 24.f});
        menu->addChild(rb);

        rebuild();
        return true;
    }

public:
    static InsightPopup* create(LevelEditorLayer* lel) {
        auto ret = new InsightPopup();
        if (ret->init(lel)) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }
};
