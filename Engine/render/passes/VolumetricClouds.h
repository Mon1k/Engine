#pragma once

#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <fstream>
#include <vector>
using namespace std;

#include "../../shaders/AbstractShader.h"

class VolumetricClouds : public AbstractShader
{
private:
	struct CloudParameters
	{
		int shape_noise_frequency = 4;
		int shape_noise_resolution = 128;
		int detail_noise_frequency = 6;
		int detail_noise_resolution = 32;

		int max_num_steps = 64;
		float cloud_min_height = 1500.0f;
		float cloud_max_height = 4000.0f;
		float shape_noise_scale = 0.3f;
		float detail_noise_scale = 3.2f;
		float detail_noise_modifier = 0.33f;
		float cloud_coverage = 0.625f;
		float cloud_type = 0.5f;
		float global_density = 0.25f;

		float planet_radius = 35000.0f;
		float light_step_length = 64.0f;
		float light_cone_radius = 0.4f;

		float cloud_base_color[3] = { 0.78f, 0.86f, 1.0f };
		float cloud_top_color[3] = { 1.0f, 1.0f, 1.0f };
		float precipitation = 1.78f;
		float ambient_light_factor = 0.12f;
		float sun_light_factor = 0.7f;
		float henyey_greenstein_g_forward = 0.4f;
		float henyey_greenstein_g_backward = 0.179f;
	};

public:
	VolumetricClouds();
	~VolumetricClouds();

	bool Initialize(ID3D11Device*);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int);

	ID3D11ShaderResourceView* getShapeNoiseResource()
	{
		return m_resourceShapeNoise;

		ID3D11ShaderResourceView* texture;
		D3D11_SHADER_RESOURCE_VIEW_DESC srDesc;
		ZeroMemory(&srDesc, sizeof(srDesc));
		srDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		srDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srDesc.Texture2D.MostDetailedMip = 0;
		srDesc.Texture2D.MipLevels = 1;

		m_D3D->GetDevice()->CreateShaderResourceView(m_cloudType, &srDesc, &texture);

		return texture;
	}

	void computeShaders();

private:
	bool InitializeShader(ID3D11Device*, WCHAR*, WCHAR*);
	void ShutdownShader();

	bool SetShaderParameters(ID3D11DeviceContext*);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	ID3D11InputLayout* m_layout;

	ID3D11Buffer* m_cloudsBuffer;
	ID3D11SamplerState* m_sampleStateWrap;

	ID3D11ComputeShader* m_cloudShapeNoiseShader;
	ID3D11ComputeShader* m_cloudDetailNoiseShader;
	ID3D11ComputeShader* m_cloudTypeShader;
	ID3D11Buffer* m_cloudsBufferNoise;
	ID3D11Buffer* m_cloudsBufferNoiseUnorderer; // buffer 1
	ID3D11UnorderedAccessView* m_cloudsUnorderedView;
	ID3D11Buffer* m_cloudsReadBackBuffer;
	ID3D11ShaderResourceView* m_resourceShapeNoise;

	ID3D11Texture3D* m_prevClouds;
	ID3D11Texture3D* m_cloudShapeNoise;
	ID3D11Texture3D* m_cloudDetailNoise;
	ID3D11Texture2D* m_cloudType;

	CloudParameters m_params{};
};