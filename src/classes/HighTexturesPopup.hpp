#pragma once
#include <Geode/Geode.hpp>
#include <Geode/ui/Popup.hpp> // Obligatorio en Geode v5
#include <Geode/utils/web.hpp>

using namespace geode::prelude;
namespace fs = std::filesystem;

class HighTexturesPopup : public geode::Popup<bool> {
protected:
    bool setup(bool zipExists) override;

public:
    static HighTexturesPopup* create(bool zipExists);

    // Declaraciones faltantes que causaban errores de "out-of-line definition"
    CCMenuItemSpriteExtra* createButton(const char* text, float width, const char* sprite, std::string id, SEL_MenuHandler selector);
    void keyDown(cocos2d::enumKeyCodes key, double p1) override;
    
    void onDownload(CCObject* sender);
    void onExtract(CCObject* sender);
    void onRetry(CCObject* sender);
    void onRestart(CCObject* sender);
    
    void startDownload();
    void startExtract(fs::path file, fs::path path);
    void downloadSucceeded(fs::path file, fs::path path);
    void downloadFailed(std::string reason);

    // Miembros de clase para control de UI
    float m_downloadPercentage = 0.f;
    std::string m_gameVersion;
    bool m_finished = false;
    CCLabelBMFont* m_chatLabel = nullptr;
    CCMenuItemSpriteExtra* m_restartBtn = nullptr;
    CCMenuItemSpriteExtra* m_downloadBtn = nullptr;
    CCMenuItemSpriteExtra* m_extractBtn = nullptr;
    CCMenuItemSpriteExtra* m_retryBtn = nullptr;
    CCMenu* m_menu = nullptr;
    CCSprite* m_progressBG = nullptr;
    CCNode* m_downloadStencil = nullptr;

    // WebTask es el nombre correcto en la v5
    EventListener<web::WebTask> m_downloadListener;
    
    inline static std::map<std::string, std::vector<std::string>> m_links = {
        {"2.2081", {"https://github.com/Weebifying/gd-textures/releases/download/2.2074/2.2074.zip"}}
    };
};
