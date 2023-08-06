#include "gridclass.h"


GridClass::GridClass(): ModelClass()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
}


GridClass::GridClass(const GridClass& other)
{
}


GridClass::~GridClass()
{
}


bool GridClass::Initialize(D3DClass* d3dClass, int width, int height)
{
	bool result;

	m_D3D = d3dClass;

	// Manually set the width and height of the terrain.
	m_terrainWidth = width;
	m_terrainHeight = height;

	// Initialize the vertex and index buffer that hold the geometry for the terrain.
	result = InitializeBuffers(m_D3D->GetDevice());
	if (!result) {
		return false;
	}

	m_shader = new ColorShaderClass;
	m_shader->Initialize(m_D3D->GetDevice());

	return true;
}

bool GridClass::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	int index, i, j;
	float positionX, positionZ;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;


	// Calculate the number of vertices in the terrain mesh.
	m_vertexCount = (m_terrainWidth - 1) * (m_terrainHeight - 1) * 8;

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

	// Initialize the index to the vertex array.
	index = 0;

	m_Min = D3DXVECTOR3(FLT_MAX, FLT_MAX, FLT_MAX);
	m_Max = D3DXVECTOR3(FLT_MIN, FLT_MIN, FLT_MIN);

	// Load the vertex and index arrays with the terrain data.
	for (j = 0; j < (m_terrainHeight - 1); j++) {
		for (i = 0; i < (m_terrainWidth - 1); i++) {
			// LINE 1
			// Upper left.
			positionX = (float)i * scale.x + position.x;
			positionZ = (float)(j + 1) * scale.z + position.z;

			vertices[index].position = D3DXVECTOR3(positionX, position.y, positionZ);
			vertices[index].color = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
			indices[index] = index;

			m_Min.x = min(m_Min.x, vertices[index].position.x);
			m_Min.y = min(m_Min.y, vertices[index].position.y);
			m_Min.z = min(m_Min.z, vertices[index].position.z);

			m_Max.x = max(m_Max.x, vertices[index].position.x);
			m_Max.y = max(m_Max.y, vertices[index].position.y);
			m_Max.z = max(m_Max.z, vertices[index].position.z);

			index++;

			// Upper right.
			positionX = (float)(i + 1) * scale.x + position.x;
			positionZ = (float)(j + 1) * scale.z + position.z;

			vertices[index].position = D3DXVECTOR3(positionX, position.y, positionZ);
			vertices[index].color = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
			indices[index] = index;
			index++;

			// LINE 2
			// Upper right.
			positionX = (float)(i + 1) * scale.x + position.x;
			positionZ = (float)(j + 1) * scale.z + position.z;

			vertices[index].position = D3DXVECTOR3(positionX, position.y, positionZ);
			vertices[index].color = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
			indices[index] = index;
			index++;

			// Bottom right.
			positionX = (float)(i + 1) * scale.x + position.x;
			positionZ = (float)j * scale.z + position.z;

			vertices[index].position = D3DXVECTOR3(positionX, position.y, positionZ);
			vertices[index].color = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
			indices[index] = index;
			index++;

			// LINE 3
			// Bottom right.
			positionX = (float)(i + 1) * scale.x + position.x;
			positionZ = (float)j * scale.z + position.z;

			vertices[index].position = D3DXVECTOR3(positionX, position.y, positionZ);
			vertices[index].color = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
			indices[index] = index;
			index++;

			// Bottom left.
			positionX = (float)i * scale.x + position.x;
			positionZ = (float)j * scale.z + position.z;

			vertices[index].position = D3DXVECTOR3(positionX, position.y, positionZ);
			vertices[index].color = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
			indices[index] = index;
			index++;

			// LINE 4
			// Bottom left.
			positionX = (float)i * scale.x + position.x;
			positionZ = (float)j * scale.z + position.z;

			vertices[index].position = D3DXVECTOR3(positionX, position.y, positionZ);
			vertices[index].color = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
			indices[index] = index;
			index++;

			// Upper left.
			positionX = (float)i * scale.x + position.x;
			positionZ = (float)(j + 1) * scale.z + position.z;

			vertices[index].position = D3DXVECTOR3(positionX, position.y, positionZ);
			vertices[index].color = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
			indices[index] = index;
			index++;
		}
	}

	// Set up the description of the static vertex buffer.
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

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result)) {
		return false;
	}

	// Set up the description of the static index buffer.
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

	// Release the arrays now that the buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}

void GridClass::Render(CameraClass* camera)
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix;
	unsigned int stride;
	unsigned int offset;

	m_D3D->GetWorldMatrix(worldMatrix);
	camera->GetViewMatrix(viewMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	m_D3D->GetWorldMatrix(worldMatrix);
	camera->GetViewMatrix(viewMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	RenderBuffers(m_D3D->GetDeviceContext());
	m_shader->Render(m_D3D->GetDeviceContext(), GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);
}

void GridClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
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

	// Set the type of primitive that should be rendered from this vertex buffer, in this case a line list.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
}