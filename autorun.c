#include <string.h>
#include <windows.h>
#include <winreg.h>
#include "autorun.h"
#include "main.h"

#define RUN_KEY "Software\\Microsoft\\Windows\\CurrentVersion\\Run"
#define VALUE_NAME "ClockTray"
#define PATH_MAX_SIZE 256

char loaded_exe_path[PATH_MAX_SIZE];
unsigned int loaded_exe_path_size = 0;

int autorun_init()
{
    loaded_exe_path_size = GetModuleFileName(NULL, loaded_exe_path,
        PATH_MAX_SIZE) + 1;

    return GetLastError() == ERROR_SUCCESS;
}

int autorun_get(int * existsp)
{
    HKEY hk;
    char path[PATH_MAX_SIZE];
    DWORD size = PATH_MAX_SIZE;
    DWORD type;
    int failed;

    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, RUN_KEY, 0, KEY_READ, &hk))
        return 0;
    failed = RegQueryValueEx(hk, VALUE_NAME, NULL, &type, (BYTE *) path,
        &size);
    RegFlushKey(hk);
    RegCloseKey(hk);

    if (existsp)
        *existsp = !failed;

    return !failed &&
        type == REG_SZ &&
        size == loaded_exe_path_size &&
        path[size - 1] == '\0' &&
        strcmp(path, loaded_exe_path) == 0;
}

void autorun_set(int enable)
{
    HKEY hk;

    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, RUN_KEY, 0, KEY_SET_VALUE, &hk))
        return;

    if (enable) {
        RegSetValueEx(hk, VALUE_NAME, 0, REG_SZ, (BYTE *) loaded_exe_path,
            loaded_exe_path_size);
    } else {
        RegDeleteValue(hk, VALUE_NAME);
    }

    RegFlushKey(hk);
    RegCloseKey(hk);
}

void autorun_fix()
{
    int exists;
    int match = autorun_get(&exists);

    if (exists && !match)
        autorun_set(1);
}
