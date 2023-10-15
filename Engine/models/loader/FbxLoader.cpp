#include "FbxLoader.h"
#include "../actor/Actor.h"

#include "opendbx/src/ofbx.h"

#include "assimp/Importer.hpp"
#include "assimp/ProgressHandler.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

bool FbxLoader::load(char* filename, ModelClass* model)
{
	m_model = model;
	Actor* actor = dynamic_cast<Actor*>(model);

	Assimp::Importer importer;
	//importer.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS, aiComponent_CAMERAS | aiComponent_LIGHTS);

	/*const aiScene* scene = importer.ReadFile(
		filename, aiProcess_ConvertToLeftHanded | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals |
		aiProcess_JoinIdenticalVertices | aiProcess_ImproveCacheLocality |
		aiProcess_LimitBoneWeights | aiProcess_RemoveRedundantMaterials |
		aiProcess_SplitLargeMeshes | aiProcess_Triangulate | aiProcess_GenUVCoords |
		aiProcess_SortByPType | aiProcess_FindDegenerates | aiProcess_FindInvalidData |
		aiProcess_FindInstances | aiProcess_ValidateDataStructure | aiProcess_OptimizeMeshes);*/
	/*m_Scene = importer.ReadFile(filename, aiProcess_GenSmoothNormals | aiProcess_SplitLargeMeshes | aiProcess_Triangulate | aiProcess_MakeLeftHanded | aiProcess_SortByPType | aiProcess_CalcTangentSpace | aiProcess_FindDegenerates | aiProcess_GenUVCoords | aiProcess_TransformUVCoords | aiProcess_PopulateArmatureData);*/
	m_Scene = importer.ReadFile(filename, aiProcess_FlipWindingOrder | aiProcess_MakeLeftHanded | aiProcess_GenNormals | aiProcess_CalcTangentSpace);

	int vertexCount = 0, indexCount = 0;
	for (size_t i = 0; i < m_Scene->mNumMeshes; ++i) {
		aiMesh* mesh = m_Scene->mMeshes[i];
		vertexCount += mesh->mNumVertices;	
	}
	indexCount = vertexCount;

	model->setVertexCount(vertexCount);
	model->setIndexCount(indexCount);

	std::vector<Actor::Weight> weights;
	weights.resize(vertexCount);

	int index = 0, index2 = 0;
	int baseVertex = 0;
	const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);


	// process mesh
	for (size_t i = 0; i < m_Scene->mNumMeshes; ++i) {
		aiMesh* mesh = m_Scene->mMeshes[i];
		for (size_t j = 0; j < mesh->mNumVertices; ++j) {
			m_model->m_model[index].x = mesh->mVertices[j].x;
			m_model->m_model[index].y = mesh->mVertices[j].y;
			m_model->m_model[index].z = mesh->mVertices[j].z;

			if (mesh->mNormals) {
				m_model->m_model[index].nx = mesh->mNormals[j].x;
				m_model->m_model[index].ny = mesh->mNormals[j].y;
				m_model->m_model[index].nz = mesh->mNormals[j].z;
			}
			else {
				m_model->m_model[index].nx = 0.0f;
				m_model->m_model[index].ny = 1.0f;
				m_model->m_model[index].nz = 0.0f;
			}

			const aiVector3D& pTexCoord = mesh->HasTextureCoords(0) ? mesh->mTextureCoords[0][j] : Zero3D;
			m_model->m_model[index].tu = pTexCoord.x;
			m_model->m_model[index].tv = pTexCoord.y;

			index++;
		}

		Actor::HierarchyMesh* hiearchyMesh = new Actor::HierarchyMesh;
		hiearchyMesh->name = mesh->mName.C_Str();
		hiearchyMesh->baseVertex = baseVertex;
		actor->m_Mesh.push_back(hiearchyMesh);

		baseVertex += mesh->mNumVertices;
	}

	// process node
	createTreeNode(m_Scene->mRootNode, actor, nullptr, 0);
	sort(actor->m_NodeInfo.begin(), actor->m_NodeInfo.end(),
		[](const Actor::NodeInfo* a, const Actor::NodeInfo* b)->bool {	return a->depth < b->depth; });

	// process skin
	for (size_t i = 0; i < m_Scene->mNumMeshes; ++i) {
		aiMesh* mesh = m_Scene->mMeshes[i];
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
				weights[GlobalVertexID].name = pBone->mName.C_Str();
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

				if (assimp_node_anim->mNumPositionKeys > idx) {
					const aiVectorKey anim_key_position = assimp_node_anim->mPositionKeys[idx];

					key.time = anim_key_position.mTime;
					key.position.x = anim_key_position.mValue.x;
					key.position.y = anim_key_position.mValue.y;
					key.position.z = anim_key_position.mValue.z;
				}

				if (assimp_node_anim->mNumRotationKeys > idx) {
					const aiQuatKey anim_key_rotate = assimp_node_anim->mRotationKeys[idx];

					key.time = anim_key_rotate.mTime;
					key.rotation.x = anim_key_rotate.mValue.x;
					key.rotation.y = anim_key_rotate.mValue.y;
					key.rotation.z = anim_key_rotate.mValue.z;
					key.rotation.w = anim_key_rotate.mValue.w;
				}

				if (assimp_node_anim->mNumScalingKeys > idx) {
					const aiVectorKey anim_key_scale = assimp_node_anim->mScalingKeys[idx];

					key.time = anim_key_scale.mTime;
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

int FbxLoader::GetBoneId(std::string boneName)
{
	int BoneIndex = 0;

	if (m_BoneNameToIndexMap.find(boneName) == m_BoneNameToIndexMap.end()) {
		// Allocate an index for a new bone
		BoneIndex = (int)m_BoneNameToIndexMap.size();
		m_BoneNameToIndexMap[boneName] = BoneIndex;
	}
	else {
		BoneIndex = m_BoneNameToIndexMap[boneName];
	}

	return BoneIndex;
}

D3DXMATRIX FbxLoader::toD3DXMATRIX(aiMatrix4x4 matrix)
{
	/*return D3DXMATRIX(
		(float)matrix.a1, (float)matrix.b1, (float)matrix.c1, (float)matrix.d1,
		(float)matrix.a2, (float)matrix.b2, (float)matrix.c2, (float)matrix.d2,
		(float)matrix.a3, (float)matrix.b3, (float)matrix.c3, (float)matrix.d3,
		(float)matrix.a4, (float)matrix.b4, (float)matrix.c4, (float)matrix.d4
	);*/
	return D3DXMATRIX(
		(float)matrix.a1, (float)matrix.a2, (float)matrix.a3, (float)matrix.a4,
		(float)matrix.b1, (float)matrix.b2, (float)matrix.b3, (float)matrix.b4,
		(float)matrix.c1, (float)matrix.c2, (float)matrix.c3, (float)matrix.c4,
		(float)matrix.d1, (float)matrix.d2, (float)matrix.d3, (float)matrix.d4
	);
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