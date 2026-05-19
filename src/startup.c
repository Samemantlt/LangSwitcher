/*
 * startup.c - "Start with Windows" helpers for LangSwitcher
 *
 * Because the executable requests administrator elevation (requireAdministrator
 * manifest), the HKCU\...\Run registry key cannot be used for autostart:
 * Windows silently skips Run entries that require UAC elevation at login.
 *
 * Instead we create a Task Scheduler task with "Run with highest privileges"
 * using the built-in schtasks.exe command-line tool — no COM/XML required.
 *
 *   Task name : LangSwitcher
 *   Trigger   : At log on of the current user
 *   Action    : Launch the running executable
 *   Run level : HIGHEST  (equivalent to "Run as administrator")
 */

#include "startup.h"

#include <wchar.h>
#include <stdio.h>

#define TASK_NAME L"LangSwitcher"

/* ---------------------------------------------------------------------------
 * Internal helpers
 * --------------------------------------------------------------------------- */

/** Run a command via cmd.exe /C and return its exit code. */
static int run_command(const WCHAR *cmdLine)
{
    STARTUPINFOW        si = {0};
    PROCESS_INFORMATION pi = {0};
    si.cb = sizeof(si);

    /* We need a mutable copy for CreateProcessW */
    WCHAR buf[2048];
    _snwprintf(buf, 2047, L"cmd.exe /C %s", cmdLine);
    buf[2047] = L'\0';

    BOOL ok = CreateProcessW(
        NULL, buf,
        NULL, NULL, FALSE,
        CREATE_NO_WINDOW,   /* hide the console window */
        NULL, NULL,
        &si, &pi);

    if (!ok)
        return -1;

    WaitForSingleObject(pi.hProcess, 10000);

    DWORD exitCode = 0;
    GetExitCodeProcess(pi.hProcess, &exitCode);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return (int)exitCode;
}

/** Query whether the scheduled task named TASK_NAME exists. */
static BOOL task_exists(void)
{
    /* schtasks /Query exits 0 if found, non-zero otherwise */
    WCHAR cmd[256];
    _snwprintf(cmd, 255,
               L"schtasks /Query /TN \"%s\" >NUL 2>&1",
               TASK_NAME);
    return (run_command(cmd) == 0);
}

/* ---------------------------------------------------------------------------
 * startup_is_enabled
 * --------------------------------------------------------------------------- */
BOOL startup_is_enabled(void)
{
    return task_exists();
}

/* ---------------------------------------------------------------------------
 * startup_set
 * --------------------------------------------------------------------------- */
BOOL startup_set(BOOL enable)
{
    if (enable) {
        /* Get the full path of the running executable */
        WCHAR exePath[MAX_PATH] = {0};
        GetModuleFileNameW(NULL, exePath, MAX_PATH);

        /*
         * Create (or replace) the task:
         *   /SC ONLOGON          – trigger: at logon
         *   /RL HIGHEST          – run level: highest privileges (admin)
         *   /F                   – force-overwrite if task already exists
         *   /DELAY 0000:05       – small delay so the desktop is ready
         */
        WCHAR cmd[1024];
        _snwprintf(cmd, 1023,
                   L"schtasks /Create /TN \"%s\" /TR \"\\\"%s\\\"\" "
                   L"/SC ONLOGON /RL HIGHEST /F /DELAY 0000:05 >NUL 2>&1",
                   TASK_NAME, exePath);
        return (run_command(cmd) == 0);
    } else {
        /* Delete the task */
        WCHAR cmd[256];
        _snwprintf(cmd, 255,
                   L"schtasks /Delete /TN \"%s\" /F >NUL 2>&1",
                   TASK_NAME);
        int rc = run_command(cmd);
        /* Exit code 1 means "task not found" — treat as success */
        return (rc == 0 || rc == 1);
    }
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
