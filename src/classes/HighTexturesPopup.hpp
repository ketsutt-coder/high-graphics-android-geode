#pragma once
#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>

using namespace geode::prelude;
namespace fs = std::filesystem;

class HighTexturesPopup : public geode::Popup<bool> {
protected:
    // Firma correcta para setup en Geode v5
    bool setup(bool zipExists) override;

public:
    inline static std::map<std::string, std::vector<std::string>> m_links = {
        {"2.2081", {"https://github.com/Weebifying/gd-textures/releases/download/2.2074/2.2074.zip", "https://files.catbox.moe/buykym.zip"}}
    };

    float m_downloadPercentage = 0.f;
    std::string m_gameVersion;
    bool m_finished = false;

    CCLabelBMFont* m_chatLabel = nullptr;
    CCMenuItemSpriteExtra* m_restartBtn = nullptr;
    CCMenuItemSpriteExtra* m_downloadBtn = nullptr;
    CCSprite* m_progressBG = nullptr;
    CCNode* m_downloadStencil = nullptr;

    // WebResponseTask es el estándar actual en Geode v5
    EventListener<web::WebResponseTask> m_downloadListener;

    static HighTexturesPopup* create(bool zipExists);
    
    // Firma de keyDown actualizada para evitar el error de "non-virtual"
    void keyDown(cocos2d::enumKeyCodes key, double p1) override;

    void onDownload(CCObject* sender);
    void onRestart(CCObject* sender);
    void startDownload();
    void startExtract(fs::path file, fs::path path);
};
