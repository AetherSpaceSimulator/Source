#include "stdafx.h"

class fpsClass
{
public:
	fpsClass()
	{
		numFrames = 0;
		timeElapsed = 0;
	};

	fpsClass(const fpsClass&){};
	~fpsClass(){};

	void StartCounter()
	{
		LARGE_INTEGER li;
		if (!QueryPerformanceFrequency(&li))
			MessageBox(0, L"QueryPerformanceFrequency failed!", L"Error", MB_OK);

		PCFreq = float(li.QuadPart) / 1000.0f;

		QueryPerformanceCounter(&li);
		CounterStart = li.QuadPart;
	}

	float GetCounter()
	{
		LARGE_INTEGER li;
		QueryPerformanceCounter(&li);
		return float(li.QuadPart - CounterStart) / PCFreq;
	};

	float get_dt() { return dt; };
	float get_fps() { return fps; };
	void update()
	{
		dt = GetCounter() / 1000.0f - pasado;
		pasado = GetCounter() / 1000.0f;

		numFrames += 1.0f;
		timeElapsed += dt;
		if (timeElapsed >= 1.0f)
		{
			fps = numFrames;
			timeElapsed = 0.0f;
			numFrames = 0.0f;
		}
	};

private:
	float fps, dt, pasado, timeElapsed, numFrames, PCFreq;
	long long CounterStart;
};