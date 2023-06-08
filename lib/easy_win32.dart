import 'dart:ffi';

import 'easy_win32_platform_interface.dart';

class EasyWin32 {
  Future<String?> getPlatformVersion() {
    return EasyWin32Platform.instance.getPlatformVersion();
  }

  Future<Map?> getDefaultInterfaceIP() {
    return EasyWin32Platform.instance.getDefaultInterfaceIP();
  }
}
