#include "custom-types/shared/macros.hpp"
#include "HMUI/FlowCoordinator.hpp"
#include "HMUI/ViewController.hpp"

DECLARE_CLASS_CODEGEN(CustomBackgrounds, BackgroundsFlowCoordinator, HMUI::FlowCoordinator,
    DECLARE_OVERRIDE_METHOD(void, DidActivate, il2cpp_utils::FindMethodUnsafe("HMUI", "FlowCoordinator", "DidActivate", 3), bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);
    DECLARE_OVERRIDE_METHOD(void, BackButtonWasPressed, il2cpp_utils::FindMethodUnsafe("HMUI", "FlowCoordinator", "BackButtonWasPressed", 1), HMUI::ViewController* topViewController);

    DECLARE_INSTANCE_FIELD(HMUI::ViewController*, bgListView);
    DECLARE_INSTANCE_FIELD(HMUI::ViewController*, bgConfigView);
    DECLARE_INSTANCE_FIELD(HMUI::ViewController*, bgEnvView);

    REGISTER_FUNCTION(
        REGISTER_FIELD(bgListView);
        REGISTER_FIELD(bgConfigView);
        REGISTER_METHOD(DidActivate);
        REGISTER_METHOD(BackButtonWasPressed);
    )
)