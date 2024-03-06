#include "input.h"
#include "../Options.h"
#include "../tool/String.h"

Input::Input()
{
	m_TextureShader = 0;
	m_Bitmap = 0;
	m_Text = 0;

	m_IsFocused = false;
	m_Flash = true;
	m_usesFlashCursor = false;
	m_FrameCounter = 0;
	m_CursorShift = 0;
	m_MaxSize = 0;
	m_String.clear();
}

Input::~Input()
{
}

Input* Input::createString(void* value, D3DClass* d3d, int width, int height, int positionX, int positionY)
{
	Input* input = new Input;
	input->m_D3D = d3d;
	input->initialize(width, height, positionX, positionY);
	input->setValue(&typeid(std::string), value);
	return input;
}

Input* Input::createFloat(void* value, D3DClass* d3d, int width, int height, int positionX, int positionY)
{
	Input* input = new Input;
	input->m_D3D = d3d;
	input->initialize(width, height, positionX, positionY);
	input->setValue(&typeid(float), value);
	return input;
}

Input* Input::createInt(void* value, D3DClass* d3d, int width, int height, int positionX, int positionY)
{
	Input* input = new Input;
	input->m_D3D = d3d;
	input->initialize(width, height, positionX, positionY);
	input->setValue(&typeid(int), value);
	return input;
}

bool Input::initialize(int width, int height, int positionX, int positionY)
{
	bool result;

	// Create the text object.
	m_Text = new TextClass;
	if (!m_Text) {
		return false;
	}

	// Initialize the text object.
	result = m_Text->Initialize(m_D3D->GetDevice(), m_D3D->GetDeviceContext(), Options::screen_width, Options::screen_height);
	if (!result) {
		return false;
	}

	// Create the texture shader object.
	m_TextureShader = new TextureShaderClass;
	if (!m_TextureShader) {
		return false;
	}

	// Initialize the texture shader object.
	result = m_TextureShader->Initialize(m_D3D->GetDevice());
	if (!result) {
		return false;
	}

	// Create the bitmap object.
	m_Bitmap = new BitmapClass;
	if (!m_Bitmap) {
		return false;
	}

	m_width = width;
	m_height = height;
	m_x = positionX;
	m_y = positionY;

	// Initialize the bitmap object.
	WCHAR* textureFilename = L"data/textures/ui/button.png";
	result = m_Bitmap->Initialize(m_D3D->GetDevice(), Options::screen_width, Options::screen_height, textureFilename, m_width, m_height);
	if (!result) {
		return false;
	}

	return true;
}

void Input::Shutdown()
{
	if (m_Text) {
		m_Text->Shutdown();
		delete m_Text;
		m_Text = 0;
	}

	if (m_Bitmap) {
		m_Bitmap->Shutdown();
		delete m_Bitmap;
		m_Bitmap = 0;
	}

	if (m_TextureShader) {
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}
}

bool Input::Render()
{
	bool result;
	D3DXMATRIX worldMatrix, orthoMatrix;

	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetOrthoMatrix(orthoMatrix);

	m_Bitmap->Render(m_D3D->GetDeviceContext(), m_x, m_y);
	result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_Bitmap->GetIndexCount(), worldMatrix, m_baseViewMatrix, orthoMatrix, m_Bitmap->GetTexture());
	if (!result) {
		return false;
	}

	result = m_Text->Render(m_D3D->GetDeviceContext(), worldMatrix, orthoMatrix, m_baseViewMatrix);
	if (!result) {
		return false;
	}

	return true;
}

bool Input::setText(std::string text)
{
	m_String = text;
	m_CursorShift = m_String.length();

	return updateText(text);
}

bool Input::updateText(std::string text)
{
	if (String::trim(m_String).size() > 0) {
		std::string typeName = std::string(m_typeInfo->name());
		if (typeName.compare("float") == 0) {
			*(float*)m_value = getValueFloat();
		}
		else if (typeName.compare("int") == 0) {
			*(int*)m_value = getValueInt();
		}
		else if (String::search(typeName, "std::basic_string")) {
			*(std::string*)m_value = getValue();
		}
	}

	return m_Text->AddText(&text[0], m_x + 4, m_y + m_height / 3, 1.0f, 1.0f, 1.0f);
}

