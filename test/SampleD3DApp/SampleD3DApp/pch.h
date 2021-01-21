#pragma once

#include <win32/Windows.Win32.Base.h>

#include <stdint.h>

#define IID_PPV_ARGS(ppType) win32::guid_of<**(ppType)>(), IID_PPV_ARGS_Helper(ppType)
