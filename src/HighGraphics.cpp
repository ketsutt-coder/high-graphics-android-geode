#include "HighGraphics.hpp"

HighGraphics* HighGraphics::get() {
    static HighGraphics instance;
    return &instance;
}

// Lógica de conteo de archivos simplificada para evitar bloqueos en iOS
int HighGraphics::getNumFiles(fs::path path) {
    if (!fs::exists(path)) return 0;
    int count = 0;
    for (const auto& entry : fs::directory_iterator(path)) {
        if (entry.is_regular_file()) count++;
    }
    return count;
}

$on_mod(Loaded) {
    std::string version = Loader::get()->getGameVersion();
    fs::path path = Mod::get()->getConfigDir() / version;

    if (fs::exists(path)) {
        log::debug("Adding priority path for High Graphics: {}", path.string());
        CCFileUtils::get()->addPriorityPath(path.string().c_str());
    }
}
