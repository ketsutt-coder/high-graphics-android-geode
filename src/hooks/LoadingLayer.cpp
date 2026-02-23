#include <Geode/modify/LoadingLayer.hpp>
#include "../HighGraphics.hpp"

using namespace geode::prelude;

class $modify(LoadingLayer) {
    void loadAssets() {
        LoadingLayer::loadAssets();
        
        // Reemplazado 'as' por static_cast para compatibilidad con v5
        auto quality = static_cast<TextureQuality>(GameManager::sharedState()->m_texQuality);
        CCDirector::get()->updateContentScale(quality);
    }
};
