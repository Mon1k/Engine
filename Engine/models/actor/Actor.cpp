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
	
	// 1000 / 25 = 40 ms = 25 fps
	if (m_counter < 40) {
		return;
	}

	m_counterTotal += fmod(m_counter / 40 * m_animations[m_currentAnimation].tick, m_animations[m_currentAnimation].totalTime);

	float AnimationTimeSec = m_counterTotal / 1000.0f;
	float TimeInTicks = AnimationTimeSec * m_animations[m_currentAnimation].tick;
	float AnimationTimeTicks = fmod(TimeInTicks, m_animations[m_currentAnimation].totalTime);

	m_animations[m_currentAnimation].currentTime = m_counterTotal;
	//m_animations[m_currentAnimation].currentTime += m_counter / 1000.0f * m_animations[m_currentAnimation].tick;
	if (m_animations[m_currentAnimation].currentTime >= m_animations[m_currentAnimation].maxTime) {
		m_animations[m_currentAnimation].currentTime = 0;
		m_counterTotal = 0;
	}
	AnimationTimeTicks = m_counterTotal;	

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


	// update animation matrix for all nodes
	D3DXVECTOR3 Z = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	for (size_t j = 0; j < m_animations[m_currentAnimation].nodes.size(); j++) {
		Actor::AnimationNode* aniNode = &m_animations[m_currentAnimation].nodes[j];

		if (aniNode->frames.front().time >= AnimationTimeTicks) {
			D3DXVECTOR3 P = aniNode->frames.front().position;
			D3DXVECTOR3 S = aniNode->frames.front().scaling;
			D3DXQUATERNION R = aniNode->frames.front().rotation;

			D3DXMatrixAffineTransformation(&aniNode->transform, 1.0f, &Z, &R, &P);
			//D3DXMatrixTransformation(&aniNode->transform, &Z, NULL, &S, &Z, &R, &P);
		}
		else if (aniNode->frames.back().time <= AnimationTimeSec) {
			D3DXVECTOR3 P = aniNode->frames.back().position;
			D3DXVECTOR3 S = aniNode->frames.back().scaling;
			D3DXQUATERNION R = aniNode->frames.back().rotation;

			D3DXMatrixAffineTransformation(&aniNode->transform, 1.0f, &Z, &R, &P);
			//D3DXMatrixTransformation(&aniNode->transform, &Z, NULL, &S, &Z, &R, &P);
		}
		else {
			for (size_t k = 0; k < aniNode->frames.size() - 1; k++) {
				Actor::KeyFrame* key = &aniNode->frames[k];
				Actor::KeyFrame* keyNext = &aniNode->frames[k + 1];
				if (AnimationTimeTicks >= key->time && AnimationTimeTicks <= keyNext->time) {
					float lerp = (AnimationTimeTicks - key->time) / (keyNext->time - key->time);

					D3DXVECTOR3 s1 = key->scaling;
					D3DXVECTOR3 s2 = keyNext->scaling;

					D3DXVECTOR3 p1 = key->position;
					D3DXVECTOR3 p2 = keyNext->position;

					D3DXQUATERNION r1 = key->rotation;
					D3DXQUATERNION r2 = keyNext->rotation;

					D3DXVECTOR3 S, P;
					D3DXQUATERNION R;

					D3DXVec3Lerp(&S, &s1, &s2, lerp);
					D3DXVec3Lerp(&P, &p1, &p2, lerp);
					D3DXQuaternionSlerp(&R, &r1, &r2, lerp);

					D3DXMatrixAffineTransformation(&aniNode->transform, 1.0f, &Z, &R, &P);
					//D3DXMatrixTransformation(&aniNode->transform, &Z, NULL, &S, &Z, &R, &P);
					break;
				}
			}
		}
	}

	// update node transform with animation matrix
	for (size_t i = 0; i < m_NodeInfo.size(); i++) {
		Actor::NodeInfo* node = m_NodeInfo[i];
		D3DXMATRIX transform = node->localTransformation;

		// search concret node for get matrix
		for (size_t j = 0; j < m_animations[m_currentAnimation].nodes.size(); j++) {
			Actor::AnimationNode* aniNode = &m_animations[m_currentAnimation].nodes[j];
			if (aniNode->name == node->name) {
				transform = aniNode->transform;
				break;
			}
		}

		node->globalTransformation = transform;
		if (node->parent) {
			node->globalTransformation = transform * node->parent->globalTransformation;
		}
	}

	// collect bones with final matrix
	std::vector<Actor::BoneInfo*> bones;
	for (size_t i = 0; i < m_Mesh.size(); i++) {
		Actor::HierarchyMesh* mesh = m_Mesh[i];
		for (size_t j = 0; j < mesh->bones.size(); j++) {
			Actor::BoneInfo* bone = &mesh->bones[j];
			bone->t = bone->OffsetMatrix * bone->node->globalTransformation;
			//D3DXMatrixTranspose(&bone->t, &bone->t); // !!! dont - invert and weight not right
			bones.push_back(bone);
		}
	}

	// inside vertex shader must
	for (size_t i = 0; i < m_weights.size(); i++) {
		AbstractModel::ModelType vertex = m_model[i];
		float finalWeight = 1 - (m_weights[i].Weights[0] + m_weights[i].Weights[1] + m_weights[i].Weights[2]);

		Actor::BoneInfo* bone = bones[m_weights[i].BoneIDs[0]];
		D3DXMATRIX boneTransform = bone->t * m_weights[i].Weights[0];

		bone = bones[m_weights[i].BoneIDs[1]];
		boneTransform += bone->t * m_weights[i].Weights[1];
		bone = bones[m_weights[i].BoneIDs[2]];
		boneTransform += bone->t * m_weights[i].Weights[2];
		bone = bones[m_weights[i].BoneIDs[3]];
		boneTransform += bone->t * finalWeight;

		D3DXVECTOR4 position4, from4 = D3DXVECTOR4(vertex.x, vertex.y, vertex.z, 1.0f);
		D3DXVec4Transform(&position4, &from4, &boneTransform);

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
