#pragma once
#include <Geode/Geode.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>
using namespace geode::prelude;

namespace ci {
    struct Scan {
        int total = 0, triggers = 0, solids = 0, hazards = 0, decor = 0, other = 0;
        std::map<int, int> groups;
        std::set<int> items;
        int freeGroup = 1, freeItem = 1, maxGroup = 0;
        std::vector<std::pair<int, int>> top;
    };

    inline int firstFree(std::set<int> const& used) {
        int i = 1;
        while (used.count(i)) i++;
        return i;
    }

    inline Scan run(LevelEditorLayer* lel) {
        Scan s;
        if (!lel || !lel->m_objects) return s;
        s.total = (int)lel->m_objects->count();
        for (auto obj : CCArrayExt<GameObject*>(lel->m_objects)) {
            if (!obj) continue;
            if (obj->m_isTrigger) {
                s.triggers++;
            } else {
                switch (obj->m_objectType) {
                    case GameObjectType::Solid: s.solids++; break;
                    case GameObjectType::Hazard:
                    case GameObjectType::AnimatedHazard: s.hazards++; break;
                    case GameObjectType::Decoration: s.decor++; break;
                    default: s.other++; break;
                }
            }
            if (obj->m_groups) {
                for (int i = 0; i < obj->m_groupCount && i < 10; i++) {
                    int g = (*obj->m_groups)[i];
                    if (g > 0) s.groups[g]++;
                }
            }
            if (auto e = typeinfo_cast<EffectGameObject*>(obj)) {
                if (obj->m_isTrigger) {
                    if (e->m_targetGroupID > 0) s.groups[e->m_targetGroupID]++;
                    if (e->m_centerGroupID > 0) s.groups[e->m_centerGroupID]++;
                }
                if (e->m_itemID > 0) s.items.insert(e->m_itemID);
                if (e->m_itemID2 > 0) s.items.insert(e->m_itemID2);
            }
        }
        std::set<int> used;
        for (auto const& [g, n] : s.groups) {
            used.insert(g);
            s.maxGroup = std::max(s.maxGroup, g);
            s.top.push_back({g, n});
        }
        s.freeGroup = firstFree(used);
        s.freeItem = firstFree(s.items);
        std::sort(s.top.begin(), s.top.end(), [](auto const& a, auto const& b) { return a.second > b.second; });
        if (s.top.size() > 3) s.top.resize(3);
        return s;
    }
}
