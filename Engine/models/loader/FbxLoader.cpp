#include "FbxLoader.h"
#include "../actor/Actor.h"

#include "opendbx/src/ofbx.h"

#include "assimp/Importer.hpp"
#include "assimp/ProgressHandler.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

bool FbxLoader::load(char* filename, ModelClass* model)
{
	Actor* actor = dynamic_cast<Actor*>(model);

	Assimp::Importer importer;
	m_Scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_OptimizeMeshes | aiProcess_FlipUVs | aiProcess_FlipWindingOrder | aiProcess_MakeLeftHanded | aiProcess_GenNormals | aiProcess_CalcTangentSpace);

	////
	// 0 - x, 1 - y, 2 - z
	int32_t upAxis = 1;
	int32_t upAxisSign = 1;
	int32_t frontAxis = 2;
	int32_t frontAxisSign = 1;
	int32_t coordAxis = 0;
	int32_t coordAxisSign = 1;

	// values will only be populated if key exists
	bool reqMetadataExists = true;
	reqMetadataExists &= m_Scene->mMetaData->Get<int32_t>("UpAxis", upAxis);
	reqMetadataExists &= m_Scene->mMetaData->Get<int32_t>("UpAxisSign", upAxisSign);
	reqMetadataExists &= m_Scene->mMetaData->Get<int32_t>("FrontAxis", frontAxis);
	reqMetadataExists &= m_Scene->mMetaData->Get<int32_t>("FrontAxisSign", frontAxisSign);
	reqMetadataExists &= m_Scene->mMetaData->Get<int32_t>("CoordAxis", coordAxis);
	reqMetadataExists &= m_Scene->mMetaData->Get<int32_t>("CoordAxisSign", coordAxisSign);
	if (reqMetadataExists) {
		aiVector3D uV;
		aiVector3D fV;
		aiVector3D rV;
		uV[upAxis] = upAxisSign;
		fV[frontAxis] = frontAxisSign;
		rV[coordAxis] = coordAxisSign;
		aiMatrix4x4 orientationCorrection = aiMatrix4x4(
			rV.x, rV.y, rV.z, 0.0f,
			uV.x, uV.y, uV.z, 0.0f,
			fV.x, fV.y, fV.z, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);

		m_Scene->mRootNode->mTransformation *= orientationCorrection;
	}
	////


	int vertexCount = 0;
	for (size_t i = 0; i < m_Scene->mNumMeshes; ++i) {
		aiMesh* mesh = m_Scene->mMeshes[i];
		vertexCount += mesh->mNumVertices;	
	}

	model->setVertexCount(vertexCount);

	std::vector<Actor::Weight> weights;
	weights.resize(vertexCount);

	int index = 0;
	int baseVertex = 0;
	const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);


	// process mesh
	for (size_t i = 0; i < m_Scene->mNumMeshes; ++i) {
		aiMesh* mesh = m_Scene->mMeshes[i];

		// fill vertices
		for (size_t j = 0; j < mesh->mNumVertices; ++j) {
			model->m_model[index].x = mesh->mVertices[j].x;
			model->m_model[index].y = mesh->mVertices[j].y;
			model->m_model[index].z = mesh->mVertices[j].z;

			if (mesh->mNormals) {
				model->m_model[index].nx = mesh->mNormals[j].x;
				model->m_model[index].ny = mesh->mNormals[j].y;
				model->m_model[index].nz = mesh->mNormals[j].z;
			}
			else {
				model->m_model[index].nx = 0.0f;
				model->m_model[index].ny = 1.0f;
				model->m_model[index].nz = 0.0f;
			}

			const aiVector3D& pTexCoord = mesh->HasTextureCoords(0) ? mesh->mTextureCoords[0][j] : Zero3D;
			model->m_model[index].tu = pTexCoord.x;
			model->m_model[index].tv = pTexCoord.y;

			index++;
		}

		// fill indices
		for (size_t j = 0; j < mesh->mNumFaces; j++) {
			aiFace face = mesh->mFaces[j];
			for (size_t k = 0; k < face.mNumIndices; k++) {
				model->m_ModelIndices.push_back(face.mIndices[k]);
				model->m_ModelIndices.back() += baseVertex;
			}
		}

		// create mesh
		Actor::HierarchyMesh* hiearchyMesh = new Actor::HierarchyMesh;
		hiearchyMesh->name = mesh->mName.C_Str();
		hiearchyMesh->baseVertex = baseVertex;
		actor->m_Mesh.push_back(hiearchyMesh);

		baseVertex += mesh->mNumVertices;
	}
	model->setIndexCount(model->m_ModelIndices.size());

	// process node
	createTreeNode(m_Scene->mRootNode, actor, nullptr, 0);
	sort(actor->m_NodeInfo.begin(), actor->m_NodeInfo.end(),
		[](const Actor::NodeInfo* a, const Actor::NodeInfo* b)->bool {
			return a->depth < b->depth; 
		});

	// process skin
	for (size_t i = 0; i < m_Scene->mNumMeshes; ++i) {
		aiMesh* mesh = m_Scene->mMeshes[i];
		if (!mesh->HasBones()) {
			continue;
		}

		Actor::HierarchyMesh* hiearchyMesh = actor->m_Mesh[i];
		for (size_t j = 0; j < mesh->mNumBones; j++) {
			const aiBone* pBone = mesh->mBones[j];

			Actor::BoneInfo bi;
			bi.name = pBone->mName.C_Str();
			bi.OffsetMatrix = D3DXMATRIX(pBone->mOffsetMatrix[0]);
			D3DXMatrixTranspose(&bi.OffsetMatrix, &bi.OffsetMatrix);

			bi.node = FindNode(actor, bi);
			hiearchyMesh->bones.push_back(bi);

			for (size_t k = 0; k < pBone->mNumWeights; k++) {
				const aiVertexWeight vw = pBone->mWeights[k];
				unsigned int GlobalVertexID = hiearchyMesh->baseVertex + vw.mVertexId;

				weights[GlobalVertexID].AddBoneData(j, vw.mWeight);
			}
		}
	}

	// process animation
	std::vector<Actor::Animation> animations;
	animations.resize(m_Scene->mNumAnimations);
	for (size_t i = 0; i < m_Scene->mNumAnimations; ++i) {
		const aiAnimation* assimp_anim = m_Scene->mAnimations[i];

		float tickPerSecond = assimp_anim->mTicksPerSecond;
		if (tickPerSecond < 0.001f) {
			tickPerSecond = 25.0f;
		}

		animations[i].name = assimp_anim->mName.C_Str();
		animations[i].maxTime = 0;
		animations[i].totalTime = assimp_anim->mDuration;
		animations[i].tick = tickPerSecond;
		animations[i].currentTime = 0;

		for (size_t j = 0; j < assimp_anim->mNumChannels; ++j) {
			const aiNodeAnim* assimp_node_anim = assimp_anim->mChannels[j];
			Actor::AnimationNode animationNode;

			animationNode.name = assimp_node_anim->mNodeName.C_Str();
			int keyCount = max(assimp_node_anim->mNumPositionKeys, assimp_node_anim->mNumRotationKeys);
			keyCount = max(keyCount, assimp_node_anim->mNumScalingKeys);

			for (size_t idx = 0; idx < keyCount; ++idx) {
				Actor::KeyFrame key;
				key.position = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
				key.scaling = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
				key.rotation = D3DXQUATERNION(0.0f, 0.0f, 0.0f, 0.0f);
				key.time = 0.0f;

				if (assimp_node_anim->mNumPositionKeys > idx) {
					const aiVectorKey anim_key_position = assimp_node_anim->mPositionKeys[idx];

					key.time = (float)anim_key_position.mTime;
					key.position.x = anim_key_position.mValue.x;
					key.position.y = anim_key_position.mValue.y;
					key.position.z = anim_key_position.mValue.z;
				}

				if (assimp_node_anim->mNumRotationKeys > idx) {
					const aiQuatKey anim_key_rotate = assimp_node_anim->mRotationKeys[idx];

					key.time = (float)anim_key_rotate.mTime;
					key.rotation.x = anim_key_rotate.mValue.x;
					key.rotation.y = anim_key_rotate.mValue.y;
					key.rotation.z = anim_key_rotate.mValue.z;
					key.rotation.w = anim_key_rotate.mValue.w;
				}

				if (assimp_node_anim->mNumScalingKeys > idx) {
					const aiVectorKey anim_key_scale = assimp_node_anim->mScalingKeys[idx];

					key.time = (float)anim_key_scale.mTime;
					key.scaling.x = anim_key_scale.mValue.x;
					key.scaling.y = anim_key_scale.mValue.y;
					key.scaling.z = anim_key_scale.mValue.z;
				}

				animations[i].maxTime = max(animations[i].maxTime, key.time);
				animationNode.frames.push_back(key);
			}

			animations[i].nodes.push_back(animationNode);
		}
	}

	actor->m_animations = animations;
	actor->m_weights = weights;

	return true;
}

