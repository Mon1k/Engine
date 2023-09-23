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

		if (parentBone.boneId < 0) {
			continue;
		}

		D3DXMATRIX nodeTransformation, translation, scaling, rotation;
		nodeTransformation = bone.globalTansformation;
		//D3DXMatrixIdentity(&nodeTransformation);

		int found = 0;
		for (size_t j = 0; j < m_animations[m_currentAnimation].joints.size(); j++) {
			Actor::Joint joint = m_animations[m_currentAnimation].joints[j];
			if (joint.name == bone.name) {
				int positionIndex = 0;
				D3DXVECTOR3 out, start, end;
				float t1, t2, factor;


				if (joint.position.size() > 1) {
					for (size_t k = 0; k < joint.position.size() - 1; k++) {
						if (AnimationTimeTicks < joint.position[k + 1].time) {
							positionIndex = k;
							break;
						}
					}
					t1 = joint.position[positionIndex].time;
					t2 = joint.position[positionIndex + 1].time;
					factor = (AnimationTimeTicks - t1) / (t2 - t1);
					start = joint.position[positionIndex].position;
					end = joint.position[positionIndex + 1].position;
					out = start + factor * (end - start);
				}
				else {
					out = joint.position[0].position;
				}
				D3DXMatrixIdentity(&translation);
				D3DXMatrixTranslation(&translation, out.x, out.y, out.z);
				D3DXVECTOR3 translationV = out;

				if (joint.scaling.size() > 1) {
					positionIndex = 0;
					for (size_t k = 0; k < joint.scaling.size() - 1; k++) {
						if (AnimationTimeTicks < joint.scaling[k + 1].time) {
							positionIndex = k;
							break;
						}
					}
					t1 = joint.scaling[positionIndex].time;
					t2 = joint.scaling[positionIndex + 1].time;
					factor = (AnimationTimeTicks - t1) / (t2 - t1);
					start = joint.scaling[positionIndex].scaling;
					end = joint.scaling[positionIndex + 1].scaling;
					out = start + factor * (end - start);
				}
				else {
					out = joint.scaling[0].scaling;
				}
				D3DXMatrixIdentity(&scaling);
				D3DXMatrixScaling(&scaling, out.x, out.y, out.z);

				D3DXQUATERNION quat;
				if (joint.rotation.size() > 1) {
					positionIndex = 0;
					for (size_t k = 0; k < joint.rotation.size() - 1; k++) {
						if (AnimationTimeTicks < joint.rotation[k + 1].time) {
							positionIndex = k;
							break;
						}
					}
					t1 = joint.rotation[positionIndex].time;
					t2 = joint.rotation[positionIndex + 1].time;
					factor = (AnimationTimeTicks - t1) / (t2 - t1);
					D3DXQUATERNION start4 = joint.rotation[positionIndex].rotation;
					D3DXQUATERNION end4 = joint.rotation[positionIndex + 1].rotation;
					D3DXQuaternionIdentity(&quat);
					D3DXQuaternionSlerp(&quat, &start4, &end4, factor);
					//D3DXQuaternionNormalize(&quat, &quat);
				}
				else {
					quat = joint.rotation[0].rotation;
				}
				D3DXMatrixIdentity(&rotation);
				D3DXMatrixRotationQuaternion(&rotation, &quat);

				nodeTransformation = scaling * rotation * translation;
				//nodeTransformation = translation * rotation * scaling;
				/*nodeTransformation = rotation;
				nodeTransformation._41 = translationV.x;
				nodeTransformation._42 = translationV.y;
				nodeTransformation._43 = translationV.z;*/

				m_BoneInfo[i].transformation = nodeTransformation;
				nodeTransformation = CalculateGlobalTransform(bone.name, nodeTransformation);
				m_BoneInfo[i].globalTansformation = nodeTransformation;
				found = 1;
				break;
			}
		}

		D3DXMATRIX globalTransformation = nodeTransformation;// parentBone.globalTansformation* nodeTransformation;

		//m_BoneInfo[i].globalTansformation = globalTransformation;
		if (found != 0) {
			m_BoneInfo[i].FinalTransformation = m_animations[m_currentAnimation].globalInverseTransformation * globalTransformation *bone.OffsetMatrix;
		}
	}

	for (size_t i = 0; i < m_weights.size(); i++) {
		AbstractModel::ModelType vertex = m_model[i];

		D3DXMATRIX boneTransform;
		D3DXMatrixIdentity(&boneTransform);
		boneTransform = m_BoneInfo[m_weights[i].BoneIDs[0]].FinalTransformation *m_weights[i].Weights[0];
		for (int j = 1; j < 4; j++) {
			boneTransform += m_BoneInfo[m_weights[i].BoneIDs[j]].FinalTransformation *m_weights[i].Weights[j];
		}

		D3DXVECTOR3 position, from = D3DXVECTOR3(vertex.x, vertex.y, vertex.z);
		D3DXVECTOR4 position4, from4 = D3DXVECTOR4(vertex.x, vertex.y, vertex.z, 1.0f);
		D3DXVec4Transform(&position4, &from4, &boneTransform);

		/*D3DXVECTOR4 position4, out4, from4 = D3DXVECTOR4(vertex.x, vertex.y, vertex.z, 1.0f);
		D3DXVec4Transform(&out4, &from4, &m_BoneInfo[m_weights[i].BoneIDs[0]].FinalTransformation);
		position4 = out4 * m_weights[i].Weights[0];
		for (int j = 1; j < 4; j++) {
			D3DXVec4Transform(&out4, &from4, &m_BoneInfo[m_weights[i].BoneIDs[j]].FinalTransformation);
			position4 += out4 * m_weights[i].Weights[j];
		}*/


		vertices[i].position = D3DXVECTOR3(position4.x, position4.y, position4.z);
		vertices[i].normal = D3DXVECTOR3(vertex.nx, vertex.ny, vertex.nz);
		vertices[i].texture = D3DXVECTOR2(vertex.tu, vertex.tv);
	}

	m_D3D->GetDeviceContext()->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	verticesPtr = (VertexType*)mappedResource.pData;
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * m_vertexCount));

	m_D3D->GetDeviceContext()->Unmap(m_vertexBuffer, 0);


	delete[] vertices;
	vertices = 0;

	m_counter = 0;
}

D3DXMATRIX Actor::CalculateGlobalTransform(std::string boneName, D3DXMATRIX transform)
{
	Actor::BoneInfo bone;
	for (size_t i = 0; i < m_BoneInfo.size(); i++) {
		if (m_BoneInfo[i].name == boneName) {
			bone = m_BoneInfo[i];
			break;
		}
	}

	bone.transformation = transform;
	bone.globalTansformation = bone.transformation;
	std::string name = bone.parent;
	do {
		Actor::BoneInfo parentBone;
		for (size_t i = 0; i < m_BoneInfo.size(); i++) {
			if (m_BoneInfo[i].name == name) {
				parentBone = m_BoneInfo[i];
				break;
			}
		}

		if (parentBone.boneId == -1) {
			break;
		}

		bone.globalTansformation = parentBone.transformation * bone.globalTansformation;
		name = parentBone.parent;
	} while (true);

	return bone.globalTansformation;
}