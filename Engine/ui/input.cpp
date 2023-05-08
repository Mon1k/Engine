#include "input.h"


Input::Input()
{
	m_TextureShader = 0;
	m_Bitmap = 0;
	m_Text = 0;
	m_bIsFocused = false;
}

Input::~Input()
{
}

bool Input::Initialize(int screenWidth, int screenHeight, WCHAR* textureFilename, int bitmapWidth, int bitmapHeight, int positionX, int positionY)
{
	bool result;

	// Create the text object.
	m_Text = new TextClass;
	if (!m_Text) {
		return false;
	}

	// Initialize the text object.
	result = m_Text->Initialize(m_D3D->GetDevice(), m_D3D->GetDeviceContext(), screenWidth, screenHeight);
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

	// Initialize the bitmap object.
	m_width = bitmapWidth;
	m_height = bitmapHeight;
	result = m_Bitmap->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight, textureFilename, m_width, m_height);
	if (!result) {
		return false;
	}

	m_x = positionX;
	m_y = positionY;

	return true;
}


bool Input::setText(char* text)
{
	return m_Text->AddText(text, m_x + m_width / 3, m_y + m_height / 3, 1.0f, 1.0f, 1.0f);
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