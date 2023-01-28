#pragma once

#include <d3d11.h>
#include <d3dx10math.h>

#include "../../modelclass.h"
#include "../../textures/particleshaderclass.h"

class AbstractParticle : public ModelClass
{
private:
	struct ParticleType
	{
		float positionX, positionY, positionZ;
		float red, green, blue;
		D3DXVECTOR3 velocity;
		D3DXVECTOR3 direction;
		bool active;
	};

	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
		D3DXVECTOR4 color;
	};

public:
	AbstractParticle() {
		m_particleList = 0;
		m_vertices = 0;
		m_vertexBuffer = 0;
		m_indexBuffer = 0;
	};

	virtual bool Initialize(D3DClass*, std::wstring) = 0;

	virtual void Shutdown() {
		ShutdownParticleSystem();
		ModelClass::Shutdown();
	};

	virtual bool Frame(float frameTime) {
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
	};
	
	virtual void RenderBuffers(ID3D11DeviceContext* deviceContext) {
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
	};
	
	virtual void Render(CameraClass* camera) {
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

	void setDeviation(D3DXVECTOR3 deviation) {
		m_particleDeviationX = deviation.x;
		m_particleDeviationY = deviation.y;
		m_particleDeviationZ = deviation.z;
	};

	void setColor(D3DXVECTOR3 color) {
		m_color = color;
	};

	void setMaxParticles(int maxParticles) {
		m_maxParticles = maxParticles;

		m_particleList = new ParticleType[m_maxParticles];

		for (int i = 0; i < m_maxParticles; i++) {
			m_particleList[i].active = false;
		}
	};

	void setParticlesPerSecond(float timeout) {
		m_particlesPerSecond = timeout;
	};

	void setParticlesSize(float size) {
		m_particleSize = size;
	};

	void setMaxPosition(D3DXVECTOR3 position) {
		m_maxPosition = position;
	};

	void setMinPosition(D3DXVECTOR3 position) {
		m_minPosition = position;
	};

	void setVelocity(D3DXVECTOR3 velocity) {
		m_particleVelocity = velocity;
	};

protected:
	virtual bool InitializeParticleSystem() = 0;
	virtual void EmitParticles(float) = 0;
	virtual void UpdateParticles(float) = 0;
	virtual void KillParticles() = 0;

	void ShutdownParticleSystem()
	{
		// Release the particle list.
		if (m_particleList) {
			delete[] m_particleList;
			m_particleList = 0;
		}
	};

	bool InitializeBuffers() {
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
	};

	void CalcMinMax() {
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
	};

	virtual bool UpdateBuffers() {
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
	};

protected:
	float m_particleDeviationX, m_particleDeviationY, m_particleDeviationZ;
	float m_particleVelocityVariation;
	float m_particleSize, m_particlesPerSecond;
	int m_maxParticles;
	D3DXVECTOR3 m_maxPosition;
	D3DXVECTOR3 m_minPosition;
	D3DXVECTOR3 m_particleVelocity;
	D3DXVECTOR3 m_color;

	int m_currentParticleCount;
	float m_accumulatedTime;

	ParticleType* m_particleList;
	VertexType* m_vertices;
};