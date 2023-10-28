#include "AssimpLoader.h"

#include "assimp/Importer.hpp"
#include "assimp/ProgressHandler.hpp"
#include "assimp/postprocess.h"

bool AssimpLoader::load(char* filename, ModelClass* model)
{
	Actor* actor = dynamic_cast<Actor*>(model);

	Assimp::Importer importer;
	unsigned int flags = aiProcess_Triangulate | aiProcess_OptimizeMeshes | aiProcess_FlipUVs | aiProcess_FlipWindingOrder | aiProcess_MakeLeftHanded | aiProcess_CalcTangentSpace;
	// smooth normal for not fbx with artefact
	flags |= aiProcess_GenSmoothNormals;
	m_Scene = importer.ReadFile(filename, flags);

	//// @todo - correct rotation for fbx
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


	int baseVertex = 0;
	const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

	// process mesh with seprate for model and subset for his
	ModelClass* refModel;
	for (size_t i = 0; i < m_Scene->mNumMeshes; ++i) {
		aiMesh* mesh = m_Scene->mMeshes[i];
		int index = 0;

		if (i == 0) {
			refModel = model;
		}
		else {
			ModelClass* subset = new ModelClass;
			refModel = subset;
		}
		refModel->setVertexCount(mesh->mNumVertices);

		// fill vertices
		for (size_t j = 0; j < mesh->mNumVertices; ++j) {
			refModel->m_model[index].x = mesh->mVertices[j].x;
			refModel->m_model[index].y = mesh->mVertices[j].y;
			refModel->m_model[index].z = mesh->mVertices[j].z;

			if (mesh->mNormals) {
				refModel->m_model[index].nx = mesh->mNormals[j].x;
				refModel->m_model[index].ny = mesh->mNormals[j].y;
				refModel->m_model[index].nz = mesh->mNormals[j].z;
			}
			else {
				refModel->m_model[index].nx = 0.0f;
				refModel->m_model[index].ny = 1.0f;
				refModel->m_model[index].nz = 0.0f;
			}

			const aiVector3D& pTexCoord = mesh->HasTextureCoords(0) ? mesh->mTextureCoords[0][j] : Zero3D;
			refModel->m_model[index].tu = pTexCoord.x;
			refModel->m_model[index].tv = pTexCoord.y;

			index++;
		}

		// fill indices
		for (size_t j = 0; j < mesh->mNumFaces; j++) {
			aiFace face = mesh->mFaces[j];
			for (size_t k = 0; k < face.mNumIndices; k++) {
				refModel->m_ModelIndices.push_back(face.mIndices[k]);
				//refModel->m_ModelIndices.back() += baseVertex; // only one mesh needed
			}
		}
		refModel->setIndexCount(refModel->m_ModelIndices.size());

		// fill subset
		if (i > 0) {
			refModel->setD3D(model->getD3D());
			refModel->setAlpha(model->getAlpha());
			refModel->addLights(model->getLights());
			refModel->addShader(model->getShader());
			std::string texture = model->GetTextureClass()->getTexturePath(i);
			refModel->LoadTextures(texture.size() > 0 ? texture : model->GetTextureClass()->getTexturePath(0));

			refModel->InitializeBuffers();
			refModel->CalcMinMax();

			model->addSubset(refModel);
		}

		// create mesh
		Actor::MeshInfo* hiearchyMesh = new Actor::MeshInfo;
		hiearchyMesh->name = mesh->mName.C_Str();
		hiearchyMesh->baseVertex = baseVertex;
		hiearchyMesh->count = mesh->mNumVertices;
		actor->addMesh(hiearchyMesh);

		baseVertex += mesh->mNumVertices;
	}

	// process node
	createTreeNode(m_Scene->mRootNode, actor, nullptr, 0);
	std::vector<Actor::NodeInfo*> nodes = actor->getNodes();
	std::sort(nodes.begin(), nodes.end(),
		[](const Actor::NodeInfo* a, const Actor::NodeInfo* b)->bool {
			return a->depth < b->depth; 
		});

	// process skin
	std::vector<Actor::Weight> weights;
	weights.resize(baseVertex);
	for (size_t i = 0; i < m_Scene->mNumMeshes; ++i) {
		aiMesh* mesh = m_Scene->mMeshes[i];
		if (!mesh->HasBones()) {
			continue;
		}

		// fill bones for current mesh
		Actor::MeshInfo* hiearchyMesh = actor->getMesh(i);
		for (size_t j = 0; j < mesh->mNumBones; j++) {
			const aiBone* pBone = mesh->mBones[j];

			Actor::BoneInfo bi;
			bi.name = pBone->mName.C_Str();
			bi.OffsetMatrix = D3DXMATRIX(pBone->mOffsetMatrix[0]);
			D3DXMatrixTranspose(&bi.OffsetMatrix, &bi.OffsetMatrix);

			bi.node = FindNode(actor, bi);
			hiearchyMesh->bones.push_back(bi);

			// fill weights
			for (size_t k = 0; k < pBone->mNumWeights; k++) {
				const aiVertexWeight vw = pBone->mWeights[k];
				unsigned int GlobalVertexID = hiearchyMesh->baseVertex + vw.mVertexId;

				weights[GlobalVertexID].AddBoneData(j, vw.mWeight);
			}
		}
	}
	actor->setWeights(weights);

	// process animation
	for (size_t i = 0; i < m_Scene->mNumAnimations; ++i) {
		const aiAnimation* assimp_anim = m_Scene->mAnimations[i];

		float tickPerSecond = assimp_anim->mTicksPerSecond;
		if (tickPerSecond < 0.001f) {
			tickPerSecond = 25.0f;
		}

		Actor::Animation animation;
		animation.name = assimp_anim->mName.C_Str();
		animation.maxTime = 0;
		animation.totalTime = assimp_anim->mDuration;
		animation.tick = tickPerSecond;
		animation.currentTime = 0;


		// fill anim nodes for current animation
		for (size_t j = 0; j < assimp_anim->mNumChannels; ++j) {
			const aiNodeAnim* assimp_node_anim = assimp_anim->mChannels[j];
			Actor::AnimationNode animationNode;

			animationNode.name = assimp_node_anim->mNodeName.C_Str();
			int keyCount = max(assimp_node_anim->mNumPositionKeys, assimp_node_anim->mNumRotationKeys);
			keyCount = max(keyCount, assimp_node_anim->mNumScalingKeys);

			// fill frame for current anim node
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

				animation.maxTime = max(animation.maxTime, key.time);
				animationNode.frames.push_back(key);
			}

			animation.nodes.push_back(animationNode);
		}

		actor->addAnimation(animation);
	}

	return true;
}

