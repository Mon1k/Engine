#include "cursor.h"


Cursor::Cursor()
{
	m_D3D = 0;
	m_TextureShader = 0;
	m_Bitmap = 0;

	m_width = 0;
	m_height = 0;
	m_x = 0;
	m_y = 0;
}


Cursor::Cursor(const Cursor& other)
{
}

Cursor::~Cursor()
{
}

bool Cursor::Initialize(D3DClass* d3d, int screenWidth, int screenHeight, HWND hwnd, WCHAR* textureFilename, int bitmapWidth, int bitmapHeight, D3DXMATRIX baseViewMatrix)
{
	bool result;

	m_D3D = d3d;
	m_ViewMatrix = baseViewMatrix;

	// Create the texture shader object.
	m_TextureShader = new TextureShaderClass;
	if (!m_TextureShader) {
		return false;
	}

	// Initialize the texture shader object.
	result = m_TextureShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
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
		MessageBox(hwnd, L"Could not initialize the bitmap object.", L"Error", MB_OK);
		return false;
	}
}


bool Cursor::Set(int positionX, int positionY)
{
	m_x = positionX;
	m_y = positionY;

	return true;
}

void Cursor::Shutdown()
{
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


bool Cursor::Render()
{
	bool result;
	D3DXMATRIX worldMatrix, orthoMatrix;

	if (!visible) {
		return true;
	}

	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetOrthoMatrix(orthoMatrix);

	// Turn off the Z buffer to begin all 2D rendering.
	m_D3D->TurnZBufferOff();

	// Turn on the alpha blending before rendering the text.
	m_D3D->TurnOnAlphaBlending();

	m_Bitmap->Render(m_D3D->GetDeviceContext(), m_x, m_y);
	// Render the bitmap with the texture shader.
	result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_Bitmap->GetIndexCount(), worldMatrix, m_ViewMatrix, orthoMatrix, m_Bitmap->GetTexture());
	if (!result) {
		return false;
	}

	// Turn off alpha blending after rendering the text.
	m_D3D->TurnOffAlphaBlending();

	// Turn the Z buffer back on now that all 2D rendering has completed.
	m_D3D->TurnZBufferOn();

	return true;
}