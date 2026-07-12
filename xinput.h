#pragma once

#include <Windows.h>

#ifdef __cplusplus
extern "C" {
#endif

#define XINPUT_FRAMEWORK_MAX_CONTROLLERS 4

#pragma pack(push, 1)
typedef struct tGamepadState {
	BYTE btn_a;
	BYTE btn_b;
	BYTE btn_x;
	BYTE btn_y;
	BYTE btn_l1;
	BYTE btn_r1;
	BYTE btn_l3;
	BYTE btn_r3;
	BYTE btn_start;
	BYTE btn_back;
	BYTE dpad_up;
	BYTE dpad_down;
	BYTE dpad_left;
	BYTE dpad_right;
	BYTE trigger_l2;
	BYTE trigger_r2;
	SHORT analog_lx;
	SHORT analog_ly;
	SHORT analog_rx;
	SHORT analog_ry;
}GamepadState;
#pragma pack(pop)



// Public snapshots updated by update_gamepad(). Disconnected controllers are zeroed.
extern GamepadState gamepad_p1;
extern GamepadState gamepad_p2;
extern GamepadState gamepad_p3;
extern GamepadState gamepad_p4;

// Poll XInput and refresh gamepad_p1 through gamepad_p4.
void update_gamepad(void);

// Set the thumbstick deadzone threshold in raw XInput units, clamped to 0..32767.
void set_gamepad_deadzone(SHORT deadzone);

// Read the active thumbstick deadzone threshold.
SHORT get_gamepad_deadzone(void);

// Enable or disable deadzone filtering.
void set_gamepad_deadzone_enabled(BOOL enabled);

// Set motor speeds for controller_index 0..3. Speeds are 0..65535.
DWORD set_gamepad_vibration(DWORD controller_index, WORD left_motor_speed, WORD right_motor_speed);

// Stop both vibration motors for controller_index 0..3.
DWORD stop_gamepad_vibration(DWORD controller_index);

#ifdef __cplusplus
}
#endif
