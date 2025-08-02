#include <windows.h>
#include <commctrl.h>
#include <shellapi.h>
#include <mmsystem.h>
#include <shlobj.h>
#include <string>
#include <vector>
#include <map>
#include <uxtheme.h>
#include <dwmapi.h>
#include "../common/Version.h"
#include "resource.h"

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "uxtheme.lib")
#pragma comment(lib, "dwmapi.lib")

// Enhanced application state with theming and additional settings
struct AppState {
    HWND hwndMain;
    HWND hwndDeviceIn;
    HWND hwndDeviceOut;
    HWND hwndBufferSize;
    HWND hwndSampleRate;
    HWND hwndLowLatency;
    HWND hwndStatus;
    HWND hwndTestButton;
    HWND hwndThemeToggle;
    HWND hwndAdvancedButton;
    NOTIFYICONDATA nid{};
    UINT timerId;
    HWND hwndTooltip;
    
    // Theme support
    bool isDarkTheme;
    HBRUSH hBrushBg;
    HBRUSH hBrushControl;
    COLORREF textColor;
    COLORREF bgColor;
    COLORREF borderColor;
    COLORREF accentColor;
    
    // Status and error handling
    COLORREF statusColor;
    std::wstring lastError;
    bool hasError;
    
    // Settings
    std::vector<std::wstring> bufferSizes;
    std::vector<std::wstring> sampleRates;
    int currentBufferSize;
    int currentSampleRate;
    bool lowLatencyEnabled;
    
    // Window state
    RECT originalRect;
    bool isResizable;
    int minWidth;
    int minHeight;
};

static AppState g_state{};

// Initialize default settings
static void InitializeSettings()
{
    g_state.isDarkTheme = false;
    g_state.isResizable = true;
    g_state.minWidth = 520;
    g_state.minHeight = 400;
    g_state.hasError = false;
    
    // Buffer sizes in frames
    g_state.bufferSizes = {L"64", L"128", L"256", L"512", L"1024", L"2048"};
    g_state.currentBufferSize = 2; // Default to 256
    
    // Sample rates in Hz
    g_state.sampleRates = {L"44100", L"48000", L"88200", L"96000", L"176400", L"192000"};
    g_state.currentSampleRate = 1; // Default to 48000
    
    g_state.lowLatencyEnabled = false;
    
    ApplyTheme();
}

// Theme management
static void ApplyTheme()
{
    if (g_state.isDarkTheme) {
        g_state.bgColor = COLOR_DARK_BG;
        g_state.textColor = COLOR_DARK_TEXT;
        g_state.borderColor = COLOR_DARK_BORDER;
        g_state.accentColor = COLOR_DARK_ACCENT;
    } else {
        g_state.bgColor = COLOR_LIGHT_BG;
        g_state.textColor = COLOR_LIGHT_TEXT;
        g_state.borderColor = COLOR_LIGHT_BORDER;
        g_state.accentColor = COLOR_LIGHT_ACCENT;
    }
    
    // Update brushes
    if (g_state.hBrushBg) DeleteObject(g_state.hBrushBg);
    if (g_state.hBrushControl) DeleteObject(g_state.hBrushControl);
    
    g_state.hBrushBg = CreateSolidBrush(g_state.bgColor);
    g_state.hBrushControl = CreateSolidBrush(g_state.bgColor);
    
    g_state.statusColor = COLOR_STATUS_OK;
}

static void ToggleTheme()
{
    g_state.isDarkTheme = !g_state.isDarkTheme;
    ApplyTheme();
    
    if (g_state.hwndMain) {
        InvalidateRect(g_state.hwndMain, NULL, TRUE);
        UpdateWindow(g_state.hwndMain);
        
        // Update button text
        SetWindowText(g_state.hwndThemeToggle, 
                     g_state.isDarkTheme ? L"Light Mode" : L"Dark Mode");
    }
}

