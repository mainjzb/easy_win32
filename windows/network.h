#ifndef FLUTTER_PLUGIN_NETWORK_H_
#define FLUTTER_PLUGIN_NETWORK_H_

#include <tuple>

typedef unsigned long DWORD;

DWORD getDefaultInterfaceIP();

std::tuple<DWORD, DWORD> getDefaultIFIndex();
#endif