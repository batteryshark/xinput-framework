
#include <Windows.h>
#include <stdlib.h>
#include <string.h>

#include <XInput.h>
#ifdef _MSC_VER
#pragma comment(lib,"xinput9_1_0.lib")
#endif

#include "xinput.h"

typedef struct tCONTROLLER_STATE{
	XINPUT_STATE state;
	BOOL connected;
}CONTROLLER_STATE;

GamepadState gamepad_p1;
GamepadState gamepad_p2;
GamepadState gamepad_p3;
GamepadState gamepad_p4;

#ifndef XINPUT_FRAMEWORK_INPUT_DEADZONE
#define XINPUT_FRAMEWORK_INPUT_DEADZONE 3277  // 10% of the +/- 32767 thumbstick range.
#endif

static CONTROLLER_STATE controllers[XINPUT_FRAMEWORK_MAX_CONTROLLERS];
static BYTE deadzone_on = TRUE;
static SHORT deadzone = XINPUT_FRAMEWORK_INPUT_DEADZONE;
static BOOL environment_loaded = FALSE;

static SHORT clamp_deadzone(long value)
{
	if (value < 0)
		return 0;

	if (value > 32767)
		return 32767;

	return (SHORT)value;
}

static void load_environment_settings(void)
{
	char value[32];
	DWORD length;
	char* end;
	long parsed;

	if (environment_loaded)
		return;

	environment_loaded = TRUE;
	length = GetEnvironmentVariableA("XINPUT_FRAMEWORK_DEADZONE", value, sizeof(value));
	if (length == 0 || length >= sizeof(value))
		return;

	parsed = strtol(value, &end, 10);
	if (end == value)
		return;

	deadzone = clamp_deadzone(parsed);
}

static GamepadState* gamepad_for_index(DWORD index)
{
	switch (index)
	{
	case 0:
		return &gamepad_p1;
	case 1:
		return &gamepad_p2;
	case 2:
		return &gamepad_p3;
	case 3:
		return &gamepad_p4;
	default:
		return NULL;
	}
}

static void apply_deadzone(SHORT* x, SHORT* y)
{
	if ((*x < deadzone && *x > -deadzone) &&
		(*y < deadzone && *y > -deadzone))
	{
		*x = 0;
		*y = 0;
	}
}

static void map_xinput_gamepad(const XINPUT_GAMEPAD* source, GamepadState* target)
{
	WORD buttons = source->wButtons;

	target->analog_lx = source->sThumbLX;
	target->analog_ly = source->sThumbLY;
	target->analog_rx = source->sThumbRX;
	target->analog_ry = source->sThumbRY;

	if (deadzone_on)
	{
		apply_deadzone(&target->analog_lx, &target->analog_ly);
		apply_deadzone(&target->analog_rx, &target->analog_ry);
	}

	target->btn_a = (buttons & XINPUT_GAMEPAD_A) ? TRUE : FALSE;
	target->btn_b = (buttons & XINPUT_GAMEPAD_B) ? TRUE : FALSE;
	target->btn_x = (buttons & XINPUT_GAMEPAD_X) ? TRUE : FALSE;
	target->btn_y = (buttons & XINPUT_GAMEPAD_Y) ? TRUE : FALSE;
	target->btn_l1 = (buttons & XINPUT_GAMEPAD_LEFT_SHOULDER) ? TRUE : FALSE;
	target->btn_r1 = (buttons & XINPUT_GAMEPAD_RIGHT_SHOULDER) ? TRUE : FALSE;
	target->btn_start = (buttons & XINPUT_GAMEPAD_START) ? TRUE : FALSE;
	target->btn_back = (buttons & XINPUT_GAMEPAD_BACK) ? TRUE : FALSE;
	target->dpad_down = (buttons & XINPUT_GAMEPAD_DPAD_DOWN) ? TRUE : FALSE;
	target->dpad_left = (buttons & XINPUT_GAMEPAD_DPAD_LEFT) ? TRUE : FALSE;
	target->dpad_right = (buttons & XINPUT_GAMEPAD_DPAD_RIGHT) ? TRUE : FALSE;
	target->dpad_up = (buttons & XINPUT_GAMEPAD_DPAD_UP) ? TRUE : FALSE;
	target->btn_l3 = (buttons & XINPUT_GAMEPAD_LEFT_THUMB) ? TRUE : FALSE;
	target->btn_r3 = (buttons & XINPUT_GAMEPAD_RIGHT_THUMB) ? TRUE : FALSE;
	target->trigger_l2 = source->bLeftTrigger;
	target->trigger_r2 = source->bRightTrigger;
}

static HRESULT update_controller_state(void)
{
	for (DWORD i = 0; i < XINPUT_FRAMEWORK_MAX_CONTROLLERS; i++)
	{
		DWORD result = XInputGetState(i, &controllers[i].state);
		controllers[i].connected = (result == ERROR_SUCCESS);
	}

	return S_OK;
}

void update_gamepad(void)
{
	load_environment_settings();
	update_controller_state();

	for (DWORD i = 0; i < XINPUT_FRAMEWORK_MAX_CONTROLLERS; i++)
	{
		GamepadState* target = gamepad_for_index(i);
		if (target == NULL)
			continue;

		memset(target, 0x00, sizeof(*target));
		if (controllers[i].connected)
			map_xinput_gamepad(&controllers[i].state.Gamepad, target);
	}
}

void set_gamepad_deadzone(SHORT value)
{
	load_environment_settings();
	deadzone = clamp_deadzone(value);
}

SHORT get_gamepad_deadzone(void)
{
	load_environment_settings();
	return deadzone;
}

void set_gamepad_deadzone_enabled(BOOL enabled)
{
	deadzone_on = enabled ? TRUE : FALSE;
}

DWORD set_gamepad_vibration(DWORD controller_index, WORD left_motor_speed, WORD right_motor_speed)
{
	XINPUT_VIBRATION vibration;

	if (controller_index >= XINPUT_FRAMEWORK_MAX_CONTROLLERS)
		return ERROR_INVALID_PARAMETER;

	vibration.wLeftMotorSpeed = left_motor_speed;
	vibration.wRightMotorSpeed = right_motor_speed;

	return XInputSetState(controller_index, &vibration);
}

DWORD stop_gamepad_vibration(DWORD controller_index)
{
	return set_gamepad_vibration(controller_index, 0, 0);
}
