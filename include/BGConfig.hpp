#include "modloader/shared/modloader.hpp"
#include "main.hpp"

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