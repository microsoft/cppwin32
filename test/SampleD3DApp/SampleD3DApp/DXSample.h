#pragma once

#include "pch.h"

struct DXSample
{
    DXSample(uint32_t width, uint32_t height, std::wstring title) noexcept
        : m_width(width)
        , m_height(height)
        , m_title(std::move(title))
    {
    }

    virtual ~DXSample() noexcept = default;

    auto Width() const noexcept { return m_width; }
    auto Height() const noexcept { return m_height; }

protected:
    win32::Microsoft::Windows::Sdk::Win32::IDXGIAdapter1 GetHardwareAdapter(
        win32::Microsoft::Windows::Sdk::Win32::IDXGIFactory1 const& factory,
        bool requestHighPerformanceAdapter = false);

    uint32_t m_width;
    uint32_t m_height;

private:
    std::wstring m_title;
};