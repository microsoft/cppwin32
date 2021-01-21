#include "pch.h"

#include "D3D12HelloWindow.h"

using namespace win32;
using namespace win32::Windows::Win32;
using namespace win32::Windows::Win32::Direct3D12;
using namespace win32::Windows::Win32::SystemServices;
using namespace win32::Windows::Win32::Dxgi;
using namespace win32::Windows::Win32::Direct3D11;

void D3D12HelloWindow::OnInit()
{
    LoadPipeline();
    LoadAssets();
}

void D3D12HelloWindow::OnUpdate()
{

}

void D3D12HelloWindow::OnRender()
{

}
void D3D12HelloWindow::OnDestroy()
{

}

void D3D12HelloWindow::LoadPipeline()
{
    uint32_t dxgiFactoryFlags = 0;

#if defined(_DEBUG)
    com_ptr<ID3D12Debug> debugController;
    if (D3D12GetDebugInterface((guid*)(&guid_of<ID3D12Debug>()), debugController.put_void()).Value == 0)
    {
        debugController->EnableDebugLayer();
        dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
    }
#endif

    com_ptr<IDXGIFactory1> factory;
    CreateDXGIFactory2(dxgiFactoryFlags, (guid*)(&guid_of<IDXGIFactory>()), factory.put_void());

    auto adapter = GetHardwareAdapter(factory.get());
    D3D12CreateDevice(adapter.get(), D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0, (guid*)(&guid_of<ID3D12Device>()), m_device.put_void());

    D3D12_COMMAND_QUEUE_DESC queueDesc{};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAGS::D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT;

    m_device->CreateCommandQueue(&queueDesc, (guid*)(&guid_of<ID3D12CommandQueue>()), m_commandQueue.put_void());
}

void D3D12HelloWindow::LoadAssets()
{

}
