/*
 * startup.c - "Start with Windows" registry helpers for LangSwitcher
 *
 * Registry key: HKCU\Software\Microsoft\Windows\CurrentVersion\Run
 * Value name  : LangSwitcher
 * Value data  : Full path to the running executable (quoted).
 *
 * No elevated privileges are required because we write to HKCU.
 */

#include "startup.h"

#include <wchar.h>

#define RUN_KEY  L"Software\\Microsoft\\Windows\\CurrentVersion\\Run"
#define APP_NAME L"LangSwitcher"

/* ---------------------------------------------------------------------------
 * startup_is_enabled
 * --------------------------------------------------------------------------- */
BOOL startup_is_enabled(void)
{
    HKEY  hKey   = NULL;
    BOOL  result = FALSE;

    if (RegOpenKeyExW(HKEY_CURRENT_USER, RUN_KEY, 0, KEY_READ, &hKey)
            != ERROR_SUCCESS)
        return FALSE;

    /* Check whether the value exists */
    DWORD type  = 0;
    DWORD bytes = 0;
    LONG  rc    = RegQueryValueExW(hKey, APP_NAME, NULL, &type, NULL, &bytes);
    if (rc == ERROR_SUCCESS && type == REG_SZ)
        result = TRUE;

    RegCloseKey(hKey);
    return result;
}

/* ---------------------------------------------------------------------------
 * startup_set
 * --------------------------------------------------------------------------- */
BOOL startup_set(BOOL enable)
{
    HKEY hKey = NULL;
    LONG rc;

    rc = RegOpenKeyExW(HKEY_CURRENT_USER, RUN_KEY, 0, KEY_SET_VALUE, &hKey);
    if (rc != ERROR_SUCCESS)
        return FALSE;

    if (enable) {
        /* Build a quoted path: "C:\path\to\LangSwitcher.exe" */
        WCHAR exePath[MAX_PATH] = {0};
        GetModuleFileNameW(NULL, exePath, MAX_PATH);

        WCHAR quoted[MAX_PATH + 4] = {0};
        _snwprintf(quoted, MAX_PATH + 3, L"\"%s\"", exePath);

        DWORD bytes = (DWORD)((wcslen(quoted) + 1) * sizeof(WCHAR));
        rc = RegSetValueExW(hKey, APP_NAME, 0, REG_SZ,
                            (const BYTE *)quoted, bytes);
    } else {
        rc = RegDeleteValueW(hKey, APP_NAME);
        /* Treat "value not found" as success */
        if (rc == ERROR_FILE_NOT_FOUND)
            rc = ERROR_SUCCESS;
    }

    RegCloseKey(hKey);
    return (rc == ERROR_SUCCESS);
}

/* ---------------------------------------------------------------------------
 * startup_prompt_if_first_run
 *
 * Sentinel key : HKCU\Software\LangSwitcher
 * Sentinel value: "FirstRunDone"  (DWORD, 1)
 *
 * If the sentinel is absent the user is prompted once.
 * --------------------------------------------------------------------------- */
void startup_prompt_if_first_run(void)
{
#define SENTINEL_KEY  L"Software\\LangSwitcher"
#define SENTINEL_VAL  L"FirstRunDone"

    HKEY  hKey = NULL;
    DWORD done = 0, bytes = sizeof(done), type = 0;

    /* Try to read the sentinel (key may not exist yet) */
    LONG rc = RegOpenKeyExW(HKEY_CURRENT_USER, SENTINEL_KEY,
                            0, KEY_READ | KEY_WRITE, &hKey);

    if (rc == ERROR_SUCCESS) {
        rc = RegQueryValueExW(hKey, SENTINEL_VAL, NULL,
                              &type, (BYTE *)&done, &bytes);
        if (rc == ERROR_SUCCESS && done == 1) {
            /* Already ran before — nothing to do */
            RegCloseKey(hKey);
            return;
        }
    } else {
        /* Key doesn't exist yet — create it */
        DWORD disposition = 0;
        rc = RegCreateKeyExW(HKEY_CURRENT_USER, SENTINEL_KEY,
                             0, NULL, REG_OPTION_NON_VOLATILE,
                             KEY_READ | KEY_WRITE, NULL,
                             &hKey, &disposition);
        if (rc != ERROR_SUCCESS)
            return; /* Silently skip if we can't write registry */
    }

    /* --- First run: ask the user --- */
    int answer = MessageBoxW(
        NULL,
        L"Would you like LangSwitcher to start automatically with Windows?\n\n"
        L"You can change this at any time from the tray icon menu.",
        L"LangSwitcher \u2014 Start with Windows?",
        MB_YESNO | MB_ICONQUESTION);

    if (answer == IDYES)
        startup_set(TRUE);

    /* Record the sentinel so we never ask again */
    DWORD one = 1;
    RegSetValueExW(hKey, SENTINEL_VAL, 0, REG_DWORD,
                   (const BYTE *)&one, sizeof(one));

    RegCloseKey(hKey);

#undef SENTINEL_KEY
#undef SENTINEL_VAL
}
