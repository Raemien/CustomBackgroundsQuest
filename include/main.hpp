#pragma once
#include <string>
#include "modloader/shared/modloader.hpp"

#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/config/config-utils.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"

#include "UnityEngine/Texture2D.hpp"
#include "UnityEngine/GameObject.hpp"

Configuration& getConfig();
extern Logger& getLogger();
extern UnityEngine::Texture2D* backgroundTexture;
extern UnityEngine::GameObject* backgroundObject;
extern std::string bgDirectoryPath;
extern void LoadBackground(std::string filename);