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
	const aiScene* scene = importer.ReadFile(filename, aiProcess_GenSmoothNormals | aiProcess_SplitLargeMeshes | aiProcess_Triangulate | aiProcess_MakeLeftHanded | aiProcess_SortByPType | aiProcess_CalcTangentSpace | aiProcess_FindDegenerates | aiProcess_GenUVCoords | aiProcess_TransformUVCoords | aiProcess_PopulateArmatureData/*aiProcess_Triangulate | aiProcess_PopulateArmatureData*/);

	int vertexCount = 0, indexCount = 0;
	for (size_t i = 0; i < scene->mNumMeshes; ++i)
	{
		aiMesh* mesh = scene->mMeshes[i];
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

	for (size_t i = 0; i < scene->mNumMeshes; ++i) {
		aiMesh* mesh = scene->mMeshes[i];
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

		for (size_t j = 0; j < mesh->mNumBones; j++) {
			const aiBone* pBone = mesh->mBones[j];
			int boneId = GetBoneId(pBone->mName.C_Str());

			if (boneId == actor->m_BoneInfo.size()) {
				Actor::BoneInfo bi(toD3DXMATRIX(pBone->mOffsetMatrix));
				bi.name = pBone->mNode->mName.C_Str();
				bi.parent = pBone->mNode->mParent->mName.C_Str();
				bi.transformation = toD3DXMATRIX(pBone->mNode->mTransformation);
				bi.boneId = boneId;
				actor->m_BoneInfo.push_back(bi);
				CalculateGlobalTransform(actor, bi.name);
			}

			for (size_t k = 0; k < pBone->mNumWeights; k++) {
				const aiVertexWeight vw = pBone->mWeights[k];
				unsigned int GlobalVertexID = baseVertex + vw.mVertexId;

				weights[GlobalVertexID].AddBoneData(boneId, vw.mWeight);
				weights[GlobalVertexID].name = pBone->mName.C_Str();
			}
		}

		baseVertex += mesh->mNumVertices;
	}

	std::vector<Actor::Animation> animations;
	animations.resize(scene->mNumAnimations);

	for (size_t i = 0; i < scene->mNumAnimations; ++i) {
		const aiAnimation* assimp_anim = scene->mAnimations[i];

		float tickPerSecond = assimp_anim->mTicksPerSecond;
		if (tickPerSecond < 0.001) {
			tickPerSecond = 25.0f;
		}

		animations[i].name = assimp_anim->mName.C_Str();
		animations[i].totalTime = assimp_anim->mDuration;
		animations[i].tick = tickPerSecond;
		animations[i].currentTime = 0;
		animations[i].globalInverseTransformation = toD3DXMATRIX(scene->mRootNode->mTransformation.Inverse());

		animations[i].joints.resize(assimp_anim->mNumChannels);
		for (size_t j = 0; j < assimp_anim->mNumChannels; ++j) {
			const aiNodeAnim* assimp_node_anim = assimp_anim->mChannels[j];
			animations[i].joints[j].name = assimp_node_anim->mNodeName.C_Str();

			animations[i].joints[j].position.resize(assimp_node_anim->mNumPositionKeys);
			for (size_t idx = 0; idx < assimp_node_anim->mNumPositionKeys; ++idx) {
				const aiVectorKey anim_key = assimp_node_anim->mPositionKeys[idx];

				Actor::KeyFrame key;
				key.time = anim_key.mTime;
				key.position.x = anim_key.mValue.x;
				key.position.y = anim_key.mValue.y;
				key.position.z = anim_key.mValue.z;
				animations[i].joints[j].position[idx] = key;
			}

			animations[i].joints[j].scaling.resize(assimp_node_anim->mNumScalingKeys);
			for (size_t idx = 0; idx < assimp_node_anim->mNumScalingKeys; ++idx) {
				const aiVectorKey anim_key = assimp_node_anim->mScalingKeys[idx];

				Actor::KeyFrame key;
				key.time = anim_key.mTime;
				key.scaling.x = anim_key.mValue.x;
				key.scaling.y = anim_key.mValue.y;
				key.scaling.z = anim_key.mValue.z;
				animations[i].joints[j].scaling[idx] = key;
			}

			animations[i].joints[j].rotation.resize(assimp_node_anim->mNumRotationKeys);
			for (size_t idx = 0; idx < assimp_node_anim->mNumRotationKeys; ++idx) {
				const aiQuatKey anim_key = assimp_node_anim->mRotationKeys[idx];

				Actor::KeyFrame key;
				key.time = anim_key.mTime;
				key.rotation.x = anim_key.mValue.x;
				key.rotation.y = anim_key.mValue.y;
				key.rotation.z = anim_key.mValue.z;
				key.rotation.w = anim_key.mValue.w;
				animations[i].joints[j].rotation[idx] = key;
			}
		}
	}

	//actor->addAnimation(animations);
	//actor->addWeights(weights);
	actor->m_animations = animations;
	actor->m_weights = weights;
	int t69 = 1;

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

void FbxLoader::CalculateGlobalTransform(Actor* actor, std::string boneName)
{
	Actor::BoneInfo* bone;
	for (size_t i = 0; i < actor->m_BoneInfo.size(); i++) {
		if (actor->m_BoneInfo[i].name == boneName) {
			bone = &actor->m_BoneInfo[i];
			break;
		}
	}

	bone->globalTansformation = bone->transformation;
	std::string name = bone->parent;
	do {
		Actor::BoneInfo parentBone;
		for (size_t i = 0; i < actor->m_BoneInfo.size(); i++) {
			if (actor->m_BoneInfo[i].name == name) {
				parentBone = actor->m_BoneInfo[i];
				break;
			}
		}

		if (parentBone.boneId == -1) {
			break;
		}

		bone->globalTansformation = parentBone.transformation * bone->globalTansformation;
		name = parentBone.parent;
	} while (true);
}