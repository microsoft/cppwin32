#pragma once

#include <win32/Windows.Win32.Dxgi.h>
#include <win32/Windows.Win32.Direct3D12.h>
#include <win32/Windows.Win32.Direct3D11.h>
#include <win32/Windows.Win32.DisplayDevices.h>
#include <win32/Windows.Win32.MenusAndResources.h>
#include <win32/Windows.Win32.SystemServices.h>
#include <win32/Windows.Win32.WindowsAndMessaging.h>

#include <stdint.h>

#define IID_PPV_ARGS(ppType) win32::guid_of<**(ppType)>(), IID_PPV_ARGS_Helper(ppType)
