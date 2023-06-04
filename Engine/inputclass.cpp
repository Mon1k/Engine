#include "inputclass.h"

InputClass::InputClass()
{
	m_directInput = 0;
	m_keyboard = 0;
	m_mouse = 0;
}


InputClass::InputClass(const InputClass& other)
{
}


InputClass::~InputClass()
{
}

bool InputClass::Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight)
{
	int i;
	HRESULT result;

	// Store the screen size which will be used for positioning the mouse cursor.
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// Initialize the location of the mouse on the screen.
	m_mouseX = m_screenWidth / 2;
	m_mouseY = m_screenHeight / 2;
	for (int i = 0; i < 4; i++) {
		m_PrevMouseState[i] = false;
	}
	for (int i = 0; i < 256; i++) {
		m_PrevKeyboardState[i] = false;
	}

	// Initialize the main direct input interface.
	result = DirectInput8Create(hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_directInput, NULL);
	if (FAILED(result)) {
		return false;
	}

	// Initialize the direct input interface for the keyboard.
	result = m_directInput->CreateDevice(GUID_SysKeyboard, &m_keyboard, NULL);
	if (FAILED(result)) {
		return false;
	}

	// Set the data format.  In this case since it is a keyboard we can use the predefined data format.
	result = m_keyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(result)) {
		return false;
	}

	// Set the cooperative level of the keyboard to not share with other programs.
	result = m_keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if (FAILED(result)) {
		return false;
	}

	// Now acquire the keyboard.
	result = m_keyboard->Acquire();
	if (FAILED(result)) {
		return false;
	}

	// Initialize the direct input interface for the mouse.
	result = m_directInput->CreateDevice(GUID_SysMouse, &m_mouse, NULL);
	if (FAILED(result)) {
		return false;
	}

	// Set the data format for the mouse using the pre-defined mouse data format.
	result = m_mouse->SetDataFormat(&c_dfDIMouse);
	if (FAILED(result)) {
		return false;
	}

	// Set the cooperative level of the mouse to share with other programs.
	result = m_mouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE | DISCL_NOWINKEY);
	if (FAILED(result)) {
		return false;
	}

	// Acquire the mouse.
	result = m_mouse->Acquire();
	if (FAILED(result)) {
		return false;
	}

	return true;
}

void InputClass::Shutdown()
{
	// Release the mouse.
	if (m_mouse) {
		m_mouse->Unacquire();
		m_mouse->Release();
		m_mouse = 0;
	}

	// Release the keyboard.
	if (m_keyboard) {
		m_keyboard->Unacquire();
		m_keyboard->Release();
		m_keyboard = 0;
	}

	// Release the main interface to direct input.
	if (m_directInput) {
		m_directInput->Release();
		m_directInput = 0;
	}
}

bool InputClass::IsKeyDown(unsigned int key)
{
	if (m_keyboardState[key] & 0x80) {
		return true;
	}

	return false;
}

bool InputClass::IsKeyDown()
{
	int size = sizeof(m_keyboardState) / sizeof(m_keyboardState[0]);
	for (int i = 0; i < size; i++) {
		if (m_keyboardState[i] & 0x80) {
			return true;
		}
	}

	return false;
}

InputClass::EventKey InputClass::getKeyDown()
{
	int size = sizeof(m_keyboardState) / sizeof(m_keyboardState[0]);
	EventKey event = {
		0,
		0,
		false
	};

	for (int i = 0; i < size; i++) {
		if ((m_keyboardState[i] & 0x80) && (i == DIK_LSHIFT || i == DIK_RSHIFT)) {
			event.shift = true;
			continue;
		}

		if ((m_keyboardState[i] & 0x80) && !m_PrevKeyboardState[i]) {
			event.key = i;
			event.symbol = getSymbolKey(i);
			m_PrevKeyboardState[i] = true;
		} else if (!(m_keyboardState[i] & 0x80) && m_PrevKeyboardState[i]) {
			m_PrevKeyboardState[i] = false;
		}
	}

	return event;
}

char InputClass::getSymbolKey(int code)
{
	int key = MapVirtualKeyA(code, MAPVK_VSC_TO_VK);
	unsigned short asciiValue;
	ToAscii(key, code, m_keyboardState, &asciiValue, 0);

	return static_cast<char>(asciiValue);
}

bool InputClass::IsEscapePressed()
{
	// Do a bitwise and on the keyboard state to check if the escape key is currently being pressed.
	if (m_keyboardState[DIK_ESCAPE] & 0x80) {
		return true;
	}

	return false;
}

void InputClass::GetMouseLocation(int& mouseX, int& mouseY)
{
	mouseX = m_mouseX;
	mouseY = m_mouseY;
}

int InputClass::getMouseButton()
{
	if (m_mouseButton >= 0) {
		return m_mouseButton;
	}

	for (int i = 0; i < 4; i++) {
		if (m_mouseState.rgbButtons[i] && !m_PrevMouseState[i]) {
			m_PrevMouseState[i] = true;
			m_mouseButton = i;
			return i;
		} else if (!m_mouseState.rgbButtons[i] && m_PrevMouseState[i]) {
			m_PrevMouseState[i] = false;
		}
	}
	
	return -1;
}

int InputClass::getMouseButtonPress()
{
	for (int i = 0; i < 4; i++) {
		if (m_mouseState.rgbButtons[i]) {
			return i;
		}
	}

	return -1;
}

bool InputClass::Frame()
{
	bool result;

	// Read the current state of the keyboard.
	result = ReadKeyboard();
	if (!result) {
		return false;
	}

	// Read the current state of the mouse.
	result = ReadMouse();
	if (!result) {
		return false;
	}

	// Process the changes in the mouse and keyboard.
	ProcessInput();

	return true;
}

void InputClass::resetState()
{
	for (int i = 0; i < 4; i++) {
		m_mouseState.rgbButtons[i] = 0;
	}

	int size = sizeof(m_keyboardState) / sizeof(m_keyboardState[0]);
	for (int i = 0; i < size; i++) {
		m_keyboardState[i] = 0;
	}
}

bool InputClass::ReadKeyboard()
{
	HRESULT result;

	// Read the keyboard device.
	result = m_keyboard->GetDeviceState(sizeof(m_keyboardState), (LPVOID)&m_keyboardState);
	if (FAILED(result)) {
		// If the keyboard lost focus or was not acquired then try to get control back.
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED)) {
			m_keyboard->Acquire();
		} else {
			return false;
		}
	}

	return true;
}

bool InputClass::ReadMouse()
{
	HRESULT result;
	// Read the mouse device.
	result = m_mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_mouseState);
	if (FAILED(result)) {
		// If the mouse lost focus or was not acquired then try to get control back.
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED)) {
			m_mouse->Acquire();
		} else {
			return false;
		}
	}

	return true;
}

void InputClass::ProcessInput()
{
	// Update the location of the mouse cursor based on the change of the mouse location during the frame.
	m_mouseX += m_mouseState.lX;
	m_mouseY += m_mouseState.lY;

	// Ensure the mouse location doesn't exceed the screen width or height.
	if (m_mouseX < 0) {
		m_mouseX = 0;
	}
	if (m_mouseY < 0) { 
		m_mouseY = 0; 
	}

	if (m_mouseX > m_screenWidth) { 
		m_mouseX = m_screenWidth; 
	}
	if (m_mouseY > m_screenHeight) { 
		m_mouseY = m_screenHeight; 
	}

	m_mouseButton = -1;
}