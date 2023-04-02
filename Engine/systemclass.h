#ifndef _SYSTEMCLASS_H_
#define _SYSTEMCLASS_H_

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#include "Options.h"
#include "inputclass.h"
#include "graphicsclass.h"
#include "fpsclass.h"
#include "timerclass.h"
#include "positionclass.h"
#include "resources/ResourceManager.h"

class SystemClass
{
public:
	SystemClass();
	SystemClass(const SystemClass&);
	~SystemClass();

	virtual bool init();
	void shutdown();
	void run();
	GraphicsClass* getGraphic() {
		return m_Graphics;
	};
	InputClass* getInput() {
		return m_Input;
	};


	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	void InitializeWindows(int&, int&);
	void ShutdownWindows();
	void createScreenshot();

protected:
	virtual bool frame();

protected:
	LPCWSTR m_applicationName;
	HINSTANCE m_hinstance;
	HWND m_hwnd;

	InputClass* m_Input;
	GraphicsClass* m_Graphics;
	FpsClass* m_Fps;
	TimerClass* m_Timer;
	PositionClass* m_Position;
	ResourceManager* m_ResourceManager;

	bool done;

public:
	int screenWidth = 1920;
	int screenHeight = 1080;
};


static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static SystemClass* ApplicationHandle = 0;


#endif