void AssimpLoader::createTreeNode(aiNode* node, Actor* actor, Actor::NodeInfo* parent, int depth)
{
	Actor::NodeInfo* internalNode = new Actor::NodeInfo;
	internalNode->name = node->mName.C_Str();
	internalNode->localTransformation = D3DXMATRIX(node->mTransformation[0]);
	D3DXMatrixTranspose(&internalNode->localTransformation, &internalNode->localTransformation);
	internalNode->globalTransformation = internalNode->localTransformation;
	internalNode->setParent(parent);
	internalNode->depth = depth;

	actor->addNode(internalNode);

	if (node->mNumMeshes > 0) {
		Actor::MeshInfo* mesh = actor->getMesh(node->mMeshes[0]);
		mesh->node = internalNode;
	}

	for (size_t i = 0; i < node->mNumChildren; i++) {
		createTreeNode(node->mChildren[i], actor, internalNode, depth + 1);
	}
}

Actor::NodeInfo* AssimpLoader::FindNode(Actor* actor, Actor::BoneInfo bone)
{
	Actor::NodeInfo* findNode = nullptr;

	std::vector<Actor::NodeInfo*> nodes = actor->getNodes();
	for (size_t i = 0; i < nodes.size(); i++) {
		if (nodes[i]->name == bone.name) {
			findNode = nodes[i];
			break;
		}
	}

	return findNode;
}