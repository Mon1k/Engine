#include "input.h"
#include "../Options.h"

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

bool Input::Initialize(WCHAR* textureFilename, int bitmapWidth, int bitmapHeight, int positionX, int positionY)
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

	m_width = bitmapWidth;
	m_height = bitmapHeight;
	m_x = positionX;
	m_y = positionY;

	// Initialize the bitmap object.
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
	if (button == MOUSE_BUTTON1) {
		m_IsFocused = true;
		m_Flash = false;
		m_FrameCounter = 0;
		updateText();
		proccesedEventHandlers(AbstractGui::EventType::OBJECT_FOCUS);
	}

	AbstractGui::onMousePress(x, y, button);
}

void Input::onKeyboardPress(int key, char symbol)
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

	if (key == DIK_BACKSPACE && m_CursorShift > 0) {
		m_String = m_String.substr(0, m_CursorShift - 1) + chunkRight;
		m_CursorShift = min(m_CursorShift, m_String.length());
		m_Flash = false;
		updateText();
	}
	else if (key == DIK_LEFTARROW) {
		m_CursorShift--;
		m_CursorShift = max(m_CursorShift, 0);
		m_Flash = false;
		updateText();
	}
	else if (key == DIK_RIGHTARROW) {
		m_CursorShift++;
		m_CursorShift = min(m_CursorShift, size);
		m_Flash = false;
		updateText();
	} else if (key <= DIK_SPACE && (m_MaxSize == 0 || size < m_MaxSize)) {
		m_String = chunkLeft + symbol + chunkRight;
		m_CursorShift++;
		m_CursorShift = min(m_CursorShift, size);
		updateText();
	}

	AbstractGui::onKeyboardPress(key, symbol);
}

bool Input::isIntersect(int x, int y) {
	bool result = AbstractGui::isIntersect(x, y);
	if (!result && m_IsFocused) {
		m_IsFocused = false;
		updateText(&m_String[0]);
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
		m_Flash = !m_Flash;
		m_FrameCounter = 0;
		updateText();
	}
}