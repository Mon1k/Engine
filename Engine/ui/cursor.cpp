#include "cursor.h"
#include "../Options.h"

Cursor::Cursor()
{
	m_D3D = 0;
	m_TextureShader = 0;
	m_Bitmap = 0;

	m_width = 32;
	m_height = 32;
	m_x = Options::screen_width / 2;
	m_y = Options::screen_height / 2;
}

Cursor::~Cursor()
{
}

bool Cursor::Initialize()
{
	return Initialize(m_width, m_height);
}

bool Cursor::Initialize(int width, int height)
{
	bool result;

	m_width = width;
	m_height = height;

	m_TextureShader = new TextureShaderClass;
	result = m_TextureShader->Initialize(m_D3D->GetDevice());
	if (!result) {
		MessageBox(NULL, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the bitmap object.
	m_Bitmap = new BitmapClass;
	WCHAR* textureFilename = L"data/textures/ui/cursor.png";
	result = m_Bitmap->Initialize(m_D3D->GetDevice(), Options::screen_width, Options::screen_height, textureFilename, m_width, m_height);
	if (!result) {
		MessageBox(NULL, L"Could not initialize the bitmap object.", L"Error", MB_OK);
		return false;
	}

	return true;
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