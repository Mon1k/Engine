#include "VolumetricClouds.h"
#include <time.h>
#include <stdio.h>

VolumetricClouds::VolumetricClouds()
{
	m_layout = 0;
	m_cloudsBuffer = 0;

	m_cloudShapeNoiseShader = 0;
	m_cloudDetailNoiseShader = 0;
	m_cloudTypeShader = 0;

	m_cloudsBufferNoise = 0;

	m_cloudsUnorderedViewShapeNoise = 0;
	m_cloudsUnorderedViewDetailNoise = 0;
	m_cloudsUnorderedView = 0;

	m_resourceShapeNoise = 0;
	m_resourceDetailNoise = 0;
	m_resourceCloudType = 0;
}

VolumetricClouds::~VolumetricClouds()
{

}

bool VolumetricClouds::Initialize(ID3D11Device* device)
{
	return InitializeShader(device, L"./data/shaders/VolumetricClouds.hlsl", L"./data/shaders/VolumetricClouds.hlsl");
}


void VolumetricClouds::Shutdown()
{
	VolumetricClouds();
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
	D3D11_INPUT_ELEMENT_DESC polygonLayout[3];
	unsigned int numElements;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC cloudsBufferDesc;

	// Initialize the pointers this function will use to null.
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;
	computeShaderBuffer = 0;

	// Compile the vertex shader code.
	/*result = D3DX11CompileFromFile(vsFilename, NULL, NULL, "ShadowVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL,
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

	// Compile the pixel shader code.
	result = D3DX11CompileFromFile(psFilename, NULL, NULL, "ShadowPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL,
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

	// Create the vertex shader from the buffer.
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
	{
		return false;
	}

	// Create the pixel shader from the buffer.
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result)) {
		return false;
	}*/


	////

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

	//// compile compute shaders
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
	uavbuffer_desc.Format = DXGI_FORMAT_UNKNOWN;// D3D11_UAV_DIMENSION_BUFFER;
	uavbuffer_desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;// D3D11_SRV_DIMENSION_BUFFER;
	uavbuffer_desc.Texture2D.MipSlice = 0;
	result = device->CreateUnorderedAccessView(m_cloudType, &uavbuffer_desc, &m_cloudsUnorderedView);
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

	polygonLayout[2].SemanticName = "NORMAL";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	// Get a count of the elements in the layout.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the vertex input layout.
	/*result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(),
		&m_layout);
	if (FAILED(result)) {
		return false;
	}

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;*/


	// 1 wrap - linear - texture
	// Create a wrap texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = samplerDesc.BorderColor[1] = samplerDesc.BorderColor[2] = samplerDesc.BorderColor[3] = 1.0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	device->CreateSamplerState(&samplerDesc, &m_sampleStateWrap);

	// Setup the description of the light dynamic constant buffer that is in the pixel shader.
	cloudsBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cloudsBufferDesc.ByteWidth = sizeof(CloudParameters);
	cloudsBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cloudsBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cloudsBufferDesc.MiscFlags = 0;
	cloudsBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the pixel shader constant buffer from within this class.
	result = device->CreateBuffer(&cloudsBufferDesc, NULL, &m_cloudsBuffer);
	if (FAILED(result)) {
		return false;
	}

	return true;
}

void VolumetricClouds::computeShaders()
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

void VolumetricClouds::ShutdownShader()
{
	if (m_layout) {
		m_layout->Release();
		m_layout = 0;
	}

	if (m_cloudsBuffer) {
		m_cloudsBuffer->Release();
		m_cloudsBuffer = 0;
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

	AbstractShader::Shutdown();
}


bool VolumetricClouds::SetShaderParameters(ID3D11DeviceContext* deviceContext)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	unsigned int bufferNumber;
	CloudParameters* dataPtr;
	

	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(m_cloudsBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)) {
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr = (CloudParameters*)mappedResource.pData;

	dataPtr = &m_params;

	// Unlock the constant buffer.
	deviceContext->Unmap(m_cloudsBuffer, 0);

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	// Now set the constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_cloudsBuffer);

	// Set shader texture resource in the pixel shader.
	//deviceContext->PSSetShaderResources(0, 1, &depthMapTexture);
	//deviceContext->PSSetShaderResources(1, 1, &texture);

	return true;
}


void VolumetricClouds::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// Set the vertex input layout.
	deviceContext->IASetInputLayout(m_layout);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// Set the sampler states in the pixel shader.
	deviceContext->PSSetSamplers(0, 1, &m_sampleStateWrap);

	// Render the triangle.
	deviceContext->DrawIndexed(indexCount, 0, 0);
}
