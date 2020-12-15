#pragma once

#include "../../../Generated Files/Windows.Win32.h"

#include <stdint.h>

#define IID_PPV_ARGS(ppType) win32::guid_of<**(ppType)>(), IID_PPV_ARGS_Helper(ppType)
