#include "main.hpp"
#include "EnvironmentHider.hpp"
#include "BackgroundEnvViewController.hpp"

#include "questui/shared/ArrayUtil.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/CustomTypes/Components/Backgroundable.hpp"
#include "questui/shared/CustomTypes/Components/Settings/IncrementSetting.hpp"

#include "GlobalNamespace/EnvironmentOverrideSettingsPanelController.hpp"
#include "GlobalNamespace/PlayerDataModel.hpp"
#include "GlobalNamespace/PlayerData.hpp"
#include "GlobalNamespace/NumberTag.hpp"
#include "UnityEngine/Canvas.hpp"
#include "UnityEngine/Object.hpp"
#include "UnityEngine/Resources.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/Quaternion.hpp"
#include "UnityEngine/RectTransform.hpp"
#include "UnityEngine/RectOffset.hpp"
#include "UnityEngine/Events/UnityAction.hpp"
#include "HMUI/ViewController.hpp"
#include "HMUI/SimpleTextDropdown.hpp"
#include "HMUI/ToggleWithCallbacks.hpp"
#include "HMUI/TextSegmentedControl.hpp"
#include "VRUIControls/VRGraphicRaycaster.hpp"
#include "UnityEngine/UI/Selectable.hpp"
#include "UnityEngine/UI/LayoutElement.hpp"
#include "UnityEngine/UI/ContentSizeFitter.hpp"
#include "UnityEngine/UI/VerticalLayoutGroup.hpp"
#include "UnityEngine/UI/HorizontalLayoutGroup.hpp"
#include "UnityEngine/Events/UnityAction.hpp"
#include "TMPro/TextMeshProUGUI.hpp"
#include "Zenject/DiContainer.hpp"

using namespace CustomBackgrounds;
DEFINE_CLASS(BackgroundEnvViewController);

Zenject::DiContainer* diContainer = nullptr;
GlobalNamespace::EnvironmentOverrideSettingsPanelController* envPanel = nullptr;

void FixModalView(HMUI::SimpleTextDropdown* dropdown)
{
    // lifted from questui as dependency injection weird
    auto* physicsRaycaster = QuestUI::BeatSaberUI::GetPhysicsRaycasterWithCache();
    if (physicsRaycaster) reinterpret_cast<VRUIControls::VRGraphicRaycaster*>(dropdown->GetComponentInChildren(csTypeOf(VRUIControls::VRGraphicRaycaster*), true))->physicsRaycaster = physicsRaycaster;
    if (!diContainer) diContainer = QuestUI::ArrayUtil::First(UnityEngine::Resources::FindObjectsOfTypeAll<HMUI::TextSegmentedControl*>(), [](HMUI::TextSegmentedControl* x) { return to_utf8(csstrtostr(x->get_transform()->get_parent()->get_name())) == "PlayerStatisticsViewController" && x->container; })->container;
    reinterpret_cast<HMUI::ModalView*>(dropdown->GetComponentInChildren(csTypeOf(HMUI::ModalView*), true))->container = diContainer;
}

void OnToggleOverride()
{
    if (!envPanel) return;
    auto* dropdownArray = envPanel->get_gameObject()->GetComponentsInChildren<HMUI::SimpleTextDropdown*>();
    for (size_t i = 0; i < dropdownArray->Length(); i++)
    {
        HMUI::SimpleTextDropdown* dropdown = dropdownArray->values[i];
        if (!dropdown || dropdown->GetComponent<GlobalNamespace::NumberTag*>()) return;
        FixModalView(dropdown);
    }
}

void OnChangeHideEnv(bool newval)
{
    auto& modcfg = getConfig().config;
    modcfg["hideEnvironment"].SetBool(newval);
}

void OnChangeHideRings(bool newval)
{
    auto& modcfg = getConfig().config;
    modcfg["hideRings"].SetBool(newval);
}

void OnChangeHideLasers(bool newval)
{
    auto& modcfg = getConfig().config;
    modcfg["hideLasers"].SetBool(newval);
}

