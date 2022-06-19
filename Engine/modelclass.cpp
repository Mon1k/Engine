#include "modelclass.h"


ModelClass::ModelClass(): AbstractModel()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_TextureArray = 0;
	m_model = 0;
	m_shader = 0;
	m_isAlpha = false;

	position = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	m_rotation = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
}


ModelClass::ModelClass(const ModelClass& other)
{
}


ModelClass::~ModelClass()
{
}


bool ModelClass::Initialize(D3DClass* d3dClass, char* modelFilename, std::vector<std::wstring> texturesFilename)
{
	bool result;

	m_D3D = d3dClass;

	if (!LoadModel(modelFilename)) {
		return false;
	}

	// Initialize the vertex and index buffers.
	result = InitializeBuffers(m_D3D->GetDevice());
	if (!result) {
		return false;
	}
	CalcMinMax();

	if (texturesFilename.size() > 1) {
		// Load the textures for this model.
		result = LoadTexturesArray(m_D3D->GetDevice(), texturesFilename);
		if (!result) {
			return false;
		}
	} else if (texturesFilename[0].size() > 0) {
		result = LoadTextures(m_D3D->GetDevice(), texturesFilename[0]);
		if (!result) {
			return false;
		}
	}


	return true;
}


void ModelClass::Shutdown()
{
	ReleaseTexture();
	ShutdownBuffers();
	ReleaseModel();
}

ID3D11ShaderResourceView* ModelClass::GetTexture()
{
	return m_TextureArray->GetTextureArray()[0];
}

ID3D11ShaderResourceView* ModelClass::GetTexture(int index)
{
	return m_TextureArray->GetTextureArray()[index];
}

ID3D11ShaderResourceView** ModelClass::GetTextureArray()
{
	return m_TextureArray->GetTextureArray();
}

ID3D11ShaderResourceView** ModelClass::GetTextureArray(int size)
{
	return m_TextureArray->GetTextureArray(size);
}

bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;

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
		vertices[i].texture = D3DXVECTOR2(m_model[i].tu, m_model[i].tv);
		vertices[i].normal = D3DXVECTOR3(m_model[i].nx, m_model[i].ny, m_model[i].nz);

		indices[i] = i;
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

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;

	delete [] indices;
	indices = 0;

	return true;
}

void ModelClass::CalcMinMax()
{
	m_Min = D3DXVECTOR3(FLT_MAX, FLT_MAX, FLT_MAX);
	m_Max = D3DXVECTOR3(FLT_MIN, FLT_MIN, FLT_MIN);

	for (int i = 0; i < m_vertexCount; i++) {
		if (m_model[i].x > m_Max.x) {
			m_Max.x = m_model[i].x;
		}
		if (m_model[i].y > m_Max.y) {
			m_Max.y = m_model[i].y;
		}
		if (m_model[i].z > m_Max.z) {
			m_Max.z = m_model[i].z;
		}
		if (m_model[i].x < m_Min.x) {
			m_Min.x = m_model[i].x;
		}
		if (m_model[i].y < m_Min.y) {
			m_Min.y = m_model[i].y;
		}
		if (m_model[i].z < m_Min.z) {
			m_Min.z = m_model[i].z;
		}
	}
}

bool ModelClass::LoadTextures(ID3D11Device* device, std::wstring filename)
{
	bool result;

	// Create the texture array object.
	m_TextureArray = new TextureArrayClass;
	if (!m_TextureArray) {
		return false;
	}

	// Initialize the texture array object.
	result = m_TextureArray->Initialize(device, &filename[0]);
	if (!result) {
		return false;
	}

	return true;
}

bool ModelClass::LoadTexturesArray(ID3D11Device* device, std::vector<std::wstring> filenames)
{
	bool result;

	// Create the texture array object.
	m_TextureArray = new TextureArrayClass;
	if (!m_TextureArray) {
		return false;
	}

	// Initialize the texture array object.
	result = m_TextureArray->Initialize(device, &filenames[0][0]);
	if (!result) {
		return false;
	}

	int size = filenames.size();
	if (size > 1) {
		for (int i = 1; i < size; i++) {
			m_TextureArray->AddTexture(device, &filenames[i][0], i);
		}
	}

	return true;
}

