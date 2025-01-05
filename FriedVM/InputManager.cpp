#include "InputManager.h"

// Platform-specific includes
#ifdef _WIN32
#include <windows.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

#ifdef _WIN32
class InputManager::WindowsConsole {
public:
    HANDLE hConsole;
    DWORD originalConsoleMode;

    WindowsConsole() {
        hConsole = GetStdHandle(STD_INPUT_HANDLE);
        GetConsoleMode(hConsole, &originalConsoleMode);
    }

    void setInputModePrinting() {
        SetConsoleMode(hConsole, originalConsoleMode); // Return to default mode
    }

    void setInputModeReading() {
        DWORD newMode = ENABLE_PROCESSED_INPUT | ENABLE_MOUSE_INPUT;
        SetConsoleMode(hConsole, newMode); // Set mode to allow input without buffering
    }
};
#endif

InputManager::InputManager()
{
    keyCodeMap[0x57] = 0x19; // VK_W -> XK_W
    keyCodeMap[0x41] = 0x1E; // VK_A -> XK_A

#ifdef _WIN32
    pConsole = new WindowsConsole();
#endif
}

bool InputManager::getKeyDown(int vkKey) {
#ifdef _WIN32
    return GetAsyncKeyState(vkKey) & 0x8000;
#else
    auto it = keyCodeMap.find(vkKey);
    if (it != keyCodeMap.end()) {
        int linuxKey = it->second;
        // Implement reading logic using evdev or X11
        return false; // Placeholder for Linux
    }
    return false;
#endif
}

void InputManager::setInputModePrinting() {
#ifdef _WIN32
    pConsole->setInputModePrinting();
#else
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag |= (ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
#endif
}

void InputManager::setInputModeReading() {
#ifdef _WIN32
    pConsole->setInputModeReading();
#else
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
#endif
}