void BackgroundEnvViewController::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling)
{
    auto& modcfg = getConfig().config;
    if(firstActivation && addedToHierarchy) 
    {
        UnityEngine::UI::VerticalLayoutGroup* container = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(get_rectTransform());
        container->set_spacing(0.2f);

        // Title
        auto* titlecontainer = QuestUI::BeatSaberUI::CreateHorizontalLayoutGroup(container->get_rectTransform());
        titlecontainer->get_gameObject()->AddComponent<QuestUI::Backgroundable*>()->ApplyBackground(il2cpp_utils::createcsstr("panel-top"));
        titlecontainer->set_padding(UnityEngine::RectOffset::New_ctor(10, 10, 0, 0));
        titlecontainer->GetComponent<UnityEngine::UI::ContentSizeFitter*>()->set_horizontalFit(2);

        auto* titletext = QuestUI::BeatSaberUI::CreateText(titlecontainer->get_rectTransform(), "Environment", false);
        titletext->set_alignment(TMPro::TextAlignmentOptions::Center);
        titletext->set_fontSize(10);

        // Bool settings
        this->envcontainer = QuestUI::BeatSaberUI::CreateHorizontalLayoutGroup(container->get_rectTransform());
        envcontainer->set_padding(UnityEngine::RectOffset::New_ctor(0, 0, 0, 30));

        bool enabled_initval = getConfig().config["enabled"].GetBool();

        // Base game's 'Override Environment' panel
        auto* playerData = UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::PlayerDataModel*>()->values[0]->playerData;
        auto* _envpanelref = UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::EnvironmentOverrideSettingsPanelController*>()->values[0];
        envPanel = UnityEngine::Object::Instantiate(_envpanelref, this->envcontainer->get_rectTransform());
        auto* envtoggle = envPanel->overrideEnvironmentsToggle;
        envPanel->get_gameObject()->SetActive(true);
        envPanel->SetData(playerData->get_overrideEnvironmentSettings());
        envPanel->Refresh();
        if (!envtoggle->m_IsOn) QuestUI::BeatSaberUI::AddHoverHint(envtoggle->get_gameObject(), "Please activate this setting via the 'Gameplay Setup' panel.");
        envPanel->GetComponentInChildren<UnityEngine::UI::VerticalLayoutGroup*>()->set_childAlignment(UnityEngine::TextAnchor::UpperRight);
        auto* dropdownarray = envPanel->get_gameObject()->GetComponentsInChildren<HMUI::SimpleTextDropdown*>();
        for (size_t i = 0; i < dropdownarray->Length(); i++)
        {
            HMUI::SimpleTextDropdown* dropdown = dropdownarray->values[i];
            dropdown->get_gameObject()->AddComponent<GlobalNamespace::NumberTag*>();
            FixModalView(dropdown);
        }

        auto* togglecontainer = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(container->get_rectTransform());
        togglecontainer->set_padding(UnityEngine::RectOffset::New_ctor(0, 0, 0, 4));
        togglecontainer->set_spacing(0.2f);

        bool hideenv_initval = modcfg["hideEnvironment"].GetBool();
        auto* hideEnvToggle = QuestUI::BeatSaberUI::CreateToggle(togglecontainer->get_rectTransform(), "Hide Environment", hideenv_initval, UnityEngine::Vector2(0, 0), OnChangeHideEnv);

        bool hiderings_initval = modcfg["hideRings"].GetBool();
        auto* hideRingsToggle = QuestUI::BeatSaberUI::CreateToggle(togglecontainer->get_rectTransform(), "Hide Rings", hiderings_initval, UnityEngine::Vector2(0, 0), OnChangeHideRings);

        bool hidelasers_initval = modcfg["hideLasers"].GetBool();
        auto* hideLasersToggle = QuestUI::BeatSaberUI::CreateToggle(togglecontainer->get_rectTransform(), "Hide Lights", hidelasers_initval, UnityEngine::Vector2(0, 0), OnChangeHideLasers);

    }
    envPanel->overrideEnvironmentsToggle->set_interactable(envPanel->overrideEnvironmentsToggle->m_IsOn);
}

void BackgroundEnvViewController::DidDeactivate(bool removedFromHierarchy, bool systemScreenDisabling)
{
    getConfig().Write();
}