#include "Actor.h"
#include "../../tool/random.h"
#include "../loader/FbxLoader.h"

Actor::Actor() : Model()
{
	m_counter = 0;
	m_counterTotal = 0;
	m_currentAnimation = 0;
}

void Actor::frame(CameraClass* camera, float time)
{
	//return;
	m_counter += time;
	m_counterTotal += time;
	// 1000 / 25 = 40 ms = 25 fps
	if (m_counter < 40) {
		return;
	}

	m_animations[m_currentAnimation].currentTime += m_counter / 1000.0f * m_animations[m_currentAnimation].tick;
	if (m_animations[m_currentAnimation].currentTime >= m_animations[m_currentAnimation].totalTime) {
		m_animations[m_currentAnimation].currentTime = 0;
	}

	float AnimationTimeSec = m_counterTotal / 1000.0f;
	float TimeInTicks = AnimationTimeSec * m_animations[m_currentAnimation].tick;
	float AnimationTimeTicks = fmod(TimeInTicks, m_animations[m_currentAnimation].totalTime);

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


	for (size_t i = 0; i < m_BoneInfo.size(); i++) {
		Actor::BoneInfo bone = m_BoneInfo[i];
		Actor::BoneInfo parentBone;

		for (size_t j = 0; j < m_BoneInfo.size(); j++) {
			if (m_BoneInfo[j].name == bone.parent) {
				parentBone = m_BoneInfo[j];
				break;
			}
		}

		if (parentBone.boneId == 0) {
			continue;
		}

		D3DXVECTOR3 out;
		D3DXMATRIX nodeTransformation, translation, scale, rotation;
		nodeTransformation = bone.transformation;

		for (size_t j = 0; j < m_animations[m_currentAnimation].joints.size(); j++) {
			Actor::Joint joint = m_animations[m_currentAnimation].joints[j];
			if (joint.name == bone.name) {
				int positionIndex = 0;
				for (size_t k = 0; k < joint.animation.size() - 1; k++) {
					if (AnimationTimeTicks < joint.animation[k + 1].time) {
						positionIndex = k;
						break;
					}
				}

				float t1 = joint.animation[positionIndex].time;
				float t2 = joint.animation[positionIndex + 1].time;
				float factor = (AnimationTimeTicks - t1) / (t2 - t1);
				D3DXVECTOR3 start = joint.animation[positionIndex].position;
				D3DXVECTOR3 end = joint.animation[positionIndex + 1].position;
				out = start + factor * (end - start);
				D3DXMatrixIdentity(&translation);
				D3DXMatrixTranslation(&translation, out.x, out.y, out.z);


				start = joint.animation[positionIndex].scaling;
				end = joint.animation[positionIndex + 1].scaling;
				out = start + factor * (end - start);
				D3DXMatrixIdentity(&scale);
				D3DXMatrixScaling(&scale, out.x, out.y, out.z);

				// later add scale and rotation

				nodeTransformation = /*scale * */translation;
				break;
			}
		}

		D3DXMATRIX globalTransformation;
		globalTransformation = parentBone.globalTansformation * nodeTransformation;

		m_BoneInfo[i].globalTansformation = globalTransformation;
		m_BoneInfo[i].FinalTransformation = m_animations[m_currentAnimation].globalInverseTransformation * globalTransformation;// *bone.OffsetMatrix;
	}


	/*std::vector<D3DXMATRIX> transforms;
	transforms.resize(m_BoneInfo.size());
	for (size_t i = 0; i < m_BoneInfo.size(); i++) {
		transforms[i] = m_BoneInfo[i].FinalTransformation;
	}*/


	for (size_t i = 0; i < m_weights.size(); i++) {
		AbstractModel::ModelType vertex = m_model[i];

		D3DXMATRIX boneTransform;
		D3DXMatrixIdentity(&boneTransform);
		for (int j = 0; j < 4; j++) {
			boneTransform += m_BoneInfo[m_weights[i].BoneIDs[j]].FinalTransformation;// *m_weights[i].Weights[j];
		}

		D3DXVECTOR3 position, from = D3DXVECTOR3(vertex.x, vertex.y, vertex.z);
		D3DXVec3TransformCoord(&position, &from, &boneTransform);
		vertices[i].position = position;
		vertices[i].normal = D3DXVECTOR3(vertex.nx, vertex.ny, vertex.nz);
		vertices[i].texture = D3DXVECTOR2(vertex.tu, vertex.tv);
	}

	int t = 1;
	
	/*for (int boneIndex = 0; boneIndex < m_weights.size(); boneIndex++) {
		int vertexIndex = boneIndex;
		AbstractModel::ModelType vertex = m_model[vertexIndex];

		for (int i = 0; i < m_animations[m_currentAnimation].joints.size(); i++) {
			Actor::Joint joint = m_animations[m_currentAnimation].joints[i];
			if (joint.name == m_weights[boneIndex].name) {
				D3DXVECTOR3 diff = D3DXVECTOR3(0, 0, 0);
				for (int j = 0; j < joint.animation.size(); j++) {
					if (AnimationTimeSec < joint.animation[j].time) {
						diff = joint.animation[j].position;
						break;
					}
				}
				vertex.x += diff.x;
				vertex.y += diff.y;
				vertex.z += diff.z;
				break;
			}
		}

		vertices[vertexIndex].position = D3DXVECTOR3(vertex.x, vertex.y, vertex.z);
		vertices[vertexIndex].normal = D3DXVECTOR3(vertex.nx, vertex.ny, vertex.nz);
		vertices[vertexIndex].texture = D3DXVECTOR2(vertex.tu, vertex.tv);
	}*/

	/*for (int vertexIndex = 0; vertexIndex < m_vertexCount; vertexIndex++) {
		AbstractModel::ModelType vertex = m_model[vertexIndex];
		D3DXVECTOR3 position = D3DXVECTOR3(vertex.x, vertex.y, vertex.z);







		vertices[vertexIndex].position = D3DXVECTOR3(vertex.x, vertex.y, vertex.z);
		vertices[vertexIndex].normal = D3DXVECTOR3(vertex.nx, vertex.ny, vertex.nz);
		vertices[vertexIndex].texture = D3DXVECTOR2(vertex.tu, vertex.tv);

		/*std::vector<Actor::Joint> joints;
		Actor::Weight weight = m_weights[vertexIndex];
		

		/*for (int weightIndex = 0; weightIndex < m_weights.size(); weightIndex++) {
			if (m_weights[weightIndex].index == vertexIndex) {
				weight = m_weights[weightIndex];
				break;
			}
		}*/
		
		

		/*for (int i = 0; i < m_animations[m_currentAnimation].joints.size(); i++) {
			if (weight.joint == i) {
				joints.push_back(m_animations[m_currentAnimation].joints[i]);
			}
		}*/
		/*joints.push_back(m_animations[m_currentAnimation].joints[weight.joint]);

		D3DXMATRIX transformation;
		D3DXMatrixIdentity(&transformation);
		for (int i = 0; i < joints.size(); i++) {
			//for (int j = 0; j < joints[i].animation.size(); j++) {
			int j = m_animations[m_currentAnimation].currentTime;
			D3DXMATRIX matrix;
			D3DXVECTOR3 position2;
			//joints[i].animation[j].position = position;

			
				//if (joints[i].animation[j].numFrame == m_animations[m_currentAnimation].currentTime) {
			//weight.bias = max(0, min(255, weight.bias * 255.0 + 0.5));
			//weight.bias = 1;
			/*vertex.x += (joints[i].inverse._41 * joints[i].animation[j].transform._41) * weight.bias;
			vertex.y += (joints[i].inverse._42 * joints[i].animation[j].transform._42) * weight.bias;
			vertex.z += (joints[i].inverse._43 * joints[i].animation[j].transform._43) * weight.bias;*/
			//D3DXMatrixMultiply(&transformation, &joints[i].inverse, &joints[i].animation[j].transform);
			/*D3DXVECTOR3 position = D3DXVECTOR3(vertex.x, vertex.y, vertex.z), position2;
			joints[i].animation[j].transform *= weight.bias;
			D3DXVec3TransformCoord(&position2, &position, &joints[i].animation[j].transform);*/

			
			//matrix = joints[i].animation[j].transform * weight.bias;
			//D3DXVec3TransformCoord(&position2, &position, &joints[i].animation[j].transform);
			/*vertex.x += joints[i].animation[j].transform._41 * weight.bias;
			vertex.y += joints[i].animation[j].transform._42 * weight.bias;
			vertex.z += joints[i].animation[j].transform._43 * weight.bias;*/
			/*vertex.x += (m_animations[m_currentAnimation].joints[joints[i].parentId].animation[j].transform._41 * joints[i].animation[j].transform._41) * weight.bias;
			vertex.y += (m_animations[m_currentAnimation].joints[joints[i].parentId].animation[j].transform._42 * joints[i].animation[j].transform._42) * weight.bias;
			vertex.z += (m_animations[m_currentAnimation].joints[joints[i].parentId].animation[j].transform._43 * joints[i].animation[j].transform._43) * weight.bias;*/
			/*vertex.x += joints[i].animation[j].position.x;
			vertex.y += joints[i].animation[j].position.y;
			vertex.z += joints[i].animation[j].position.z;*/
			/*vertex.x += joints[i].inverse._41 * weight.bias;
			vertex.y += joints[i].inverse._42 * weight.bias;
			vertex.z += joints[i].inverse._43 * weight.bias;*/

			
					
			/*vertices[vertexIndex].position = D3DXVECTOR3(vertex.x, vertex.y, vertex.z);
			//vertices[vertexIndex].position = position2;
			vertices[vertexIndex].normal = D3DXVECTOR3(vertex.nx, vertex.ny, vertex.nz);
			vertices[vertexIndex].texture = D3DXVECTOR2(vertex.tu, vertex.tv);
			break;
				//}
			//}
		}*/
	//}


	m_D3D->GetDeviceContext()->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	verticesPtr = (VertexType*)mappedResource.pData;
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * m_vertexCount));

	m_D3D->GetDeviceContext()->Unmap(m_vertexBuffer, 0);


	delete[] vertices;
	vertices = 0;

	m_counter = 0;
}