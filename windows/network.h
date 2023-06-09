#ifndef FLUTTER_PLUGIN_NETWORK_H_
#define FLUTTER_PLUGIN_NETWORK_H_

#include <tuple>
#include <string>
#include <flutter/standard_method_codec.h>


typedef unsigned long DWORD;


DWORD getDefaultInterfaceIP();

std::tuple<DWORD, DWORD> getDefaultInterfaceIndex();
std::tuple<flutter::EncodableMap, DWORD> getAdaptersAddresses(DWORD);



#endif