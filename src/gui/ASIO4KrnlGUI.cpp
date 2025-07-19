#include <windows.h>
#include <commctrl.h>
#include <shellapi.h>
#include <mmsystem.h>
#include <string>

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "winmm.lib")

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
    HWND hwndTooltip;
    COLORREF statusColor;
};

static AppState g_state{};

// Forward declarations
LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void UpdateStatus();
void AddTooltip(HWND hwnd, HWND control, const wchar_t* text);
void ShowWizardIfRequested(const wchar_t* cmdLine, HWND owner);
void CheckDevices(HWND hwnd);

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
                               450, 300, nullptr, nullptr, hInstance, nullptr);
    if (!hwnd)
        return 0;

    g_state.hwndTooltip = CreateWindowEx(WS_EX_TRANSPARENT, TOOLTIPS_CLASS, NULL,
                                         WS_POPUP | TTS_ALWAYSTIP, CW_USEDEFAULT,
                                         CW_USEDEFAULT, CW_USEDEFAULT,
                                         CW_USEDEFAULT, hwnd, nullptr, hInstance,
                                         nullptr);
    g_state.statusColor = RGB(0, 128, 0);

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    ShowWizardIfRequested(lpCmdLine, hwnd);

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

static void ShowTrayMenu(HWND hwnd)
{
    HMENU menu = CreatePopupMenu();
    AppendMenu(menu, MF_STRING, 2001, L"Open Settings");
    AppendMenu(menu, MF_STRING, 2002, L"Toggle Low Latency");
    AppendMenu(menu, MF_SEPARATOR, 0, NULL);
    AppendMenu(menu, MF_STRING, 2003, L"Exit");

    POINT pt;
    GetCursorPos(&pt);
    SetForegroundWindow(hwnd);
    TrackPopupMenu(menu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, NULL);
    DestroyMenu(menu);
}

static void RemoveTrayIcon()
{
    if (g_state.nid.hWnd)
        Shell_NotifyIcon(NIM_DELETE, &g_state.nid);
}

void AddTooltip(HWND hwnd, HWND control, const wchar_t* text)
{
    if (!g_state.hwndTooltip)
        return;

    TOOLINFO ti{};
    ti.cbSize   = sizeof(TOOLINFO);
    ti.uFlags   = TTF_SUBCLASS | TTF_IDISHWND;
    ti.hwnd     = hwnd;
    ti.uId      = (UINT_PTR)control;
    ti.lpszText = const_cast<LPWSTR>(text);
    ti.rect     = {0,0,0,0};
    SendMessage(g_state.hwndTooltip, TTM_ADDTOOL, 0, (LPARAM)&ti);
}

void ShowWizardIfRequested(const wchar_t* cmdLine, HWND owner)
{
    if (cmdLine && wcsstr(cmdLine, L"/wizard")) {
        MessageBox(owner,
                   L"Welcome to ASIO4Krnl.\nSelect your audio device and buffer\nsize to get started.",
                   L"ASIO4Krnl Setup Wizard", MB_ICONINFORMATION);
    }
}

void CheckDevices(HWND hwnd)
{
    if (waveInGetNumDevs() == 0 && waveOutGetNumDevs() == 0) {
        MessageBox(hwnd,
                   L"No compatible USB audio device detected. Settings will be disabled.",
                   L"ASIO4Krnl", MB_ICONWARNING);
        EnableWindow(g_state.hwndDeviceIn, FALSE);
        EnableWindow(g_state.hwndDeviceOut, FALSE);
    }
}