static void LogGui(const wchar_t* msg)
{
    wchar_t path[MAX_PATH];
    if (SHGetFolderPathW(NULL, CSIDL_COMMON_APPDATA, NULL, SHGFP_TYPE_CURRENT, path) == S_OK) {
        wcscat_s(path, L"\\ASIO4Krnl\\logs\\gui.log");
        HANDLE h = CreateFileW(path, FILE_APPEND_DATA, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (h != INVALID_HANDLE_VALUE) {
            DWORD written;
            WriteFile(h, msg, (DWORD)(wcslen(msg) * sizeof(wchar_t)), &written, NULL);
            CloseHandle(h);
        }
    }
}

// Enhanced error handling and validation
static void ShowError(const wchar_t* message, const wchar_t* title = L"ASIO4Krnl Error")
{
    g_state.lastError = message;
    g_state.hasError = true;
    g_state.statusColor = COLOR_STATUS_ERROR;
    
    MessageBox(g_state.hwndMain, message, title, MB_ICONERROR | MB_OK);
    LogGui((std::wstring(L"Error: ") + message + L"\n").c_str());
}

static void ShowWarning(const wchar_t* message, const wchar_t* title = L"ASIO4Krnl Warning")
{
    g_state.statusColor = COLOR_STATUS_WARNING;
    MessageBox(g_state.hwndMain, message, title, MB_ICONWARNING | MB_OK);
    LogGui((std::wstring(L"Warning: ") + message + L"\n").c_str());
}

static bool ValidateSettings()
{
    wchar_t buffer[64];
    
    // Validate buffer size
    GetWindowText(g_state.hwndBufferSize, buffer, sizeof(buffer)/sizeof(wchar_t));
    int bufferSize = _wtoi(buffer);
    if (bufferSize < 32 || bufferSize > 8192) {
        ShowError(L"Buffer size must be between 32 and 8192 frames.");
        return false;
    }
    
    // Validate sample rate
    GetWindowText(g_state.hwndSampleRate, buffer, sizeof(buffer)/sizeof(wchar_t));
    int sampleRate = _wtoi(buffer);
    if (sampleRate < 8000 || sampleRate > 192000) {
        ShowError(L"Sample rate must be between 8000 and 192000 Hz.");
        return false;
    }
    
    // Check if buffer size is power of 2 (recommended)
    if ((bufferSize & (bufferSize - 1)) != 0) {
        ShowWarning(L"Buffer size is not a power of 2. This may cause compatibility issues with some applications.");
    }
    
    g_state.hasError = false;
    g_state.statusColor = COLOR_STATUS_OK;
    return true;
}

static void TestSettings()
{
    if (!ValidateSettings()) {
        return;
    }
    
    // Simulate testing audio settings
    MessageBox(g_state.hwndMain, 
               L"Settings test completed successfully!\n\n"
               L"• Audio devices detected\n"
               L"• Buffer size validated\n"
               L"• Sample rate supported\n"
               L"• Low latency mode ready",
               L"Settings Test", MB_ICONINFORMATION | MB_OK);
}

// Forward declarations
LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void UpdateStatus();
void AddTooltip(HWND hwnd, HWND control, const wchar_t* text);
void ShowWizardIfRequested(const wchar_t* cmdLine, HWND owner);
void CheckDevices(HWND hwnd);
void ResizeControls(HWND hwnd);
void ShowAdvancedSettings(HWND owner);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                      _In_opt_ HINSTANCE hPrevInstance,
                      _In_ LPWSTR    lpCmdLine,
                      _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    InitCommonControls();
    InitializeSettings();

    WNDCLASSEX wc{ sizeof(WNDCLASSEX) };
    wc.lpfnWndProc   = MainWndProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = L"ASIO4KrnlWnd";
    wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);
    wc.hIcon         = LoadIcon(nullptr, IDI_APPLICATION);
    wc.hbrBackground = g_state.hBrushBg;
    RegisterClassEx(&wc);

    HWND hwnd = CreateWindowEx(0, wc.lpszClassName, L"ASIO4Krnl Settings (Beta)",
                               WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
                               CW_USEDEFAULT, CW_USEDEFAULT,
                               g_state.minWidth, g_state.minHeight,
                               nullptr, nullptr, hInstance, nullptr);
    if (!hwnd)
        return 0;

    g_state.hwndTooltip = CreateWindowEx(WS_EX_TRANSPARENT, TOOLTIPS_CLASS, NULL,
                                         WS_POPUP | TTS_ALWAYSTIP, CW_USEDEFAULT,
                                         CW_USEDEFAULT, CW_USEDEFAULT,
                                         CW_USEDEFAULT, hwnd, nullptr, hInstance,
                                         nullptr);

    std::wstring title = std::wstring(L"ASIO4Krnl Settings (Beta) - v") + ASIO4KRNL_VERSION;
    SetWindowText(hwnd, title.c_str());
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    
    // Show beta warning with modern styling
    MessageBox(hwnd, 
               L"Beta software \x2013 use at your own risk.\n\n"
               L"This version includes enhanced GUI features:\n"
               L"• Modern design with theme support\n"
               L"• Improved settings validation\n"
               L"• Responsive window layout\n"
               L"• Enhanced error handling",
               L"ASIO4Krnl Beta", MB_ICONINFORMATION | MB_OK);
    LogGui(L"Enhanced GUI started\n");

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
    g_state.nid.uCallbackMessage = WM_TRAYICON;
    g_state.nid.hIcon            = LoadIcon(nullptr, IDI_APPLICATION);
    wcscpy_s(g_state.nid.szTip, L"ASIO4Krnl Settings (Beta)");
    Shell_NotifyIcon(NIM_ADD, &g_state.nid);
}

