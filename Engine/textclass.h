#ifndef _TEXTCLASS_H_
#define _TEXTCLASS_H_

#include "fontclass.h"
#include "fontshaderclass.h"
#include "math/Rectangle.h"

#include <string>

class TextClass
{
private:
	struct SentenceType
	{
		ID3D11Buffer* vertexBuffer, * indexBuffer;
		int vertexCount, indexCount, maxLength;
		float red, green, blue;
	};

	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
	};

public:
	TextClass();
	TextClass(const TextClass&);
	~TextClass();

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, int, int);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX);
	bool AddText(std::string, int, int, float, float, float);

	ds::math::Rectangle getRectangle() {
		return m_rectangle;
	}
	void updateRectangle(float, float);

	std::string getText()
	{
		return m_text;
	}

	D3DXVECTOR3 getColor()
	{
		return D3DXVECTOR3(m_sentence->red, m_sentence->green, m_sentence->blue);
	}

private:
	bool InitializeSentence(SentenceType**, int, ID3D11Device*);
	bool UpdateSentence(SentenceType*, std::string, int, int, float, float, float, ID3D11DeviceContext*);
	void ReleaseSentence(SentenceType**);
	bool RenderSentence(ID3D11DeviceContext*, SentenceType*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX);

private:
	FontClass* m_Font;
	FontShaderClass* m_FontShader;

	ID3D11Device* m_device;
	ID3D11DeviceContext* m_deviceContext;

	ds::math::Rectangle m_rectangle;
	SentenceType* m_sentence;
	std::string m_text;
};

#endif