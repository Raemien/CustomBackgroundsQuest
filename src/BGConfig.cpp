#include "main.hpp"
#include "beatsaber-hook/shared/config/config-utils.hpp"

struct Config_t
{
    bool enabled = false;
    bool hideEnvironment = true;
    bool hideLasers = true;
    bool hideRings = true;
    int imageWidth = 3840;
    int rotationOffset = 0;
    std::string selectedFile = "SteamVR.png";
}
        Settings;

void SetupConfig()
{
    getConfig().config.RemoveAllMembers();
    getConfig().config.SetObject();
    auto& allocator = getConfig().config.GetAllocator();
    getConfig().config.AddMember("enabled", Settings.enabled, allocator);
    getConfig().config.AddMember("hideEnvironment", Settings.hideEnvironment, allocator);
    getConfig().config.AddMember("hideLasers", Settings.hideLasers, allocator);
    getConfig().config.AddMember("hideRings", Settings.hideLasers, allocator);
    getConfig().config.AddMember("imageWidth", Settings.imageWidth, allocator);
    getConfig().config.AddMember("rotationOffset", Settings.rotationOffset, allocator);
    getConfig().config.AddMember("selectedFile", Settings.selectedFile, allocator);
    getConfig().Write();
}

bool LoadConfig()
{
    getConfig().Load();
    if(!getConfig().config.HasMember("enabled") || !getConfig().config["enabled"].IsBool()) return false;
    if(!getConfig().config.HasMember("hideEnvironment") || !getConfig().config["hideEnvironment"].IsBool()) return false;
    if(!getConfig().config.HasMember("hideLasers") || !getConfig().config["hideLasers"].IsBool()) return false;
    if(!getConfig().config.HasMember("hideRings") || !getConfig().config["hideRings"].IsBool()) return false;
    if(!getConfig().config.HasMember("imageWidth") || !getConfig().config["imageWidth"].IsInt()) return false;
    if(!getConfig().config.HasMember("rotationOffset") || !getConfig().config["rotationOffset"].IsInt()) return false;
    if(!getConfig().config.HasMember("selectedFile") || !getConfig().config["selectedFile"].IsString()) return false;
    return true;
}