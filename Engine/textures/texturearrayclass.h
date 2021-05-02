#ifndef _TEXTUREARRAYCLASS_H_
#define _TEXTUREARRAYCLASS_H_

#include <d3d11.h>
#include <d3dx11tex.h>
#include <algorithm>
#include <iterator>

class TextureArrayClass
{
public:
	TextureArrayClass();
	TextureArrayClass(const TextureArrayClass&);
	~TextureArrayClass();

	bool Initialize(ID3D11Device*, WCHAR*, WCHAR*);
	void AddTexture(ID3D11Device*, WCHAR*, int);
	void Shutdown();

	ID3D11ShaderResourceView** GetTextureArray();
	ID3D11ShaderResourceView** GetTextureArray(int);

private:
	ID3D11ShaderResourceView* m_textures[3];
};

#endif