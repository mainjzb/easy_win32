import 'package:flutter/foundation.dart';
import 'package:flutter/services.dart';

import 'easy_win32_platform_interface.dart';

/// An implementation of [EasyWin32Platform] that uses method channels.
class MethodChannelEasyWin32 extends EasyWin32Platform {
  /// The method channel used to interact with the native platform.
  @visibleForTesting
  final methodChannel = const MethodChannel('easy_win32');

  @override
  Future<String?> getPlatformVersion() async {
    final version =
        await methodChannel.invokeMethod<String>('getPlatformVersion');
    return version;
  }

  @override
  Future<int?> getDefaultInterfaceIndex() async {
    final ifIndex =
        await methodChannel.invokeMethod<int>('getDefaultInterfaceIndex');
    return ifIndex;
  }
}