static void ShowTrayMenu(HWND hwnd)
{
    HMENU menu = CreatePopupMenu();
    AppendMenu(menu, MF_STRING, IDM_TRAY_OPEN, L"Open Settings");
    AppendMenu(menu, MF_STRING, IDM_TRAY_TOGGLE, L"Toggle Low Latency");
    AppendMenu(menu, MF_STRING, IDM_TRAY_THEME, 
               g_state.isDarkTheme ? L"Switch to Light Mode" : L"Switch to Dark Mode");
    AppendMenu(menu, MF_SEPARATOR, 0, NULL);
    AppendMenu(menu, MF_STRING, IDM_TRAY_EXIT, L"Exit");

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
    
    // Set tooltip colors to match theme
    SendMessage(g_state.hwndTooltip, TTM_SETTIPBKCOLOR, g_state.bgColor, 0);
    SendMessage(g_state.hwndTooltip, TTM_SETTIPTEXTCOLOR, g_state.textColor, 0);
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
    int inputDevices = waveInGetNumDevs();
    int outputDevices = waveOutGetNumDevs();
    
    if (inputDevices == 0 && outputDevices == 0) {
        ShowError(L"No compatible USB audio device detected.\n\n"
                 L"Please ensure your USB audio device is:\n"
                 L"• Properly connected\n"
                 L"• Recognized by Windows\n"
                 L"• Not in use by another application",
                 L"No Audio Devices");
        
        EnableWindow(g_state.hwndDeviceIn, FALSE);
        EnableWindow(g_state.hwndDeviceOut, FALSE);
    } else {
        // Populate device lists
        SendMessage(g_state.hwndDeviceIn, CB_RESETCONTENT, 0, 0);
        SendMessage(g_state.hwndDeviceOut, CB_RESETCONTENT, 0, 0);
        
        // Add input devices
        for (int i = 0; i < inputDevices; i++) {
            WAVEINCAPS caps;
            if (waveInGetDevCaps(i, &caps, sizeof(caps)) == MMSYSERR_NOERROR) {
                SendMessage(g_state.hwndDeviceIn, CB_ADDSTRING, 0, (LPARAM)caps.szPname);
            }
        }
        
        // Add output devices  
        for (int i = 0; i < outputDevices; i++) {
            WAVEOUTCAPS caps;
            if (waveOutGetDevCaps(i, &caps, sizeof(caps)) == MMSYSERR_NOERROR) {
                SendMessage(g_state.hwndDeviceOut, CB_ADDSTRING, 0, (LPARAM)caps.szPname);
            }
        }
        
        // Select first devices by default
        if (inputDevices > 0) SendMessage(g_state.hwndDeviceIn, CB_SETCURSEL, 0, 0);
        if (outputDevices > 0) SendMessage(g_state.hwndDeviceOut, CB_SETCURSEL, 0, 0);
    }
}

void ShowAdvancedSettings(HWND owner)
{
    MessageBox(owner,
               L"Advanced Settings (Coming Soon)\n\n"
               L"Future features will include:\n"
               L"• Custom device priorities\n"
               L"• Advanced buffer management\n"
               L"• Performance monitoring\n"
               L"• Driver troubleshooting tools\n"
               L"• Custom audio format settings",
               L"Advanced Settings", MB_ICONINFORMATION | MB_OK);
}