void ModelClass::ReleaseTexture()
{
	// Release the texture array object.
	if (m_TextureArray) {
		m_TextureArray->Shutdown();
		delete m_TextureArray;
		m_TextureArray = 0;
	}
}

void ModelClass::ReleaseModel()
{
	if (m_shader) {
		m_shader->Shutdown();
		delete m_shader;
		m_shader = 0;
	}

	if (m_model) {
		delete[] m_model;
		m_model = 0;
	}
}

void ModelClass::ShutdownBuffers()
{
	// Release the index buffer.
	if (m_indexBuffer) {
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// Release the vertex buffer.
	if (m_vertexBuffer) {
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}
}


void ModelClass::Render(CameraClass* camera)
{
	D3DXMATRIX viewMatrix, projectionMatrix;

	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(m_D3D->GetDeviceContext());
	
	if (m_shader) {
		camera->GetViewMatrix(viewMatrix);
		m_D3D->GetProjectionMatrix(projectionMatrix);

		if (m_isAlpha) {
			m_D3D->TurnOnAlphaBlending();
		}

		m_shader->Render(m_D3D->GetDeviceContext(), GetIndexCount(), GetWorldMatrix(), viewMatrix, projectionMatrix,
			GetTextureArray(), camera->GetPosition());

		if (m_isAlpha) {
			m_D3D->TurnOffAlphaBlending();
		}
	}
}

void ModelClass::Render()
{
	RenderBuffers(m_D3D->GetDeviceContext());
}

void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
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

void ModelClass::GetBoundingBox(D3DXVECTOR3& position, D3DXVECTOR3& size)
{
	size.x = m_Max.x - m_Min.x;
	size.y = m_Max.y - m_Min.y;
	size.z = m_Max.z - m_Min.z;

	position.x = m_Min.x + size.x / 2;
	position.y = m_Min.y + size.y / 2;
	position.z = m_Min.z + size.z / 2;
}

void ModelClass::SetPosition(D3DXVECTOR3 _position)
{
	D3DXVECTOR3 delta;
	delta.x = _position.x - position.x;
	delta.y = _position.y - position.y;
	delta.z = _position.z - position.z;
	position = _position;

	m_Max.x += delta.x;
	m_Max.y += delta.y;
	m_Max.z += delta.z;
	m_Min.x += delta.x;
	m_Min.y += delta.y;
	m_Min.z += delta.z;
}

/**
* @todo - Неверно по Y масштабирует
*/
void ModelClass::SetScale(D3DXVECTOR3 _scale)
{
	D3DXVECTOR3 delta;
	delta.x = _scale.x / scale.x;
	delta.y = _scale.y / scale.y;
	delta.z = _scale.z / scale.z;
	scale = _scale;

	m_Max.x *= delta.x;
	m_Max.y *= delta.y;
	m_Max.z *= delta.z;
}

D3DXMATRIX ModelClass::GetWorldMatrix()
{
	D3DXMATRIX scaleMatrix, translationMatrix, rotateMatrix, worldMatrix;

	m_D3D->GetWorldMatrix(worldMatrix);

	if (scale.x != 1.0f || scale.y != 1.0f || scale.z != 1.0f) {
		D3DXMatrixScaling(&scaleMatrix, scale.x, scale.y, scale.z);
		D3DXMatrixMultiply(&worldMatrix, &worldMatrix, &scaleMatrix);
	}

	if (m_rotation.y != 0.0f) {
		D3DXMatrixRotationY(&rotateMatrix, m_rotation.y);
		D3DXMatrixMultiply(&worldMatrix, &worldMatrix, &rotateMatrix);
	}
	
	if (position.x != 0.0f || position.y != 0.0f || position.z != 0.0f) {
		D3DXMatrixTranslation(&translationMatrix, position.x, position.y, position.z);
		D3DXMatrixMultiply(&worldMatrix, &worldMatrix, &translationMatrix);
	}

	return worldMatrix;
}