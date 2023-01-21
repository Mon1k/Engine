#include "Fountain.h"

Fountain::Fountain()
{
	m_particleList = 0;
	m_vertices = 0;
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
}


Fountain::Fountain(const Fountain& other)
{
}


Fountain::~Fountain()
{
}

bool Fountain::Initialize(D3DClass* d3dClass, std::wstring textureFilename)
{
	m_D3D = d3dClass;

	if (!LoadTextures(m_D3D->GetDevice(), textureFilename)) {
		return false;
	}

	// Initialize the particle system.
	if (!InitializeParticleSystem()) {
		return false;
	}

	// Create the buffers that will be used to render the particles with.
	if (!InitializeBuffers()) {
		return false;
	}

	return true;
}

void Fountain::Shutdown()
{
	ShutdownParticleSystem();

	ModelClass::Shutdown();
}

bool Fountain::Frame(float frameTime)
{
	bool result;

	// Release old particles.
	KillParticles();

	// Emit new particles.
	EmitParticles(frameTime);

	// Update the position of the particles.
	UpdateParticles(frameTime);

	// Update the dynamic vertex buffer with the new position of each particle.
	result = UpdateBuffers();
	if (!result) {
		return false;
	}

	return true;
}

void Fountain::RenderBuffers(ID3D11DeviceContext* deviceContext)
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

