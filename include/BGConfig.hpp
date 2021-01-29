#include "modloader/shared/modloader.hpp"
#include "main.hpp"

#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/config/config-utils.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"

struct Config_t
{
    bool enabled;
    bool hideEnvironment;
    bool hideLasers;
    int rotationOffset;
    std::string selectedFile;
};

bool LoadConfig();
void SetupConfig();