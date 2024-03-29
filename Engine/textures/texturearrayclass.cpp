#include "texturearrayclass.h"

TextureArrayClass::TextureArrayClass()
{
}


TextureArrayClass::TextureArrayClass(const TextureArrayClass& other)
{
}


TextureArrayClass::~TextureArrayClass()
{
}

bool TextureArrayClass::Initialize(ID3D11Device* device, WCHAR* filename)
{
	m_textures.clear();
	AddTexture(device, filename);

	return true;
}

void TextureArrayClass::AddTexture(ID3D11Device* device, WCHAR* filename)
{
	ID3D11ShaderResourceView* texture;

	D3DX11CreateShaderResourceViewFromFile(device, filename, NULL, NULL, &texture, NULL);
	m_textures.push_back(texture);

	std::wstring filenameW = filename;
	m_texturesPath.push_back(std::string(filenameW.begin(), filenameW.end()));
}

void TextureArrayClass::AddResource(ID3D11ShaderResourceView* texture)
{
	m_textures.push_back(texture);
}

void TextureArrayClass::Shutdown()
{
	// Release the texture resources.
	int size = m_textures.size();
	for (int i = 0; i < size; i++) {
		if (m_textures[i]) {
			m_textures[i]->Release();
			m_textures[i] = 0;
		}
	}

	m_textures.clear();
}

ID3D11ShaderResourceView* TextureArrayClass::GetTexture(int index)
{
	if (index < 0 || index >= m_textures.size()) {
		return 0;
	}

	return m_textures[index];
}

ID3D11ShaderResourceView** TextureArrayClass::GetTextureArray()
{
	return m_textures.data();
}