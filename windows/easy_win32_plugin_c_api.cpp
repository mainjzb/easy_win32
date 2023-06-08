#include "include/easy_win32/easy_win32_plugin_c_api.h"

#include <flutter/plugin_registrar_windows.h>

#include "easy_win32_plugin.h"

void EasyWin32PluginCApiRegisterWithRegistrar(
    FlutterDesktopPluginRegistrarRef registrar) {
  easy_win32::EasyWin32Plugin::RegisterWithRegistrar(
      flutter::PluginRegistrarManager::GetInstance()
          ->GetRegistrar<flutter::PluginRegistrarWindows>(registrar));
}
