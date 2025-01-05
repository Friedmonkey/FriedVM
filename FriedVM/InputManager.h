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

private:
    // Map of Windows VK codes to Linux evdev codes or X11 equivalents
    std::unordered_map<int, int> keyCodeMap;

    // Forward declaration for Windows-specific variables
#ifdef _WIN32
    class WindowsConsole;
    WindowsConsole* pConsole;
#endif
};
