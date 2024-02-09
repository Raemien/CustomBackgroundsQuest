#include "modloader/shared/modloader.hpp"
#include "main.hpp"

struct Config_t
{
    bool enabled;
    bool hideEnvironment;
    bool hideLasers;
    bool hideRings;
    int imageWidth;
    int rotationOffset;
    std::string selectedFile;
};

bool LoadConfig();
void SetupConfig();