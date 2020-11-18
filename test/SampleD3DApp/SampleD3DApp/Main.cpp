#include "../../../Generated Files/Microsoft.Windows.Sdk.h"

#define MAKEINTRESOURCE(i) ((uint16_t*)((size_t)((uint16_t)(i))))
#define IDC_ARROW           MAKEINTRESOURCE(32512)
constexpr uint32_t WS_OVERLAPPEDWINDOW = 0x00c00000 | 0x00080000 | 0x00040000 | 0x00020000 | 0x00010000;

using namespace win32::Microsoft::Windows::Sdk;

intptr_t __stdcall WindowProc(intptr_t hwnd, uint32_t message, size_t wParam, intptr_t lParam);

int main()
{
    auto hInstance = Apis::GetModuleHandleW(nullptr);

    WNDCLASSEXW windowClass{};
    windowClass.cbSize = sizeof(WNDCLASSEXW);
    windowClass.style = 2 | 1; // CS_HREDRAW | CS_VREDRAW // TODO: constants
    {
        WNDPROC temp = WindowProc;
        windowClass.lpfnWndProc = reinterpret_cast<intptr_t>(temp);
    }
    windowClass.hInstance = hInstance;
    windowClass.hCursor = Apis::LoadCursorW(0, IDC_ARROW);
    windowClass.lpszClassName = (uint16_t*)(L"DXSampleClass");

    Apis::RegisterClassExW(&windowClass);

    RECT rect{ 0, 0, 640, 480 };
    Apis::AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, 0);

    auto hwnd = Apis::CreateWindowExW(
        0,
        windowClass.lpszClassName,
        (uint16_t*)(L"Title"),
        WS_OVERLAPPEDWINDOW,
        0x80000000, // TODO: Constant SW_USEDEFAULT
        0x80000000,
        640,
        480,
        0,
        0,
        hInstance,
        nullptr);
    Apis::ShowWindow(hwnd, 10); // TODO: Constants SW_SHOWDEFAULT

    MSG msg;
    do
    {
        if (Apis::PeekMessageW(&msg, 0, 0, 0, 0x0001) != 0) // TODO: Constant PM_REMOVE
        {
            Apis::TranslateMessage(&msg);
            Apis::DispatchMessageW(&msg);
        }
    } while (msg.message != 0x12); // TODO: Constant WM_QUIT
}

intptr_t __stdcall WindowProc(intptr_t hwnd, uint32_t message, size_t wParam, intptr_t lParam)
{
    switch (message)
    {
    case 0x02: // TODO: constant WM_DESTROY
        Apis::PostQuitMessage(0);
        return 0;
    }

    return Apis::DefWindowProcW(hwnd, message, wParam, lParam);
}
