#ifndef _WIN_SYSTEM_H_
#define _WIN_SYSTEM_H_

#include "stdafx.h"
#include "Resource.h"

class win_systemClass
{
public:
	win_systemClass();
	win_systemClass(const win_systemClass&);
	~win_systemClass();

	bool Initialize(bool full_screen_resizeable, bool fullscreen, int screenWidth, int screenHeight);
	HWND getHWND();
	HINSTANCE getHINSTANCE();
	void setWindowed();

private:
	LPCWSTR m_applicationName;
	HINSTANCE m_hinstance;
	bool fullscreen;
	int screen_width, screen_height;
};


/////////////////////////
// FUNCTION PROTOTYPES //
/////////////////////////
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

/////////////
// GLOBALS //
/////////////
static win_systemClass* ApplicationHandle = nullptr;

#endif

