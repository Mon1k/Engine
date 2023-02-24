#ifndef _PROJECTIONSHADERCLASS_H_
#define _PROJECTIONSHADERCLASS_H_


#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <fstream>
using namespace std;

#include "../shaders/AbstractShader.h"
#include "../render/viewpointclass.h"
#include "../textureclass.h"
#include "../lightclass.h"

class ProjectionShaderClass : public AbstractShader
{
private:
	struct MatrixBufferType
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
		D3DXMATRIX view2;
		D3DXMATRIX projection2;
	};

	struct LightPositionBufferType
	{
		D3DXVECTOR3 lightPosition;
		float intensity;
	};

	struct LightBufferType
	{
		D3DXVECTOR4 ambientColor;
		D3DXVECTOR4 diffuseColor;
		D3DXVECTOR3 lightDirection;
		float padding;
	};

public:
	ProjectionShaderClass();
	ProjectionShaderClass(const ProjectionShaderClass&);
	~ProjectionShaderClass();

	bool Initialize(ID3D11Device*, WCHAR*);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView**, D3DXVECTOR3);
	ViewPointClass* getPointView() {
		return m_pointView;
	};
	void addLight(LightClass* light) {
		m_light = light;
	}

private:
	bool InitializeShader(ID3D11Device*, WCHAR*, WCHAR*);
	void ShutdownShader();

	bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, D3DXVECTOR4, D3DXVECTOR4, D3DXVECTOR3, float,
		D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*);
	void RenderShader(ID3D11DeviceContext*, int);
		
private:
	ID3D11InputLayout* m_layout;
	ID3D11SamplerState* m_sampleState;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_lightPositionBuffer;
	ID3D11Buffer* m_lightBuffer;	

	ViewPointClass* m_pointView;
	TextureClass* m_Texture;
	LightClass* m_light;
};

#endif