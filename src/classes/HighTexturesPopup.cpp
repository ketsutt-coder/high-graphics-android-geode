#include "HighTexturesPopup.hpp"
#include "../HighGraphics.hpp"
#include <thread>

using namespace geode::utils::file;

HighTexturesPopup* HighTexturesPopup::create(bool zipExists) {
    auto ret = new HighTexturesPopup();
    // initAnchored es el método recomendado en la v5 para Popups
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
        game::restart(true); // Argumento bool obligatorio en v5
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

    m_buttonsMenu = CCMenu::create();
    m_buttonsMenu->setPosition({0, 0});
    m_mainLayer->addChild(m_buttonsMenu);

    m_downloadBtn = createButton("Download", 100, "GJ_button_01.png", "download-btn", menu_selector(HighTexturesPopup::onDownload));
    m_restartBtn = createButton("Restart", 100, "GJ_button_02.png", "restart-btn", menu_selector(HighTexturesPopup::onRestart));
    
    m_downloadBtn->setVisible(!zipExists);
    m_restartBtn->setVisible(false);
    
    return true;
}

void HighTexturesPopup::startDownload() {
    fs::path path = Mod::get()->getConfigDir();
    m_downloadListener.bind([=, this](web::WebTask::Event* e) {
        if (auto res = e->getValue()) {
            if (res->ok()) {
                fs::path file = path / (m_gameVersion + ".zip");
                res->into(file);
                startExtract(file, path);
            }
        }
    });
    auto req = web::WebRequest();
    m_downloadListener.setFilter(req.get(m_links["2.2081"][0]));
}

void HighTexturesPopup::startExtract(fs::path file, fs::path path) {
    std::thread([=, this]() {
        auto res = file::Unzip::intoDir(file, path, true);
        Loader::get()->queueInMainThread([=, this, res]() {
            if (res.isOk()) {
                m_finished = true;
                m_restartBtn->setVisible(true);
                m_chatLabel->setString("Success! Restart the game.");
            }
        });
    }).detach();
}

void HighTexturesPopup::onDownload(CCObject* sender) { startDownload(); }
void HighTexturesPopup::onRestart(CCObject* sender) { game::restart(true); }

CCMenuItemSpriteExtra* HighTexturesPopup::createButton(const char* text, float width, const char* sprite, std::string id, SEL_MenuHandler selector) {
    auto spr = ButtonSprite::create(text, width, true, "bigFont.fnt", sprite, 30, 0.6f);
    auto btn = CCMenuItemSpriteExtra::create(spr, this, selector);
    btn->setID(id);
    btn->setPosition({ m_mainLayer->getContentSize().width / 2, 40 });
    m_buttonsMenu->addChild(btn);
    return btn;
}
