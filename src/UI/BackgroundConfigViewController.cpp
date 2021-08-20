#include "main.hpp"
#include "UI/BackgroundConfigViewController.hpp"

#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/CustomTypes/Components/Backgroundable.hpp"
#include "questui/shared/CustomTypes/Components/Settings/IncrementSetting.hpp"

#include <map>

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
DEFINE_TYPE(CustomBackgrounds, BackgroundConfigViewController);

BackgroundConfigViewController* ConfigView;

void OnChangeResolution(float fval)
{
    std::map<int, std::string> resmap {{3840, "4k"}, {7680, "8K"}, {11520, "12K"}};
    int newval = std::round(fval);
    auto& modcfg = getConfig().config;
    auto* element = ConfigView->resolutionSetting;
    bool ismin = false;
    bool ismax = false;
    if (newval <= 3840) { // Minimum resoulution is 4k
        newval = 3840;
        ismin = true;
    }
    if (newval >= 11520) { // Maximum resolution is 12k
        newval = 11520; 
        ismax = true;
    }
    element->GetComponentsInChildren<UnityEngine::UI::Button*>()->values[0]->set_interactable(!ismin);
    element->GetComponentsInChildren<UnityEngine::UI::Button*>()->values[1]->set_interactable(!ismax);
    element->CurrentValue = newval;
    element->Increment = newval;
    if (resmap.count(newval) == 1) {
        element->Text->set_text(il2cpp_utils::createcsstr(resmap[newval]));
    }
    else element->Text->set_text(il2cpp_utils::createcsstr(std::to_string(newval) + " x " + std::to_string(std::round((float)newval * (2.0f / 3.0f)))));
    modcfg["imageWidth"].SetInt(newval);
}

void OnChangeRotation(float newval)
{
    auto& modcfg = getConfig().config;
    auto* element = ConfigView->rotationSetting;
    if (newval < 0) newval = (360 - element->Increment);
    if (newval > 359) newval = 0;
    element->CurrentValue = newval;
    element->Text->set_text(il2cpp_utils::createcsstr(std::to_string((int)newval)));
    modcfg["rotationOffset"].SetFloat(newval);
    if (backgroundObject) backgroundObject->get_transform()->set_localEulerAngles(UnityEngine::Vector3(0, (newval - 90), 180));
}

void BackgroundConfigViewController::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling)
{
    ConfigView = this;
    if(firstActivation && addedToHierarchy) 
    {
        auto& modcfg = getConfig().config;
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

        int rotation_initval = modcfg["rotationOffset"].GetInt();
        this->rotationSetting = QuestUI::BeatSaberUI::CreateIncrementSetting(configcontainer->get_rectTransform(), "Rotation Offset", 0, (int)15, rotation_initval, OnChangeRotation);;
        
        int width_initval = modcfg["imageWidth"].GetInt();
        this->resolutionSetting = QuestUI::BeatSaberUI::CreateIncrementSetting(configcontainer->get_rectTransform(), "Target Resolution", 0, width_initval, width_initval, OnChangeResolution);
        QuestUI::BeatSaberUI::AddHoverHint(this->resolutionSetting->get_gameObject(), "WARNING: This will affect load times!");
        OnChangeResolution(width_initval);
    }
}

void BackgroundConfigViewController::DidDeactivate(bool removedFromHierarchy, bool systemScreenDisabling)
{
    getConfig().Write();
}