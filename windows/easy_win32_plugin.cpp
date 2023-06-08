
#include "easy_win32_plugin.h"

// This must be included before many other Windows headers.

#include <Windows.h>

// For getPlatformVersion; remove unless needed for your plugin implementation.
#include <VersionHelpers.h>

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <flutter/standard_method_codec.h>

#include <memory>
#include <sstream>
#include "network.h"

namespace {
    using flutter::EncodableMap;
    using flutter::EncodableValue;
    using flutter::MethodCall;
}  // namespace

namespace easy_win32 {

// static
void EasyWin32Plugin::RegisterWithRegistrar(
    flutter::PluginRegistrarWindows *registrar) {
  auto channel =
      std::make_unique<flutter::MethodChannel<flutter::EncodableValue>>(
          registrar->messenger(), "easy_win32",
          &flutter::StandardMethodCodec::GetInstance());

  auto plugin = std::make_unique<EasyWin32Plugin>();

  channel->SetMethodCallHandler(
      [plugin_pointer = plugin.get()](const auto &call, auto result) {
        plugin_pointer->HandleMethodCall(call, std::move(result));
      });

  registrar->AddPlugin(std::move(plugin));
}

EasyWin32Plugin::EasyWin32Plugin() {}

EasyWin32Plugin::~EasyWin32Plugin() {}

void EasyWin32Plugin::HandleMethodCall(
    const flutter::MethodCall<flutter::EncodableValue> &method_call,
    std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
  if (method_call.method_name().compare("getPlatformVersion") == 0) {
    std::ostringstream version_stream;
    version_stream << "Windows ";
    if (IsWindows10OrGreater()) {
      version_stream << "10+";
    } else if (IsWindows8OrGreater()) {
      version_stream << "8";
    } else if (IsWindows7OrGreater()) {
      version_stream << "7";
    }
    result->Success(flutter::EncodableValue(version_stream.str()));

  } else if (method_call.method_name().compare("getDefaultInterfaceIndex") == 0){
    auto [ifIndex,err] = getDefaultInterfaceIndex();
    if( err != 0 ) {
      result->Error(std::to_string(err),"getDefaultInterfaceIndex error");
      return;
    }
    result->Success(flutter::EncodableValue(int(ifIndex)));

  } else if(method_call.method_name().compare("getInterfaceEntry") == 0){
      getInterfaceEntry(22);
  } else {
    result->NotImplemented();
  }
}

}  // namespace easy_win32




