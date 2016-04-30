#include "stdafx.h"
#include "win_system.h"

HWND m_hwnd;

win_systemClass::win_systemClass(){}

win_systemClass::win_systemClass(const win_systemClass& other){}


win_systemClass::~win_systemClass()
{
	if (fullscreen) ChangeDisplaySettings(NULL, 0);

	DestroyWindow(m_hwnd);
	m_hwnd = nullptr;

	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = nullptr;

	ApplicationHandle = nullptr;
}

bool win_systemClass::Initialize(bool full_screen_resizeable, bool full_screen, int sWidth, int sHeight)
{
	fullscreen = full_screen;
	screen_width = sWidth;
	screen_height = sHeight;
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;


	// Get an external pointer to this object.
	ApplicationHandle = this;

	// Get the instance of this application.
	m_hinstance = GetModuleHandle(nullptr);

	// Give the application a name.
	m_applicationName = L"Aether Space Dynamics Simulator";

	// Setup the windows class with default settings.
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hinstance;
	wc.hIcon = LoadIconW(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = m_applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	// Register the window class.
	RegisterClassEx(&wc);

	// Setup the screen settings depending on whether it is running in full screen or in windowed mode.
	if (fullscreen)
	{
		// Determine the resolution of the clients desktop screen.
		if (!full_screen_resizeable)
		{
			sWidth = GetSystemMetrics(SM_CXSCREEN);
			sHeight = GetSystemMetrics(SM_CYSCREEN);
		}

		// If full screen set the screen to maximum size of the users desktop and 32bit.
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)sWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)sHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Change the display settings to full screen.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// Set the position of the window to the top left corner.
		posX = posY = 0;
	}

	else
	{
		// Place the window in the middle of the screen.
		posX = (GetSystemMetrics(SM_CXSCREEN) - sWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - sHeight) / 2;
	}

	// Create the window with the screen settings and get the handle to it.
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		posX, posY, sWidth, sHeight, NULL, NULL, m_hinstance, NULL);

	// Bring the window up on the screen and set it as main focus.
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	// Hide the mouse cursor.
	//ShowCursor(false);

	return true;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;

	switch (message)
	{

	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case ACCE_DOCK:
		{
						  Config.command = "dock";
						  break;
		}

		case ACCE_STAGE:
		{
						  Config.command = "stage";
						  break;
		}

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;

	// Check if the window is being destroyed.
	case WM_DESTROY:
	{
					   PostQuitMessage(0);
					   return 0;
	}

		// Check if the window is being closed.
	case WM_CLOSE:
	{
					 PostQuitMessage(0);
					 return 0;
	}

	case WM_CHAR:
	{
					switch (wParam)
					{
					case VK_F12:
					{

					}

					default:
					{
							   key_pressed = (char)wParam;
					}
					}
	}

		// All other messages pass to the message handler in the system class.
	default:
	{
			   return DefWindowProc(hWnd, message, wParam, lParam);
	}
	}

	return 0;
}

HWND win_systemClass::getHWND()
{
	return m_hwnd;
}

HINSTANCE win_systemClass::getHINSTANCE()
{
	return m_hinstance;
}

void win_systemClass::setWindowed()
{
	SetWindowLongPtr(m_hwnd, GWL_STYLE,
		WS_SYSMENU | WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE);
	MoveWindow(m_hwnd, (GetSystemMetrics(SM_CXSCREEN) - screen_width) / 2,
		(GetSystemMetrics(SM_CYSCREEN) - screen_height) / 2, screen_width, screen_height, TRUE);
}