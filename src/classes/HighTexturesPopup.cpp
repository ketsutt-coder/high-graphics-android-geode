#include "HighTexturesPopup.hpp"
#include "../HighGraphics.hpp"
#include <thread>

using namespace geode::utils::file;

CCMenuItemSpriteExtra* HighTexturesPopup::createButton(const char* text, float width, const char* sprite, std::string id, SEL_MenuHandler selector) {
    CCSize size = m_mainLayer->getContentSize();
    auto spr = ButtonSprite::create(text, width, true, "bigFont.fnt", sprite, 30, 0.65f);
    auto btn = CCMenuItemSpriteExtra::create(spr, this, selector);
    btn->setVisible(false);
    btn->setPosition({ size.width/2, 25 });
    btn->setContentSize({ width, 30 });
    spr->setPosition(btn->getContentSize()/2);
    btn->setID(id);
    m_menu->addChild(btn);
    return btn;
}

void HighTexturesPopup::keyDown(cocos2d::enumKeyCodes key, double p1) {
    if (key == cocos2d::enumKeyCodes::KEY_Escape) return;
    if (m_finished && key == cocos2d::enumKeyCodes::KEY_Space) {
        Mod::get()->setSavedValue("applied", false);
        game::restart(true); // Corregido: pide argumento
        return;
    }
    Popup::keyDown(key, p1);
}

bool HighTexturesPopup::setup(bool zipExists) {
    this->setTitle("High Textures");
    CCSize size = m_mainLayer->getContentSize();
    m_gameVersion = Loader::get()->getGameVersion();
    
    auto chatLabel = CCLabelBMFont::create("Necessary high graphics textures missing. Download now? (107MB)", "chatFont.fnt");
    chatLabel->setPosition({ size.width/2, size.height - 70 });
    chatLabel->setScale(0.9f);
    chatLabel->setID("chat-label");
    m_mainLayer->addChild(chatLabel);
    m_chatLabel = chatLabel;

    auto menu = CCMenu::create();
    menu->setPosition({ 0, 0 });
    menu->setID("buttons-menu");
    m_mainLayer->addChild(menu);
    m_menu = menu;

    m_downloadBtn = createButton("Download", 104, "GJ_button_01.png", "download-button", menu_selector(HighTexturesPopup::onDownload));
    m_extractBtn = createButton("Extract", 104, "GJ_button_02.png", "extract-button", menu_selector(HighTexturesPopup::onExtract));
    m_retryBtn = createButton("Retry", 104, "GJ_button_04.png", "retry-button", menu_selector(HighTexturesPopup::onRetry));
    m_restartBtn = createButton("Restart", 104, "GJ_button_01.png", "restart-button", menu_selector(HighTexturesPopup::onRestart));
    m_hideBtn = createButton("Run in Background", 180, "GJ_button_03.png", "hide-button", menu_selector(HighTexturesPopup::onHide));

    auto progressBG = CCSprite::create("GJ_progressBar_001.png");
    progressBG->setVisible(false);
    progressBG->setPosition({ size.width/2, 60 });
    progressBG->setID("progress-bar-background");
    m_mainLayer->addChild(progressBG);
    m_progressBG = progressBG;

    auto downloadStencil = CCScale9Sprite::create("square02_001.png");
    downloadStencil->setAnchorPoint({ 0, 0.5f });
    m_downloadStencil = downloadStencil;

    auto extractStencil = CCScale9Sprite::create("square02_001.png");
    extractStencil->setAnchorPoint({ 0, 0.5f });
    m_extractStencil = extractStencil;

    m_downloadLabel = CCLabelBMFont::create("0.00%", "bigFont.fnt");
    m_downloadLabel->setVisible(false);
    m_mainLayer->addChild(m_downloadLabel);

    m_extractLabel = CCLabelBMFont::create("0.00%", "bigFont.fnt");
    m_extractLabel->setVisible(false);
    m_mainLayer->addChild(m_extractLabel);

    if (zipExists) {
        m_extractBtn->setVisible(true);
    } else {
        m_downloadBtn->setVisible(true);
    }
    return true;
}

HighTexturesPopup* HighTexturesPopup::create(bool zipExists) {
    auto ret = new HighTexturesPopup();
    if (ret && ret->initAnchored(360.f, 200.f, zipExists)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

void HighTexturesPopup::onDownload(CCObject* sender) { startDownload(); }
void HighTexturesPopup::onExtract(CCObject* sender) { 
    fs::path path = Mod::get()->getConfigDir();
    startExtract(path / (m_gameVersion + ".zip"), path); 
}
void HighTexturesPopup::onRetry(CCObject* sender) { onDownload(nullptr); }
void HighTexturesPopup::onRestart(CCObject* sender) { game::restart(true); }
void HighTexturesPopup::onHide(CCObject* sender) { this->setVisible(false); }

void HighTexturesPopup::setDownloadPercentage(float percentage, ccColor3B color) {
    m_downloadPercentage = percentage;
    m_downloadStencil->setScaleX(m_downloadPercentage / 100);
}

void HighTexturesPopup::setExtractPercentage(float percentage, ccColor3B color) {
    m_extractPercentage = percentage;
    m_extractStencil->setScaleX(m_extractPercentage / 100);
}

void HighTexturesPopup::startDownload() {
    m_progressBG->setVisible(true);
    m_downloadLabel->setVisible(true);
    fs::path path = Mod::get()->getConfigDir();

    m_downloadListener.bind([=, this](web::WebTask::Event* e) {
        if (auto res = e->getValue()) {
            if (res->ok()) {
                downloadSucceeded(path / (m_gameVersion + ".zip"), path);
            } else {
                downloadFailed("Server error");
            }
        } else if (auto p = e->getProgress()) {
            setDownloadPercentage(p->downloadProgress().value_or(0.f), {255, 255, 255});
        }
    });

    auto req = web::WebRequest();
    m_downloadListener.setFilter(req.get(m_links[m_gameVersion][0]));
}

void HighTexturesPopup::startExtract(fs::path file, fs::path path) {
    std::thread([=, this]() {
        auto res = file::Unzip::intoDir(file, path, true);
        Loader::get()->queueInMainThread([=, this]() {
            if (res.isOk()) extractSucceeded();
            else extractFailed("Zip error");
        });
    }).detach();
}

void HighTexturesPopup::downloadSucceeded(fs::path file, fs::path path) { startExtract(file, path); }
void HighTexturesPopup::downloadFailed(std::string reason) { log::error("{}", reason); }
void HighTexturesPopup::extractSucceeded() { m_finished = true; m_restartBtn->setVisible(true); }
void HighTexturesPopup::extractFailed(std::string reason) { log::error("{}", reason); }
void HighTexturesPopup::notifySuccess() {}
void HighTexturesPopup::notifyFailure(std::string which, std::string reason) {}
