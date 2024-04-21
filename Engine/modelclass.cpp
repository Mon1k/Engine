#include "modelclass.h"

#include "models/loader/DsLoader.h"
#include "models/loader/ObjLoader.h"
#include "models/loader/AssimpLoader.h"

ModelClass::ModelClass(): AbstractModel()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_TextureArray = 0;
	
	m_model = 0;
	m_ModelIndices.clear();

	m_shader = 0;
	m_isAlpha = false;
	m_isShadow = false;

	m_subsets = 0;

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


bool ModelClass::Initialize(D3DClass* d3dClass, char* modelFilename, std::vector<std::string> texturesFilename)
{
	bool result;

	m_D3D = d3dClass;

	position = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	m_rotation = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	if (!texturesFilename.empty()) {
		if (texturesFilename.size() > 1) {
			// Load the textures for this model.
			result = LoadTexturesArray(texturesFilename);
			if (!result) {
				return false;
			}
		}
		else if (texturesFilename[0].size() > 0) {
			result = LoadTextures(texturesFilename[0]);
			if (!result) {
				return false;
			}
		}
	}

	if (!LoadModel(modelFilename)) {
		return false;
	}

	InitializeBuffers();
	CalcMinMax();

	return true;
}

bool ModelClass::LoadModel(char* filename)
{
	m_path = std::string(filename);

	if (m_path.rfind(".ds") != std::string::npos || m_path.rfind(".txt") != std::string::npos) {
		DsLoader* loader = new DsLoader;
		return loader->load(filename, this);
	} else if (m_path.rfind(".obj") != std::string::npos) {
		ObjLoader* loader = new ObjLoader;
		return loader->load(filename, this);
	} else {
		AssimpLoader* loader = new AssimpLoader;
		return loader->load(filename, this);
	}

	return false;
}

void ModelClass::Shutdown()
{
	ReleaseTexture();
	ShutdownBuffers();
	ReleaseModel();
}

ID3D11ShaderResourceView* ModelClass::GetTexture()
{
	return m_TextureArray->GetTexture(0);
}

ID3D11ShaderResourceView* ModelClass::GetTexture(int index)
{
	return m_TextureArray->GetTexture(index);
}

ID3D11ShaderResourceView** ModelClass::GetTextureArray()
{
	return m_TextureArray->GetTextureArray();
}

bool ModelClass::InitializeBuffers()
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

		if (!m_ModelIndices.size()) {
			indices[i] = i;
		}
	}

	if (m_ModelIndices.size()) {
		for (i = 0; i < m_indexCount; i++) {
			indices[i] = m_ModelIndices[i];
		}
	}

	// Set up the description of the static vertex buffer.
    //vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    //vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
    result = m_D3D->GetDevice()->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
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
	result = m_D3D->GetDevice()->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
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
	
	if (m_subsets) {
		CalcMinMaxSubsets();
	}
}

void ModelClass::CalcMinMaxSubsets()
{
	D3DXVECTOR3 min, max;
	min = m_subsets->getMinPosition();
	max = m_subsets->getMaxPosition();

	m_Min.x = min(m_Min.x, min.x);
	m_Min.y = min(m_Min.y, min.y);
	m_Min.z = min(m_Min.z, min.z);
	m_Max.x = max(m_Max.x, max.x);
	m_Max.y = max(m_Max.y, max.y);
	m_Max.z = max(m_Max.z, max.z);
}

bool ModelClass::LoadTextures(std::string filename)
{
	bool result;

	// Create the texture array object.
	m_TextureArray = new TextureArrayClass;
	if (!m_TextureArray) {
		return false;
	}

	// Initialize the texture array object.
	std::wstring texture(filename.begin(), filename.end());
	result = m_TextureArray->Initialize(m_D3D->GetDevice(), &texture[0]);
	if (!result) {
		return false;
	}

	return true;
}

bool ModelClass::LoadTexturesArray(std::vector<std::string> filenames)
{
	bool result;

	// Create the texture array object.
	m_TextureArray = new TextureArrayClass;
	if (!m_TextureArray) {
		return false;
	}

	// Initialize the texture array object.
	std::wstring texture(filenames[0].begin(), filenames[0].end());
	result = m_TextureArray->Initialize(m_D3D->GetDevice(), &texture[0]);
	if (!result) {
		return false;
	}

	int size = filenames.size();
	if (size > 1) {
		for (int i = 1; i < size; i++) {
			texture = std::wstring(filenames[i].begin(), filenames[i].end());
			m_TextureArray->AddTexture(m_D3D->GetDevice(), &texture[0]);
		}
	}

	return true;
}

bool ModelClass::addTexture(std::string filename)
{
	std::wstring texture(filename.begin(), filename.end());
	m_TextureArray->AddTexture(m_D3D->GetDevice(), &texture[0]);

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
	if (m_shader && m_shader != nullptr) {
		m_shader->Shutdown();
		m_shader = nullptr;
	}

	if (m_model) {
		delete[] m_model;
		m_model = 0;
	}
	m_ModelIndices.clear();

	if (m_subsets) {
		m_subsets->Shutdown();
		m_subsets = 0;
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
	Render();

	if (m_shader) {
		D3DXMATRIX viewMatrix, projectionMatrix;

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

	if (m_subsets) {
		m_subsets->Render(camera);
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

D3DXMATRIX ModelClass::GetWorldMatrix()
{
	D3DXMATRIX scaleMatrix, translationMatrix, rotateMatrix, worldMatrix;

	m_D3D->GetWorldMatrix(worldMatrix);

	if (scale.x != 1.0f || scale.y != 1.0f || scale.z != 1.0f) {
		D3DXMatrixScaling(&scaleMatrix, scale.x, scale.y, scale.z);
		D3DXMatrixMultiply(&worldMatrix, &worldMatrix, &scaleMatrix);
	}

	if (m_rotation.x != 0.0f) {
		D3DXMatrixRotationX(&rotateMatrix, m_rotation.x);
		D3DXMatrixMultiply(&worldMatrix, &worldMatrix, &rotateMatrix);
	}

	if (m_rotation.y != 0.0f) {
		D3DXMatrixRotationY(&rotateMatrix, m_rotation.y);
		D3DXMatrixMultiply(&worldMatrix, &worldMatrix, &rotateMatrix);
	}

	if (m_rotation.z != 0.0f) {
		D3DXMatrixRotationZ(&rotateMatrix, m_rotation.z);
		D3DXMatrixMultiply(&worldMatrix, &worldMatrix, &rotateMatrix);
	}
	
	if (position.x != 0.0f || position.y != 0.0f || position.z != 0.0f) {
		D3DXMatrixTranslation(&translationMatrix, position.x, position.y, position.z);
		D3DXMatrixMultiply(&worldMatrix, &worldMatrix, &translationMatrix);
	}

	return worldMatrix;
}

void ModelClass::addLights(std::vector<LightClass*> lights)
{
	this->m_lights.clear();
	for (int i = 0; i < lights.size(); i++) {
		this->m_lights.push_back(lights[i]);
	}
}

void ModelClass::frame(CameraClass* camera, float time)
{

}

void ModelClass::addSubset(ModelClass* subset)
{
	if (!m_subsets) {
		m_subsets = new CompositeModel;
	}
	m_subsets->addChild(subset);
}