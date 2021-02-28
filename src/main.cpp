#include "BGConfig.hpp"
#include "EnvironmentHider.hpp"
#include "BackgroundEnvViewController.hpp"
#include "BackgroundListViewController.hpp"
#include "BackgroundConfigViewController.hpp"
#include "BackgroundsFlowCoordinator.hpp"
using namespace CustomBackgrounds;

#include "modloader/shared/modloader.hpp"
#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp" 
#include "beatsaber-hook/shared/utils/typedefs.h"
#include "beatsaber-hook/shared/config/config-utils.hpp"
#include "bs-utils/shared/utils.hpp"

#include <sys/stat.h>
#include <fstream>

#include "questui/shared/QuestUI.hpp"
#include "custom-types/shared/register.hpp"

#include "UnityEngine/Color.hpp"
#include "UnityEngine/Camera.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Object.hpp"
#include "UnityEngine/Resources.hpp"
#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/Renderer.hpp"
#include "UnityEngine/Material.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/Shader.hpp"
#include "UnityEngine/Texture2D.hpp"
#include "UnityEngine/TextureFormat.hpp"
#include "UnityEngine/ImageConversion.hpp"
#include "UnityEngine/PrimitiveType.hpp"
#include "UnityEngine/SceneManagement/SceneManager.hpp"
#include "UnityEngine/SceneManagement/Scene.hpp"
#include "GlobalNamespace/MultiplayerLobbyAvatarPlace.hpp"
#include "GlobalNamespace/MenuEnvironmentManager.hpp"
#include "GlobalNamespace/MenuEnvironmentManager_MenuEnvironmentObjects.hpp"
#include "GlobalNamespace/MainCamera.hpp"

using namespace GlobalNamespace;

UnityEngine::GameObject* backgroundObject;
UnityEngine::Material* backgroundMat;
UnityEngine::Texture2D* backgroundTexture;
long mainOriginalCullMask = 0; // Store the default culling mask to easily revert changes

static ModInfo modInfo;

std::string bgDirectoryPath;

Logger& getLogger() {
    static Logger* logger = new Logger(modInfo, LoggerOptions(false, true));
    return *logger;
}

Configuration& getConfig() {
    static Configuration config(modInfo);
    return config;
}

void CreateBGObject()
{
    if (!backgroundObject)
    {
        // Create object
        backgroundObject = UnityEngine::GameObject::CreatePrimitive(UnityEngine::PrimitiveType::Sphere);
        backgroundObject->set_name(il2cpp_utils::createcsstr("_CustomBackground"));
        backgroundObject->set_layer(29);
        UnityEngine::Object::DontDestroyOnLoad(backgroundObject);

        // Material + shader management
        UnityEngine::Renderer* bgrenderer = backgroundObject->GetComponent<UnityEngine::Renderer*>();
        backgroundMat = bgrenderer->get_material();
        bgrenderer->set_sortingOrder(-8192);
        static auto set_shader = reinterpret_cast<function_ptr_t<void, UnityEngine::Material*, UnityEngine::Shader*>>(il2cpp_functions::resolve_icall("UnityEngine.Material::set_shader"));
        set_shader(backgroundMat, UnityEngine::Shader::Find(il2cpp_utils::createcsstr("Custom/SimpleTexture")));
        backgroundMat->SetTexture(il2cpp_utils::createcsstr("_MainTex"), backgroundTexture);

        // Set transforms
        UnityEngine::Transform* bgtrans = backgroundObject->get_transform();
        bgtrans->set_localScale(UnityEngine::Vector3::get_one() * -800);
        bgtrans->set_localPosition(UnityEngine::Vector3::get_zero());
        bgtrans->set_localEulerAngles(UnityEngine::Vector3(0, (getConfig().config["rotationOffset"].GetInt() - 90), 180));
    }
}

void LoadBackground(std::string path)
{
    std::string filename = path.substr(path.find_last_of("/"));
    if (fileexists(path))
    {
        std::ifstream instream(path, std::ios::in | std::ios::binary);
        std::vector<uint8_t> data((std::istreambuf_iterator<char>(instream)), std::istreambuf_iterator<char>());
        Array<uint8_t>* bytes = il2cpp_utils::vectorToArray(data);

        backgroundTexture = UnityEngine::Texture2D::New_ctor(4096, 2048, UnityEngine::TextureFormat::RGBA32, false, false);
        bool success = UnityEngine::ImageConversion::LoadImage(backgroundTexture, bytes, false);
        std::string resulttxt = success ? "[CustomBackgrounds] successfully loaded '" + filename + "'." : "[CustomBackgrounds] failed to load '" + filename + "'.";
        getLogger().info(resulttxt);

        if (backgroundMat && success) {
            backgroundMat->SetTexture(il2cpp_utils::createcsstr("_MainTex"), backgroundTexture);
        }
        else if (success) CreateBGObject();
    }
}

