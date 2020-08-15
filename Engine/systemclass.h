#ifndef _SYSTEMCLASS_H_
#define _SYSTEMCLASS_H_


// PRE-PROCESSING DIRECTIVES //
#define WIN32_LEAN_AND_MEAN


// INCLUDES //
#include <windows.h>


#include "inputclass.h"
#include "graphicsclass.h"


class SystemClass
{
public:
	SystemClass();
	SystemClass(const SystemClass&);
	~SystemClass();

	bool Initialize();
	void Shutdown();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	bool Frame();
	void InitializeWindows(int&, int&);
	void ShutdownWindows();
	void createScreenshot();

private:
	LPCWSTR m_applicationName;
	HINSTANCE m_hinstance;
	HWND m_hwnd;

	InputClass* m_Input;
	GraphicsClass* m_Graphics;

	int screenWidth = 1024;
	int screenHeight = 768;
};


static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


static SystemClass* ApplicationHandle = 0;


#endif