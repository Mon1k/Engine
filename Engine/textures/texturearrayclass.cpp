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

void TextureArrayClass::setTexture(ID3D11Device* device, std::string filename, int index)
{
	D3DX11CreateShaderResourceViewFromFile(device, std::wstring(filename.begin(), filename.end()).c_str(), NULL, NULL, &m_textures[index], NULL);

	m_texturesPath[index] = filename;
}

void TextureArrayClass::AddResource(ID3D11ShaderResourceView* texture)
{
	m_textures.push_back(texture);
}

void TextureArrayClass::Shutdown()
{
	for (size_t i = 0; i < m_textures.size(); i++) {
		if (m_textures[i] && m_texturesPath[i].length() > 0) {
			m_textures[i]->Release();
			m_textures[i] = 0;
		}
	}

	m_textures.clear();
	m_texturesPath.clear();
}

void TextureArrayClass::ReleaseTexture(int index)
{
	if (index < m_textures.size()) {
		m_textures[index]->Release();
		m_textures[index] = 0;

		m_texturesPath[index] = "";
	}
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