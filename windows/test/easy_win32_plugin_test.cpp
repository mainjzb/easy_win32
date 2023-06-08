#include <flutter/method_call.h>
#include <flutter/method_result_functions.h>
#include <flutter/standard_method_codec.h>
#include <gtest/gtest.h>
#include <windows.h>

#include <memory>
#include <string>
#include <variant>
#include <map>

#include "easy_win32_plugin.h"

namespace easy_win32 {
namespace test {

namespace {

using flutter::EncodableMap;
using flutter::EncodableValue;
using flutter::MethodCall;
using flutter::MethodResultFunctions;

}  // namespace


TEST(EasyWin32Plugin, GetPlatformVersion) {
  EasyWin32Plugin plugin;
  // Save the reply value from the success callback.
  std::string result_string;
  plugin.HandleMethodCall(
      MethodCall("getPlatformVersion", std::make_unique<EncodableValue>()),
      std::make_unique<MethodResultFunctions<>>(
          [&result_string](const EncodableValue* result) {
            result_string = std::get<std::string>(*result);
          },
          nullptr, nullptr));


  EncodableMap dataMap;
  plugin.HandleMethodCall(
      MethodCall("getInterfaceEntry", std::make_unique<EncodableValue>()),
      std::make_unique<MethodResultFunctions<>>(
          [&dataMap](const EncodableValue* result) {
            dataMap = std::get<EncodableMap>(*result);
          },
          nullptr, nullptr));

  EXPECT_TRUE(std::get<int>(dataMap[EncodableValue("err")]) == 0);
  // Since the exact string varies by host, just ensure that it's a string
  // with the expected format.
  EXPECT_TRUE(result_string.rfind("Windows ", 0) == 0);
}

}  // namespace test
}  // namespace easy_win32
