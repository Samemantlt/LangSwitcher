@echo off
echo Resetting LangSwitcher first-run state...

reg delete "HKCU\Software\LangSwitcher" /v "FirstRunDone" /f 2>nul
if not errorlevel 1 (
    echo   [OK] Deleted FirstRunDone sentinel.
) else (
    echo   [--] FirstRunDone was not set, nothing to delete.
)

reg delete "HKCU\Software\Microsoft\Windows\CurrentVersion\Run" /v "LangSwitcher" /f 2>nul
if not errorlevel 1 (
    echo   [OK] Removed autostart Run entry.
) else (
    echo   [--] Autostart Run entry was not set, nothing to delete.
)

echo.
echo Done. Launch LangSwitcher to see the first-run prompt again.
