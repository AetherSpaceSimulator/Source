// Aether Space Dynamics Simulator.cpp : Defines the entry point for the application.
//
#include "stdafx.h"
#include "Aether Space Dynamics Simulator.h"

#include "showMaster.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// Check for memory leaks during Debug runtime
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
#endif

	//_crtBreakAlloc = 1396217; // break at memory allocation number where the leak has occured

	showMasterClass* showMaster = new showMasterClass;

	HACCEL hAccelTable;

	if (showMaster->Initialize(true, 1280, 720, true, false))
	{
		MSG msg;
		bool done = false;

		ZeroMemory(&msg, sizeof(MSG));

		hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_AETHERSPACEDYNAMICSSIMULATOR));

		while (!done)
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}

			if (msg.message == WM_QUIT)	done = true;

			else done = showMaster->run_show();
		}
	}

	SAFE_DELETE(showMaster);

#if defined(DEBUG) | defined(_DEBUG)
	_CrtDumpMemoryLeaks();
#endif

	return 0;
}

/*
Common resolutions for 16:9

Width	Height	Standard
640		360		nHD
720		405
854		480
960		540		qHD
1024	576
1280	720		HD
1366	768
1600	900		HD+
1920	1080	Full HD
2048	1152
2560	1440	QHD
2880	1620
3200	1800
3840	2160	4K UHDTV
4096	2304
5120	2880	5K
7680	4320	8K UHDTV
15360	8640	16K
*/

/*
object types:
1: star
2: planet
3: moon
4: spacecraft
5: inside_out atmosphere
6: frontal atmosphere
7: clouds layer
8: planetary rings
9: landing triangle
10: Ground object
11: Landing surface

collision shape types:
0: primitive
1: compound
2: convex hull
*/