void Fountain::Render(CameraClass* camera)
{
	D3DXMATRIX viewMatrix, projectionMatrix;

	camera->GetViewMatrix(viewMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	if (m_isAlpha) {
		m_D3D->TurnOnAlphaBlending();
	}

	RenderBuffers(m_D3D->GetDeviceContext());

	ParticleShaderClass* shader = dynamic_cast<ParticleShaderClass*>(m_shader);
	shader->Render(m_D3D->GetDeviceContext(), GetIndexCount(), GetWorldMatrix(), viewMatrix, projectionMatrix, GetTexture());

	if (m_isAlpha) {
		m_D3D->TurnOffAlphaBlending();
	}
}

bool Fountain::InitializeParticleSystem()
{
	// Set the random deviation of where the particles can be located when emitted.
	m_particleDeviationX = 0.5f;
	m_particleDeviationY = 0.1f;
	m_particleDeviationZ = 2.0f;

	// Set the speed and speed variation of particles.
	m_particleVelocity = D3DXVECTOR3(0.2f, 2.0f, 0.2f);
	m_particleVelocityVariation = 0.2f;

	// Set the physical size of the particles.
	m_particleSize = 0.2f;

	// Set the number of particles to emit per second.
	m_particlesPerSecond = 250.0f;

	// Set the maximum number of particles allowed in the particle system.
	setMaxParticles(5000);

	// Initialize the current particle count to zero since none are emitted yet.
	m_currentParticleCount = 0;

	// Clear the initial accumulated time for the particle per second emission rate.
	m_accumulatedTime = 0.0f;

	m_maxPosition = D3DXVECTOR3(0.0f, 10.0f, 0.0f);
	m_minPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	m_color = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	return true;
}

void Fountain::setMaxParticles(int maxParticles)
{
	m_maxParticles = maxParticles;

	m_particleList = new ParticleType[m_maxParticles];

	for (int i = 0; i < m_maxParticles; i++) {
		m_particleList[i].active = false;
	}
}

void Fountain::ShutdownParticleSystem()
{
	// Release the particle list.
	if (m_particleList) {
		delete[] m_particleList;
		m_particleList = 0;
	}
}

bool Fountain::InitializeBuffers()
{
	unsigned long* indices;
	int i;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;


	// Set the maximum number of vertices in the vertex array.
	m_vertexCount = m_maxParticles * 6;

	// Set the maximum number of indices in the index array.
	m_indexCount = m_vertexCount;

	// Create the vertex array for the particles that will be rendered.
	m_vertices = new VertexType[m_vertexCount];
	if (!m_vertices) {
		return false;
	}

	// Create the index array.
	indices = new unsigned long[m_indexCount];
	if (!indices) {
		return false;
	}

	// Initialize vertex array to zeros at first.
	memset(m_vertices, 0, (sizeof(VertexType) * m_vertexCount));

	// Initialize the index array.
	for (i = 0; i < m_indexCount; i++) {
		indices[i] = i;
	}

	// Set up the description of the dynamic vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = m_vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now finally create the vertex buffer.
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

	// Release the index array since it is no longer needed.
	delete[] indices;
	indices = 0;

	return true;
}

void Fountain::EmitParticles(float frameTime)
{
	bool emitParticle, found;
	float positionX, positionY, positionZ, red, green, blue;
	D3DXVECTOR3 velocity, direction;
	int index, i, j;


	// Increment the frame time.
	m_accumulatedTime += frameTime;

	// Set emit particle to false for now.
	emitParticle = false;

	// Check if it is time to emit a new particle or not.
	if (m_accumulatedTime > (1000.0f / m_particlesPerSecond)) {
		m_accumulatedTime = 0.0f;
		emitParticle = true;
	}

	// If there are particles to emit then emit one per frame.
	if ((emitParticle == true) && (m_currentParticleCount < (m_maxParticles - 1))) {
		m_currentParticleCount++;

		// Now generate the randomized particle properties.
		positionX = position.x + (((float)rand() - (float)rand()) / RAND_MAX) * m_particleDeviationX;
		positionY = position.y + (((float)rand() - (float)rand()) / RAND_MAX) * m_particleDeviationY;
		positionZ = position.z + (((float)rand() - (float)rand()) / RAND_MAX) * m_particleDeviationZ;

		direction = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
		if (Random::randDouble(-1.0f, 1.0f) < 0.0f) {
			direction.x *= -1;
		}
		if (Random::randDouble(-1.0f, 1.0f) < 0.0f) {
			direction.z *= -1;
		}

		velocity.x = m_particleVelocity.x + (((float)rand() - (float)rand()) / RAND_MAX) * m_particleVelocityVariation;
		velocity.y = m_particleVelocity.y + (((float)rand() - (float)rand()) / RAND_MAX) * m_particleVelocityVariation;
		velocity.z = m_particleVelocity.z + (((float)rand() - (float)rand()) / RAND_MAX) * m_particleVelocityVariation;

		red = m_color.x ? m_color.x : (((float)rand() - (float)rand()) / RAND_MAX) + 0.5f;
		green = m_color.y ? m_color.y : (((float)rand() - (float)rand()) / RAND_MAX) + 0.5f;
		blue = m_color.z ? m_color.z : (((float)rand() - (float)rand()) / RAND_MAX) + 0.5f;

		// Now since the particles need to be rendered from back to front for blending we have to sort the particle array.
		// We will sort using Z depth so we need to find where in the list the particle should be inserted.
		index = 0;
		found = false;
		while (!found) {
			if ((m_particleList[index].active == false) || (m_particleList[index].positionZ < positionZ)) {
				found = true;
			} else {
				index++;
			}
		}

		// Now that we know the location to insert into we need to copy the array over by one position from the index to make room for the new particle.
		i = m_currentParticleCount;
		j = i - 1;

		while (i != index) {
			m_particleList[i].positionX = m_particleList[j].positionX;
			m_particleList[i].positionY = m_particleList[j].positionY;
			m_particleList[i].positionZ = m_particleList[j].positionZ;
			m_particleList[i].red = m_particleList[j].red;
			m_particleList[i].green = m_particleList[j].green;
			m_particleList[i].blue = m_particleList[j].blue;
			m_particleList[i].velocity = m_particleList[j].velocity;
			m_particleList[i].direction = m_particleList[j].direction;
			m_particleList[i].active = m_particleList[j].active;
			i--;
			j--;
		}

		// Now insert it into the particle array in the correct depth order.
		m_particleList[index].positionX = positionX;
		m_particleList[index].positionY = positionY;
		m_particleList[index].positionZ = positionZ;
		m_particleList[index].red = red;
		m_particleList[index].green = green;
		m_particleList[index].blue = blue;
		m_particleList[index].velocity = velocity;
		m_particleList[index].direction = direction;
		m_particleList[index].active = true;
	}

	CalcMinMax();
}

void Fountain::UpdateParticles(float frameTime)
{
	int i;

	// Each frame we update all the particles by making them move downwards using their position, velocity, and the frame time.
	for (i = 0; i < m_currentParticleCount; i++) {
		if (m_particleList[i].positionY > m_maxPosition.y) {
			m_particleList[i].direction.y *= -1;
		}
		m_particleList[i].positionX += (m_particleList[i].direction.x * m_particleList[i].velocity.x) * frameTime * 0.001f;
		m_particleList[i].positionY += (m_particleList[i].direction.y * m_particleList[i].velocity.y) * frameTime * 0.001f;
		m_particleList[i].positionZ += (m_particleList[i].direction.z * m_particleList[i].velocity.z) * frameTime * 0.001f;
	}
}

void Fountain::KillParticles()
{
	int i, j;


	// Kill all the particles that have gone below a certain height range.
	for (i = 0; i < m_maxParticles; i++) {
		if ((m_particleList[i].active == true) && (m_particleList[i].positionY < m_minPosition.y)) {
			m_particleList[i].active = false;
			m_currentParticleCount--;

			// Now shift all the live particles back up the array to erase the destroyed particle and keep the array sorted correctly.
			for (j = i; j < m_maxParticles - 1; j++) {
				m_particleList[j].positionX = m_particleList[j + 1].positionX;
				m_particleList[j].positionY = m_particleList[j + 1].positionY;
				m_particleList[j].positionZ = m_particleList[j + 1].positionZ;
				m_particleList[j].red = m_particleList[j + 1].red;
				m_particleList[j].green = m_particleList[j + 1].green;
				m_particleList[j].blue = m_particleList[j + 1].blue;
				m_particleList[j].velocity = m_particleList[j + 1].velocity;
				m_particleList[j].direction = m_particleList[j + 1].direction;
				m_particleList[j].active = m_particleList[j + 1].active;
			}
		}
	}
}

bool Fountain::UpdateBuffers()
{
	int index, i;
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* verticesPtr;


	// Initialize vertex array to zeros at first.
	memset(m_vertices, 0, (sizeof(VertexType) * m_vertexCount));

	// Now build the vertex array from the particle list array.  Each particle is a quad made out of two triangles.
	index = 0;

	for (i = 0; i < m_currentParticleCount; i++) {
		// Bottom left.
		m_vertices[index].position = D3DXVECTOR3(m_particleList[i].positionX - m_particleSize, m_particleList[i].positionY - m_particleSize, m_particleList[i].positionZ);
		m_vertices[index].texture = D3DXVECTOR2(0.0f, 1.0f);
		m_vertices[index].color = D3DXVECTOR4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;

		// Top left.
		m_vertices[index].position = D3DXVECTOR3(m_particleList[i].positionX - m_particleSize, m_particleList[i].positionY + m_particleSize, m_particleList[i].positionZ);
		m_vertices[index].texture = D3DXVECTOR2(0.0f, 0.0f);
		m_vertices[index].color = D3DXVECTOR4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;

		// Bottom right.
		m_vertices[index].position = D3DXVECTOR3(m_particleList[i].positionX + m_particleSize, m_particleList[i].positionY - m_particleSize, m_particleList[i].positionZ);
		m_vertices[index].texture = D3DXVECTOR2(1.0f, 1.0f);
		m_vertices[index].color = D3DXVECTOR4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;

		// Bottom right.
		m_vertices[index].position = D3DXVECTOR3(m_particleList[i].positionX + m_particleSize, m_particleList[i].positionY - m_particleSize, m_particleList[i].positionZ);
		m_vertices[index].texture = D3DXVECTOR2(1.0f, 1.0f);
		m_vertices[index].color = D3DXVECTOR4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;

		// Top left.
		m_vertices[index].position = D3DXVECTOR3(m_particleList[i].positionX - m_particleSize, m_particleList[i].positionY + m_particleSize, m_particleList[i].positionZ);
		m_vertices[index].texture = D3DXVECTOR2(0.0f, 0.0f);
		m_vertices[index].color = D3DXVECTOR4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;

		// Top right.
		m_vertices[index].position = D3DXVECTOR3(m_particleList[i].positionX + m_particleSize, m_particleList[i].positionY + m_particleSize, m_particleList[i].positionZ);
		m_vertices[index].texture = D3DXVECTOR2(1.0f, 0.0f);
		m_vertices[index].color = D3DXVECTOR4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;
	}

	// Lock the vertex buffer.
	result = m_D3D->GetDeviceContext()->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)) {
		return false;
	}

	// Get a pointer to the data in the vertex buffer.
	verticesPtr = (VertexType*)mappedResource.pData;

	// Copy the data into the vertex buffer.
	memcpy(verticesPtr, (void*)m_vertices, (sizeof(VertexType) * m_vertexCount));

	// Unlock the vertex buffer.
	m_D3D->GetDeviceContext()->Unmap(m_vertexBuffer, 0);

	return true;
}

void Fountain::CalcMinMax()
{
	m_Min = D3DXVECTOR3(FLT_MAX, FLT_MAX, FLT_MAX);
	m_Max = D3DXVECTOR3(FLT_MIN, FLT_MIN, FLT_MIN);

	for (int i = 0; i < m_maxParticles; i++) {
		if (!m_particleList[i].active) {
			continue;
		}

		if (m_particleList[i].positionX > m_Max.x) {
			m_Max.x = m_particleList[i].positionX;
		}
		if (m_particleList[i].positionY > m_Max.y) {
			m_Max.y = m_particleList[i].positionY;
		}
		if (m_particleList[i].positionZ > m_Max.z) {
			m_Max.z = m_particleList[i].positionZ;
		}
		if (m_particleList[i].positionX < m_Min.x) {
			m_Min.x = m_particleList[i].positionX;
		}
		if (m_particleList[i].positionY < m_Min.y) {
			m_Min.y = m_particleList[i].positionY;
		}
		if (m_particleList[i].positionZ < m_Min.z) {
			m_Min.z = m_particleList[i].positionZ;
		}
	}
}