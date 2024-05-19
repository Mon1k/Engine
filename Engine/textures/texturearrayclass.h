#ifndef _TEXTUREARRAYCLASS_H_
#define _TEXTUREARRAYCLASS_H_

#include <d3d11.h>
#include <d3dx11tex.h>
#include <algorithm>
#include <iterator>
#include <vector>
#include <string>


class TextureArrayClass
{
public:
	TextureArrayClass();
	TextureArrayClass(const TextureArrayClass&);
	~TextureArrayClass();

	bool Initialize(ID3D11Device*, WCHAR*);
	void AddTexture(ID3D11Device*, WCHAR*);
	void setTexture(ID3D11Device* device, std::string filename, int index);
	void AddResource(ID3D11ShaderResourceView*);
	void Shutdown();
	void ReleaseTexture(int index);

	ID3D11ShaderResourceView* GetTexture(int);
	ID3D11ShaderResourceView** GetTextureArray();
	std::vector<ID3D11ShaderResourceView*> getTextures()
	{
		return m_textures;
	}

	std::string getTexturePath()
	{
		return m_texturesPath[0];
	}

	std::string getTexturePath(int index)
	{
		if (index >= m_texturesPath.size()) {
			return "";
		}

		return m_texturesPath[index];
	}

	std::vector<std::string> getTexturesPath()
	{
		return m_texturesPath;
	}


private:
	std::vector<ID3D11ShaderResourceView*> m_textures;
	std::vector<std::string> m_texturesPath;
};

#endif