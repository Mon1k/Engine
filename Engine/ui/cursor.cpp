#include "cursor.h"
#include "../Options.h"

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

bool Cursor::Initialize(WCHAR* textureFilename, int bitmapWidth, int bitmapHeight)
{
	bool result;

	m_width = bitmapWidth;
	m_height = bitmapHeight;

	m_TextureShader = new TextureShaderClass;
	result = m_TextureShader->Initialize(m_D3D->GetDevice());
	if (!result) {
		MessageBox(NULL, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the bitmap object.
	m_Bitmap = new BitmapClass;
	result = m_Bitmap->Initialize(m_D3D->GetDevice(), Options::screen_width, Options::screen_height, textureFilename, m_width, m_height);
	if (!result) {
		MessageBox(NULL, L"Could not initialize the bitmap object.", L"Error", MB_OK);
		return false;
	}
}


void Cursor::Shutdown()
{
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


bool Cursor::Render()
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

	return true;
}