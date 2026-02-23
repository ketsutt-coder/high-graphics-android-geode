#include <Geode/modify/LoadingLayer.hpp>
#include "../HighGraphics.hpp"

using namespace geode::prelude;

class $modify(LoadingLayer) {
    void loadAssets() {
        LoadingLayer::loadAssets();
        
        // En Geode v5, usamos static_cast para tipos definidos
        auto texQuality = GameManager::sharedState()->m_texQuality;
        auto quality = static_cast<TextureQuality>(static_cast<int>(texQuality));
        
        CCDirector::get()->updateContentScale(quality);
    }
};
