#include "foliageclass.h"
#include "../tool/random.h"

FoliageClass::FoliageClass(): ModelClass()
{
	m_foliageArray = 0;
	m_Instances = 0;

	m_instanceBuffer = 0;
}


FoliageClass::FoliageClass(const FoliageClass& other)
{
}


FoliageClass::~FoliageClass()
{
}


bool FoliageClass::Initialize(D3DClass* d3dClass, std::string textureFilename, int fCount)
{
	bool result;

	m_D3D = d3dClass;

	// Set the foliage count.
	m_foliageCount = fCount;

	// Load the texture for this model.
	result = LoadTextures(m_D3D->GetDevice(), textureFilename);
	if (!result) {
		return false;
	}

	// Set the initial wind rotation and direction.
	m_windRotation = 0.0f;
	m_windDirection = 1;
	m_windPower = 0.05f;
	m_counter = 0;

	m_shader = new FoliageShaderClass;
	m_shader->Initialize(m_D3D->GetDevice());

	return true;
}


void FoliageClass::Shutdown()
{
	if (m_foliageArray) {
		delete[] m_foliageArray;
		m_foliageArray = 0;
	}

	ModelClass::Shutdown();
}


void FoliageClass::Render(CameraClass* camera)
{
	D3DXMATRIX viewMatrix, projectionMatrix;
	
	camera->GetViewMatrix(viewMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	m_D3D->TurnOnAlphaBlending();

	RenderBuffers(m_D3D->GetDeviceContext());
	m_shader->Render(m_D3D->GetDeviceContext(), m_vertexCount, m_instanceCount, viewMatrix, projectionMatrix, GetTexture());

	m_D3D->TurnOffAlphaBlending();
}

void FoliageClass::frame(CameraClass* camera, float time)
{
	m_counter += time;
	if (m_counter < 10) {
		return;
	}
	m_counter = 0;


	D3DXMATRIX rotateMatrix, translationMatrix, rotateMatrix2, finalMatrix;
	D3DXVECTOR3 modelPosition, cameraPosition;
	int i;
	double angle;
	float rotation, windRotation;
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	InstanceType* instancesPtr;

	cameraPosition = camera->GetPosition();

	// Update the wind rotation.
	if (m_windDirection == 1) {
		m_windRotation += m_windPower;
		if (m_windRotation > 10.0f) {
			m_windDirection = 2;
		}
	}
	else {
		m_windRotation -= m_windPower;
		if (m_windRotation < -10.0f) {
			m_windDirection = 1;
		}
	}

	// Load the instance buffer with the updated locations.
	for (i = 0; i < m_foliageCount; i++) {
		// Get the position of this piece of foliage.
		modelPosition.x = m_foliageArray[i].x;
		modelPosition.y = m_foliageArray[i].y;
		modelPosition.z = m_foliageArray[i].z;

		// Calculate the rotation that needs to be applied to the billboard model to face the current camera position using the arc tangent function.
		angle = atan2(modelPosition.x - cameraPosition.x, modelPosition.z - cameraPosition.z) * (180.0 / D3DX_PI);

		// Convert rotation into radians.
		rotation = (float)angle * 0.0174532925f;

		// Setup the X rotation of the billboard.
		D3DXMatrixRotationY(&rotateMatrix, rotation);

		// Get the wind rotation for the foliage.
		windRotation = m_windRotation * 0.0174532925f;

		// Setup the wind rotation.
		D3DXMatrixRotationX(&rotateMatrix2, windRotation);

		// Setup the translation matrix.
		D3DXMatrixTranslation(&translationMatrix, modelPosition.x, modelPosition.y, modelPosition.z);

		// Create the final matrix and store it in the instances array.
		D3DXMatrixMultiply(&finalMatrix, &rotateMatrix, &rotateMatrix2);
		D3DXMatrixMultiply(&m_Instances[i].matrix, &finalMatrix, &translationMatrix);
	}

	// Lock the instance buffer so it can be written to.
	result = m_D3D->GetDeviceContext()->Map(m_instanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)) {
		return;
	}

	// Get a pointer to the data in the instance buffer.
	instancesPtr = (InstanceType*)mappedResource.pData;

	// Copy the instances array into the instance buffer.
	memcpy(instancesPtr, (void*)m_Instances, (sizeof(InstanceType) * m_foliageCount));

	// Unlock the instance buffer.
	m_D3D->GetDeviceContext()->Unmap(m_instanceBuffer, 0);
}

