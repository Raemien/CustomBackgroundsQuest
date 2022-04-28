#include "BGConfig.hpp"
#include "EnvironmentHider.hpp"
#include "Helpers/AssetHelper.hpp"
#include "UI/BackgroundEnvViewController.hpp"
#include "UI/BackgroundListViewController.hpp"
#include "UI/BackgroundConfigViewController.hpp"
#include "UI/BackgroundsFlowCoordinator.hpp"
using namespace CustomBackgrounds;

#include "modloader/shared/modloader.hpp"
#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "beatsaber-hook/shared/utils/typedefs.h"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "beatsaber-hook/shared/config/config-utils.hpp"

#include <sys/stat.h>
#include <fstream>

#include "questui/shared/QuestUI.hpp"
#include "custom-types/shared/register.hpp"

#include "UnityEngine/Color.hpp"
#include "UnityEngine/Camera.hpp"
#include "UnityEngine/Component.hpp"
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
#include "GlobalNamespace/TrackLaneRing.hpp"
#include "GlobalNamespace/Spectrogram.hpp"
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
        backgroundObject->set_name("_CustomBackground");
        backgroundObject->set_layer(29);
        UnityEngine::Object::DontDestroyOnLoad(backgroundObject);

        // Material + shader management
        UnityEngine::Renderer* bgrenderer = backgroundObject->GetComponent<UnityEngine::Renderer*>();
        bgrenderer->set_sortingOrder(-8192);
        bgrenderer->set_material(LoadSkyMaterial());
        backgroundMat = bgrenderer->get_material();
        backgroundMat->SetTexture("_MainTex", backgroundTexture);

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

        int width = getConfig().config["imageWidth"].GetInt();
        int height = std::floor((float)width * (2.0f / 3.0f));
        UnityEngine::Component::Destroy(backgroundTexture);
        backgroundTexture = UnityEngine::Texture2D::New_ctor(width, height, UnityEngine::TextureFormat::RGBA32, false, false);
        bool success = UnityEngine::ImageConversion::LoadImage(backgroundTexture, bytes, false);
        std::string resulttxt = success ? "[CustomBackgrounds] successfully loaded '" + filename + "'." : "[CustomBackgrounds] failed to load '" + filename + "'.";
        getLogger().info("%s", resulttxt.c_str());

        if (backgroundMat && success) {
            backgroundMat->SetTexture("_MainTex", backgroundTexture);
        }
        else if (success) CreateBGObject();
    }
}

void InitBackgrounds()
{
    auto& modcfg = getConfig().config;
    std::string backgroundPath = bgDirectoryPath;
    backgroundPath += modcfg["selectedFile"].GetString();
    if (fileexists(backgroundPath))
    {
        LoadBackground(backgroundPath);
    }
}

MAKE_HOOK_MATCH(SceneManager_SceneChanged, &UnityEngine::SceneManagement::SceneManager::Internal_ActiveSceneChanged, void, UnityEngine::SceneManagement::Scene previousScene, UnityEngine::SceneManagement::Scene nextScene)
{
    SceneManager_SceneChanged(previousScene, nextScene);
    std::string nextSceneName = to_utf8(csstrtostr(nextScene.get_name()));
    auto& modcfg = getConfig().config;
    if ((nextSceneName == "HealthWarning" || nextSceneName == "MenuViewControllers" ) && !backgroundObject && modcfg["enabled"].GetBool())
    {
        InitBackgrounds();
        CreateBGObject();
    }
}

MAKE_HOOK_MATCH(TrackLaneRing_Init, &GlobalNamespace::TrackLaneRing::Init, void, GlobalNamespace::TrackLaneRing* instance, UnityEngine::Vector3 pos, UnityEngine::Vector3 offset)
{
    TrackLaneRing_Init(instance, pos, offset);
    bool hide = getConfig().config["hideRings"].GetBool() && getConfig().config["enabled"].GetBool();
    auto renderers = instance->GetComponentsInChildren<UnityEngine::Renderer*>();
    for (size_t i = 0; i < renderers->Length(); i++)
    {
        UnityEngine::Renderer* rend = renderers->values[i];
        rend->get_gameObject()->set_layer(hide ? 14 : 0);
        rend->set_enabled(!hide);
    }
}

MAKE_HOOK_MATCH(Spectrogram_Awake, &GlobalNamespace::Spectrogram::Awake, void, GlobalNamespace::Spectrogram* instance)
{
    Spectrogram_Awake(instance);
    if (backgroundObject && getConfig().config["enabled"].GetBool()) HideGameEnv();
}

MAKE_HOOK_MATCH(MainCamera_Awake, &GlobalNamespace::MainCamera::Awake, void, GlobalNamespace::MainCamera* caminstance)
{
    MainCamera_Awake(caminstance);
    auto& modcfg = getConfig().config;
    Il2CppString* sceneName = UnityEngine::SceneManagement::SceneManager::GetActiveScene().get_name();
    UnityEngine::Camera* maincam = caminstance->dyn__camera();
    if (maincam && sceneName == il2cpp_utils::newcsstr("GameCore") && modcfg["enabled"].GetBool())
    {
        mainOriginalCullMask = (mainOriginalCullMask == 0) ? maincam->get_cullingMask() : mainOriginalCullMask;
        if (modcfg["hideEnvironment"].GetBool()) maincam->set_cullingMask(maincam->get_cullingMask() & ~(1 << 14));
        if (modcfg["hideLasers"].GetBool()) maincam->set_cullingMask(maincam->get_cullingMask() & ~(1 << 13));
    }
}

MAKE_HOOK_MATCH(MenuEnvManager_ShowEnv, &GlobalNamespace::MenuEnvironmentManager::ShowEnvironmentType, void, GlobalNamespace::MenuEnvironmentManager* instance, GlobalNamespace::MenuEnvironmentManager::MenuEnvironmentType envtype)
{
    MenuEnvManager_ShowEnv(instance, envtype);
    if (backgroundObject) HideMenuEnv();
}

extern "C" void setup(ModInfo& info) {

    info.id = MOD_ID;
    info.version = "1.3.2";
    modInfo = info;
    bgDirectoryPath = getDataDir(info);
    (void)mkdir(bgDirectoryPath.c_str(), 0777);
    getConfig().Load();
}

extern "C" void load() {
    if (!LoadConfig()) SetupConfig();
    il2cpp_functions::Init();
    QuestUI::Init();
    INSTALL_HOOK(getLogger(), SceneManager_SceneChanged);
    INSTALL_HOOK(getLogger(), MenuEnvManager_ShowEnv);
    INSTALL_HOOK(getLogger(), TrackLaneRing_Init);
    INSTALL_HOOK(getLogger(), Spectrogram_Awake);
    INSTALL_HOOK(getLogger(), MainCamera_Awake);
    custom_types::Register::AutoRegister();
    QuestUI::Register::RegisterMainMenuModSettingsFlowCoordinator<BackgroundsFlowCoordinator*>(modInfo);
}