void InitBackgrounds()
{
    auto& modcfg = getConfig().config;
    int success = mkdir(bgDirectoryPath.c_str(), 0777);
    std::string backgroundPath = bgDirectoryPath;
    backgroundPath += modcfg["selectedFile"].GetString();

    if (success != 0 && fileexists(backgroundPath))
    {
        LoadBackground(backgroundPath);
    }
}

MAKE_HOOK_OFFSETLESS(SceneManager_SceneChanged, void, UnityEngine::SceneManagement::Scene previousScene, UnityEngine::SceneManagement::Scene nextScene)
{
    SceneManager_SceneChanged(previousScene, nextScene);
    std::__ndk1::string_view nextSceneName = to_utf8(csstrtostr(nextScene.get_name()));
    auto& modcfg = getConfig().config;
    if ((nextSceneName == "HealthWarning" || nextSceneName == "MenuViewControllers" ) && !backgroundObject && modcfg["enabled"].GetBool())
    {
        InitBackgrounds();
        CreateBGObject();
    }
}

MAKE_HOOK_OFFSETLESS(Spectrogram_Awake, void)
{
    Spectrogram_Awake();
    if (backgroundObject) HideGameEnv();
}

MAKE_HOOK_OFFSETLESS(MainCamera_Awake, void, GlobalNamespace::MainCamera* caminstance)
{
    MainCamera_Awake(caminstance);
    auto& modcfg = getConfig().config;
    std::__ndk1::string_view sceneName = to_utf8(csstrtostr(UnityEngine::SceneManagement::SceneManager::GetActiveScene().get_name()));
    UnityEngine::Camera* maincam = caminstance->camera;
    if (maincam && sceneName == "GameCore" && modcfg["enabled"].GetBool())
    {
        mainOriginalCullMask = (mainOriginalCullMask == 0) ? maincam->get_cullingMask() : mainOriginalCullMask;
        if (modcfg["hideEnvironment"].GetBool()) maincam->set_cullingMask(maincam->get_cullingMask() & ~(1 << 14));
        if (modcfg["hideLasers"].GetBool()) maincam->set_cullingMask(maincam->get_cullingMask() & ~(1 << 13));
    }
}

MAKE_HOOK_OFFSETLESS(MenuEnvManager_ShowEnv, void, GlobalNamespace::MenuEnvironmentManager* instance, GlobalNamespace::MenuEnvironmentManager::MenuEnvironmentType envtype)
{
    MenuEnvManager_ShowEnv(instance, envtype);
    if (backgroundObject) HideMenuEnv();
}

extern "C" void setup(ModInfo& info) {

    info.id = "CustomBackgrounds";
    info.version = "1.0.0";
    modInfo = info;
    bgDirectoryPath = bs_utils::getDataDir(info);
    getConfig().Load();
}

extern "C" void load() {
    if (!LoadConfig()) SetupConfig();
    il2cpp_functions::Init();
    QuestUI::Init();
    INSTALL_HOOK_OFFSETLESS(getLogger(), SceneManager_SceneChanged, il2cpp_utils::FindMethodUnsafe("UnityEngine.SceneManagement", "SceneManager", "Internal_ActiveSceneChanged", 2));
    INSTALL_HOOK_OFFSETLESS(getLogger(), MenuEnvManager_ShowEnv, il2cpp_utils::FindMethodUnsafe("", "MenuEnvironmentManager", "ShowEnvironmentType", 1));
    INSTALL_HOOK_OFFSETLESS(getLogger(), Spectrogram_Awake, il2cpp_utils::FindMethod("", "Spectrogram", "Awake"));
    INSTALL_HOOK_OFFSETLESS(getLogger(), MainCamera_Awake, il2cpp_utils::FindMethod("", "MainCamera", "Awake"));
    custom_types::Register::RegisterTypes<::BackgroundsFlowCoordinator, ::BackgroundListViewController, ::BackgroundConfigViewController, ::BackgroundEnvViewController>();
    QuestUI::Register::RegisterModSettingsFlowCoordinator<BackgroundsFlowCoordinator*>(modInfo);
}