int FoliageClass::GetInstanceCount()
{
	return m_instanceCount;
}

bool FoliageClass::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	D3D11_BUFFER_DESC vertexBufferDesc, instanceBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, instanceData;
	HRESULT result;
	int i;
	D3DXMATRIX matrix;

	// Set the number of vertices in the vertex array.
	m_vertexCount = 6;

	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];
	if (!vertices) {
		return false;
	}

	// Load the vertex array with data.
	vertices[0].position = D3DXVECTOR3(0.0f, 0.0f, 0.0f);  // Bottom left.
	vertices[0].texture = D3DXVECTOR2(0.0f, 1.0f);

	vertices[1].position = D3DXVECTOR3(0.0f, 1.0f * scale.y, 0.0f);  // Top left.
	vertices[1].texture = D3DXVECTOR2(0.0f, 0.0f);

	vertices[2].position = D3DXVECTOR3(1.0f * scale.x, 0.0f, 0.0f);  // Bottom right.
	vertices[2].texture = D3DXVECTOR2(1.0f, 1.0f);

	vertices[3].position = D3DXVECTOR3(1.0f * scale.x, 0.0f, 0.0f);  // Bottom right.
	vertices[3].texture = D3DXVECTOR2(1.0f, 1.0f);

	vertices[4].position = D3DXVECTOR3(0.0f, 1.0f * scale.y, 0.0f);  // Top left.
	vertices[4].texture = D3DXVECTOR2(0.0f, 0.0f);

	vertices[5].position = D3DXVECTOR3(1.0f * scale.x, 1.0f * scale.y, 0.0f);  // Top right.
	vertices[5].texture = D3DXVECTOR2(1.0f, 0.0f);

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

	// Release the array now that the vertex buffer has been created and loaded.
	delete[] vertices;
	vertices = 0;

	// Set the number of instances in the array.
	m_instanceCount = m_foliageCount;

	// Create the instance array.
	m_Instances = new InstanceType[m_instanceCount];
	if (!m_Instances) {
		return false;
	}

	// Setup an initial matrix.
	D3DXMatrixIdentity(&matrix);

	// Load the instance array with data.
	for (i = 0; i < m_instanceCount; i++) {
		m_Instances[i].matrix = matrix;
		m_Instances[i].color = D3DXVECTOR3(m_foliageArray[i].r, m_foliageArray[i].g, m_foliageArray[i].b);
	}

	// Set up the description of the instance buffer.
	instanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	instanceBufferDesc.ByteWidth = sizeof(InstanceType) * m_instanceCount;
	instanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	instanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	instanceBufferDesc.MiscFlags = 0;
	instanceBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the instance data.
	instanceData.pSysMem = m_Instances;
	instanceData.SysMemPitch = 0;
	instanceData.SysMemSlicePitch = 0;

	// Create the instance buffer.
	result = device->CreateBuffer(&instanceBufferDesc, &instanceData, &m_instanceBuffer);
	if (FAILED(result)) {
		return false;
	}

	return true;
}


void FoliageClass::ShutdownBuffers()
{
	// Release the instance buffer.
	if (m_instanceBuffer) {
		m_instanceBuffer->Release();
		m_instanceBuffer = 0;
	}

	// Release the instance array.
	if (m_Instances) {
		delete[] m_Instances;
		m_Instances = 0;
	}
}


void FoliageClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int strides[2];
	unsigned int offsets[2];
	ID3D11Buffer* bufferPointers[2];


	// Set the buffer strides.
	strides[0] = sizeof(VertexType);
	strides[1] = sizeof(InstanceType);

	// Set the buffer offsets.
	offsets[0] = 0;
	offsets[1] = 0;

	// Set the array of pointers to the vertex and instance buffers.
	bufferPointers[0] = m_vertexBuffer;
	bufferPointers[1] = m_instanceBuffer;

	// Set the vertex and instance buffers to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 2, bufferPointers, strides, offsets);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

