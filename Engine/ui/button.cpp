#include "button.h"
#include "../Options.h"

Button::Button()
{
	m_TextureShader = 0;
	m_Bitmap = 0;
	m_Text = 0;
	m_align = ALIGN_CENTER;
}


Button::Button(const Button& other)
{
}

Button::~Button()
{
}

bool Button::Initialize(int width, int height)
{
	return Initialize(L"data/textures/ui/button.png", width, height);
}

bool Button::Initialize(WCHAR* texture, int width, int height)
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
		MessageBox(NULL, L"Could not initialize the text object.", L"Error", MB_OK);
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
		MessageBox(NULL, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the bitmap object.
	m_Bitmap = new BitmapClass;
	if (!m_Bitmap) {
		return false;
	}

	// Initialize the bitmap object.
	m_width = width;
	m_height = height;

	result = m_Bitmap->Initialize(m_D3D->GetDevice(), Options::screen_width, Options::screen_height, texture, m_width, m_height);
	if (!result) {
		MessageBox(NULL, L"Could not initialize the bitmap object.", L"Error", MB_OK);
		return false;
	}

	return true;
}


bool Button::Add(std::string text, int positionX, int positionY, float red, float green, float blue)
{
	m_x = positionX;
	m_y = positionY;

	int x;
	if (m_align == ALIGN_LEFT) {
		x = m_x;
	}
	else if (m_align == ALIGN_RIGHT) {
		x = m_x + m_width - std::string(text).size() * 6.3;
	}
	else {
		x = m_x + m_width / 2 - std::string(text).size() / 2 * 6.3;
	}

	return m_Text->AddText(text, x, m_y + m_height / 3, red, green, blue);
}

bool Button::Add(std::string text, int positionX, int positionY)
{
	return Add(text, positionX, positionY, 1.0f, 1.0f, 1.0f);
}

bool Button::setText(std::string text)
{
	return Add(text, m_x, m_y);
}

void Button::Shutdown()
{
	// Release the text object.
	if (m_Text) {
		m_Text->Shutdown();
		delete m_Text;
		m_Text = 0;
	}

	// Release the bitmap object.
	if (m_Bitmap) {
		m_Bitmap->Shutdown();
		delete m_Bitmap;
		m_Bitmap = 0;
	}

	// Release the texture shader object.
	if (m_TextureShader) {
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}
}

bool Button::Render()
{
	bool result;
	D3DXMATRIX worldMatrix, orthoMatrix;

	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetOrthoMatrix(orthoMatrix);

	m_Bitmap->Render(m_D3D->GetDeviceContext(), m_x, m_y);
	// Render the bitmap with the texture shader.
	result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_Bitmap->GetIndexCount(), worldMatrix, m_baseViewMatrix, orthoMatrix, m_Bitmap->GetTexture());
	if (!result) {
		return false;
	}

	// Render the text strings.
	result = m_Text->Render(m_D3D->GetDeviceContext(), worldMatrix, orthoMatrix, m_baseViewMatrix);
	if (!result) {
		return false;
	}

	return true;
}