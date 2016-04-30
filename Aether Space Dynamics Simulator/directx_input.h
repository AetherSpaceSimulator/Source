#ifndef _DIRECTX_INPUT_H_
#define _DIRECTX_INPUT_H_

#include "stdafx.h"
#include <dinput.h>

class directx_input
{
public:
	directx_input();
	directx_input(const directx_input&);
	~directx_input();

	bool Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight);
	bool update();

	bool KeyPressed(int key);
	Vector3 GetMouseSate();
	bool MouseButtonDown(int Button);

private:
	bool ReadKeyboard();
	bool ReadMouse();

private:
	IDirectInput8* m_directInput;
	IDirectInputDevice8* m_keyboard;
	IDirectInputDevice8* m_mouse;

	unsigned char m_keyboardState[256];
	DIMOUSESTATE m_mouseState;
};

#endif