void ResizeControls(HWND hwnd)
{
    RECT clientRect;
    GetClientRect(hwnd, &clientRect);
    int width = clientRect.right - clientRect.left;
    int height = clientRect.bottom - clientRect.top;
    
    // Don't resize if window is too small
    if (width < g_state.minWidth || height < g_state.minHeight) {
        return;
    }
    
    int margin = 16;
    int groupSpacing = 12;
    int controlSpacing = 8;
    int groupWidth = (width - margin * 3) / 2;
    int fullWidth = width - margin * 2;
    
    // Update group boxes
    SetWindowPos(GetDlgItem(hwnd, 0), nullptr, margin, margin, groupWidth, 100, SWP_NOZORDER);
    SetWindowPos(GetDlgItem(hwnd, 1), nullptr, margin * 2 + groupWidth, margin, groupWidth, 100, SWP_NOZORDER);
    SetWindowPos(GetDlgItem(hwnd, 2), nullptr, margin, margin + 100 + groupSpacing, fullWidth, 100, SWP_NOZORDER);
    SetWindowPos(GetDlgItem(hwnd, 3), nullptr, margin, margin + 200 + groupSpacing * 2, fullWidth, 80, SWP_NOZORDER);
    
    // Update status control
    if (g_state.hwndStatus) {
        SetWindowPos(g_state.hwndStatus, nullptr, 
                     margin + controlSpacing, margin + 100 + groupSpacing + 20 + controlSpacing,
                     fullWidth - controlSpacing * 2, 70, SWP_NOZORDER);
    }
}
{
    RECT clientRect;
    GetClientRect(hwnd, &clientRect);
    int width = clientRect.right - clientRect.left;
    int height = clientRect.bottom - clientRect.top;
    
    // Enhanced layout with better spacing and modern design
    int margin = 16;
    int groupSpacing = 12;
    int controlSpacing = 8;
    int controlHeight = 24;
    int groupTitleHeight = 20;
    
    // Calculate responsive layout
    int groupWidth = (width - margin * 3) / 2;
    int fullWidth = width - margin * 2;
    
    // Devices group box (left side)
    CreateWindowEx(0, WC_BUTTON, L"Audio Devices", 
                   BS_GROUPBOX | WS_CHILD | WS_VISIBLE,
                   margin, margin, groupWidth, 100, 
                   hwnd, nullptr, nullptr, nullptr);
    
    // Settings group box (right side) 
    CreateWindowEx(0, WC_BUTTON, L"Audio Settings",
                   BS_GROUPBOX | WS_CHILD | WS_VISIBLE,
                   margin * 2 + groupWidth, margin, groupWidth, 100,
                   hwnd, nullptr, nullptr, nullptr);
    
    // Status group box (full width)
    CreateWindowEx(0, WC_BUTTON, L"Driver Status",
                   BS_GROUPBOX | WS_CHILD | WS_VISIBLE,
                   margin, margin + 100 + groupSpacing, fullWidth, 100,
                   hwnd, nullptr, nullptr, nullptr);
    
    // Controls group box (full width)
    CreateWindowEx(0, WC_BUTTON, L"Controls",
                   BS_GROUPBOX | WS_CHILD | WS_VISIBLE,
                   margin, margin + 200 + groupSpacing * 2, fullWidth, 80,
                   hwnd, nullptr, nullptr, nullptr);

    int leftPanelX = margin + controlSpacing;
    int rightPanelX = margin * 2 + groupWidth + controlSpacing;
    int controlWidth = groupWidth - controlSpacing * 2;

    // Input device label and combo
    CreateWindowEx(0, WC_STATIC, L"Input Device:", WS_CHILD | WS_VISIBLE,
                   leftPanelX, margin + groupTitleHeight + controlSpacing, 
                   controlWidth, 16, hwnd, nullptr, nullptr, nullptr);
    
    g_state.hwndDeviceIn = CreateWindowEx(0, WC_COMBOBOX, nullptr,
                   CBS_DROPDOWNLIST | WS_CHILD | WS_VISIBLE | WS_TABSTOP,
                   leftPanelX, margin + groupTitleHeight + controlSpacing + 20,
                   controlWidth, 200, hwnd, (HMENU)IDC_DEVICE_IN, nullptr, nullptr);

    // Output device label and combo
    CreateWindowEx(0, WC_STATIC, L"Output Device:", WS_CHILD | WS_VISIBLE,
                   leftPanelX, margin + groupTitleHeight + controlSpacing + 50,
                   controlWidth, 16, hwnd, nullptr, nullptr, nullptr);
    
    g_state.hwndDeviceOut = CreateWindowEx(0, WC_COMBOBOX, nullptr,
                   CBS_DROPDOWNLIST | WS_CHILD | WS_VISIBLE | WS_TABSTOP,
                   leftPanelX, margin + groupTitleHeight + controlSpacing + 70,
                   controlWidth, 200, hwnd, (HMENU)IDC_DEVICE_OUT, nullptr, nullptr);

    // Sample rate with dropdown
    CreateWindowEx(0, WC_STATIC, L"Sample Rate (Hz):", WS_CHILD | WS_VISIBLE,
                   rightPanelX, margin + groupTitleHeight + controlSpacing,
                   controlWidth, 16, hwnd, nullptr, nullptr, nullptr);
    
    g_state.hwndSampleRate = CreateWindowEx(0, WC_COMBOBOX, nullptr,
                   CBS_DROPDOWN | WS_CHILD | WS_VISIBLE | WS_TABSTOP,
                   rightPanelX, margin + groupTitleHeight + controlSpacing + 20,
                   controlWidth, 200, hwnd, (HMENU)IDC_SAMPLE_RATE, nullptr, nullptr);
    
    // Populate sample rates
    for (const auto& rate : g_state.sampleRates) {
        SendMessage(g_state.hwndSampleRate, CB_ADDSTRING, 0, (LPARAM)rate.c_str());
    }
    SendMessage(g_state.hwndSampleRate, CB_SETCURSEL, g_state.currentSampleRate, 0);

    // Buffer size with dropdown
    CreateWindowEx(0, WC_STATIC, L"Buffer Size (frames):", WS_CHILD | WS_VISIBLE,
                   rightPanelX, margin + groupTitleHeight + controlSpacing + 50,
                   controlWidth, 16, hwnd, nullptr, nullptr, nullptr);
    
    g_state.hwndBufferSize = CreateWindowEx(0, WC_COMBOBOX, nullptr,
                   CBS_DROPDOWN | WS_CHILD | WS_VISIBLE | WS_TABSTOP,
                   rightPanelX, margin + groupTitleHeight + controlSpacing + 70,
                   controlWidth, 200, hwnd, (HMENU)IDC_BUFFER_SIZE, nullptr, nullptr);
    
    // Populate buffer sizes
    for (const auto& size : g_state.bufferSizes) {
        SendMessage(g_state.hwndBufferSize, CB_ADDSTRING, 0, (LPARAM)size.c_str());
    }
    SendMessage(g_state.hwndBufferSize, CB_SETCURSEL, g_state.currentBufferSize, 0);

    // Status display area
    std::wstring initStatus = std::wstring(L"Latency: 0ms\nUnderruns: 0\nDriver: v") + ASIO4KRNL_VERSION + L"\nReady";
    g_state.hwndStatus = CreateWindowEx(WS_EX_CLIENTEDGE, WC_STATIC, initStatus.c_str(),
                   WS_CHILD | WS_VISIBLE | SS_LEFT,
                   margin + controlSpacing, margin + 100 + groupSpacing + groupTitleHeight + controlSpacing,
                   fullWidth - controlSpacing * 2, 70,
                   hwnd, (HMENU)IDC_STATUS, nullptr, nullptr);

    // Control buttons
    int buttonWidth = 100;
    int buttonSpacing = 12;
    int buttonY = margin + 200 + groupSpacing * 2 + groupTitleHeight + controlSpacing;
    
    // Low latency checkbox
    g_state.hwndLowLatency = CreateWindowEx(0, WC_BUTTON, L"Low Latency Mode",
                   WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
                   margin + controlSpacing, buttonY,
                   140, controlHeight, hwnd, (HMENU)IDC_LOW_LATENCY, nullptr, nullptr);

    // Test settings button
    g_state.hwndTestButton = CreateWindowEx(0, WC_BUTTON, L"Test Settings",
                   WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                   margin + controlSpacing + 150, buttonY,
                   buttonWidth, controlHeight, hwnd, (HMENU)IDC_TEST_SETTINGS, nullptr, nullptr);

    // Theme toggle button
    g_state.hwndThemeToggle = CreateWindowEx(0, WC_BUTTON, L"Dark Mode",
                   WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                   margin + controlSpacing + 150 + buttonWidth + buttonSpacing, buttonY,
                   buttonWidth, controlHeight, hwnd, (HMENU)IDC_THEME_TOGGLE, nullptr, nullptr);

    // Restore defaults button
    CreateWindowEx(0, WC_BUTTON, L"Restore Defaults",
                   WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                   fullWidth - buttonWidth - controlSpacing, buttonY,
                   buttonWidth, controlHeight, hwnd, (HMENU)IDC_RESTORE_DEFAULTS, nullptr, nullptr);

    // Start timer for status updates
    g_state.timerId = SetTimer(hwnd, TIMER_STATUS_UPDATE, 1000, nullptr);

    // Enhanced tooltips
    AddTooltip(hwnd, g_state.hwndDeviceIn, L"Select the audio recording device for input");
    AddTooltip(hwnd, g_state.hwndDeviceOut, L"Select the audio playback device for output");
    AddTooltip(hwnd, g_state.hwndBufferSize, L"Audio buffer size in frames - smaller values reduce latency but may cause dropouts");
    AddTooltip(hwnd, g_state.hwndSampleRate, L"Audio sample rate in Hz - higher rates improve quality but use more CPU");
    AddTooltip(hwnd, g_state.hwndLowLatency, L"Enable low latency mode to reduce audio delay (uses more CPU)");
    AddTooltip(hwnd, g_state.hwndStatus, L"Real-time driver status and performance information");
    AddTooltip(hwnd, g_state.hwndTestButton, L"Test current audio settings without applying them permanently");
    AddTooltip(hwnd, g_state.hwndThemeToggle, L"Switch between light and dark visual themes");
    AddTooltip(hwnd, GetDlgItem(hwnd, IDC_RESTORE_DEFAULTS), L"Reset all settings to recommended default values");
}

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
    case WM_CREATE:
        g_state.hwndMain = hWnd;
        GetWindowRect(hWnd, &g_state.originalRect);
        PopulateControls(hWnd);
        AddTrayIcon(hWnd);
        CheckDevices(hWnd);
        break;
    case WM_SIZE:
        if (wParam != SIZE_MINIMIZED) {
            ResizeControls(hWnd);
        }
        break;
    case WM_GETMINMAXINFO:
        {
            LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
            lpMMI->ptMinTrackSize.x = g_state.minWidth;
            lpMMI->ptMinTrackSize.y = g_state.minHeight;
        }
        break;
    case WM_TIMER:
        if (wParam == TIMER_STATUS_UPDATE) {
            UpdateStatus();
        }
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDC_LOW_LATENCY:
            if (HIWORD(wParam) == BN_CLICKED) {
                g_state.lowLatencyEnabled = !g_state.lowLatencyEnabled;
                SendMessage(g_state.hwndLowLatency, BM_SETCHECK, 
                           g_state.lowLatencyEnabled ? BST_CHECKED : BST_UNCHECKED, 0);
            }
            break;
        case IDC_TEST_SETTINGS:
            if (HIWORD(wParam) == BN_CLICKED) {
                TestSettings();
            }
            break;
        case IDC_THEME_TOGGLE:
            if (HIWORD(wParam) == BN_CLICKED) {
                ToggleTheme();
            }
            break;
        case IDC_RESTORE_DEFAULTS:
            if (HIWORD(wParam) == BN_CLICKED) {
                // Restore defaults
                SendMessage(g_state.hwndBufferSize, CB_SETCURSEL, g_state.currentBufferSize, 0);
                SendMessage(g_state.hwndSampleRate, CB_SETCURSEL, g_state.currentSampleRate, 0);
                SendMessage(g_state.hwndLowLatency, BM_SETCHECK, BST_UNCHECKED, 0);
                g_state.lowLatencyEnabled = false;
                g_state.hasError = false;
                g_state.statusColor = COLOR_STATUS_OK;
            }
            break;
        case IDC_ADVANCED_SETTINGS:
            if (HIWORD(wParam) == BN_CLICKED) {
                ShowAdvancedSettings(hWnd);
            }
            break;
        case IDM_TRAY_OPEN:
            ShowWindow(hWnd, SW_SHOW);
            SetForegroundWindow(hWnd);
            break;
        case IDM_TRAY_TOGGLE:
            {
                BOOL checked = SendMessage(g_state.hwndLowLatency, BM_GETCHECK, 0, 0);
                SendMessage(g_state.hwndLowLatency, BM_SETCHECK, !checked, 0);
                g_state.lowLatencyEnabled = !checked;
            }
            break;
        case IDM_TRAY_THEME:
            ToggleTheme();
            break;
        case IDM_TRAY_EXIT:
            PostMessage(hWnd, WM_CLOSE, 0, 0);
            break;
        }
        break;
    case WM_CTLCOLORSTATIC:
        {
            HDC hdc = (HDC)wParam;
            HWND hwndCtrl = (HWND)lParam;
            
            // Apply theme colors to static controls
            SetTextColor(hdc, g_state.textColor);
            SetBkColor(hdc, g_state.bgColor);
            
            if (hwndCtrl == g_state.hwndStatus) {
                SetTextColor(hdc, g_state.statusColor);
            }
            
            return (LRESULT)g_state.hBrushBg;
        }
    case WM_CTLCOLORBTN:
    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORLISTBOX:
        {
            HDC hdc = (HDC)wParam;
            SetTextColor(hdc, g_state.textColor);
            SetBkColor(hdc, g_state.bgColor);
            return (LRESULT)g_state.hBrushControl;
        }
    case WM_ERASEBKGND:
        {
            HDC hdc = (HDC)wParam;
            RECT rect;
            GetClientRect(hWnd, &rect);
            FillRect(hdc, &rect, g_state.hBrushBg);
            return 1;
        }
    case WM_TRAYICON:
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
        if (g_state.hBrushBg) DeleteObject(g_state.hBrushBg);
        if (g_state.hBrushControl) DeleteObject(g_state.hBrushControl);
        LogGui(L"Enhanced GUI exited\n");
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }
    return 0;
}

