#include "skydomeclass.h"

SkyDomeClass::SkyDomeClass(): ModelClass()
{
	m_model = 0;
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
}


SkyDomeClass::SkyDomeClass(const SkyDomeClass& other)
{
}


SkyDomeClass::~SkyDomeClass()
{
}

bool SkyDomeClass::Initialize(D3DClass* d3dClass, char* fileName)
{
	bool result;

	m_D3D = d3dClass;

	if (!LoadModel(fileName)) {
		return false;
	}


	// Load the sky dome into a vertex and index buffer for rendering.
	result = InitializeBuffers(m_D3D->GetDevice());
	if (!result) {
		return false;
	}

	m_shader = new SkyDomeShaderClass;
	if (!m_shader->Initialize(m_D3D->GetDevice())) {
		return false;
	}

	// Set the color at the top of the sky dome.
	m_apexColor = D3DXVECTOR4(0.28f, 0.56f, 1.0f, 1.0f);

	// Set the color at the center of the sky dome.
	m_centerColor = D3DXVECTOR4(0.7f, 0.87f, 1.0f, 1.0f);

	return true;
}

D3DXVECTOR4 SkyDomeClass::GetApexColor()
{
	return m_apexColor;
}

D3DXVECTOR4 SkyDomeClass::GetCenterColor()
{
	return m_centerColor;
}

bool SkyDomeClass::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;


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

	// Load the vertex array and index array with data.
	for (i = 0; i < m_vertexCount; i++) {
		vertices[i].position = D3DXVECTOR3(m_model[i].x, m_model[i].y, m_model[i].z);
		indices[i] = i;
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

void SkyDomeClass::Render(CameraClass* camera)
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix;
	D3DXVECTOR3 cameraPosition;

	
	camera->GetViewMatrix(viewMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);

	// Translate the sky dome to be centered around the camera position.
	cameraPosition = camera->GetPosition();
	D3DXMatrixTranslation(&worldMatrix, cameraPosition.x, cameraPosition.y, cameraPosition.z);


	m_D3D->TurnOffCulling();
	m_D3D->TurnZBufferOff();

	RenderBuffers(m_D3D->GetDeviceContext());
	m_shader->Render(m_D3D->GetDeviceContext(), GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_apexColor, m_centerColor);

	m_D3D->TurnOnCulling();
	m_D3D->TurnZBufferOn();
}

void SkyDomeClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
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