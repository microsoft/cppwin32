#include "../../../Generated Files/Microsoft.Windows.Sdk.h"

using namespace win32::Microsoft::Windows::Sdk;

intptr_t __stdcall WindowProc(intptr_t hwnd, uint32_t message, size_t wParam, intptr_t lParam);

int main()
{
    auto hInstance = Apis::GetModuleHandleW(nullptr);

    WNDCLASSEXW windowClass{};
    windowClass.cbSize = sizeof(WNDCLASSEXW);
    windowClass.style = Apis::CS_HREDRAW | Apis::CS_VREDRAW;
    {
        WNDPROC temp = WindowProc;
        windowClass.lpfnWndProc = reinterpret_cast<intptr_t>(temp);
    }
    windowClass.hInstance = hInstance;
    windowClass.hCursor = Apis::LoadCursorW(0, (uint16_t*)(Apis::IDC_ARROW)); // TODO: 
    windowClass.lpszClassName = (uint16_t*)(L"DXSampleClass");

    Apis::RegisterClassExW(&windowClass);

    RECT rect{ 0, 0, 640, 480 };
    Apis::AdjustWindowRect(&rect, Apis::WS_OVERLAPPEDWINDOW, 0);

    auto hwnd = Apis::CreateWindowExW(
        0,
        windowClass.lpszClassName,
        (uint16_t*)(L"Title"),
        Apis::WS_OVERLAPPEDWINDOW,
        Apis::CW_USEDEFAULT,
        Apis::CW_USEDEFAULT,
        640,
        480,
        0,
        0,
        hInstance,
        nullptr);
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
}

intptr_t __stdcall WindowProc(intptr_t hwnd, uint32_t message, size_t wParam, intptr_t lParam)
{
    switch (message)
    {
    case Apis::WM_DESTROY: // TODO: constant WM_DESTROY
        Apis::PostQuitMessage(0);
        return 0;
    }

    return Apis::DefWindowProcW(hwnd, message, wParam, lParam);
}
