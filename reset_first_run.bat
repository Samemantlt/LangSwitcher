@echo off
echo Resetting LangSwitcher first-run state...

reg delete "HKCU\Software\LangSwitcher" /v "FirstRunDone" /f 2>nul
if not errorlevel 1 (
    echo   [OK] Deleted FirstRunDone sentinel.
) else (
    echo   [--] FirstRunDone was not set, nothing to delete.
)

rem Remove the Task Scheduler task (replaced the old Run key approach)
schtasks /Delete /TN "LangSwitcher" /F >nul 2>&1
if not errorlevel 1 (
    echo   [OK] Removed autostart scheduled task.
) else (
    echo   [--] Autostart task was not present, nothing to delete.
)

rem Also clean up the old Run key entry in case it was set by an older build
reg delete "HKCU\Software\Microsoft\Windows\CurrentVersion\Run" /v "LangSwitcher" /f 2>nul
if not errorlevel 1 (
    echo   [OK] Removed legacy autostart Run entry.
)

echo.
echo Done. Launch LangSwitcher to see the first-run prompt again.
