/*
  ==============================================================================

    WindowsKeyMapper.cpp
    Created: 4 May 2020 12:44:11pm
    License: MIT (see LICENSE.txt)
    Author:  Connor Reviere

    Converts custom keyboard shortcuts into virtual key codes.

    Uses RegisterHotKey() to register the custom keyboard shortcuts,
    and processes the shortcuts in the message loop whenever
    WM_HOTKEY messages are received.

  ==============================================================================
*/

#include <windows.h>
#include <vector>
#include <iostream>

//==============================================================================
/** Maps a keyboard shortcut to the key code that it should generate. */
struct MappedKeyboardShortcut
{
    int id = 0;
    unsigned int modifier = 0;
    unsigned short keyCode = 0, targetKeyCode = 0;
};

//==============================================================================
/** Returns the custom keyboard shortcuts that should be registered. */
const std::vector<MappedKeyboardShortcut>& getCustomShortcuts()
{
    static const std::vector<MappedKeyboardShortcut> shortcuts = {
        { 1, 0x50, MOD_CONTROL | MOD_SHIFT, VK_MEDIA_PLAY_PAUSE },
    };

    return shortcuts;
}

//==============================================================================
/** Registers custom shortcuts as hotkeys that should be forwarded to this thread.

    @param setNoRepeatFlag  When true, the MOD_NOREPEAT flag is enabled so that
                            keyboard auto-repeat does not yield multiple hotkey notifications.
                            (Not supported on Windows Vista).
*/
bool registerHotKeys (const bool setNoRepeatFlag = true)
{
    std::cout << "Registering hot keys...\n";
    const auto& shortcuts = getCustomShortcuts();

    for (const auto& s : shortcuts)
    {
        UINT mod = setNoRepeatFlag ? s.modifier | MOD_NOREPEAT : s.modifier;

        if (RegisterHotKey (NULL, s.id, mod, static_cast<unsigned int> (s.keyCode)) == 0)
            return false;
    }

    return true;
}

//==============================================================================
/** Simulates a press and release for the given \p virtualKeyCode. */
bool simulateKeyPress (const unsigned short virtualKeyCode)
{
    std::cout << "Simulating key press (keycode: " << virtualKeyCode << ")...\n";
    INPUT in { 0 };
    in.type = INPUT_KEYBOARD;
    in.ki.wVk = virtualKeyCode;

    if (SendInput (1, &in, sizeof (INPUT)) != 1)
        return false;

    in.ki.dwFlags = KEYEVENTF_KEYUP;

    return SendInput (1, &in, sizeof (INPUT)) == 1;
}

//==============================================================================
/** Processes hot key messages, converting custom shortcuts into their target key codes. */
bool processHotKeyMessage (const MSG& msg)
{
    std::cout << "Processing hot key message...\n";
    const auto& shortcuts = getCustomShortcuts();
    const auto it = std::find_if (shortcuts.begin(), shortcuts.end(), [&msg](const MappedKeyboardShortcut& s) { 
        return static_cast<unsigned int> (s.id) == msg.wParam;
    });

    if (it != shortcuts.end())
    {
        if (! simulateKeyPress (it->targetKeyCode))
        {
            std::cerr << "Failed to simulate keypress.\n";
            return false;
        }
    }

    return true;
}

//==============================================================================
/** Define status codes that can be returned by the program. */
enum class StatusCode
{
    success = 0,
    registrationFailed = 1,
    virtualInputFailed = 2
};

//==============================================================================
/** Main message loop, passes hotkey messages to processHotKeyMessage(). */
StatusCode runMessageLoop()
{
    std::cout << "Starting message loop...\n";
    MSG msg = { 0 };

    while (GetMessage (&msg, NULL, 0, 0) != 0)
    {
        if (msg.message == WM_HOTKEY)
        {
            if (! processHotKeyMessage (msg))
            {
                std::cerr << "Failed to send virtual keypress.\n";
                return StatusCode::virtualInputFailed;
            }
        }
    }

    return StatusCode::success;
}

//==============================================================================
/** Program entry point. */
int main(int, char**)
{
    std::cout << "Starting application...\n";
    const auto res = registerHotKeys() ? runMessageLoop() : StatusCode::registrationFailed;
    std::cout << "Closing application...\n";
    return static_cast<int> (res);
}
