#include "VolumetricClouds.h"
#include "../../Options.h"
#include <time.h>
#include <stdio.h>


VolumetricClouds::VolumetricClouds()
{
	m_layout = 0;

	m_cloudShapeNoiseShader = 0;
	m_cloudDetailNoiseShader = 0;
	m_cloudTypeShader = 0;
	m_prevCloudShader = 0;

	m_cloudsBufferNoise = 0;
	m_prevCloudsBufferNoise = 0;
	m_frameBufferNoise;

	m_cloudsUnorderedViewShapeNoise = 0;
	m_cloudsUnorderedViewDetailNoise = 0;
	m_cloudsUnorderedView = 0;
	m_prevCloudsUnorderedView = 0;

	m_resourceShapeNoise = 0;
	m_resourceDetailNoise = 0;
	m_resourceCloudType = 0;
	m_resourcePrevClouds = 0;
}

VolumetricClouds::~VolumetricClouds()
{

}

bool VolumetricClouds::Initialize(ID3D11Device* device)
{
	m_width = Options::screen_width;
	m_height = Options::screen_height;

	bool result = InitializeShader(device, L"./data/shaders/VolumetricClouds.hlsl", L"./data/shaders/VolumetricClouds.hlsl");
	
	if (!result) {
		return false;
	}

	computeNoiseShaders();

	return true;
}


void VolumetricClouds::Shutdown()
{
	ShutdownShader();
}

bool VolumetricClouds::Render(ID3D11DeviceContext* deviceContext, int indexCount)
{
	bool result;

	// Set the shader parameters that it will use for rendering.
	result = SetShaderParameters(deviceContext);
	if (!result) {
		return false;
	}

	// Now render the prepared buffers with the shader.
	RenderShader(deviceContext, indexCount);

	return true;
}

