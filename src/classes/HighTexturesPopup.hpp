#pragma once
#include <Geode/Geode.hpp>
#include <Geode/ui/Popup.hpp> // OBLIGATORIO en v5
#include <Geode/utils/web.hpp>

using namespace geode::prelude;
namespace fs = std::filesystem;

class HighTexturesPopup : public geode::Popup<bool> {
protected:
    bool setup(bool zipExists) override;

public:
    static HighTexturesPopup* create(bool zipExists);
    
    // Estos miembros son requeridos por tu MenuLayer.cpp según el error
    bool m_isHidden = false;
    CCScene* m_scene = nullptr;
    
    // Miembros de UI
    float m_downloadPercentage = 0.f;
    std::string m_gameVersion;
    bool m_finished = false;
    CCLabelBMFont* m_chatLabel = nullptr;
    CCMenuItemSpriteExtra* m_restartBtn = nullptr;
    CCMenuItemSpriteExtra* m_downloadBtn = nullptr;
    CCMenu* m_buttonsMenu = nullptr;

    // Cambiado a WebResponseTask para Geode v5
    EventListener<web::WebResponseTask> m_downloadListener;

    // Declaración de funciones necesarias
    void keyDown(cocos2d::enumKeyCodes key, double p1) override;
    void onDownload(CCObject* sender);
    void onRestart(CCObject* sender);
    void startDownload();
    void startExtract(fs::path file, fs::path path);
    CCMenuItemSpriteExtra* createButton(const char* text, float width, const char* sprite, std::string id, SEL_MenuHandler selector);

    inline static std::map<std::string, std::vector<std::string>> m_links = {
        {"2.2081", {"https://github.com/Weebifying/gd-textures/releases/download/2.2074/2.2074.zip"}}
    };
};
