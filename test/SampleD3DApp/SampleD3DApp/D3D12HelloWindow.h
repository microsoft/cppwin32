#pragma once

#include "DXSample.h"

struct D3D12HelloWindow : public DXSample
{
    using DXSample::DXSample;

    void OnInit() override;
    void OnUpdate() override;
    void OnRender() override;
    void OnDestroy() override;

private:
    win32::com_ptr<win32::Windows::Win32::Direct3D12::ID3D12Device> m_device;
    win32::com_ptr<win32::Windows::Win32::Direct3D12::ID3D12CommandQueue> m_commandQueue;
    uint32_t m_rtvDescriptorSize = 0;

    uint32_t m_frameIndex = 0;

    void LoadPipeline();
    void LoadAssets();
};