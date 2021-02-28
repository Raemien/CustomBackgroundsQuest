#include "main.hpp"
#include "BackgroundConfigViewController.hpp"

#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/CustomTypes/Components/Backgroundable.hpp"
#include "questui/shared/CustomTypes/Components/Settings/IncrementSetting.hpp"

#include "UnityEngine/Object.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/RectTransform.hpp"
#include "UnityEngine/RectOffset.hpp"
#include "HMUI/ViewController.hpp"
#include "UnityEngine/UI/Selectable.hpp"
#include "UnityEngine/UI/LayoutElement.hpp"
#include "UnityEngine/UI/ContentSizeFitter.hpp"
#include "UnityEngine/UI/VerticalLayoutGroup.hpp"
#include "UnityEngine/UI/HorizontalLayoutGroup.hpp"
#include "UnityEngine/Events/UnityAction.hpp"
#include "TMPro/TextMeshProUGUI.hpp"

using namespace CustomBackgrounds;
DEFINE_CLASS(BackgroundConfigViewController);

void OnChangeHideEnv(BackgroundConfigViewController* instance, bool newval)
{
    auto& modcfg = getConfig().config;
    modcfg["hideEnvironment"].SetBool(newval);
}

void OnChangeHideLasers(BackgroundConfigViewController* instance, bool newval)
{
    auto& modcfg = getConfig().config;
    modcfg["hideLasers"].SetBool(newval);
}

void OnChangeRotation(BackgroundConfigViewController* instance, float newval)
{
    auto& modcfg = getConfig().config;
    auto* element = instance->rotationSetting;
    if (newval < 0) newval = (360 - element->Increment);
    if (newval > 359) newval = 0;
    element->CurrentValue = newval;
    element->Text->set_text(il2cpp_utils::createcsstr(std::to_string((int)newval)));
    modcfg["rotationOffset"].SetFloat(newval);
    if (backgroundObject) backgroundObject->get_transform()->set_localEulerAngles(UnityEngine::Vector3(0, (newval - 90), 180));
}

void BackgroundConfigViewController::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling)
{
    if(firstActivation && addedToHierarchy) 
    {
        UnityEngine::UI::VerticalLayoutGroup* container = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(get_rectTransform());
        container->set_spacing(0.2f);
        container->GetComponent<UnityEngine::UI::LayoutElement*>()->set_minWidth(25.0);

        // Title
        auto* titlecontainer = QuestUI::BeatSaberUI::CreateHorizontalLayoutGroup(container->get_rectTransform());
        titlecontainer->get_gameObject()->AddComponent<QuestUI::Backgroundable*>()->ApplyBackground(il2cpp_utils::createcsstr("panel-top"));
        titlecontainer->set_padding(UnityEngine::RectOffset::New_ctor(10, 10, 0, 0));
        titlecontainer->GetComponent<UnityEngine::UI::ContentSizeFitter*>()->set_horizontalFit(2);

        auto* titletext = QuestUI::BeatSaberUI::CreateText(titlecontainer->get_rectTransform(), "Settings", false);
        titletext->set_alignment(TMPro::TextAlignmentOptions::Center);
        titletext->set_fontSize(10);

        // Bool settings
        this->configcontainer = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(container->get_rectTransform());
        configcontainer->set_childAlignment(UnityEngine::TextAnchor::UpperCenter);
        configcontainer->set_childForceExpandHeight(false);
        configcontainer->set_childControlHeight(true);

        bool enabled_initval = getConfig().config["enabled"].GetBool();

        bool hideenv_initval = getConfig().config["hideEnvironment"].GetBool();
        auto onChangeHideEnvAction = il2cpp_utils::MakeDelegate<UnityEngine::Events::UnityAction_1<bool>*>(classof(UnityEngine::Events::UnityAction_1<bool>*), this, OnChangeHideEnv);
        this->hideEnvToggle = QuestUI::BeatSaberUI::CreateToggle(configcontainer->get_rectTransform(), "Hide Environment", hideenv_initval, UnityEngine::Vector2(0, 0), onChangeHideEnvAction);

        bool hidelasersinitval = getConfig().config["hideLasers"].GetBool();
        auto onChangeHideLasersAction = il2cpp_utils::MakeDelegate<UnityEngine::Events::UnityAction_1<bool>*>(classof(UnityEngine::Events::UnityAction_1<bool>*), this, OnChangeHideLasers);
        this->hideEnvToggle = QuestUI::BeatSaberUI::CreateToggle(configcontainer->get_rectTransform(), "Hide Lights", hidelasersinitval, UnityEngine::Vector2(0, 0), onChangeHideLasersAction);


        int rotation_initval = getConfig().config["rotationOffset"].GetInt();
        auto onChangeRotationAction = il2cpp_utils::MakeDelegate<UnityEngine::Events::UnityAction_1<float>*>(classof(UnityEngine::Events::UnityAction_1<float>*), this, OnChangeRotation);
        this->rotationSetting = QuestUI::BeatSaberUI::CreateIncrementSetting(configcontainer->get_rectTransform(), "Rotation Offset", 0, (int)15, rotation_initval, onChangeRotationAction);;
    }
}

void BackgroundConfigViewController::DidDeactivate(bool removedFromHierarchy, bool systemScreenDisabling)
{
    getConfig().Write();
}