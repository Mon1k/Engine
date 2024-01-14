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
	m_cloudsBufferNoiseUnorderer = 0;
	m_cloudsUnorderedView = 0;
	m_cloudsReadBackBuffer = 0;
	m_resourceShapeNoise = 0;
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
	cloudShapeNoiseDesc.Width = m_params.shape_noise_resolution;
	cloudShapeNoiseDesc.Height = m_params.shape_noise_resolution;
	cloudShapeNoiseDesc.Depth = m_params.shape_noise_resolution;
	cloudShapeNoiseDesc.MipLevels = 1;
	cloudShapeNoiseDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	cloudShapeNoiseDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	device->CreateTexture3D(&cloudShapeNoiseDesc, NULL, &m_cloudShapeNoise);

	D3D11_TEXTURE3D_DESC cloudDetailNoiseDesc{};
	cloudDetailNoiseDesc.Width = m_params.shape_noise_resolution;
	cloudDetailNoiseDesc.Height = m_params.shape_noise_resolution;
	cloudDetailNoiseDesc.Depth = m_params.shape_noise_resolution;
	cloudDetailNoiseDesc.MipLevels = 4;
	cloudDetailNoiseDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	cloudDetailNoiseDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	device->CreateTexture3D(&cloudDetailNoiseDesc, NULL, &m_cloudDetailNoise);

	D3D11_TEXTURE2D_DESC cloudTypeDesc{};
	cloudTypeDesc.Width = 512;
	cloudTypeDesc.Height = 512;
	cloudTypeDesc.MipLevels = 1;

	cloudTypeDesc.ArraySize = 1;
	cloudTypeDesc.SampleDesc.Count = 1;
	cloudTypeDesc.SampleDesc.Quality = 0;
	cloudTypeDesc.CPUAccessFlags = 0;
	cloudTypeDesc.MiscFlags = 0;
	cloudTypeDesc.Usage = D3D11_USAGE_DEFAULT;

	cloudTypeDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	cloudTypeDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	device->CreateTexture2D(&cloudTypeDesc, NULL, &m_cloudType);

	// compile compute shaders
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

	D3D11_BUFFER_DESC buffer_desc;
	ZeroMemory(&buffer_desc, sizeof(buffer_desc));
	buffer_desc.ByteWidth = sizeof(ID3D11Texture2D);
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	buffer_desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	buffer_desc.StructureByteStride = sizeof(ID3D11Texture2D);
	result = device->CreateBuffer(&buffer_desc, NULL, &m_cloudsBufferNoiseUnorderer);
	if (FAILED(result)) {
		return false;
	}

	// create resource view for compute shader
	D3D11_SHADER_RESOURCE_VIEW_DESC srvbuffer_desc;
	ZeroMemory(&srvbuffer_desc, sizeof(srvbuffer_desc));
	srvbuffer_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;// DXGI_FORMAT_UNKNOWN;
	srvbuffer_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;// D3D11_SRV_DIMENSION_BUFFER;
	//srvbuffer_desc.Buffer.ElementWidth = 1;
	srvbuffer_desc.Texture2D.MostDetailedMip = 0;
	srvbuffer_desc.Texture2D.MipLevels = 1;
	result = device->CreateShaderResourceView(m_cloudType, &srvbuffer_desc, &m_resourceShapeNoise);
	if (FAILED(result)) {
		return false;
	}

	// create unordered and back buffer
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavbuffer_desc;
	ZeroMemory(&uavbuffer_desc, sizeof(uavbuffer_desc));
	//uavbuffer_desc.Format = DXGI_FORMAT_UNKNOWN;
	//uavbuffer_desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavbuffer_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	uavbuffer_desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	//uavbuffer_desc.Buffer.NumElements = 1;
	uavbuffer_desc.Texture2D.MipSlice = 0;
	//result = device->CreateUnorderedAccessView(m_cloudsBufferNoiseUnorderer, &uavbuffer_desc, &m_cloudsUnorderedView);
	result = device->CreateUnorderedAccessView(m_cloudType, &uavbuffer_desc, &m_cloudsUnorderedView);
	if (FAILED(result)) {
		return false;
	}

	// create readback buffer
	D3D11_BUFFER_DESC readback_buffer_desc;
	ZeroMemory(&readback_buffer_desc, sizeof(readback_buffer_desc));
	readback_buffer_desc.ByteWidth = sizeof(ID3D11Texture2D);
	readback_buffer_desc.Usage = D3D11_USAGE_STAGING;
	readback_buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	readback_buffer_desc.StructureByteStride = sizeof(ID3D11Texture2D);
	result = device->CreateBuffer(&readback_buffer_desc, NULL, &m_cloudsReadBackBuffer);
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
	D3DXVECTOR4 texture, texture2;
	ID3D11Texture3D* texture3;

	struct CloudNoiseConstants
	{
		float resolution_inv;
		int frequency;
		int output_idx;
		float padding;
	} params, params2;

	params.resolution_inv = 1 / m_params.shape_noise_resolution;
	params.frequency = m_params.shape_noise_frequency;
	params.output_idx = 0;
	params.padding = 0;

	// upload data, read write var
	m_D3D->GetDeviceContext()->UpdateSubresource(m_cloudsBufferNoiseUnorderer, 0, NULL, &m_cloudType, 0, 0);

	// set constant
	m_D3D->GetDeviceContext()->UpdateSubresource(m_cloudsBufferNoise, 0, NULL, &params, 0, 0);
	m_D3D->GetDeviceContext()->CSSetConstantBuffers(0, 1, &m_cloudsBufferNoise);

	// 
	m_D3D->GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, &m_cloudsUnorderedView, 0);
	m_D3D->GetDeviceContext()->CSSetShaderResources(0, 1, &m_resourceShapeNoise);
	m_D3D->GetDeviceContext()->CSSetShader(m_cloudShapeNoiseShader, NULL, 0);

	// execute
	m_D3D->GetDeviceContext()->Dispatch(8, 8, 8);

	// download data
	D3D11_MAPPED_SUBRESOURCE MappedResource = { 0 };
	m_D3D->GetDeviceContext()->CopyResource(m_cloudsReadBackBuffer, m_cloudsBufferNoiseUnorderer);
	m_D3D->GetDeviceContext()->Map(m_cloudsReadBackBuffer, 0, D3D11_MAP_READ, 0, &MappedResource);
	memcpy(&m_cloudType, MappedResource.pData, sizeof(ID3D11Texture2D));
	int t = 1;
	m_D3D->GetDeviceContext()->Unmap(m_cloudsReadBackBuffer, 0);


	/*time_t ttt;
	struct tm local_time;
	time(&ttt);
	localtime_s(&local_time, &ttt);
	char filename[80];
	sprintf(filename, "screenshot_%d_%02d_%02d__%02d_%02d_%02d.jpg", local_time.tm_year + 1900, local_time.tm_mon + 1, local_time.tm_mday, local_time.tm_hour, local_time.tm_min, local_time.tm_sec);
	D3DX11SaveTextureToFileA(m_D3D->GetDeviceContext(), m_cloudType, D3DX11_IFF_JPG, filename);*/
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
