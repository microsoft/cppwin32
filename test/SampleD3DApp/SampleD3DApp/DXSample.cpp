#include "pch.h"
#include "DXSample.h"

using namespace win32;
using namespace win32::Windows::Win32;

inline constexpr int32_t DXGI_ERROR_NOT_FOUND = 0x887a0002;

win32::com_ptr<IDXGIAdapter1> DXSample::GetHardwareAdapter(
    IDXGIFactory1* factory,
    bool requestHighPerformanceAdapter)
{
    win32::com_ptr<IDXGIAdapter1> adapter;
    for (uint32_t adapterIndex = 0; DXGI_ERROR_NOT_FOUND != factory->EnumAdapters1(adapterIndex, adapter.put_void()); ++adapterIndex, adapter = nullptr)
    {
        DXGI_ADAPTER_DESC1 desc;
        adapter->GetDesc1(&desc);

        if (desc.Flags & (uint32_t)DXGI_ADAPTER_FLAG::DXGI_ADAPTER_FLAG_SOFTWARE)
        {
            // Don't select the Basic Render Driver adapter.
            // If you want a software adapter, pass in "/warp" on the command line.
            continue;
        }

        // Check to see whether the adapter supports Direct3D 12, but don't create the
        // actual device yet.
        if (0 == Apis::D3D12CreateDevice(adapter.get(), D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0, &guid_of<ID3D12Device>(), nullptr))
        {
            break;
        }
    }
    return adapter;
}
