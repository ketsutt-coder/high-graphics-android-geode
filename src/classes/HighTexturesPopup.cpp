#include "HighTexturesPopup.hpp"
#include "../HighGraphics.hpp"
#include <thread>

using namespace geode::utils::file;

HighTexturesPopup* HighTexturesPopup::create(bool zipExists) {
    auto ret = new HighTexturesPopup();
    if (ret && ret->initAnchored(360.f, 200.f, zipExists)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

void HighTexturesPopup::keyDown(cocos2d::enumKeyCodes key, double p1) {
    if (key == cocos2d::enumKeyCodes::KEY_Escape) return;
    if (m_finished && key == cocos2d::enumKeyCodes::KEY_Space) {
        Mod::get()->setSavedValue("applied", false);
        game::restart(true); // Argumento obligatorio en v5
        return;
    }
    Popup::keyDown(key, p1);
}

bool HighTexturesPopup::setup(bool zipExists) {
    this->setTitle("High Textures");
    auto size = m_mainLayer->getContentSize();
    m_gameVersion = Loader::get()->getGameVersion();

    m_chatLabel = CCLabelBMFont::create("Textures missing. Download now?", "chatFont.fnt");
    m_chatLabel->setPosition({ size.width/2, size.height - 70 });
    m_mainLayer->addChild(m_chatLabel);

    m_menu = CCMenu::create();
    m_menu->setPosition({0, 0});
    m_mainLayer->addChild(m_menu);

    m_downloadBtn = createButton("Download", 100, "GJ_button_01.png", "download-btn", menu_selector(HighTexturesPopup::onDownload));
    m_restartBtn = createButton("Restart", 100, "GJ_button_02.png", "restart-btn", menu_selector(HighTexturesPopup::onRestart));
    m_extractBtn = createButton("Extract", 100, "GJ_button_01.png", "extract-btn", menu_selector(HighTexturesPopup::onExtract));
    m_retryBtn = createButton("Retry", 100, "GJ_button_04.png", "retry-btn", menu_selector(HighTexturesPopup::onRetry));

    m_downloadBtn->setVisible(!zipExists);
    m_extractBtn->setVisible(zipExists);
    
    return true;
}

void HighTexturesPopup::onDownload(CCObject* sender) { startDownload(); }
void HighTexturesPopup::onExtract(CCObject* sender) { 
    fs::path path = Mod::get()->getConfigDir();
    startExtract(path / (m_gameVersion + ".zip"), path); 
}
void HighTexturesPopup::onRetry(CCObject* sender) { onDownload(nullptr); }
void HighTexturesPopup::onRestart(CCObject* sender) { game::restart(true); }

void HighTexturesPopup::startDownload() {
    fs::path path = Mod::get()->getConfigDir();
    m_downloadListener.bind([=, this](web::WebTask::Event* e) {
        if (auto res = e->getValue()) {
            if (res->ok()) {
                downloadSucceeded(path / (m_gameVersion + ".zip"), path);
            }
        }
    });
    auto req = web::WebRequest();
    m_downloadListener.setFilter(req.get(m_links[m_gameVersion][0]));
}

void HighTexturesPopup::startExtract(fs::path file, fs::path path) {
    std::thread([=, this]() {
        auto res = file::Unzip::intoDir(file, path, true);
        Loader::get()->queueInMainThread([=, this, res]() {
            if (res.isOk()) {
                m_finished = true;
                m_restartBtn->setVisible(true);
            }
        });
    }).detach();
}

void HighTexturesPopup::downloadSucceeded(fs::path file, fs::path path) { startExtract(file, path); }
void HighTexturesPopup::downloadFailed(std::string reason) { log::error("{}", reason); }

CCMenuItemSpriteExtra* HighTexturesPopup::createButton(const char* text, float width, const char* sprite, std::string id, SEL_MenuHandler selector) {
    auto spr = ButtonSprite::create(text, width, true, "bigFont.fnt", sprite, 30, 0.6f);
    auto btn = CCMenuItemSpriteExtra::create(spr, this, selector);
    btn->setPosition({ m_mainLayer->getContentSize().width / 2, 30 });
    m_menu->addChild(btn);
    return btn;
}
