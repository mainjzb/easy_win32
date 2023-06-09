import 'package:flutter/material.dart';
import 'dart:async';

import 'package:flutter/services.dart';
import 'package:easy_win32/easy_win32.dart';

import 'dart:convert';
import 'dart:io';

void main() async {
  for (var i = 0; i < 999; i++) {
    Stopwatch stopwatch = Stopwatch()..start();
    Socket socket = await Socket.connect('104.16.154.36', 80);
    print("The function took ${stopwatch.elapsed.inMilliseconds} to run.");
    socket.close();
  }
  runApp(const MyApp());
}

class MyApp extends StatefulWidget {
  const MyApp({super.key});

  @override
  State<MyApp> createState() => _MyAppState();
}

class _MyAppState extends State<MyApp> {
  int _platformVersion = 0;
  final _easyWin32Plugin = EasyWin32();

  @override
  void initState() {
    super.initState();
    initPlatformState();
  }

  // Platform messages are asynchronous, so we initialize in an async method.
  Future<void> initPlatformState() async {
    int platformVersion = 0;
    // Platform messages may fail, so we use a try/catch PlatformException.
    // We also handle the message potentially returning null.
    try {
      platformVersion = await _easyWin32Plugin.getDefaultInterfaceIndex() ?? 0;
    } on PlatformException {
      platformVersion = 0;
    }

    // If the widget was removed from the tree while the asynchronous platform
    // message was in flight, we want to discard the reply rather than calling
    // setState to update our non-existent appearance.
    if (!mounted) return;

    setState(() {
      _platformVersion = platformVersion;
    });
  }

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      home: Scaffold(
        appBar: AppBar(
          title: const Text('Plugin example app'),
        ),
        body: Center(
          child: Text('Running on: $_platformVersion\n'),
        ),
      ),
    );
  }
}
