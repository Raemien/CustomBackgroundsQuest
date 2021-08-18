#include "questui/shared/CustomTypes/Components/Settings/IncrementSetting.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "custom-types/shared/macros.hpp"
#include "HMUI/ViewController.hpp"

DECLARE_CLASS_CODEGEN(CustomBackgrounds, BackgroundConfigViewController, HMUI::ViewController,
    DECLARE_INSTANCE_FIELD(UnityEngine::UI::VerticalLayoutGroup*, configcontainer);
    
    DECLARE_INSTANCE_FIELD(UnityEngine::UI::Toggle*, hideEnvToggle);
    DECLARE_INSTANCE_FIELD(QuestUI::IncrementSetting*, rotationSetting);
    DECLARE_INSTANCE_FIELD(QuestUI::IncrementSetting*, resolutionSetting);

    DECLARE_OVERRIDE_METHOD(void, DidActivate, il2cpp_utils::FindMethodUnsafe("HMUI", "ViewController", "DidActivate", 3), bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);
    DECLARE_OVERRIDE_METHOD(void, DidDeactivate, il2cpp_utils::FindMethodUnsafe("HMUI", "ViewController", "DidDeactivate", 2), bool removedFromHierarchy, bool systemScreenDisabling);
)