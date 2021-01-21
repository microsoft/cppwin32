#pragma once

#include "pch.h"

struct DXSample
{
    DXSample(int32_t width, int32_t height, std::wstring&& title) noexcept
        : m_width(width)
        , m_height(height)
        , m_title(std::move(title))
    {
    }

    virtual ~DXSample() noexcept = default;

    virtual void OnInit() = 0;
    virtual void OnUpdate() = 0;
    virtual void OnRender() = 0;
    virtual void OnDestroy() = 0;

    auto Width() const noexcept { return m_width; }
    auto Height() const noexcept { return m_height; }
    auto const& Title() const noexcept { return m_title; }

protected:
    win32::com_ptr<win32::Windows::Win32::Dxgi::IDXGIAdapter1> GetHardwareAdapter(
        win32::Windows::Win32::Dxgi::IDXGIFactory1* factory,
        bool requestHighPerformanceAdapter = false);

    int32_t m_width;
    int32_t m_height;

private:
    std::wstring m_title;
};