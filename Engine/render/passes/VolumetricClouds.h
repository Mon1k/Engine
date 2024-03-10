#pragma once

#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <fstream>
#include <vector>

#include "../../shaders/AbstractShader.h"
#include "../../cameraclass.h"
#include "../../lightclass.h"

class VolumetricClouds : public AbstractShader
{
public:
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

		D3DXVECTOR3 cloud_base_color = { 0.78f, 0.86f, 1.0f };
		D3DXVECTOR3 cloud_top_color = { 1.0f, 1.0f, 1.0f };
		float precipitation = 1.78f;
		float ambient_light_factor = 0.12f;
		float sun_light_factor = 0.7f;
		float henyey_greenstein_g_forward = 0.4f;
		float henyey_greenstein_g_backward = 0.179f;

		float padding;
	};

	struct CloudsConstants
	{
		float		cloud_type;
		float 	    cloud_min_height;
		float 	    cloud_max_height;
		float		padding;

		float 	    shape_noise_scale;
		float 	    detail_noise_scale;
		float 	    detail_noise_modifier;
		float       global_density;

		float 	    cloud_coverage;
		D3DXVECTOR3 cloud_base_color;
		D3DXVECTOR3 cloud_top_color;
		int	        max_num_steps;

		D3DXVECTOR3 planet_center;
		float 	    planet_radius;

		float 	    light_step_length;
		float 	    light_cone_radius;
		float 	    precipitation;
		float 	    ambient_light_factor;

		float 	    sun_light_factor;
		float 	    henyey_greenstein_g_forward;
		float 	    henyey_greenstein_g_backward;
		int			resolution_factor;
	};

	struct FrameBuffer
	{
		D3DXMATRIX	inverseView;
		D3DXMATRIX	inverseProjection;
		D3DXVECTOR4 cameraPosition;

		D3DXVECTOR4 windParams = { 1.0f, 0.0f, 1.0f, 10.0f };
		D3DXVECTOR4 sunDirection = { -0.2f, 1.0f, -0.33f, 0.0f };
		D3DXVECTOR4 sunColor = { 18.0f, 17.6f, 17.96f, 4.0f };

		D3DXVECTOR2 renderResolution = { 512, 512 };
		float		totalTime = 10;
		float		padding;
	};

	enum CloudResolution
	{
		CloudResolution_Full = 0,
		CloudResolution_Half = 1,
		CloudResolution_Quarter = 2
	};

public:
	VolumetricClouds();
	~VolumetricClouds();

	bool Initialize(ID3D11Device*);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int);

	CloudParameters getParams()
	{
		return m_params;
	}
	void setParams(CloudParameters params)
	{
		m_params = params;
	}

	ID3D11ShaderResourceView* getShapeNoise()
	{
		return m_resourceShapeNoise;
	}
	
	ID3D11ShaderResourceView* getDetailNoise()
	{
		return m_resourceDetailNoise;
	}

	ID3D11ShaderResourceView* getShapeTypeCloud()
	{
		return m_resourceCloudType;
	}

	ID3D11ShaderResourceView* getPrevClouds()
	{
		return m_resourcePrevClouds;
	}

	void computeNoiseShaders();
	void computeVolumetricCloudsShaders(CameraClass* camera);

	void frame(CameraClass* camera, float time);

private:
	bool InitializeShader(ID3D11Device*, WCHAR*, WCHAR*);
	void ShutdownShader();

	void RenderShader(ID3D11DeviceContext*, int);

private:
	ID3D11InputLayout* m_layout;

	ID3D11SamplerState* m_sampleStateWrap;

	ID3D11ComputeShader* m_cloudShapeNoiseShader;
	ID3D11ComputeShader* m_cloudDetailNoiseShader;
	ID3D11ComputeShader* m_cloudTypeShader;
	ID3D11ComputeShader* m_prevCloudShader;

	ID3D11Buffer* m_cloudsBufferNoise;
	ID3D11Buffer* m_prevCloudsBufferNoise;
	ID3D11Buffer* m_frameBufferNoise;

	ID3D11UnorderedAccessView* m_cloudsUnorderedViewShapeNoise;
	ID3D11UnorderedAccessView* m_cloudsUnorderedViewDetailNoise;
	ID3D11UnorderedAccessView* m_cloudsUnorderedView;
	ID3D11UnorderedAccessView* m_prevCloudsUnorderedView;
	
	ID3D11ShaderResourceView* m_resourceShapeNoise;
	ID3D11ShaderResourceView* m_resourceDetailNoise;
	ID3D11ShaderResourceView* m_resourceCloudType;
	ID3D11ShaderResourceView* m_resourcePrevClouds;

	ID3D11Texture3D* m_cloudShapeNoise;
	ID3D11Texture3D* m_cloudDetailNoise;
	ID3D11Texture2D* m_cloudType;
	ID3D11Texture2D* m_prevClouds;

	float m_width, m_height;
	int m_counter = 0;
	CloudParameters m_params{};
	FrameBuffer m_frameBuffer{};
};