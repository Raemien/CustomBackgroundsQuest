#include "main.hpp"
#include "BackgroundListViewController.hpp"
#include "BackgroundConfigViewController.hpp"
#include "BackgroundsFlowCoordinator.hpp"

#include "modloader/shared/modloader.hpp"
#include "beatsaber-hook/shared/utils/utils.h"
#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp" 
#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"
#include "beatsaber-hook/shared/utils/typedefs.h"
#include "beatsaber-hook/shared/config/config-utils.hpp"

#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/QuestUI.hpp"

#include "HMUI/ViewController.hpp"
#include "HMUI/ViewController_AnimationType.hpp"
#include "HMUI/ViewController_AnimationDirection.hpp"
#include "HMUI/FlowCoordinator.hpp"
#include "UnityEngine/Object.hpp"

using namespace CustomBackgrounds;
DEFINE_CLASS(BackgroundsFlowCoordinator);

void BackgroundsFlowCoordinator::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling)
{
    if (firstActivation) 
    {
        this->SetTitle(il2cpp_utils::createcsstr("Custom Skyboxes"), (int)1);
        this->showBackButton = true;

        if (!this->bgListView) this->bgListView = QuestUI::BeatSaberUI::CreateViewController<BackgroundListViewController*>();
        if (!this->bgConfigView) this->bgConfigView = QuestUI::BeatSaberUI::CreateViewController<BackgroundConfigViewController*>();
        BackgroundsFlowCoordinator::ProvideInitialViewControllers(bgListView, bgConfigView, nullptr, nullptr, nullptr);
    }
}

void BackgroundsFlowCoordinator::BackButtonWasPressed(HMUI::ViewController* topView)
{
    getConfig().Write();
    HMUI::FlowCoordinator* settingsFC = QuestUI::GetModSettingsFlowCoordinator();
    settingsFC->DismissFlowCoordinator(this, (int)0, nullptr, false);
}