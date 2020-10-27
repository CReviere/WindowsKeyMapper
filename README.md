# Windows Key Mapper
A program that allows users to create a set of keyboard shortcuts
that map to the key codes of their choice.

This program currently supports Windows only. In the future, MacOSX/Linux support may be added.

# Building
To build the program, you must have a version of the [MSVC toolchain](https://visualstudio.microsoft.com/)
installed. You can build using the Visual Studio CLI tools directly, or you can use [CMake](https://cmake.org/).

## Building with CMake
```cmd
cd WindowsKeyMapper
cmake . -B cmake-build
cmake --build cmake-build
```

## Building with cl.exe
Open a Developer Command Prompt, or load the appropriate [environment variables](https://docs.microsoft.com/en-us/cpp/build/building-on-the-command-line?view=vs-2019#developer_command_file_locations).

```cmd
cd WindowsKeyMapper
mkdir build && cd build
cl.exe /EHsc /W4 /WX ../src/WindowsKeyMapper.cpp user32.lib
```

# Usage
You can run the program directly, or put it in your startup folder so that it
starts automatically whenever you log in.

To run the program directly, invoke the executable:
```cmd
cd WindowsKeyMapper/build
./WindowsKeyMapper.exe
```

To run the program whenever you log in, move it to your startup folder:  
`move WindowsKeyMapper.exe "%ProgramData%\Microsoft\Windows\Start Menu\Programs\Startup"`

# Adding shortcuts
Currently, the shortcuts are hard-coded into the file [`WindowsKeyMapper.cpp`](src/WindowsKeyMapper.cpp#L35-L37).

Custom shortcut mappings are represented as [`MappedKeyboardShortcut`](src/WindowsKeyMapper.cpp#L24-L29)
objects, which define the custom shortcut as one or more [modifier keys](https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-registerhotkey)
in combination with a [virtual key code](https://docs.microsoft.com/en-us/windows/desktop/inputdev/virtual-key-codes).
The function `getCustomShortcuts()` creates the list of shortcuts to be used.

To add a custom shortcut, you will need to add a shortcut object to the vector 
defined in `getCustomShortcuts()`:
```cpp
static const std::vector<MappedKeyboardShortcut> shortcuts = {
    // Maps 'ctrl+shift+P' to media play/pause
    {
        1,                          // an arbitrary unique ID for the shortcut
        0x50,                       // the key code that is used for the shortcut
        MOD_CONTROL | MOD_SHIFT,    // a combination of the modifiers are used for the shortcut
        VK_MEDIA_PLAY_PAUSE         // the key code that should be triggered by the shortcut
    },
    // Add new shortcuts to the initializer list here...
};
```

In the future, shortcuts may be defined in an external configuration file.
