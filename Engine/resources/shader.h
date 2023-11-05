#pragma once

#include <d3d11.h>
#include <string>
#include <map>

#include "../d3dclass.h"

class Shader
{
public:
	Shader();



protected:
	ID3D11DeviceContext* deviceContext;
	ID3D11InputLayout* m_layout;
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;

	D3DClass* m_D3D;

	std::map<std::string, ID3D11Buffer*> m_Buffers;
};