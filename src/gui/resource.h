#pragma once

// Control IDs
#define IDC_DEVICE_IN          100
#define IDC_DEVICE_OUT         101
#define IDC_BUFFER_SIZE        102
#define IDC_SAMPLE_RATE        103
#define IDC_LOW_LATENCY        104
#define IDC_STATUS             105
#define IDC_RESTORE_DEFAULTS   106
#define IDC_TEST_SETTINGS      107
#define IDC_THEME_TOGGLE       108
#define IDC_ADVANCED_SETTINGS  109
#define IDC_ABOUT              110

// Menu IDs
#define IDM_TRAY_OPEN          2001
#define IDM_TRAY_TOGGLE        2002
#define IDM_TRAY_EXIT          2003
#define IDM_TRAY_THEME         2004

// Timer IDs
#define TIMER_STATUS_UPDATE    1

// Messages
#define WM_TRAYICON           (WM_USER + 1)

// Colors for themes
#define COLOR_LIGHT_BG         RGB(255, 255, 255)
#define COLOR_LIGHT_TEXT       RGB(0, 0, 0)
#define COLOR_LIGHT_BORDER     RGB(200, 200, 200)
#define COLOR_LIGHT_ACCENT     RGB(0, 120, 215)

#define COLOR_DARK_BG          RGB(32, 32, 32)
#define COLOR_DARK_TEXT        RGB(255, 255, 255)
#define COLOR_DARK_BORDER      RGB(68, 68, 68)
#define COLOR_DARK_ACCENT      RGB(0, 120, 215)

// Status colors
#define COLOR_STATUS_OK        RGB(0, 150, 0)
#define COLOR_STATUS_WARNING   RGB(255, 165, 0)
#define COLOR_STATUS_ERROR     RGB(220, 20, 60)