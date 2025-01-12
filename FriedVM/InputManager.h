#pragma once
#include <unordered_map>

class InputManager
{
public:
    InputManager();

    // Get whether a key is down (cross-platform)
    bool getKeyDown(int vkKey);

    // Switch terminal to default printing mode
    void setInputModePrinting();

    // Switch terminal to input-reading mode
    void setInputModeReading();

    //clear the console cross platform
    void clearScreen();

    void setCursor(int x, int y);

    void getCursor(int *x, int *y);

private:
    // Forward declaration for Windows-specific variables
#ifdef _WIN32
    class WindowsConsole;
    WindowsConsole* pConsole;
#else
    // Map of Windows VK codes to Linux evdev codes or X11 equivalents
    std::unordered_map<int, int> keyCodeMap;
#endif
};
