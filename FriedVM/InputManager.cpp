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
    HANDLE hConsoleOut;
    HANDLE hConsole;
    DWORD originalConsoleMode;

    WindowsConsole() {
        hConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);
        hConsole = GetStdHandle(STD_INPUT_HANDLE);
        GetConsoleMode(hConsole, &originalConsoleMode);
    }

    void setInputModePrinting() {
        FlushConsoleInputBuffer(hConsole);
        SetConsoleMode(hConsole, originalConsoleMode); // Return to default mode
    }

    void setInputModeReading() {
        DWORD newMode = ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT | ENABLE_PROCESSED_INPUT;
        SetConsoleMode(hConsole, newMode);  // Set raw input mode to immediately capture keypresses without buffering
    }
};
#endif

InputManager::InputManager()
{
#ifdef _WIN32
    pConsole = new WindowsConsole();
#else
    keyCodeMap[0x57] = 0x19; // VK_W -> XK_W
    keyCodeMap[0x41] = 0x1E; // VK_A -> XK_A
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

void InputManager::clearScreen()
{
#ifdef _WIN32
    HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOutput == INVALID_HANDLE_VALUE) return;

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!GetConsoleScreenBufferInfo(hOutput, &csbi)) return;

    COORD topLeft = { 0, 0 };
    DWORD dwSize = csbi.dwSize.X * csbi.dwSize.Y;
    DWORD dwCount;

    // Fill the entire screen with spaces
    FillConsoleOutputCharacter(hOutput, ' ', dwSize, topLeft, &dwCount);

    // Reset the console text attributes
    FillConsoleOutputAttribute(hOutput, csbi.wAttributes, dwSize, topLeft, &dwCount);

    // Move the cursor back to the top left
    SetConsoleCursorPosition(hOutput, topLeft);
#else
    // Use ANSI escape codes for Linux/macOS to clear the screen and move the cursor
    std::cout << "\033[2J\033[H" << std::flush;
#endif
}

void InputManager::setCursor(int x, int y)
{
#ifdef _WIN32
    COORD position = { static_cast<SHORT>(x), static_cast<SHORT>(y) };
    SetConsoleCursorPosition(pConsole->hConsoleOut, position);
#else
    std::cout << "\033[" << y + 1 << ";" << x + 1 << "H";
#endif
}

void InputManager::getCursor(int* x, int* y)
{
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(pConsole->hConsoleOut, &csbi)) {
        *x = csbi.dwCursorPosition.X;
        *y = csbi.dwCursorPosition.Y;
    }
    else {
        *x = 0;
        *y = 0; // Default fallback
    }
#else
    std::cout << "\033[6n"; // Request cursor position
    std::cin.ignore(2);    // Ignore the ESC[
    std::cin >> y;         // Read the row
    std::cin.ignore(1);    // Ignore the semicolon
    std::cin >> x;         // Read the column
    x -= 1;                // Convert to 0-based index
    y -= 1;                // Convert to 0-based index
#endif
}

