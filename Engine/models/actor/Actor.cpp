#include "Actor.h"
#include "../../tool/random.h"
#include "../loader/FbxLoader.h"

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


	///
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
		D3DXVECTOR3 normal;
	};

	VertexType* vertices = new VertexType[m_vertexCount];
	VertexType* verticesPtr;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	///



	FbxLoader* loader = static_cast<FbxLoader*>(m_loader);
	loader->m_fbxMesh->advanceTime();

	for (size_t modelIndex = 0; modelIndex < loader->m_fbxMesh->m_modelVector.size(); ++modelIndex)
	{
		for (size_t meshIndex = 0; meshIndex < loader->m_fbxMesh->m_modelVector[modelIndex].meshVector.size(); ++meshIndex)
		{
			SingleFbxMesh::tMeshRec& meshRec = loader->m_fbxMesh->m_modelVector[modelIndex].meshVector[meshIndex];
			loader->m_fbxMesh->_loadBoneMatriceVector(meshRec);

			for (size_t vertexIndex = 0; vertexIndex < meshRec.subverticeVector.size(); vertexIndex++) {
				AbstractModel::ModelType vertex = m_model[vertexIndex];
				D3DXVECTOR3 position = D3DXVECTOR3(vertex.x, vertex.y, vertex.z), position2, position3;
				position3 = D3DXVECTOR3(0, 0, 0);

				for (int indice = 0; indice < 4; ++indice)
				{
					std::string buffer, t;

					if (meshRec.subverticeVector[vertexIndex].boneWeights == 0) {
						continue;
					}
					buffer = std::to_string(meshRec.subverticeVector[vertexIndex].boneWeights);
					t = buffer[indice];
					if (std::stoi(t) > buffer.size()) {
						continue;
					}
					float normalizedBoneWeight = std::stof(t);

					if (meshRec.subverticeVector[vertexIndex].boneIndices == 0) {
						continue;
					}
					buffer = std::to_string(meshRec.subverticeVector[vertexIndex].boneIndices);
					t = buffer[indice];
					if (std::stoi(t) > buffer.size()) {
						continue;
					}

					D3DXMATRIX boneMatrix = loader->m_fbxMesh->m_boneMatrixVector[std::stoi(t)];
					D3DXVec3TransformCoord(&position2, &position, &boneMatrix);
					position2 *= normalizedBoneWeight;
					position3 += position2;
				}

				vertices[vertexIndex].position = position3;
				vertices[vertexIndex].normal = D3DXVECTOR3(vertex.nx, vertex.ny, vertex.nz);
				vertices[vertexIndex].texture = D3DXVECTOR2(vertex.tu, vertex.tv);
			}
		}
	}

	m_D3D->GetDeviceContext()->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	verticesPtr = (VertexType*)mappedResource.pData;
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType)* m_vertexCount));

	m_D3D->GetDeviceContext()->Unmap(m_vertexBuffer, 0);

	return;


	
	
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
			/*D3DXVECTOR3 position = D3DXVECTOR3(vertex.x, vertex.y, vertex.z), position2;
			joints[i].animation[j].transform *= weight.bias;
			D3DXVec3TransformCoord(&position2, &position, &joints[i].animation[j].transform);*/

			D3DXMATRIX matrix;
			D3DXVECTOR3 position = D3DXVECTOR3(vertex.x, vertex.y, vertex.z);
			D3DXVECTOR3 position2;
			//matrix = joints[i].animation[j].transform * weight.bias;
			//D3DXVec3TransformCoord(&position2, &position, &joints[i].animation[j].transform);
			vertex.x += joints[i].animation[j].transform._41 * weight.bias;
			vertex.y += joints[i].animation[j].transform._42 * weight.bias;
			vertex.z += joints[i].animation[j].transform._43 * weight.bias;
			/*vertex.x += joints[i].animation[j].position.x;
			vertex.y += joints[i].animation[j].position.y;
			vertex.z += joints[i].animation[j].position.z;*/
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

	m_counter = 0;
}