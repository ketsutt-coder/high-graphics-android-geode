// Busca la parte de CCDirector y ponla así:
CCDirector::get()->updateContentScale(static_cast<TextureQuality>(GameManager::sharedState()->m_texQuality));