bool VolumetricClouds::InitializeShader(ID3D11Device* device, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	ID3D10Blob* computeShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC cloudsBufferDesc;

	// Initialize the pointers this function will use to null.
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;
	computeShaderBuffer = 0;


	// textures
	D3D11_TEXTURE3D_DESC cloudShapeNoiseDesc{};
	ZeroMemory(&cloudShapeNoiseDesc, sizeof(cloudShapeNoiseDesc));
	cloudShapeNoiseDesc.Width = m_params.shape_noise_resolution;
	cloudShapeNoiseDesc.Height = m_params.shape_noise_resolution;
	cloudShapeNoiseDesc.Depth = m_params.shape_noise_resolution;
	cloudShapeNoiseDesc.MipLevels = 1;
	cloudShapeNoiseDesc.Usage = D3D11_USAGE_DEFAULT;
	cloudShapeNoiseDesc.CPUAccessFlags = 0;
	cloudShapeNoiseDesc.MiscFlags = 0;
	cloudShapeNoiseDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	cloudShapeNoiseDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	result = device->CreateTexture3D(&cloudShapeNoiseDesc, NULL, &m_cloudShapeNoise);
	if (FAILED(result)) {
		return false;
	}

	D3D11_TEXTURE3D_DESC cloudDetailNoiseDesc{};
	ZeroMemory(&cloudDetailNoiseDesc, sizeof(cloudDetailNoiseDesc));
	cloudDetailNoiseDesc.Width = m_params.shape_noise_resolution;
	cloudDetailNoiseDesc.Height = m_params.shape_noise_resolution;
	cloudDetailNoiseDesc.Depth = m_params.shape_noise_resolution;
	cloudDetailNoiseDesc.MipLevels = 1;
	cloudDetailNoiseDesc.Usage = D3D11_USAGE_DEFAULT;
	cloudDetailNoiseDesc.CPUAccessFlags = 0;
	cloudDetailNoiseDesc.MiscFlags = 0;
	cloudDetailNoiseDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	cloudDetailNoiseDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	result = device->CreateTexture3D(&cloudDetailNoiseDesc, NULL, &m_cloudDetailNoise);
	if (FAILED(result)) {
		return false;
	}

	D3D11_TEXTURE2D_DESC cloudTypeDesc{};
	ZeroMemory(&cloudTypeDesc, sizeof(cloudTypeDesc));
	cloudTypeDesc.Width = m_params.shape_noise_resolution;
	cloudTypeDesc.Height = m_params.shape_noise_resolution;
	cloudTypeDesc.MipLevels = 1;
	cloudTypeDesc.ArraySize = 1;
	cloudTypeDesc.SampleDesc.Count = 1;
	cloudTypeDesc.SampleDesc.Quality = 0;
	cloudTypeDesc.Usage = D3D11_USAGE_DEFAULT;
	cloudTypeDesc.CPUAccessFlags = 0;
	cloudTypeDesc.MiscFlags = 0;
	cloudTypeDesc.Format = DXGI_FORMAT_R8_UNORM;
	cloudTypeDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	result = device->CreateTexture2D(&cloudTypeDesc, NULL, &m_cloudType);
	if (FAILED(result)) {
		return false;
	}

	D3D11_TEXTURE2D_DESC prevCloudsDesc{};
	ZeroMemory(&prevCloudsDesc, sizeof(prevCloudsDesc));
	prevCloudsDesc.Width = m_width;
	prevCloudsDesc.Height = m_height;
	prevCloudsDesc.MipLevels = 1;
	prevCloudsDesc.ArraySize = 1;
	prevCloudsDesc.SampleDesc.Count = 1;
	prevCloudsDesc.SampleDesc.Quality = 0;
	prevCloudsDesc.Usage = D3D11_USAGE_DEFAULT;
	prevCloudsDesc.CPUAccessFlags = 0;
	prevCloudsDesc.MiscFlags = 0;
	prevCloudsDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	prevCloudsDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	result = device->CreateTexture2D(&prevCloudsDesc, NULL, &m_prevClouds);
	if (FAILED(result)) {
		return false;
	}

	//// compile compute shaders
	// vertex
	result = D3DX11CompileFromFile(L"./data/shaders/VolumetricClouds.hlsl", NULL, NULL, "CloudsCombineVS", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL,
		&vertexShaderBuffer, &errorMessage, NULL);
	if (FAILED(result)) {
		// If the shader failed to compile it should have writen something to the error message.
		if (errorMessage) {
			OutputShaderErrorMessage(errorMessage, vsFilename);
		}
		// If there was nothing in the error message then it simply could not find the shader file itself.
		else {
			MessageBox(NULL, vsFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result)) {
		return false;
	}

	// pixel
	result = D3DX11CompileFromFile(L"./data/shaders/VolumetricClouds.hlsl", NULL, NULL, "CloudsCombinePS", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL,
		&pixelShaderBuffer, &errorMessage, NULL);
	if (FAILED(result)) {
		// If the shader failed to compile it should have writen something to the error message.
		if (errorMessage) {
			OutputShaderErrorMessage(errorMessage, psFilename);
		}
		// If there was nothing in the error message then it simply could not find the file itself.
		else {
			MessageBox(NULL, psFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result)) {
		return false;
	}

	// shape noise
	result = D3DX11CompileFromFile(L"./data/shaders/VolumetricCloudsNoise.hlsl", NULL, NULL, "CloudShapeCS", "cs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL,
		&computeShaderBuffer, &errorMessage, NULL);
	if (FAILED(result)) {
		if (errorMessage) {
			OutputShaderErrorMessage(errorMessage, psFilename);
		}
		return false;
	}
	result = device->CreateComputeShader(computeShaderBuffer->GetBufferPointer(), computeShaderBuffer->GetBufferSize(), NULL, &m_cloudShapeNoiseShader);
	if (FAILED(result)) {
		return false;
	}
	computeShaderBuffer->Release();
	computeShaderBuffer = 0;

	// detail noise
	result = D3DX11CompileFromFile(L"./data/shaders/VolumetricCloudsNoise.hlsl", NULL, NULL, "CloudDetailCS", "cs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL,
		&computeShaderBuffer, &errorMessage, NULL);
	if (FAILED(result)) {
		if (errorMessage) {
			OutputShaderErrorMessage(errorMessage, psFilename);
		}
		return false;
	}
	result = device->CreateComputeShader(computeShaderBuffer->GetBufferPointer(), computeShaderBuffer->GetBufferSize(), NULL, &m_cloudDetailNoiseShader);
	if (FAILED(result)) {
		return false;
	}
	computeShaderBuffer->Release();
	computeShaderBuffer = 0;

	// cloud type
	result = D3DX11CompileFromFile(L"./data/shaders/VolumetricCloudsNoise.hlsl", NULL, NULL, "CloudTypeCS", "cs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL,
		&computeShaderBuffer, &errorMessage, NULL);
	if (FAILED(result)) {
		if (errorMessage) {
			OutputShaderErrorMessage(errorMessage, psFilename);
		}
		return false;
	}
	result = device->CreateComputeShader(computeShaderBuffer->GetBufferPointer(), computeShaderBuffer->GetBufferSize(), NULL, &m_cloudTypeShader);
	if (FAILED(result)) {
		return false;
	}
	computeShaderBuffer->Release();
	computeShaderBuffer = 0;

	// prev clouds
	result = D3DX11CompileFromFile(L"./data/shaders/VolumetricClouds.hlsl", NULL, NULL, "CloudsCS", "cs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL,
		&computeShaderBuffer, &errorMessage, NULL);
	if (FAILED(result)) {
		if (errorMessage) {
			OutputShaderErrorMessage(errorMessage, psFilename);
		}
		return false;
	}
	result = device->CreateComputeShader(computeShaderBuffer->GetBufferPointer(), computeShaderBuffer->GetBufferSize(), NULL, &m_prevCloudShader);
	if (FAILED(result)) {
		return false;
	}
	computeShaderBuffer->Release();
	computeShaderBuffer = 0;

	// compute shader buffer
	struct CloudNoiseConstants
	{
		float resolution_inv;
		int frequency;
		int output_idx;
		float padding;
	};

	D3D11_BUFFER_DESC constant_buffer_desc = {};
	ZeroMemory(&constant_buffer_desc, sizeof(constant_buffer_desc));
	constant_buffer_desc.ByteWidth = sizeof(CloudNoiseConstants);
	constant_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	constant_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constant_buffer_desc.CPUAccessFlags = 0;
	result = device->CreateBuffer(&constant_buffer_desc, NULL, &m_cloudsBufferNoise);
	if (FAILED(result)) {
		return false;
	}

	// compute shader prev clouds buffer
	constant_buffer_desc = {};
	ZeroMemory(&constant_buffer_desc, sizeof(constant_buffer_desc));
	constant_buffer_desc.ByteWidth = sizeof(FrameBuffer);
	constant_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	constant_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constant_buffer_desc.CPUAccessFlags = 0;
	result = device->CreateBuffer(&constant_buffer_desc, NULL, &m_frameBufferNoise);
	if (FAILED(result)) {
		return false;
	}

	constant_buffer_desc = {};
	ZeroMemory(&constant_buffer_desc, sizeof(constant_buffer_desc));
	constant_buffer_desc.ByteWidth = sizeof(CloudsConstants);
	constant_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	constant_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constant_buffer_desc.CPUAccessFlags = 0;
	result = device->CreateBuffer(&constant_buffer_desc, NULL, &m_prevCloudsBufferNoise);
	if (FAILED(result)) {
		return false;
	}

	// create resource view for compute shader
	D3D11_SHADER_RESOURCE_VIEW_DESC srvbuffer_desc;
	ZeroMemory(&srvbuffer_desc, sizeof(srvbuffer_desc));
	srvbuffer_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvbuffer_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
	srvbuffer_desc.Texture3D.MostDetailedMip = 0;
	srvbuffer_desc.Texture3D.MipLevels = 1;
	result = device->CreateShaderResourceView(m_cloudShapeNoise, &srvbuffer_desc, &m_resourceShapeNoise);
	if (FAILED(result)) {
		return false;
	}

	ZeroMemory(&srvbuffer_desc, sizeof(srvbuffer_desc));
	srvbuffer_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvbuffer_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
	srvbuffer_desc.Texture3D.MostDetailedMip = 0;
	srvbuffer_desc.Texture3D.MipLevels = 1;
	result = device->CreateShaderResourceView(m_cloudDetailNoise, &srvbuffer_desc, &m_resourceDetailNoise);
	if (FAILED(result)) {
		return false;
	}

	ZeroMemory(&srvbuffer_desc, sizeof(srvbuffer_desc));
	srvbuffer_desc.Format = DXGI_FORMAT_R8_UNORM;
	srvbuffer_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvbuffer_desc.Texture2D.MostDetailedMip = 0;
	srvbuffer_desc.Texture2D.MipLevels = 1;
	result = device->CreateShaderResourceView(m_cloudType, &srvbuffer_desc, &m_resourceCloudType);
	if (FAILED(result)) {
		return false;
	}

	ZeroMemory(&srvbuffer_desc, sizeof(srvbuffer_desc));
	srvbuffer_desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	srvbuffer_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvbuffer_desc.Texture2D.MostDetailedMip = 0;
	srvbuffer_desc.Texture2D.MipLevels = 1;
	result = device->CreateShaderResourceView(m_prevClouds, &srvbuffer_desc, &m_resourcePrevClouds);
	if (FAILED(result)) {
		return false;
	}

	// create unordered
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavbuffer_desc;
	ZeroMemory(&uavbuffer_desc, sizeof(uavbuffer_desc));
	uavbuffer_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	uavbuffer_desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE3D;
	uavbuffer_desc.Texture3D.MipSlice = 0;
	uavbuffer_desc.Texture3D.FirstWSlice = 0;
	uavbuffer_desc.Texture3D.WSize = m_params.shape_noise_resolution;
	result = device->CreateUnorderedAccessView(m_cloudShapeNoise, &uavbuffer_desc, &m_cloudsUnorderedViewShapeNoise);
	if (FAILED(result)) {
		return false;
	}

	ZeroMemory(&uavbuffer_desc, sizeof(uavbuffer_desc));
	uavbuffer_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	uavbuffer_desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE3D;
	uavbuffer_desc.Texture3D.MipSlice = 0;
	uavbuffer_desc.Texture3D.FirstWSlice = 0;
	uavbuffer_desc.Texture3D.WSize = m_params.shape_noise_resolution;
	result = device->CreateUnorderedAccessView(m_cloudDetailNoise, &uavbuffer_desc, &m_cloudsUnorderedViewDetailNoise);
	if (FAILED(result)) {
		return false;
	}

	ZeroMemory(&uavbuffer_desc, sizeof(uavbuffer_desc));
	uavbuffer_desc.Format = DXGI_FORMAT_UNKNOWN;
	uavbuffer_desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	uavbuffer_desc.Texture2D.MipSlice = 0;
	result = device->CreateUnorderedAccessView(m_cloudType, &uavbuffer_desc, &m_cloudsUnorderedView);
	if (FAILED(result)) {
		return false;
	}

	ZeroMemory(&uavbuffer_desc, sizeof(uavbuffer_desc));
	uavbuffer_desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	uavbuffer_desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	uavbuffer_desc.Texture2D.MipSlice = 0;
	result = device->CreateUnorderedAccessView(m_prevClouds, &uavbuffer_desc, &m_prevCloudsUnorderedView);
	if (FAILED(result)) {
		return false;
	}


	// Create the vertex input layout description.
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	// Get a count of the elements in the layout.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the vertex input layout.
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(),
		&m_layout);
	if (FAILED(result)) {
		return false;
	}

	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;
	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;


	// 1 wrap - linear - texture
	// Create a wrap texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = samplerDesc.BorderColor[1] = samplerDesc.BorderColor[2] = samplerDesc.BorderColor[3] = 0.0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	device->CreateSamplerState(&samplerDesc, &m_sampleStateWrap);

	return true;
}

void VolumetricClouds::computeNoiseShaders()
{
	ID3D11DeviceContext* context = m_D3D->GetDeviceContext();
	int dispatch;

	struct CloudNoiseConstants
	{
		float resolution_inv;
		int frequency;
		int output_idx;
		float padding;
	} params;

	//// pass cloud shape noise
	// set constant
	params.resolution_inv = 1.0f / m_params.shape_noise_resolution;
	params.frequency = m_params.shape_noise_frequency;
	params.output_idx = 0;
	params.padding = 0;
	context->UpdateSubresource(m_cloudsBufferNoise, 0, NULL, &params, 0, 0);
	context->CSSetConstantBuffers(0, 1, &m_cloudsBufferNoise);
	// set params
	context->CSSetUnorderedAccessViews(0, 1, &m_cloudsUnorderedViewShapeNoise, 0);
	context->CSSetShaderResources(0, 1, &m_resourceShapeNoise);
	context->VSSetShader(NULL, NULL, 0);
	context->PSSetShader(NULL, NULL, 0);
	context->CSSetShader(m_cloudShapeNoiseShader, NULL, 0);
	// execute
	dispatch = m_params.shape_noise_resolution / 8;
	context->Dispatch(dispatch, dispatch, dispatch);

	//// pass cloud detail noise
	// set constant
	params.resolution_inv = 1.0f / m_params.shape_noise_resolution;
	params.frequency = m_params.detail_noise_frequency;
	context->UpdateSubresource(m_cloudsBufferNoise, 0, NULL, &params, 0, 0);
	context->CSSetConstantBuffers(0, 1, &m_cloudsBufferNoise);
	// set params
	context->CSSetUnorderedAccessViews(0, 1, &m_cloudsUnorderedViewDetailNoise, 0);
	context->CSSetShaderResources(0, 1, &m_resourceDetailNoise);
	context->VSSetShader(NULL, NULL, 0);
	context->PSSetShader(NULL, NULL, 0);
	context->CSSetShader(m_cloudDetailNoiseShader, NULL, 0);
	// execute
	dispatch = m_params.shape_noise_resolution / 8;
	context->Dispatch(dispatch, dispatch, dispatch);

	//// pass cloud type
	// set constant
	params.resolution_inv = 1.0f / m_params.shape_noise_resolution;
	context->UpdateSubresource(m_cloudsBufferNoise, 0, NULL, &params, 0, 0);
	context->CSSetConstantBuffers(0, 1, &m_cloudsBufferNoise);
	// set params
	context->CSSetUnorderedAccessViews(0, 1, &m_cloudsUnorderedView, 0);
	context->CSSetShaderResources(0, 1, &m_resourceCloudType);
	context->VSSetShader(NULL, NULL, 0);
	context->PSSetShader(NULL, NULL, 0);
	context->CSSetShader(m_cloudTypeShader, NULL, 0);
	// execute
	dispatch = m_params.shape_noise_resolution / 8;
	context->Dispatch(dispatch, dispatch, dispatch);

	// reset uav
	ID3D11ShaderResourceView* nullSRV = nullptr;
	ID3D11UnorderedAccessView* nullUAV = nullptr;
	context->CSSetUnorderedAccessViews(0, 1, &nullUAV, 0);
	context->CSSetShaderResources(0, 1, &nullSRV);
	context->CSSetShader(nullptr, nullptr, 0);
}

void VolumetricClouds::computeVolumetricCloudsShaders(CameraClass* camera)
{
	ID3D11DeviceContext* context = m_D3D->GetDeviceContext();
	D3DXMATRIX view, project;
	D3DXMATRIX invertView, invertProject;
	
	CloudsConstants params{};
	float noise_scale = 0.00001f + m_params.shape_noise_scale * 0.0004f;

	params.cloud_type = m_params.cloud_type;
	params.cloud_min_height = m_params.cloud_min_height;
	params.cloud_max_height = m_params.cloud_max_height;
	params.padding = 0.0f;

	params.shape_noise_scale = noise_scale;
	params.detail_noise_scale = m_params.detail_noise_scale * noise_scale;
	params.detail_noise_modifier = m_params.detail_noise_modifier;
	params.global_density = m_params.global_density;

	params.cloud_coverage = m_params.cloud_coverage;
	params.cloud_base_color = D3DXVECTOR3(m_params.cloud_base_color);
	params.cloud_top_color = D3DXVECTOR3(m_params.cloud_top_color);
	params.max_num_steps = m_params.max_num_steps;

	params.planet_center = D3DXVECTOR3(0.0f, -m_params.planet_radius, 0.0f);
	params.planet_radius = m_params.planet_radius;

	params.light_step_length = m_params.light_step_length;
	params.light_cone_radius = m_params.light_cone_radius;
	params.precipitation = m_params.precipitation * 0.01f;
	params.ambient_light_factor = m_params.ambient_light_factor;

	params.sun_light_factor = m_params.sun_light_factor;
	params.henyey_greenstein_g_forward = m_params.henyey_greenstein_g_forward;
	params.henyey_greenstein_g_backward = m_params.henyey_greenstein_g_backward;
	params.resolution_factor = CloudResolution_Full;


	m_D3D->GetProjectionMatrix(project);
	camera->GetViewMatrix(view);

	D3DXMatrixInverse(&invertProject, NULL, &project);
	D3DXMatrixInverse(&invertView, NULL, &view);

	D3DXMatrixTranspose(&invertProject, &invertProject);
	D3DXMatrixTranspose(&invertView, &invertView);

	//// pass volumetric clouds
	// set constant
	m_frameBuffer.totalTime += 0.2f;
	m_frameBuffer.inverseView = invertView;
	m_frameBuffer.inverseProjection = invertProject;
	m_frameBuffer.cameraPosition = D3DXVECTOR4( camera->GetPosition().x, camera->GetPosition().y, camera->GetPosition().z, 1.0f );
	m_frameBuffer.renderResolution = D3DXVECTOR2(m_width, m_height);

	context->UpdateSubresource(m_prevCloudsBufferNoise, 0, NULL, &params, 0, 0);
	context->CSSetConstantBuffers(0, 1, &m_prevCloudsBufferNoise);
	context->UpdateSubresource(m_frameBufferNoise, 0, NULL, &m_frameBuffer, 0, 0);
	context->CSSetConstantBuffers(1, 1, &m_frameBufferNoise);
	
	// set params
	context->CSSetUnorderedAccessViews(0, 1, &m_prevCloudsUnorderedView, 0);
	context->CSSetShaderResources(0, 1, &m_resourcePrevClouds);

	context->CSSetShaderResources(1, 1, &m_resourceShapeNoise);
	context->CSSetShaderResources(2, 1, &m_resourceDetailNoise);
	context->CSSetShaderResources(3, 1, &m_resourceCloudType);
	context->CSSetSamplers(0, 1, &m_sampleStateWrap);

	context->VSSetShader(NULL, NULL, 0);
	context->PSSetShader(NULL, NULL, 0);
	context->CSSetShader(m_prevCloudShader, NULL, 0);
	// execute
	context->Dispatch(m_width / 16.0f, m_height / 16.0f, 1);

	// reset uav
	ID3D11ShaderResourceView* nullSRV = nullptr;
	ID3D11UnorderedAccessView* nullUAV = nullptr;
	context->CSSetUnorderedAccessViews(0, 1, &nullUAV, 0);
	context->CSSetShaderResources(0, 1, &nullSRV);
	context->CSSetShader(nullptr, nullptr, 0);
}

void VolumetricClouds::ShutdownShader()
{
	if (m_layout) {
		m_layout->Release();
		m_layout = 0;
	}

	if (m_cloudShapeNoiseShader) {
		m_cloudShapeNoiseShader->Release();
		m_cloudShapeNoiseShader = 0;
	}

	if (m_cloudDetailNoiseShader) {
		m_cloudDetailNoiseShader->Release();
		m_cloudDetailNoiseShader = 0;
	}

	if (m_cloudTypeShader) {
		m_cloudTypeShader->Release();
		m_cloudTypeShader = 0;
	}

	if (m_prevCloudShader) {
		m_prevCloudShader->Release();
		m_prevCloudShader = 0;
	}

	AbstractShader::Shutdown();
}


bool VolumetricClouds::SetShaderParameters(ID3D11DeviceContext* deviceContext)
{
	return true;
}

void VolumetricClouds::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// Set the vertex input layout.
	deviceContext->IASetInputLayout(m_layout);

	deviceContext->PSSetShaderResources(3, 1, &m_resourcePrevClouds);
	deviceContext->PSSetSamplers(0, 1, &m_sampleStateWrap);

	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// Render the triangle.
	deviceContext->DrawIndexed(indexCount, 0, 0);
}
