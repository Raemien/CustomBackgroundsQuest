#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"

#include "main.hpp"
#include "EmbeddedContent.hpp"

#include "UnityEngine/AssetBundle.hpp"
#include "UnityEngine/Material.hpp"
#include "UnityEngine/Object.hpp"
#include "System/Convert.hpp"
#include "System/Type.hpp"

UnityEngine::Material* LoadSkyMaterial()
{
    // Load asset bundle from Base64 string
    Array<uint8_t>* bytes = System::Convert::FromBase64String(il2cpp_utils::newcsstr(FILE_SKYASSET));
    static auto ab_LoadFromMemory = reinterpret_cast<function_ptr_t<UnityEngine::AssetBundle*, Array<uint8_t>*, uint>>(il2cpp_functions::resolve_icall("UnityEngine.AssetBundle::LoadFromMemory_Internal"));
    UnityEngine::AssetBundle* bundle = ab_LoadFromMemory(bytes, 0);

    // Load first material from assetbundle
    static auto ab_LoadAllAssets = reinterpret_cast<function_ptr_t<Array<UnityEngine::Object*>*, UnityEngine::AssetBundle*, Il2CppString*, System::Type*>>(il2cpp_functions::resolve_icall("UnityEngine.AssetBundle::LoadAssetWithSubAssets_Internal"));
    Array<UnityEngine::Object*>* materialArray = ab_LoadAllAssets(bundle, il2cpp_utils::newcsstr(""), csTypeOf(UnityEngine::Material*));
    if (materialArray->Length() < 1) return nullptr;
    return (UnityEngine::Material*)materialArray->values[0];
}