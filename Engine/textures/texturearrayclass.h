#ifndef _TEXTUREARRAYCLASS_H_
#define _TEXTUREARRAYCLASS_H_

#include <d3d11.h>
#include <d3dx11tex.h>
#include <algorithm>
#include <iterator>
#include <vector>
using namespace std;


class TextureArrayClass
{
public:
	TextureArrayClass();
	TextureArrayClass(const TextureArrayClass&);
	~TextureArrayClass();

	bool Initialize(ID3D11Device*, WCHAR*);
	void AddTexture(ID3D11Device*, WCHAR*);
	void AddResource(ID3D11ShaderResourceView*);
	void Shutdown();

	ID3D11ShaderResourceView* GetTexture(int);
	ID3D11ShaderResourceView** GetTextureArray();
	std::vector<ID3D11ShaderResourceView*> getTextures() {
		return m_textures;
	}


private:
	std::vector<ID3D11ShaderResourceView*> m_textures;
};

#endif