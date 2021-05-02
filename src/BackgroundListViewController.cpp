#include "main.hpp"
#include "EnvironmentHider.hpp"
#include "BackgroundListViewController.hpp"

#include <dirent.h>
#include <list>

#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/CustomTypes/Components/Backgroundable.hpp"
#include "questui/shared/CustomTypes/Components/ExternalComponents.hpp"

#include "UnityEngine/Object.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/RectTransform.hpp"
#include "UnityEngine/UI/Button.hpp"
#include "UnityEngine/UI/LayoutElement.hpp"
#include "UnityEngine/UI/VerticalLayoutGroup.hpp"
#include "UnityEngine/UI/HorizontalLayoutGroup.hpp"
#include "UnityEngine/Events/UnityAction.hpp"
#include "TMPro/TextMeshProUGUI.hpp"

using namespace CustomBackgrounds;
DEFINE_TYPE(BackgroundListViewController);

BackgroundListViewController* ListView;
std::list<UnityEngine::UI::Button*> bgList = {};

void OnChangeEnabled(bool newval)
{
    auto& modcfg = getConfig().config;
    modcfg["enabled"].SetBool(newval);
    ListView->bglistscroll->SetActive(newval);
    if (backgroundObject) backgroundObject->SetActive(newval);
    else LoadBackground(bgDirectoryPath + modcfg["selectedFile"].GetString());
    HideMenuEnv();
}

void SelectImage()
{
    for (UnityEngine::UI::Button* button : bgList)
    {
        if (button->hasSelection) 
        {
            std::string filename = to_utf8(csstrtostr(button->GetComponentInChildren<TMPro::TextMeshProUGUI*>()->get_text()));
            getConfig().config["selectedFile"].SetString(filename, getConfig().config.GetAllocator());
            getConfig().Write();
            LoadBackground(bgDirectoryPath + filename);
        }
    }
}

void RefreshList()
{
    if (ListView->listtxtgroup && ListView->listtxtgroup->m_CachedPtr.m_value) UnityEngine::GameObject::Destroy(ListView->listtxtgroup->get_gameObject()); 
    for (UnityEngine::UI::Button* button : bgList) UnityEngine::Object::Destroy(button->get_transform()->get_parent()->get_gameObject());
    bgList = {};
    DIR* imgdir = opendir(bgDirectoryPath.c_str());
    dirent* fileent;
    while ((fileent = readdir(imgdir)) != NULL)
    {
        std::string filename = fileent->d_name;
        for (char &ch : filename) ch = tolower(ch);
        
        if (filename.ends_with(".png") || filename.ends_with(".jpg") || filename.ends_with(".jpeg"))
        {
            UnityEngine::UI::HorizontalLayoutGroup* rowgroup = QuestUI::BeatSaberUI::CreateHorizontalLayoutGroup(ListView->bglistscroll->get_transform());
            UnityEngine::UI::Button* button = QuestUI::BeatSaberUI::CreateUIButton(rowgroup->get_rectTransform(), fileent->d_name, SelectImage);
            button->get_gameObject()->GetComponentInChildren<TMPro::TextMeshProUGUI*>()->set_fontStyle(2);
            bgList.push_back(button);
        }
    }
    if (bgList.size() == 0)
    {
        ListView->listtxtgroup = QuestUI::BeatSaberUI::CreateHorizontalLayoutGroup(ListView->bglistscroll->get_transform());
        QuestUI::BeatSaberUI::CreateText(ListView->listtxtgroup->get_rectTransform(), "No background images were found!\nPlease install a background to continue.", false);
    }
}

void BackgroundListViewController::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling)
{
    ListView = this;
    if(firstActivation && addedToHierarchy) 
    {
        UnityEngine::UI::VerticalLayoutGroup* container = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(get_rectTransform());
        container->set_spacing(0.4f);
        container->GetComponent<UnityEngine::UI::LayoutElement*>()->set_minWidth(25.0);

        // Bool settings
        this->configcontainer = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(container->get_rectTransform());
        // configcontainer->get_gameObject()->AddComponent<QuestUI::Backgroundable*>()->ApplyBackground(il2cpp_utils::createcsstr("round-rect-panel"));
        configcontainer->set_childAlignment(UnityEngine::TextAnchor::UpperCenter);
        configcontainer->set_childForceExpandHeight(false);
        configcontainer->set_childControlHeight(true);

        configcontainer->set_childForceExpandHeight(false);
        configcontainer->set_childControlHeight(true);

        bool enabled_initval = getConfig().config["enabled"].GetBool();
        this->masterEnabled = QuestUI::BeatSaberUI::CreateToggle(configcontainer->get_rectTransform(), "Enable Custom Backgrounds", enabled_initval, UnityEngine::Vector2(0, 0), OnChangeEnabled);

        // Image List (recursively adds buttons as ListView isn't an easy type to deal with)
        this->bglistscroll = QuestUI::BeatSaberUI::CreateScrollView(container->get_rectTransform());
        bglistscroll->GetComponent<QuestUI::ExternalComponents*>()->Get<UnityEngine::UI::LayoutElement*>()->set_minHeight(56.0);
        auto* bglistcontainer = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(bglistscroll->get_transform());
        // bglistcontainer->get_gameObject()->AddComponent<QuestUI::Backgroundable*>()->ApplyBackground(il2cpp_utils::createcsstr("round-rect-panel"));
        bglistcontainer->set_childAlignment(UnityEngine::TextAnchor::UpperCenter);
        bglistcontainer->set_childForceExpandHeight(false);
        bglistcontainer->set_childControlHeight(true);

        this->bglistscroll->get_gameObject()->SetActive(enabled_initval);
    }
    RefreshList();
}

void BackgroundListViewController::DidDeactivate(bool removedFromHierarchy, bool systemScreenDisabling)
{
    getConfig().Write();
    for (UnityEngine::UI::Button* button : bgList) UnityEngine::Object::Destroy(button->get_transform()->get_parent()->get_gameObject());
    bgList = {};
}