#include "skyplaneclass.h"

SkyPlaneClass::SkyPlaneClass(): ModelClass()
{
	m_skyPlane = 0;

	// Set the brightness of the clouds.
	m_brightness = 0.65f;
}


SkyPlaneClass::SkyPlaneClass(const SkyPlaneClass& other)
{
}


SkyPlaneClass::~SkyPlaneClass()
{
}

bool SkyPlaneClass::Initialize(D3DClass* d3dClass, std::vector<std::wstring> texturesFilename)
{
	int skyPlaneResolution, textureRepeat;
	float skyPlaneWidth, skyPlaneTop, skyPlaneBottom;
	bool result;

	m_D3D = d3dClass;

	// Set the sky plane parameters.
	skyPlaneResolution = 10;
	skyPlaneWidth = 10.0f;
	skyPlaneTop = 0.5f;
	skyPlaneBottom = 0.0f;
	textureRepeat = 5;

	// Setup the cloud translation speed increments.
	m_translationSpeed[0] = 0.0003f;   // First texture X translation speed.
	m_translationSpeed[1] = 0.0f;      // First texture Z translation speed.
	m_translationSpeed[2] = 0.00015f;  // Second texture X translation speed.
	m_translationSpeed[3] = 0.0f;      // Second texture Z translation speed.

	// Initialize the texture translation values.
	m_textureTranslation[0] = 0.0f;
	m_textureTranslation[1] = 0.0f;
	m_textureTranslation[2] = 0.0f;
	m_textureTranslation[3] = 0.0f;

	// Create the sky plane.
	result = InitializeSkyPlane(skyPlaneResolution, skyPlaneWidth, skyPlaneTop, skyPlaneBottom, textureRepeat);
	if (!result) {
		return false;
	}

	// Create the vertex and index buffer for the sky plane.
	result = InitializeBuffers(m_D3D->GetDevice(), skyPlaneResolution);
	if (!result) {
		return false;
	}

	// Load the sky plane textures.
	result = LoadTexturesArray(m_D3D->GetDevice(), texturesFilename);
	if (!result) {
		return false;
	}

	m_shader = new SkyPlaneShaderClass;
	if (!m_shader->Initialize(m_D3D->GetDevice())) {
		return false;
	}

	return true;
}

void SkyPlaneClass::Shutdown()
{
	// Release the sky plane textures.
	ReleaseTexture();

	// Release the vertex and index buffer that were used for rendering the sky plane.
	ShutdownBuffers();

	// Release the sky plane array.
	ShutdownSkyPlane();
}


void SkyPlaneClass::ShutdownSkyPlane()
{
	// Release the sky plane array.
	if (m_skyPlane) {
		delete[] m_skyPlane;
		m_skyPlane = 0;
	}
}

void SkyPlaneClass::Frame()
{
	// Increment the translation values to simulate the moving clouds.
	m_textureTranslation[0] += m_translationSpeed[0];
	m_textureTranslation[1] += m_translationSpeed[1];
	m_textureTranslation[2] += m_translationSpeed[2];
	m_textureTranslation[3] += m_translationSpeed[3];

	// Keep the values in the zero to one range.
	if (m_textureTranslation[0] > 1.0f) { 
		m_textureTranslation[0] -= 1.0f; 
	}
	if (m_textureTranslation[1] > 1.0f) { 
		m_textureTranslation[1] -= 1.0f; 
	}
	if (m_textureTranslation[2] > 1.0f) { 
		m_textureTranslation[2] -= 1.0f; 
	}
	if (m_textureTranslation[3] > 1.0f) { 
		m_textureTranslation[3] -= 1.0f; 
	}
}

float SkyPlaneClass::GetBrightness()
{
	return m_brightness;
}

float SkyPlaneClass::GetTranslation(int index)
{
	return m_textureTranslation[index];
}

bool SkyPlaneClass::InitializeSkyPlane(int skyPlaneResolution, float skyPlaneWidth, float skyPlaneTop, float skyPlaneBottom, int textureRepeat)
{
	float quadSize, radius, constant, textureDelta;
	int i, j, index;
	float positionX, positionY, positionZ, tu, tv;


	// Create the array to hold the sky plane coordinates.
	m_skyPlane = new SkyPlaneType[(skyPlaneResolution + 1) * (skyPlaneResolution + 1)];
	if (!m_skyPlane) {
		return false;
	}

	// Determine the size of each quad on the sky plane.
	quadSize = skyPlaneWidth / (float)skyPlaneResolution;

	// Calculate the radius of the sky plane based on the width.
	radius = skyPlaneWidth / 2.0f;

	// Calculate the height constant to increment by.
	constant = (skyPlaneTop - skyPlaneBottom) / (radius * radius);

	// Calculate the texture coordinate increment value.
	textureDelta = (float)textureRepeat / (float)skyPlaneResolution;

	// Loop through the sky plane and build the coordinates based on the increment values given.
	for (j = 0; j <= skyPlaneResolution; j++) {
		for (i = 0; i <= skyPlaneResolution; i++) {
			// Calculate the vertex coordinates.
			positionX = (-0.5f * skyPlaneWidth) + ((float)i * quadSize);
			positionZ = (-0.5f * skyPlaneWidth) + ((float)j * quadSize);
			positionY = skyPlaneTop - (constant * ((positionX * positionX) + (positionZ * positionZ)));

			// Calculate the texture coordinates.
			tu = (float)i * textureDelta;
			tv = (float)j * textureDelta;

			// Calculate the index into the sky plane array to add this coordinate.
			index = j * (skyPlaneResolution + 1) + i;

			// Add the coordinates to the sky plane array.
			m_skyPlane[index].x = positionX;
			m_skyPlane[index].y = positionY;
			m_skyPlane[index].z = positionZ;
			m_skyPlane[index].tu = tu;
			m_skyPlane[index].tv = tv;
		}
	}

	return true;
}

