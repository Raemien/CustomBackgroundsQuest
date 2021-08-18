# Builds a .zip file for loading with BMBF/QP
$NDKPath = Get-Content $PSScriptRoot/ndkpath.txt

$buildScript = "$NDKPath/build/ndk-build"
if (-not ($PSVersionTable.PSEdition -eq "Core")) {
    $buildScript += ".cmd"
}

& $buildScript NDK_PROJECT_PATH=$PSScriptRoot APP_BUILD_SCRIPT=$PSScriptRoot/Android.mk NDK_APPLICATION_MK=$PSScriptRoot/Application.mk -j 4
Compress-Archive -Path  "./libs/arm64-v8a/libbeatsaber-hook_2_2_5.so",`
                        "./libs/arm64-v8a/libcustombgs.so",`
                        "./cover.png",`
                        "./mod.json" -DestinationPath "./CustomBackgrounds.zip" -Update

& copy-item -Force "./CustomBackgrounds.zip" "./CustomBackgrounds.qmod"