void Input::updateText()
{
	m_ViewedString = m_String.substr(0, m_CursorShift);
	if (m_IsFocused && (!m_usesFlashCursor || !m_Flash)) {
		m_ViewedString += "|";
	}
	if (m_CursorShift < m_String.length()) {
		m_ViewedString += m_String.substr(m_CursorShift, m_String.length());
	}
	updateText(&m_ViewedString[0]);
}

void Input::onMousePress(int x, int y, int button)
{
	if (button == MOUSE_BUTTON1 && isIntersect(x, y)) {
		m_IsFocused = true;
		m_Flash = false;
		m_FrameCounter = 0;
		updateText();
		proccesedEventHandlers(AbstractGui::EventType::OBJECT_FOCUS);
	}

	AbstractGui::onMousePress(x, y, button);
}

void Input::onKeyboardPress(InputClass::EventKey event)
{
	std::string chunkLeft;
	std::string chunkRight;
	int size = m_String.length();

	chunkLeft.clear();
	chunkRight.clear();
	if (m_CursorShift > 0) {
		chunkLeft = m_String.substr(0, m_CursorShift);
	}
	if (m_CursorShift < size) {
		chunkRight = m_String.substr(m_CursorShift, size);
	}

	if (event.key == DIK_ESCAPE) {
		unfocus();
	}
	else if (event.key == DIK_BACKSPACE && m_CursorShift > 0) {
		m_String = m_String.substr(0, m_CursorShift - 1) + chunkRight;
		m_CursorShift--;
		m_CursorShift = min(m_CursorShift, m_String.length());
		m_Flash = false;
		updateText();
	}
	else if (event.key == DIK_LEFTARROW) {
		m_CursorShift--;
		m_CursorShift = max(m_CursorShift, 0);
		m_Flash = false;
		updateText();
	}
	else if (event.key == DIK_RIGHTARROW) {
		m_CursorShift++;
		m_CursorShift = min(m_CursorShift, size);
		m_Flash = false;
		updateText();
	} 
	else if (event.key == DIK_HOME) {
		m_CursorShift = 0;
		m_Flash = false;
		updateText();
	}
	else if (event.key == DIK_END) {
		m_CursorShift = size;
		m_Flash = false;
		updateText();
	} else if (event.key <= DIK_SPACE && (m_MaxSize == 0 || size < m_MaxSize)) {
		char symbol = replaceSymbolByEvent(event);
		m_String = chunkLeft + symbol + chunkRight;
		size = m_String.length();
		m_CursorShift++;
		m_CursorShift = min(m_CursorShift, size);
		updateText();
	}

	AbstractGui::onKeyboardPress(event);
}

char Input::replaceSymbolByEvent(InputClass::EventKey event)
{
	char symbol = event.symbol;

	if (event.shift) {
		if (event.symbol == '1') {
			symbol = '!';
		}
		else if (event.symbol == '2') {
			symbol = '@';
		}
		else if (event.symbol == '3') {
			symbol = '#';
		}
		else if (event.symbol == '4') {
			symbol = '$';
		}
		else if (event.symbol == '5') {
			symbol = '%';
		}
		else if (event.symbol == '6') {
			symbol = '^';
		}
		else if (event.symbol == '7') {
			symbol = '&';
		}
		else if (event.symbol == '8') {
			symbol = '*';
		}
		else if (event.symbol == '9') {
			symbol = '(';
		}
		else if (event.symbol == '0') {
			symbol = ')';
		}
		else if (event.symbol == '-') {
			symbol = '_';
		}
		else if (event.symbol == '=') {
			symbol = '+';
		}
		else {
			symbol = toupper(symbol);
		}
	}

	return symbol;
}

bool Input::isIntersect(int x, int y) {
	bool result = AbstractGui::isIntersect(x, y);
	if (!result && m_IsFocused) {
		m_IsFocused = false;
		updateText(m_String);
		proccesedEventHandlers(AbstractGui::EventType::OBJECT_BLUR);
	}

	return result;
}

void Input::frame(float counter)
{
	if (!m_usesFlashCursor || !m_IsFocused) {
		return;
	}

	int timeout = m_Flash ? 500 : 1000;
	m_FrameCounter += counter;
	if (m_FrameCounter > timeout) {
		hideCaret();
	}
}

void Input::unfocus()
{
	AbstractGui::unfocus();
	hideCaret();
}

void Input::hideCaret()
{
	m_Flash = false;
	m_usesFlashCursor = 0;
	updateText();
}

void Input::showCaret()
{
	m_Flash = true;
}