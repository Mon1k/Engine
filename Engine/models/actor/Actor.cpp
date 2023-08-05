#include "Actor.h"

Actor::Actor() : Model()
{
	m_counter = 0;
	m_currentAnimation = 0;
}

void Actor::frame(CameraClass* camera, float time)
{
	m_counter += time;
	if (m_counter < 50) {
		return;
	}
	m_counter = 0;

	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
		D3DXVECTOR3 normal;
	};

	VertexType* vertices = new VertexType[m_vertexCount];
	VertexType* verticesPtr;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	int index = 0;

	
	for (int vertexIndex = 0; vertexIndex < m_vertexCount; vertexIndex++) {
		AbstractModel::ModelType vertex = m_model[vertexIndex];
		std::vector<Actor::Joint> joints;
		Actor::Weight weight;

		for (int weightIndex = 0; weightIndex < m_weights.size(); weightIndex++) {
			if (m_weights[weightIndex].index == vertexIndex) {
				weight = m_weights[weightIndex];
				break;
			}
		}
		

		for (int i = 0; i < m_animations[m_currentAnimation].joints.size(); i++) {
			if (weight.joint == i) {
				joints.push_back(m_animations[m_currentAnimation].joints[i]);
			}
		}

		D3DXMATRIX transformation;
		D3DXMatrixIdentity(&transformation);
		for (int i = 0; i < joints.size(); i++) {
			for (int j = 0; j < joints[i].animation.size(); j++) {
				if (joints[i].animation[j].numFrame == m_animations[m_currentAnimation].currentTime) {
					vertex.x += joints[i].animation[j].transform._41 * weight.bias;
					vertex.y += joints[i].animation[j].transform._42 * weight.bias;
					vertex.z += joints[i].animation[j].transform._43 * weight.bias;
					
					vertices[index].position = D3DXVECTOR3(vertex.x, vertex.y, vertex.z);
					vertices[index].normal = D3DXVECTOR3(vertex.nx, vertex.ny, vertex.nz);
					vertices[index].texture = D3DXVECTOR2(vertex.tu, vertex.tv);
				}
			}
		}
		index++;
	}


	m_D3D->GetDeviceContext()->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	verticesPtr = (VertexType*)mappedResource.pData;
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * m_vertexCount));

	m_D3D->GetDeviceContext()->Unmap(m_vertexBuffer, 0);


	m_animations[m_currentAnimation].currentTime++;
	if (m_animations[m_currentAnimation].currentTime > m_animations[m_currentAnimation].totalTime) {
		m_animations[m_currentAnimation].currentTime = 0;
	}
}