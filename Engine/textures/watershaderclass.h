#ifndef _WATERSHADERCLASS_H_
#define _WATERSHADERCLASS_H_

#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <fstream>

#include "../shaders/AbstractShader.h"

class WaterShaderClass: public AbstractShader
{
private:
	struct MatrixBufferType
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
	};

	struct ReflectionBufferType
    {
        D3DXMATRIX reflection;
    };

    struct CamNormBufferType
    {
        D3DXVECTOR3 cameraPosition;
        float padding1;
        D3DXVECTOR2 normalMapTiling;
        D3DXVECTOR2 padding2;
    };

    struct WaterBufferType
    {
        D3DXVECTOR4 refractionTint;
        D3DXVECTOR3 lightDirection;
        float waterTranslation;
        float reflectRefractScale;
        float specularShininess;
        D3DXVECTOR2 padding;
    };

public:
    WaterShaderClass();
    WaterShaderClass(const WaterShaderClass&);
    ~WaterShaderClass();

    bool Initialize(ID3D11Device*);
    void Shutdown();
    bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX,
                D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*,
                ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, D3DXVECTOR3, D3DXVECTOR2, float, float, D3DXVECTOR4, D3DXVECTOR3, float);

private:
    bool InitializeShader(ID3D11Device*, WCHAR*, WCHAR*);
    void ShutdownShader();

    bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX,
                                D3DXMATRIX, D3DXMATRIX,
                                ID3D11ShaderResourceView*,
                                ID3D11ShaderResourceView*,
                                ID3D11ShaderResourceView*, D3DXVECTOR3, D3DXVECTOR2, float, float, D3DXVECTOR4, D3DXVECTOR3, float);
    void RenderShader(ID3D11DeviceContext*, int);

private:
    ID3D11InputLayout* m_layout;
    ID3D11SamplerState* m_sampleState;

    ID3D11Buffer* m_matrixBuffer;
    ID3D11Buffer* m_reflectionBuffer;
    ID3D11Buffer* m_camNormBuffer;
    ID3D11Buffer* m_waterBuffer;
};

#endif
