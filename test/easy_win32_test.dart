import 'package:flutter_test/flutter_test.dart';
import 'package:easy_win32/easy_win32.dart';
import 'package:easy_win32/easy_win32_platform_interface.dart';
import 'package:easy_win32/easy_win32_method_channel.dart';
import 'package:plugin_platform_interface/plugin_platform_interface.dart';

class MockEasyWin32Platform
    with MockPlatformInterfaceMixin
    implements EasyWin32Platform {
  @override
  Future<String?> getPlatformVersion() => Future.value('42');

  @override
  Future<int> getDefaultInterfaceIP() => Future.value(12);
}

void main() {
  final EasyWin32Platform initialPlatform = EasyWin32Platform.instance;

  test('$MethodChannelEasyWin32 is the default instance', () {
    expect(initialPlatform, isInstanceOf<MethodChannelEasyWin32>());
  });

  test('getPlatformVersion', () async {
    EasyWin32 easyWin32Plugin = EasyWin32();
    MockEasyWin32Platform fakePlatform = MockEasyWin32Platform();
    EasyWin32Platform.instance = fakePlatform;

    expect(await easyWin32Plugin.getPlatformVersion(), '42');
  });

  test('getDefaultInterfaceIP', () async {
    EasyWin32 easyWin32Plugin = EasyWin32();
    MockEasyWin32Platform fakePlatform = MockEasyWin32Platform();
    EasyWin32Platform.instance = fakePlatform;

    expect(await easyWin32Plugin.getDefaultInterfaceIP(), 12);
  });
}
