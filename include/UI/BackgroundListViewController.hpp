#include "questui/shared/BeatSaberUI.hpp"
#include "custom-types/shared/macros.hpp"
#include "HMUI/ViewController.hpp"

DECLARE_CLASS_CODEGEN(CustomBackgrounds, BackgroundListViewController, HMUI::ViewController,
    DECLARE_INSTANCE_FIELD(UnityEngine::UI::Toggle*, masterEnabled);
    DECLARE_INSTANCE_FIELD(UnityEngine::UI::VerticalLayoutGroup*, configcontainer);
    DECLARE_INSTANCE_FIELD(UnityEngine::UI::HorizontalLayoutGroup*, listtxtgroup);
    DECLARE_INSTANCE_FIELD(UnityEngine::GameObject*, bglistscroll);

    DECLARE_OVERRIDE_METHOD(void, DidActivate, il2cpp_utils::FindMethodUnsafe("HMUI", "ViewController", "DidActivate", 3), bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);
    DECLARE_OVERRIDE_METHOD(void, DidDeactivate, il2cpp_utils::FindMethodUnsafe("HMUI", "ViewController", "DidDeactivate", 2), bool removedFromHierarchy, bool systemScreenDisabling);
)