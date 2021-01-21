#include "pch.h"
#include "D3D12HelloWindow.h"

using namespace win32::Windows::Win32::Base;

intptr_t __stdcall WindowProc(intptr_t hwnd, uint32_t message, size_t wParam, intptr_t lParam);

int run(DXSample* sample);

int main()
{
    D3D12HelloWindow sample(1280, 720, L"D3D12 Hello Window");
    run(&sample);
}

int run(DXSample* sample)
{
    auto hInstance = Apis::GetModuleHandleW(nullptr);

    WNDCLASSEXW windowClass{};
    windowClass.cbSize = sizeof(WNDCLASSEXW);
    windowClass.style = Apis::CS_HREDRAW | Apis::CS_VREDRAW;
    windowClass.lpfnWndProc = WindowProc;
    windowClass.hInstance = hInstance;
    windowClass.hCursor = Apis::LoadCursorW(0, (uint16_t*)(Apis::IDC_ARROW)); // TODO: 
    windowClass.lpszClassName = (uint16_t*)(L"DXSampleClass");

    Apis::RegisterClassExW(&windowClass);

    RECT rect{ 0, 0, sample->Width(), sample->Height()};
    Apis::AdjustWindowRect(&rect, Apis::WS_OVERLAPPEDWINDOW, 0);

    auto hwnd = Apis::CreateWindowExW(
        0,
        windowClass.lpszClassName,
        (uint16_t*)(sample->Title().c_str()),
        Apis::WS_OVERLAPPEDWINDOW,
        Apis::CW_USEDEFAULT,
        Apis::CW_USEDEFAULT,
        640,
        480,
        0,
        0,
        hInstance,
        sample);

    sample->OnInit();

    Apis::ShowWindow(hwnd, Apis::SW_SHOWDEFAULT);

    MSG msg;
    do
    {
        if (Apis::PeekMessageW(&msg, 0, 0, 0, Apis::PM_REMOVE) != 0) // TODO: Constant PM_REMOVE
        {
            Apis::TranslateMessage(&msg);
            Apis::DispatchMessageW(&msg);
        }
    } while (msg.message != Apis::WM_QUIT); // TODO: Constant WM_QUIT

    sample->OnDestroy();

    return 0;
}

intptr_t __stdcall WindowProc(intptr_t hwnd, uint32_t message, size_t wParam, intptr_t lParam)
{
    auto sample = reinterpret_cast<DXSample*>(Apis::GetWindowLongW(hwnd, Apis::GWLP_USERDATA));

    switch (message)
    {
    case Apis::WM_CREATE:
    {
        auto create_struct = reinterpret_cast<CREATESTRUCTW*>(lParam);
        Apis::SetWindowLongW(hwnd, Apis::GWLP_USERDATA, reinterpret_cast<intptr_t>(create_struct->lpCreateParams));
        return 0;
    }

    case Apis::WM_PAINT:
        if (sample)
        {
            sample->OnUpdate();
            sample->OnRender();
        }
        return 0;

    case Apis::WM_DESTROY: // TODO: constant WM_DESTROY
        Apis::PostQuitMessage(0);
        return 0;
    }
    return Apis::DefWindowProcW(hwnd, message, wParam, lParam);
}
