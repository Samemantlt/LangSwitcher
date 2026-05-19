cmake_minimum_required(VERSION 3.15)

project(LangSwitcher
    VERSION     1.0.0
    DESCRIPTION "Switch keyboard layout of selected text (EN <-> RU)"
    LANGUAGES   C RC
)

# ---------------------------------------------------------------------------
# Language standard
# ---------------------------------------------------------------------------
set(CMAKE_C_STANDARD          11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS        OFF)

# ---------------------------------------------------------------------------
# Sources
# ---------------------------------------------------------------------------
set(SOURCES
    src/main.c
    src/converter.c
    src/clipboard.c
    src/input.c
    src/switcher.c
    src/tray.c
    src/resources.rc
)

# ---------------------------------------------------------------------------
# Executable
# WIN32 keyword -> -mwindows on MinGW (no console window)
# ---------------------------------------------------------------------------
add_executable(LangSwitcher WIN32 ${SOURCES})

# ---------------------------------------------------------------------------
# Compile definitions
# ---------------------------------------------------------------------------
target_compile_definitions(LangSwitcher PRIVATE
    UNICODE
    _UNICODE
    _WIN32_WINNT=0x0601     # Windows 7+
)

# ---------------------------------------------------------------------------
# Compiler warnings & optimisation
# ---------------------------------------------------------------------------
target_compile_options(LangSwitcher PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -O2
)

# ---------------------------------------------------------------------------
# Include directories (headers live next to their .c files in src/)
# ---------------------------------------------------------------------------
target_include_directories(LangSwitcher PRIVATE src)

# ---------------------------------------------------------------------------
# Linker libraries (Win32)
# ---------------------------------------------------------------------------
target_link_libraries(LangSwitcher PRIVATE
    user32
    kernel32
    gdi32
    shell32
    ole32   # provides COM types required by shellapi.h on older MinGW
)
