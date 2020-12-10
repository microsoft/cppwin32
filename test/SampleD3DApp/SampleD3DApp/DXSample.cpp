#include "pch.h"
#include "DXSample.h"

using namespace win32::Microsoft::Windows::Sdk::Win32;

IDXGIAdapter1 DXSample::GetHardwareAdapter(
    IDXGIFactory1 const& factory,
    bool requestHighPerformanceAdapter = false)
{
    IDXGIAdapter1 adapter;
    for (uint32_t adapterIndex = 0; Apis::DXGI_ERROR_NOT_FOUND != factory.EnumAdapters1(adapterIndex, adapter); ++adapterIndex)
    {
        DXGI_ADAPTER_DESC1 desc;
        adapter.GetDesc1(&desc);

        if ((DXGI_ADAPTER_FLAG)desc.Flags & DXGI_ADAPTER_FLAG::DXGI_ADAPTER_FLAG_SOFTWARE)
        {

        }
    }
}