bool SkyPlaneClass::InitializeBuffers(ID3D11Device* device, int skyPlaneResolution)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i, j, index, index1, index2, index3, index4;


	// Calculate the number of vertices in the sky plane mesh.
	m_vertexCount = (skyPlaneResolution + 1) * (skyPlaneResolution + 1) * 6;

	// Set the index count to the same as the vertex count.
	m_indexCount = m_vertexCount;

	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];
	if (!vertices) {
		return false;
	}

	// Create the index array.
	indices = new unsigned long[m_indexCount];
	if (!indices) {
		return false;
	}

	// Initialize the index into the vertex array.
	index = 0;

	// Load the vertex and index array with the sky plane array data.
	for (j = 0; j < skyPlaneResolution; j++) {
		for (i = 0; i < skyPlaneResolution; i++) {
			index1 = j * (skyPlaneResolution + 1) + i;
			index2 = j * (skyPlaneResolution + 1) + (i + 1);
			index3 = (j + 1) * (skyPlaneResolution + 1) + i;
			index4 = (j + 1) * (skyPlaneResolution + 1) + (i + 1);

			// Triangle 1 - Upper Left
			vertices[index].position = D3DXVECTOR3(m_skyPlane[index1].x, m_skyPlane[index1].y, m_skyPlane[index1].z);
			vertices[index].texture = D3DXVECTOR2(m_skyPlane[index1].tu, m_skyPlane[index1].tv);
			indices[index] = index;
			index++;

			// Triangle 1 - Upper Right
			vertices[index].position = D3DXVECTOR3(m_skyPlane[index2].x, m_skyPlane[index2].y, m_skyPlane[index2].z);
			vertices[index].texture = D3DXVECTOR2(m_skyPlane[index2].tu, m_skyPlane[index2].tv);
			indices[index] = index;
			index++;

			// Triangle 1 - Bottom Left
			vertices[index].position = D3DXVECTOR3(m_skyPlane[index3].x, m_skyPlane[index3].y, m_skyPlane[index3].z);
			vertices[index].texture = D3DXVECTOR2(m_skyPlane[index3].tu, m_skyPlane[index3].tv);
			indices[index] = index;
			index++;

			// Triangle 2 - Bottom Left
			vertices[index].position = D3DXVECTOR3(m_skyPlane[index3].x, m_skyPlane[index3].y, m_skyPlane[index3].z);
			vertices[index].texture = D3DXVECTOR2(m_skyPlane[index3].tu, m_skyPlane[index3].tv);
			indices[index] = index;
			index++;

			// Triangle 2 - Upper Right
			vertices[index].position = D3DXVECTOR3(m_skyPlane[index2].x, m_skyPlane[index2].y, m_skyPlane[index2].z);
			vertices[index].texture = D3DXVECTOR2(m_skyPlane[index2].tu, m_skyPlane[index2].tv);
			indices[index] = index;
			index++;

			// Triangle 2 - Bottom Right
			vertices[index].position = D3DXVECTOR3(m_skyPlane[index4].x, m_skyPlane[index4].y, m_skyPlane[index4].z);
			vertices[index].texture = D3DXVECTOR2(m_skyPlane[index4].tu, m_skyPlane[index4].tv);
			indices[index] = index;
			index++;
		}
	}

	// Set up the description of the vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now finally create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result)) {
		return false;
	}

	// Set up the description of the index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result)) {
		return false;
	}

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}

void SkyPlaneClass::Render(CameraClass* camera)
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix;
	D3DXVECTOR3 cameraPosition;

	camera->GetViewMatrix(viewMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);

	// Translate the sky dome to be centered around the camera position.
	cameraPosition = camera->GetPosition();
	D3DXMatrixTranslation(&worldMatrix, cameraPosition.x, cameraPosition.y, cameraPosition.z);


	m_D3D->TurnZBufferOff();
	m_D3D->EnableSecondBlendState();

	RenderBuffers(m_D3D->GetDeviceContext());
	m_shader->Render(m_D3D->GetDeviceContext(), GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, GetTexture(0), GetTexture(1), GetTranslation(0), GetTranslation(1), GetTranslation(2), GetTranslation(3), GetBrightness());

	m_D3D->TurnOffAlphaBlending();
	m_D3D->TurnZBufferOn();
}

void SkyPlaneClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}