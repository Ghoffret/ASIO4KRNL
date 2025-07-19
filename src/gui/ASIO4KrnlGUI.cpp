#include <windows.h>
#include <commctrl.h>
#include <string>

#pragma comment(lib, "comctl32.lib")

// Simple application state
struct AppState {
    HWND hwndMain;
    HWND hwndDeviceIn;
    HWND hwndDeviceOut;
    HWND hwndBufferSize;
    HWND hwndSampleRate;
    HWND hwndLowLatency;
    HWND hwndStatus;
    NOTIFYICONDATA nid{};
    UINT timerId;
};

static AppState g_state{};

// Forward declarations
LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void UpdateStatus();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                      _In_opt_ HINSTANCE hPrevInstance,
                      _In_ LPWSTR    lpCmdLine,
                      _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    InitCommonControls();

    WNDCLASSEX wc{ sizeof(WNDCLASSEX) };
    wc.lpfnWndProc   = MainWndProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = L"ASIO4KrnlWnd";
    wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);
    wc.hIcon         = LoadIcon(nullptr, IDI_APPLICATION);
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    RegisterClassEx(&wc);

    HWND hwnd = CreateWindowEx(0, wc.lpszClassName, L"ASIO4Krnl Settings",
                               WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                               420, 260, nullptr, nullptr, hInstance, nullptr);
    if (!hwnd)
        return 0;

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return static_cast<int>(msg.wParam);
}

static void AddTrayIcon(HWND hwnd)
{
    g_state.nid.cbSize           = sizeof(NOTIFYICONDATA);
    g_state.nid.hWnd             = hwnd;
    g_state.nid.uID              = 1;
    g_state.nid.uFlags           = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    g_state.nid.uCallbackMessage = WM_USER + 1;
    g_state.nid.hIcon            = LoadIcon(nullptr, IDI_APPLICATION);
    wcscpy_s(g_state.nid.szTip, L"ASIO4Krnl Settings");
    Shell_NotifyIcon(NIM_ADD, &g_state.nid);
}

static void RemoveTrayIcon()
{
    if (g_state.nid.hWnd)
        Shell_NotifyIcon(NIM_DELETE, &g_state.nid);
}

static void PopulateControls(HWND hwnd)
{
    // Input device combo
    CreateWindowEx(0, WC_COMBOBOX, nullptr,
                   CBS_DROPDOWNLIST | WS_CHILD | WS_VISIBLE,
                   10, 10, 180, 200, hwnd, (HMENU)100, nullptr, nullptr);
    g_state.hwndDeviceIn = GetDlgItem(hwnd, 100);
    SendMessage(g_state.hwndDeviceIn, CB_ADDSTRING, 0, (LPARAM)L"Input Device 1");
    SendMessage(g_state.hwndDeviceIn, CB_SETCURSEL, 0, 0);

    // Output device combo
    CreateWindowEx(0, WC_COMBOBOX, nullptr,
                   CBS_DROPDOWNLIST | WS_CHILD | WS_VISIBLE,
                   200, 10, 180, 200, hwnd, (HMENU)101, nullptr, nullptr);
    g_state.hwndDeviceOut = GetDlgItem(hwnd, 101);
    SendMessage(g_state.hwndDeviceOut, CB_ADDSTRING, 0, (LPARAM)L"Output Device 1");
    SendMessage(g_state.hwndDeviceOut, CB_SETCURSEL, 0, 0);

    // Buffer size edit
    CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, L"256",
                   WS_CHILD | WS_VISIBLE | ES_NUMBER,
                   10, 50, 80, 24, hwnd, (HMENU)102, nullptr, nullptr);
    g_state.hwndBufferSize = GetDlgItem(hwnd, 102);

    CreateWindowEx(0, WC_STATIC, L"Buffer Frames", WS_CHILD | WS_VISIBLE,
                   100, 54, 90, 20, hwnd, nullptr, nullptr, nullptr);

    // Sample rate edit
    CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, L"48000",
                   WS_CHILD | WS_VISIBLE | ES_NUMBER,
                   200, 50, 80, 24, hwnd, (HMENU)103, nullptr, nullptr);
    g_state.hwndSampleRate = GetDlgItem(hwnd, 103);

    CreateWindowEx(0, WC_STATIC, L"Sample Rate", WS_CHILD | WS_VISIBLE,
                   290, 54, 90, 20, hwnd, nullptr, nullptr, nullptr);

    // Low latency checkbox
    CreateWindowEx(0, WC_BUTTON, L"Low Latency",
                   WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
                   10, 90, 120, 24, hwnd, (HMENU)104, nullptr, nullptr);
    g_state.hwndLowLatency = GetDlgItem(hwnd, 104);

    // Status area
    CreateWindowEx(WS_EX_CLIENTEDGE, WC_STATIC, L"Latency: 0ms\nUnderruns: 0\nDriver: OK",
                   WS_CHILD | WS_VISIBLE | SS_LEFT,
                   10, 130, 370, 60, hwnd, (HMENU)105, nullptr, nullptr);
    g_state.hwndStatus = GetDlgItem(hwnd, 105);

    // Start timer for status updates
    g_state.timerId = SetTimer(hwnd, 1, 1000, nullptr);
}

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
    case WM_CREATE:
        g_state.hwndMain = hWnd;
        PopulateControls(hWnd);
        AddTrayIcon(hWnd);
        break;
    case WM_TIMER:
        if (wParam == g_state.timerId) {
            UpdateStatus();
        }
        break;
    case WM_COMMAND:
        if (HIWORD(wParam) == BN_CLICKED && LOWORD(wParam) == 104) {
            // Toggle low latency mode
            BOOL checked = SendMessage(g_state.hwndLowLatency, BM_GETCHECK, 0, 0);
            SendMessage(g_state.hwndLowLatency, BM_SETCHECK, !checked, 0);
        }
        break;
    case WM_DESTROY:
        KillTimer(hWnd, g_state.timerId);
        RemoveTrayIcon();
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }
    return 0;
}

void UpdateStatus()
{
    // In a real app, query driver for status
    static int counter = 0;
    std::wstring text = L"Latency: 10ms\nUnderruns: " + std::to_wstring(counter++) + L"\nDriver: OK";
    SetWindowText(g_state.hwndStatus, text.c_str());
}

