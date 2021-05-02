#include "texturearrayclass.h"

TextureArrayClass::TextureArrayClass()
{
	int size = sizeof(m_textures) / sizeof(m_textures[0]);
	for (int i = 0; i < size; i++) {
		m_textures[i] = 0;
	}
}


TextureArrayClass::TextureArrayClass(const TextureArrayClass& other)
{
}


TextureArrayClass::~TextureArrayClass()
{
}

bool TextureArrayClass::Initialize(ID3D11Device* device, WCHAR* filename1, WCHAR* filename2)
{
	HRESULT result;

	// Load the first texture in.
	result = D3DX11CreateShaderResourceViewFromFile(device, filename1, NULL, NULL, &m_textures[0], NULL);
	if (FAILED(result)) {
		return false;
	}

	// Load the second texture in.
	result = D3DX11CreateShaderResourceViewFromFile(device, filename2, NULL, NULL, &m_textures[1], NULL);
	if (FAILED(result)) {
		return false;
	}

	return true;
}

void TextureArrayClass::AddTexture(ID3D11Device* device, WCHAR* filename, int index)
{
	// Load the first texture in.
	D3DX11CreateShaderResourceViewFromFile(device, filename, NULL, NULL, &m_textures[index], NULL);
}

void TextureArrayClass::Shutdown()
{
	// Release the texture resources.
	int size = sizeof(m_textures) / sizeof(m_textures[0]);
	for (int i = 0; i < size; i++) {
		if (m_textures[i]) {
			m_textures[i]->Release();
			m_textures[i] = 0;
		}
	}
}

ID3D11ShaderResourceView** TextureArrayClass::GetTextureArray()
{
	return m_textures;
}

ID3D11ShaderResourceView** TextureArrayClass::GetTextureArray(int size)
{
	ID3D11ShaderResourceView** textures;
	textures = new ID3D11ShaderResourceView *[size];
	memcpy(textures, m_textures, size * sizeof(m_textures[0]));
	return textures;
}