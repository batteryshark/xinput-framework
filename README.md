# xinput_framework

`xinput_framework` is a small C wrapper around Windows XInput for adding controller support to games, emulators, and other applications that want a simple polling API. It is meant to be easy to drop into older or custom input code without pulling in a larger input framework.

## Features

- Windows/XInput only.
- C-compatible public header.
- Polling model: call `update_gamepad()` once per frame or input tick.
- Four public snapshots: `gamepad_p1` through `gamepad_p4`.
- Disconnected controllers publish neutral zero input.
- Stick values pass through a 10% deadzone, tunable at compile time, runtime, or launch (see [Deadzone Tuning](#deadzone-tuning)).
- Vibration through `set_gamepad_vibration()` and `stop_gamepad_vibration()`.

## Minimal Usage

```c
#include "xinput.h"

void game_tick(void)
{
	update_gamepad();

	if (gamepad_p1.btn_a)
	{
		/* handle A */
	}

	if (gamepad_p1.trigger_r2 > 128)
	{
		set_gamepad_vibration(0, 32000, 16000);
	}
	else
	{
		stop_gamepad_vibration(0);
	}
}
```

## Build Notes

Compile `xinput.c` with your Windows C or C++ project. The implementation includes `<Windows.h>` and `<XInput.h>` and links `xinput9_1_0.lib` automatically when built with MSVC. Other toolchains should link the XInput library in their build settings.

The public API is intentionally plain C. Vibration is output state, so it is controlled with functions instead of being stored in `GamepadState`, which represents input snapshots only.

## Deadzone Tuning

The default deadzone is `3277`, which is about 10% of the XInput thumbstick range. Different games may want different values depending on camera sensitivity, menu navigation, controller drift, or driving/aiming feel.

Set it in code:

```c
set_gamepad_deadzone(5000);
set_gamepad_deadzone_enabled(TRUE);
```

Or set it when launching a game without rebuilding:

```sh
XINPUT_FRAMEWORK_DEADZONE=5000 ./game.exe
```

Values are clamped to `0..32767`. The environment variable is read once, the first time the library needs the deadzone.

## MSYS2 / MinGW

From an MSYS2 MinGW shell, build a static library for the active shell architecture:

```sh
mingw32-make
```

Build both cross-target libraries when the prefixed toolchains are installed:

```sh
mingw32-make both
```

Outputs:

- `build/native/libxinput_framework.a`
- `build/x86/libxinput_framework.a`
- `build/x64/libxinput_framework.a`

DLL targets are available too:

```sh
mingw32-make dll-native
mingw32-make dll-x86
mingw32-make dll-x64
```

If your MSYS2 shell only exposes `gcc`/`ar`, use the native target from the matching `MINGW32` or `MINGW64` shell. If you have different cross-compiler names, override them:

```sh
mingw32-make x64 CC_X64=gcc AR_X64=ar
```

Applications that link the static library still need to link XInput, for example with `-lxinput9_1_0`.

## Why XInput

Microsoft recommends GameInput for new game input code, and it covers more device classes and haptics. This library stays on XInput because it is small, C-friendly, builds under MinGW/MSYS2, and fits retrofitting controllers into existing games. GameInput's PC package also requires Windows 10 19H1 or newer, which drops the older targets XInput still reaches.
