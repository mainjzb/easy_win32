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
using flutter::EncodableList;
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

    // Since the exact string varies by host, just ensure that it's a string
    // with the expected format.
    EXPECT_TRUE(result_string.rfind("Windows ", 0) == 0);
}

TEST(EasyWin32Plugin, GetInterfaceIndexInfo) {
    EasyWin32Plugin plugin;

    int ifIndex;
    plugin.HandleMethodCall(
        MethodCall("getDefaultInterfaceIndex", std::make_unique<EncodableValue>()),
        std::make_unique<MethodResultFunctions<>>(
            [&ifIndex](const EncodableValue* result) {
                ifIndex = std::get<int>(*result);
            },
            nullptr, nullptr));

    EXPECT_TRUE(ifIndex != 0);

    EncodableMap adapterInfo;
    plugin.HandleMethodCall(
        MethodCall("getAdaptersAddresses", std::make_unique<EncodableValue>(ifIndex)),
        std::make_unique<MethodResultFunctions<>>(
            [&adapterInfo](const EncodableValue* result) {
                adapterInfo = std::get<EncodableMap>(*result);
            },
            nullptr, nullptr));
    auto gateway = std::get<EncodableList>(adapterInfo[EncodableValue("gateway")]);
    EXPECT_TRUE(gateway.size() == 1);
}

}  // namespace test
}  // namespace easy_win32
