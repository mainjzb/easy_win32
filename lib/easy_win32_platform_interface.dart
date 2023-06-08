import 'package:plugin_platform_interface/plugin_platform_interface.dart';

import 'easy_win32_method_channel.dart';

abstract class EasyWin32Platform extends PlatformInterface {
  /// Constructs a EasyWin32Platform.
  EasyWin32Platform() : super(token: _token);

  static final Object _token = Object();

  static EasyWin32Platform _instance = MethodChannelEasyWin32();

  /// The default instance of [EasyWin32Platform] to use.
  ///
  /// Defaults to [MethodChannelEasyWin32].
  static EasyWin32Platform get instance => _instance;

  /// Platform-specific implementations should set this with their own
  /// platform-specific class that extends [EasyWin32Platform] when
  /// they register themselves.
  static set instance(EasyWin32Platform instance) {
    PlatformInterface.verifyToken(instance, _token);
    _instance = instance;
  }

  Future<String?> getPlatformVersion() {
    throw UnimplementedError('platformVersion() has not been implemented.');
  }

  Future<int?> getDefaultInterfaceIndex() {
    throw UnimplementedError(
        'getDefaultInterfaceIP() has not been implemented.');
  }
}