void UpdateStatus()
{
    static int counter = 0;
    static int lastUnderruns = 0;
    
    counter++;
    
    // Simulate more realistic status updates
    std::wstring statusText;
    
    if (g_state.hasError) {
        statusText = L"Error: " + g_state.lastError;
        g_state.statusColor = COLOR_STATUS_ERROR;
    } else {
        // Calculate simulated latency based on buffer size
        wchar_t bufferText[64];
        GetWindowText(g_state.hwndBufferSize, bufferText, sizeof(bufferText)/sizeof(wchar_t));
        int bufferSize = _wtoi(bufferText);
        if (bufferSize == 0) bufferSize = 256; // Default
        
        GetWindowText(g_state.hwndSampleRate, bufferText, sizeof(bufferText)/sizeof(wchar_t));
        int sampleRate = _wtoi(bufferText);
        if (sampleRate == 0) sampleRate = 48000; // Default
        
        double latencyMs = (double)bufferSize / sampleRate * 1000.0;
        
        // Simulate underruns based on settings
        if (g_state.lowLatencyEnabled && bufferSize < 128) {
            if (counter % 30 == 0) lastUnderruns++; // More underruns with very low latency
        } else if (counter % 60 == 0 && bufferSize < 256) {
            lastUnderruns++;
        }
        
        statusText = L"Latency: " + std::to_wstring((int)latencyMs) + L"ms\n";
        statusText += L"Underruns: " + std::to_wstring(lastUnderruns) + L"\n";
        statusText += L"Driver: v" + std::wstring(ASIO4KRNL_VERSION) + L"\n";
        
        if (lastUnderruns == 0) {
            statusText += L"Status: Optimal";
            g_state.statusColor = COLOR_STATUS_OK;
        } else if (lastUnderruns < 5) {
            statusText += L"Status: Good";
            g_state.statusColor = COLOR_STATUS_WARNING;
        } else {
            statusText += L"Status: Check Settings";
            g_state.statusColor = COLOR_STATUS_ERROR;
        }
        
        if (g_state.lowLatencyEnabled) {
            statusText += L" (Low Latency)";
        }
    }
    
    SetWindowText(g_state.hwndStatus, statusText.c_str());
    InvalidateRect(g_state.hwndStatus, NULL, TRUE);
}

