#include "Image.h"
#include "../Options.h"

Image::Image()
{
	m_TextureShader = 0;
	m_Bitmap = 0;
}

Image::~Image()
{
}

bool Image::Initialize(int width, int height, int x, int y)
{
	bool result;

	m_width = width;
	m_height = height;
	m_x = x;
	m_y = y;

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

	return true;
}

void Image::loadTextureByString(std::string path)
{
	m_Bitmap = new BitmapClass;
	m_Bitmap->Shutdown();
	wstring wide_string = wstring(path.begin(), path.end());
	m_Bitmap->Initialize(m_D3D->GetDevice(), Options::screen_width, Options::screen_height, &wide_string[0], m_width, m_height);
}

void Image::loadTextureByResource(ID3D11ShaderResourceView* texture)
{
	m_Bitmap = new BitmapClass;
	m_Bitmap->Shutdown();
	m_Bitmap->Initialize(m_D3D->GetDevice(), Options::screen_width, Options::screen_height, L"", m_width, m_height);
	m_Bitmap->LoadTextureByResource(texture);
}

void Image::Shutdown()
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

bool Image::Render()
{
	bool result;
	D3DXMATRIX worldMatrix, orthoMatrix;

	if (!m_Bitmap) {
		return true;
	}

	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetOrthoMatrix(orthoMatrix);

	m_Bitmap->Render(m_D3D->GetDeviceContext(), m_x, m_y);
	// Render the bitmap with the texture shader.
	result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_Bitmap->GetIndexCount(), worldMatrix, m_baseViewMatrix, orthoMatrix, m_Bitmap->GetTexture());
	if (!result) {
		return false;
	}

	return true;
}