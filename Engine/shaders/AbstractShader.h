#ifndef _ABSTRACTSHADER_H_
#define _ABSTRACTSHADER_H_

class AbstractShader
{
public:
	virtual bool Initialize(ID3D11Device*) = 0;
	virtual void Shutdown() = 0;
	virtual bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView**, D3DXVECTOR3)
	{
		return true;
	};

protected:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
};

#endif