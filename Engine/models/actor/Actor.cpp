#include "Actor.h"
#include "../../tool/random.h"

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
	
	for (int vertexIndex = 0; vertexIndex < m_vertexCount; vertexIndex++) {
		AbstractModel::ModelType vertex = m_model[vertexIndex];
		std::vector<Actor::Joint> joints;
		Actor::Weight weight = m_weights[vertexIndex];

		/*for (int weightIndex = 0; weightIndex < m_weights.size(); weightIndex++) {
			if (m_weights[weightIndex].index == vertexIndex) {
				weight = m_weights[weightIndex];
				break;
			}
		}*/
		

		for (int i = 0; i < m_animations[m_currentAnimation].joints.size(); i++) {
			if (weight.joint == i) {
				joints.push_back(m_animations[m_currentAnimation].joints[i]);
			}
		}

		D3DXMATRIX transformation;
		D3DXMatrixIdentity(&transformation);
		for (int i = 0; i < joints.size(); i++) {
			//for (int j = 0; j < joints[i].animation.size(); j++) {
			int j = m_animations[m_currentAnimation].currentTime;
				//if (joints[i].animation[j].numFrame == m_animations[m_currentAnimation].currentTime) {
			//weight.bias = max(0, min(255, weight.bias * 255.0 + 0.5));
			weight.bias = 1;
			/*vertex.x += (joints[i].inverse._41 * joints[i].animation[j].transform._41) * weight.bias;
			vertex.y += (joints[i].inverse._42 * joints[i].animation[j].transform._42) * weight.bias;
			vertex.z += (joints[i].inverse._43 * joints[i].animation[j].transform._43) * weight.bias;*/
			//D3DXMatrixMultiply(&transformation, &joints[i].inverse, &joints[i].animation[j].transform);
			//D3DXVECTOR3 position = D3DXVECTOR3(vertex.x, vertex.y, vertex.z), position2;
			//D3DXVec3TransformCoord(&position2, &position, &joints[i].animation[j].transform);
			//position2 *= weight.bias;
			vertex.x += joints[i].animation[j].transform._41 * weight.bias;
			vertex.y += joints[i].animation[j].transform._42 * weight.bias;
			vertex.z += joints[i].animation[j].transform._43 * weight.bias;
			/*vertex.x += joints[i].inverse._41 * weight.bias;
			vertex.y += joints[i].inverse._42 * weight.bias;
			vertex.z += joints[i].inverse._43 * weight.bias;*/

			
					
					vertices[vertexIndex].position = D3DXVECTOR3(vertex.x, vertex.y, vertex.z);
					//vertices[vertexIndex].position = position2;
					vertices[vertexIndex].normal = D3DXVECTOR3(vertex.nx, vertex.ny, vertex.nz);
					vertices[vertexIndex].texture = D3DXVECTOR2(vertex.tu, vertex.tv);
					break;
				//}
			//}
		}
	}


	m_D3D->GetDeviceContext()->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	verticesPtr = (VertexType*)mappedResource.pData;
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * m_vertexCount));

	m_D3D->GetDeviceContext()->Unmap(m_vertexBuffer, 0);


	m_animations[m_currentAnimation].currentTime++;
	if (m_animations[m_currentAnimation].currentTime >= m_animations[m_currentAnimation].totalTime) {
		m_animations[m_currentAnimation].currentTime = 0;
	}

	delete[] vertices;
	vertices = 0;
}