#include "VolumetricClouds.h"

VolumetricClouds::VolumetricClouds()
{
	m_layout = 0;
	m_cloudsBuffer = 0;
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
	D3D11_INPUT_ELEMENT_DESC polygonLayout[3];
	unsigned int numElements;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC cloudsBufferDesc;

	// Initialize the pointers this function will use to null.
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	// Compile the vertex shader code.
	result = D3DX11CompileFromFile(vsFilename, NULL, NULL, "ShadowVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL,
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
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(),
		&m_layout);
	if (FAILED(result)) {
		return false;
	}

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
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

	D3D11_TEXTURE3D_DESC cloudShapeNoiseDesc{};
	cloudShapeNoiseDesc.Width = m_params.shape_noise_resolution;
	cloudShapeNoiseDesc.Height = m_params.shape_noise_resolution;
	cloudShapeNoiseDesc.Depth = m_params.shape_noise_resolution;
	cloudShapeNoiseDesc.MipLevels = 4;
	cloudShapeNoiseDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	cloudShapeNoiseDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
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
	cloudTypeDesc.Width = 128;
	cloudTypeDesc.Height = 128;
	cloudTypeDesc.MipLevels = 1;
	cloudTypeDesc.Format = DXGI_FORMAT_R8_UNORM;
	cloudTypeDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	device->CreateTexture2D(&cloudTypeDesc, NULL, &m_cloudType);

	return true;
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
	deviceContext->PSSetShaderResources(0, 1, &depthMapTexture);
	deviceContext->PSSetShaderResources(1, 1, &texture);

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