bool FoliageClass::GeneratePositions(D3DXVECTOR3 min, D3DXVECTOR3 max)
{
	int i;
	float red, green;
	float height, x, z;

	// Create an array to store all the foliage information.
	m_foliageArray = new FoliageType[m_foliageCount];
	if (!m_foliageArray) {
		return false;
	}

	// Seed the random generator.
	srand((int)time(NULL));

	// Set random positions and random colors for each piece of foliage.
	for (i = 0; i < m_foliageCount; i++) {
		x = Random::randDouble(min.x, max.x);
		z = Random::randDouble(min.z, max.z);
		height = -0.1f;

		m_foliageArray[i].x = x;
		m_foliageArray[i].y = height;
		m_foliageArray[i].z = z;

		red = ((float)rand() / (float)(RAND_MAX)) * 1.0f;
		green = ((float)rand() / (float)(RAND_MAX)) * 1.0f;

		m_foliageArray[i].r = red + 1.0f;
		m_foliageArray[i].g = green + 0.5f;
		m_foliageArray[i].b = 0.0f;
	}

	return InitializeBuffers(m_D3D->GetDevice());
}

bool FoliageClass::GeneratePositionsFromTerrain(D3DXVECTOR3 min, D3DXVECTOR3 max, TerrainClass* terrain)
{
	int i;
	float red, green;
	int attempt, maxAttempt = 5;
	float height, x, z;

	// Create an array to store all the foliage information.
	m_foliageArray = new FoliageType[m_foliageCount];
	if (!m_foliageArray) {
		return false;
	}

	// Seed the random generator.
	srand((int)time(NULL));

	// Set random positions and random colors for each piece of foliage.
	for (i = 0; i < m_foliageCount; i++) {
		attempt = 0;
		do {
			x = Random::randDouble(min.x, max.x);
			z = Random::randDouble(min.z, max.z);
			
			terrain->getQuadTree()->GetHeightAtPosition(x, z, height);
			if (height < min.y || height > max.y) {
				attempt++;
				continue;
			}

			m_foliageArray[i].x = x;
			m_foliageArray[i].y = height - 0.3f;
			m_foliageArray[i].z = z;
			break;
		} while (attempt < maxAttempt);

		red = ((float)rand() / (float)(RAND_MAX)) * 1.0f;
		green = ((float)rand() / (float)(RAND_MAX)) * 1.0f;

		m_foliageArray[i].r = red + 1.0f;
		m_foliageArray[i].g = green + 0.5f;
		m_foliageArray[i].b = 0.0f;
	}

	return InitializeBuffers(m_D3D->GetDevice());
}

bool FoliageClass::GeneratePositionsFromTerrainWithMap(TerrainClass* terrain, std::string map)
{
	int i;
	float red, green;
	float height, x, z;

	// Create an array to store all the foliage information.
	m_foliageArray = new FoliageType[m_foliageCount];
	if (!m_foliageArray) {
		return false;
	}

	// Seed the random generator.
	srand((int)time(NULL));

	// Set random positions and random colors for each piece of foliage.
	for (i = 0; i < m_foliageCount; i++) {
		x = 1;
		z = 2;
		terrain->getQuadTree()->GetHeightAtPosition(x, z, height);
			
		m_foliageArray[i].x = x;
		m_foliageArray[i].y = height - 0.3f;
		m_foliageArray[i].z = z;

		red = ((float)rand() / (float)(RAND_MAX)) * 1.0f;
		green = ((float)rand() / (float)(RAND_MAX)) * 1.0f;

		m_foliageArray[i].r = red + 1.0f;
		m_foliageArray[i].g = green + 0.5f;
		m_foliageArray[i].b = 0.0f;
	}

	return InitializeBuffers(m_D3D->GetDevice());
}