static void PopulateControls(HWND hwnd)
{
    CreateWindowEx(0, WC_BUTTON, L"Devices", BS_GROUPBOX | WS_CHILD | WS_VISIBLE,
                   8, 8, 210, 80, hwnd, nullptr, nullptr, nullptr);
    CreateWindowEx(0, WC_BUTTON, L"Buffer && Rate", BS_GROUPBOX | WS_CHILD | WS_VISIBLE,
                   228, 8, 210, 80, hwnd, nullptr, nullptr, nullptr);
    CreateWindowEx(0, WC_BUTTON, L"Status", BS_GROUPBOX | WS_CHILD | WS_VISIBLE,
                   8, 96, 430, 90, hwnd, nullptr, nullptr, nullptr);

    // Input device combo
    CreateWindowEx(0, WC_COMBOBOX, nullptr,
                   CBS_DROPDOWNLIST | WS_CHILD | WS_VISIBLE | WS_TABSTOP,
                   20, 28, 180, 200, hwnd, (HMENU)100, nullptr, nullptr);
    g_state.hwndDeviceIn = GetDlgItem(hwnd, 100);
    SendMessage(g_state.hwndDeviceIn, CB_ADDSTRING, 0, (LPARAM)L"Input Device 1");
    SendMessage(g_state.hwndDeviceIn, CB_SETCURSEL, 0, 0);

    // Output device combo
    CreateWindowEx(0, WC_COMBOBOX, nullptr,
                   CBS_DROPDOWNLIST | WS_CHILD | WS_VISIBLE | WS_TABSTOP,
                   240, 28, 180, 200, hwnd, (HMENU)101, nullptr, nullptr);
    g_state.hwndDeviceOut = GetDlgItem(hwnd, 101);
    SendMessage(g_state.hwndDeviceOut, CB_ADDSTRING, 0, (LPARAM)L"Output Device 1");
    SendMessage(g_state.hwndDeviceOut, CB_SETCURSEL, 0, 0);

    // Buffer size edit
    CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, L"256",
                   WS_CHILD | WS_VISIBLE | ES_NUMBER | WS_TABSTOP,
                   240, 50, 80, 24, hwnd, (HMENU)102, nullptr, nullptr);
    g_state.hwndBufferSize = GetDlgItem(hwnd, 102);
    CreateWindowEx(0, WC_STATIC, L"Frames", WS_CHILD | WS_VISIBLE,
                   330, 54, 50, 20, hwnd, nullptr, nullptr, nullptr);

    // Sample rate edit
    CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, L"48000",
                   WS_CHILD | WS_VISIBLE | ES_NUMBER | WS_TABSTOP,
                   20, 50, 80, 24, hwnd, (HMENU)103, nullptr, nullptr);
    g_state.hwndSampleRate = GetDlgItem(hwnd, 103);
    CreateWindowEx(0, WC_STATIC, L"Hz", WS_CHILD | WS_VISIBLE,
                   110, 54, 40, 20, hwnd, nullptr, nullptr, nullptr);

    // Low latency checkbox
    CreateWindowEx(0, WC_BUTTON, L"Low Latency",
                   WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
                   20, 76, 120, 24, hwnd, (HMENU)104, nullptr, nullptr);
    g_state.hwndLowLatency = GetDlgItem(hwnd, 104);

    // Restore defaults button
    CreateWindowEx(0, WC_BUTTON, L"Restore Defaults", WS_CHILD | WS_VISIBLE,
                   340, 76, 90, 24, hwnd, (HMENU)106, nullptr, nullptr);

    // Status area
    CreateWindowEx(WS_EX_CLIENTEDGE, WC_STATIC, L"Latency: 0ms\nUnderruns: 0\nDriver: OK",
                   WS_CHILD | WS_VISIBLE | SS_LEFT,
                   20, 116, 410, 60, hwnd, (HMENU)105, nullptr, nullptr);
    g_state.hwndStatus = GetDlgItem(hwnd, 105);

    // Start timer for status updates
    g_state.timerId = SetTimer(hwnd, 1, 1000, nullptr);

    // Tooltips
    AddTooltip(hwnd, g_state.hwndDeviceIn, L"Select the recording device");
    AddTooltip(hwnd, g_state.hwndDeviceOut, L"Select the playback device");
    AddTooltip(hwnd, g_state.hwndBufferSize, L"Number of audio frames per buffer");
    AddTooltip(hwnd, g_state.hwndSampleRate, L"Samples per second");
    AddTooltip(hwnd, g_state.hwndLowLatency, L"Reduces latency at the cost of CPU");
    AddTooltip(hwnd, g_state.hwndStatus, L"Real-time driver status");
    AddTooltip(hwnd, GetDlgItem(hwnd, 106), L"Reset settings to defaults");
}

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
    case WM_CREATE:
        g_state.hwndMain = hWnd;
        PopulateControls(hWnd);
        AddTrayIcon(hWnd);
        CheckDevices(hWnd);
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
        } else if (LOWORD(wParam) == 106) {
            // Restore defaults
            SetWindowText(g_state.hwndBufferSize, L"256");
            SetWindowText(g_state.hwndSampleRate, L"48000");
            SendMessage(g_state.hwndLowLatency, BM_SETCHECK, BST_UNCHECKED, 0);
        } else if (LOWORD(wParam) == 2001) {
            ShowWindow(hWnd, SW_SHOW);
            SetForegroundWindow(hWnd);
        } else if (LOWORD(wParam) == 2002) {
            BOOL checked = SendMessage(g_state.hwndLowLatency, BM_GETCHECK, 0, 0);
            SendMessage(g_state.hwndLowLatency, BM_SETCHECK, !checked, 0);
        } else if (LOWORD(wParam) == 2003) {
            PostMessage(hWnd, WM_CLOSE, 0, 0);
        }
        break;
    case WM_CTLCOLORSTATIC:
        if ((HWND)lParam == g_state.hwndStatus) {
            HDC hdc = (HDC)wParam;
            SetTextColor(hdc, g_state.statusColor);
            SetBkMode(hdc, TRANSPARENT);
            return (LRESULT)GetStockObject(NULL_BRUSH);
        }
        break;
    case WM_USER + 1:
        if (lParam == WM_RBUTTONUP) {
            ShowTrayMenu(hWnd);
        } else if (lParam == WM_LBUTTONDBLCLK) {
            ShowWindow(hWnd, SW_SHOW);
            SetForegroundWindow(hWnd);
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
    g_state.statusColor = counter > 0 ? RGB(200,0,0) : RGB(0,128,0);
    InvalidateRect(g_state.hwndStatus, NULL, TRUE);
}

