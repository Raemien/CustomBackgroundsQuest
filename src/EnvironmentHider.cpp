#include "main.hpp"
#include "UnityEngine/Object.hpp"
#include "UnityEngine/Renderer.hpp"
#include "UnityEngine/Resources.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/MeshRenderer.hpp"
#include "GlobalNamespace/MultiplayerLobbyAvatarPlace.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp" 
#include "beatsaber-hook/shared/utils/typedefs.h"
#include "beatsaber-hook/shared/config/config-utils.hpp"
#include "GlobalNamespace/SetApplicationVersionText.hpp"

#include "GlobalNamespace/MenuEnvironmentManager.hpp"
#include "GlobalNamespace/MenuEnvironmentManager_MenuEnvironmentObjects.hpp"

#include <regex>
#include <list>

using namespace UnityEngine;

bool getBGActive()
{
    return (backgroundObject && backgroundObject->get_activeSelf());
}

void HideChildRenderers(GameObject* obj, bool onlymeshes, bool unhide = false)
{
    if (obj == nullptr) return;
    bool enabled = !getBGActive() || unhide;
    auto* rendarray = onlymeshes ? (Array<Renderer*>*)obj->GetComponentsInChildren<MeshRenderer*>() : obj->GetComponentsInChildren<Renderer*>();
    for (size_t i = 0; i < rendarray->Length(); i++)
    {
        Renderer* renderer = (Renderer*)rendarray->values[i];
        if (!renderer || !renderer->m_CachedPtr) continue;
        renderer->set_enabled(enabled);
    }
}

void HideChildLights(GameObject* obj)
{
    if (obj == nullptr) return;
    auto* rendarray = obj->GetComponentsInChildren<Renderer*>();
    for (size_t i = 0; i < rendarray->Length(); i++)
    {
        Renderer* renderer = (Renderer*)rendarray->values[i];
        if (!renderer || !renderer->m_CachedPtr) continue;
        if (!std::regex_search(to_utf8(csstrtostr(renderer->get_name())), std::regex("light|bloom", std::regex::icase))) continue;
        renderer->get_gameObject()->set_layer(13);
    }
}

void HideMenuEnv()
{
    bool ismulti = GameObject::Find(il2cpp_utils::createcsstr("MenuEnvironment"))->GetComponent<GlobalNamespace::MenuEnvironmentManager*>()->prevMenuEnvironmentType == (int)2;
    bool bgActive = getBGActive();
    if (!ismulti)
    {
        // Find Objects
        auto* floorObj = GameObject::Find(il2cpp_utils::createcsstr("MenuEnvironment/DefaultEnvironment/Ground"));
        auto* notesObj = GameObject::Find(il2cpp_utils::createcsstr("MenuEnvironment/DefaultEnvironment/Notes"));
        auto* notePileObj = GameObject::Find(il2cpp_utils::createcsstr("MenuEnvironment/DefaultEnvironment/NotesBehindPlayer"));
        auto* leftBuildObj = GameObject::Find(il2cpp_utils::createcsstr("MenuEnvironment/NearBuildingLeft"));
        auto* rightBuildObj = GameObject::Find(il2cpp_utils::createcsstr("MenuEnvironment/NearBuildingRight"));
        auto* multiEnvObj = Resources::FindObjectsOfTypeAll<GlobalNamespace::SetApplicationVersionText*>()->values[1]->get_transform()->get_parent()->get_parent()->get_gameObject();
        
        // Apply Visibility
        multiEnvObj->SetActive(bgActive);
        HideChildRenderers(notesObj, false);
        HideChildRenderers(notePileObj, false);
        HideChildRenderers(leftBuildObj, false);
        HideChildRenderers(rightBuildObj, false);

        floorObj->GetComponent<MeshRenderer*>()->set_enabled(!bgActive);
    }
}

void HideGameEnv()
{
    GameObject* environmentObj = GameObject::Find(il2cpp_utils::createcsstr("Environment"));
    GameObject* playersPlaceObj = GameObject::Find(il2cpp_utils::createcsstr("Environment/PlayersPlace"));
    
    HideChildLights(environmentObj);
    if(!getConfig().config["hideEnvironment"].GetBool()) return;
    HideChildRenderers(environmentObj, true);
    HideChildRenderers(playersPlaceObj, true, true);
}