void FbxLoader::createTreeNode(aiNode* node, Actor* actor, Actor::NodeInfo* parent, int depth)
{
	Actor::NodeInfo* internalNode = new Actor::NodeInfo;
	internalNode->name = node->mName.C_Str();
	internalNode->localTransformation = D3DXMATRIX(node->mTransformation[0]);
	D3DXMatrixTranspose(&internalNode->localTransformation, &internalNode->localTransformation);
	internalNode->globalTransformation = internalNode->localTransformation;
	internalNode->setParent(parent);
	internalNode->depth = depth;

	actor->m_NodeInfo.push_back(internalNode);

	if (node->mNumMeshes > 0) {
		Actor::HierarchyMesh* mesh = actor->m_Mesh[node->mMeshes[0]];
		mesh->node = internalNode;
	}

	for (size_t i = 0; i < node->mNumChildren; i++) {
		createTreeNode(node->mChildren[i], actor, internalNode, depth + 1);
	}
}

Actor::NodeInfo* FbxLoader::FindNode(Actor* actor, Actor::BoneInfo bone)
{
	Actor::NodeInfo* findNode = nullptr;

	for (size_t i = 0; i < actor->m_NodeInfo.size(); i++) {
		if (actor->m_NodeInfo[i]->name == bone.name) {
			findNode = actor->m_NodeInfo[i];
			break;
		}
	}

	return findNode;
}