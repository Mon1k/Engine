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
	const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_PopulateArmatureData/*aiProcess_Triangulate | aiProcess_PopulateArmatureData*/);

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
			m_model->m_model[index].z = mesh->mVertices[j].y;
			m_model->m_model[index].y = mesh->mVertices[j].z;

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
			}

			for (size_t k = 0; k < pBone->mNumWeights; k++) {
				const aiVertexWeight& vw = pBone->mWeights[k];
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
		animations[i].name = assimp_anim->mName.C_Str();

		float tickPerSecond = assimp_anim->mTicksPerSecond;
		if (tickPerSecond < 0.001) {
			tickPerSecond = 25.0f;
		}

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

	/*ofbx::IScene* gscene = nullptr;

	FILE* fp = fopen(filename, "rb");
	fseek(fp, 0, SEEK_END);
	long file_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	auto* content = new ofbx::u8[file_size];
	fread(content, 1, file_size, fp);


	ofbx::LoadFlags flags =
		ofbx::LoadFlags::TRIANGULATE |
		//		ofbx::LoadFlags::IGNORE_MODELS |
		ofbx::LoadFlags::IGNORE_BLEND_SHAPES |
		ofbx::LoadFlags::IGNORE_CAMERAS |
		ofbx::LoadFlags::IGNORE_LIGHTS |
		//		ofbx::LoadFlags::IGNORE_TEXTURES |
		ofbx::LoadFlags::IGNORE_SKIN |
		ofbx::LoadFlags::IGNORE_BONES |
		ofbx::LoadFlags::IGNORE_PIVOTS |
		//		ofbx::LoadFlags::IGNORE_MATERIALS |
		ofbx::LoadFlags::IGNORE_POSES |
		ofbx::LoadFlags::IGNORE_VIDEOS |
		ofbx::LoadFlags::IGNORE_LIMBS |
		//		ofbx::LoadFlags::IGNORE_MESHES |
		ofbx::LoadFlags::IGNORE_ANIMATIONS;

	gscene = ofbx::load((ofbx::u8*)content, file_size, (ofbx::u16)flags);

	int count =	gscene->getMeshCount();
	const ofbx::Mesh* mesh = gscene->getMesh(0);

	int vertexCount = mesh->getGeometry()->getVertexCount();
	int indexCount = mesh->getGeometry()->getIndexCount();
	model->setVertexCount(vertexCount);
	model->setIndexCount(indexCount);

	const ofbx::Vec3* positions = mesh->getGeometry()->getVertices();
	const ofbx::Vec3* normals = mesh->getGeometry()->getNormals();
	const ofbx::Vec2* uvs = mesh->getGeometry()->getUVs();

	for (int i = 0; i < vertexCount; i++) {
		m_model->m_model[i].x = positions[i].x;
		// change z === y, as FBX format Z is up orientation format
		m_model->m_model[i].y = positions[i].z;
		m_model->m_model[i].z = positions[i].y;

		m_model->m_model[i].nx = normals[i].x;
		m_model->m_model[i].ny = normals[i].y;
		m_model->m_model[i].nz = normals[i].z;

		m_model->m_model[i].tu = uvs[i].x;
		m_model->m_model[i].tv = uvs[i].y;
	}

	
	/*const ofbx::Object* const* objects = gscene->getAllObjects();
	int numObjects = gscene->getAllObjectCount();
	for (int indexObject = 0; indexObject < numObjects; indexObject++) {
		ofbx::Object::Type type = objects[indexObject]->getType();
		if (type == ofbx::Object::Type::MESH) {
			Actor::Joint currJoint;
			currJoint.parentId = objects[indexObject]->getParent()->id;
			currJoint.name = objects[indexObject]->name;
			m_animation.joints.push_back(currJoint);
		}
	}
	

	const ofbx::Skin* skin = mesh->getSkin();
	
	float fps = gscene->getSceneFrameRate();

	int numClusters = skin->getClusterCount();
	for (int clusterIndex = 0; clusterIndex < numClusters; clusterIndex++) {
		const ofbx::Cluster* cluster = skin->getCluster(clusterIndex);

		int animCount = gscene->getAnimationStackCount();
		for (int i = 0; i < animCount; i++) {
			const ofbx::AnimationStack* stack = gscene->getAnimationStack(i);
			const ofbx::AnimationLayer* layer = stack->getLayer(0);
			const ofbx::TakeInfo* take = gscene->getTakeInfo(stack->name);

			m_animation.name = stack->name;
			m_animation.currentTime = 0;
			m_animation.totalTime = take->local_time_to - take->local_time_from;
		}
	}

	//const ofbx::AnimationStack* stack = gscene->getAnimationStack(0);
	//stack->getGlobalTransform();

	int t = 1;

	return true;
	*/


	/*FbxManager* manager = FbxManager::Create();
	FbxIOSettings* ios = FbxIOSettings::Create(manager, IOSROOT);
	manager->SetIOSettings(ios);

	FbxImporter* importer = FbxImporter::Create(manager, "");
	importer->Initialize(filename, -1, manager->GetIOSettings());

	FbxScene* scene = FbxScene::Create(manager, "tempName");

	importer->Import(scene);
	importer->Destroy();

	FbxNode* rootNode = scene->GetRootNode();


	std::vector<D3DXVECTOR3> vertices, texcoords, normals;
	int indexModel = 0;

	int countChilds = rootNode->GetChildCount();
	for (int i = 0; i < countChilds; i++) {
		FbxMesh* mesh = rootNode->GetChild(i)->GetMesh();
		if (mesh == NULL) {
			continue;
		}

		vertices.clear();
		texcoords.clear();
		normals.clear();

		int polygonCount = mesh->GetPolygonCount();

		FbxVector4* controlPoints = mesh->GetControlPoints();
		FbxLayerElementArrayTemplate<FbxVector2>* uvVertices = 0;
		mesh->GetTextureUV(&uvVertices, FbxLayerElement::eTextureDiffuse);

		for (int j = 0; j < polygonCount; j++) {
			int polygonSize = mesh->GetPolygonSize(i);
			for (int k = 0; k < polygonSize; k++) {
				int index = mesh->GetPolygonVertex(j, k);
				D3DXVECTOR3 vertex3;

				FbxVector4 vertex = controlPoints[index];
				vertex3 = D3DXVECTOR3((float)vertex[0], (float)vertex[1], (float)vertex[2]);
				vertices.push_back(vertex3);

				FbxVector4 normal;
				mesh->GetPolygonVertexNormal(j, k, normal);
				vertex3 = D3DXVECTOR3((float)normal[0], (float)normal[1], (float)normal[2]);
				normals.push_back(vertex3);

				FbxVector2 uv = uvVertices->GetAt(mesh->GetTextureUVIndex(j, k));
				D3DXVECTOR3 vertex2 = D3DXVECTOR3((float)uv[0], (float)uv[1], 1.0f);
				texcoords.push_back(vertex2);
			}
		}

		AbstractModel::ModelType modelType;
		ModelClass* linkModel;
		if (indexModel == 0) {
			model->setVertexCount(vertices.size());
			model->setIndexCount(vertices.size());
			linkModel = model;

		} else {
			ModelClass* subset = new ModelClass;
			subset->setVertexCount(vertices.size());
			subset->setIndexCount(vertices.size());
			model->addSubset(subset);
			linkModel = subset;
		}

		for (int j = 0; j < vertices.size(); j++) {
			modelType.x = vertices[j].x;
			// change z === y, as FBX format Z is up orientation format
			modelType.z = vertices[j].y;
			modelType.y = vertices[j].z;
			modelType.nx = normals[j].x;
			modelType.ny = normals[j].y;
			modelType.nz = normals[j].z;
			modelType.tu = texcoords[j].x;
			modelType.tv = texcoords[j].y;

			linkModel->m_model[j] = modelType;
		}


		if (dynamic_cast<const Actor*>(m_model) != nullptr) {
			ProcessSkeletonHierarchy(rootNode);
			loadAnimations(scene, mesh, dynamic_cast<Actor*>(linkModel));
		}

		indexModel++;
	}*/

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

void FbxLoader::loadAnimations(FbxScene* scene, FbxMesh* mesh, Actor* actor)
{
	FbxNode* node = mesh->GetNode();
	FbxAMatrix geometryTransform = FbxAMatrix(
		node->GetGeometricTranslation(FbxNode::eSourcePivot),
		node->GetGeometricRotation(FbxNode::eSourcePivot),
		node->GetGeometricScaling(FbxNode::eSourcePivot)
	);

	std::vector<Actor::Weight> weights;
	weights.resize(actor->getVertexCount());

	std::vector<std::vector<int>> controlPointRemap;
	controlPointRemap.resize(mesh->GetPolygonCount());
	int kTriangleVertexCount = 3;
	for (long lPolygonIndex = 0; lPolygonIndex < mesh->GetPolygonCount(); lPolygonIndex++) {
		long lPolygonSize = mesh->GetPolygonSize(lPolygonIndex);
		for (long lVertexIndex = 0; lVertexIndex < lPolygonSize; lVertexIndex++) {
			long lControlPointIndex = mesh->GetPolygonVertex(lPolygonIndex, lVertexIndex);
			controlPointRemap[lControlPointIndex].push_back(lPolygonIndex * kTriangleVertexCount + lVertexIndex);
		}
	}

	int numDeformers = mesh->GetDeformerCount();
	for (int deformerIndex = 0; deformerIndex < numDeformers; deformerIndex++) {
		FbxSkin* skin = (FbxSkin*)mesh->GetDeformer(0, FbxDeformer::eSkin);
		if (!skin) {
			continue;
		}

		int numClusters = skin->GetClusterCount();
		for (int clusterIndex = 0; clusterIndex < numClusters; clusterIndex++) {
			FbxCluster* cluster = skin->GetCluster(clusterIndex);
			std::string jointName = cluster->GetLink()->GetName();
			unsigned int currJointIndex = FindJointIndexUsingName(jointName);

			FbxAMatrix transformMatrix;
			FbxAMatrix transformLinkMatrix;
			FbxAMatrix globalBindposeInverseMatrix;

			cluster->GetTransformMatrix(transformMatrix);
			cluster->GetTransformLinkMatrix(transformLinkMatrix);
			globalBindposeInverseMatrix = transformLinkMatrix.Inverse() * transformMatrix * geometryTransform;

			//m_animation.joints[currJointIndex].inverse = toD3DXMATRIX(globalBindposeInverseMatrix);

			int* boneVertexIendicies = cluster->GetControlPointIndices();
			double* boneVertexWeights = cluster->GetControlPointWeights();
			int numBoneVertexIndicies = cluster->GetControlPointIndicesCount();
			for (int j = 0; j < numBoneVertexIndicies; j++) {
				float boneWeight = (float)boneVertexWeights[j];
				if (boneWeight >= 0.1f) {
					std::vector<int> controlPointIndexes = controlPointRemap[boneVertexIendicies[j]];
					for (int k = 0; k < controlPointIndexes.size(); k++) {
						Actor::Weight weight;
						weight.bias = boneWeight;
						weight.joint = currJointIndex;
						weight.index = controlPointIndexes[k];
						weights[controlPointIndexes[k]] = weight;
						//actor->addWeight(weight);
					}
				}
			}

			//FbxAnimStack* currAnimStack = scene->GetSrcObject<FbxAnimStack>(0);
			int numAnimations = scene->GetSrcObjectCount<FbxAnimStack>();
			for (int j = 0; j < numAnimations; j++) {
				FbxAnimStack* currAnimStack = (FbxAnimStack*)scene->GetSrcObject<FbxAnimStack>(j);
				FbxString animStackName = currAnimStack->GetName();
				m_animation.name = animStackName.Buffer();
				FbxTakeInfo* takeInfo = scene->GetTakeInfo(animStackName);
				FbxTime start = takeInfo->mLocalTimeSpan.GetStart();
				FbxTime end = takeInfo->mLocalTimeSpan.GetStop();
				m_animation.currentTime = 0;
				m_animation.totalTime = end.GetFrameCount(FbxTime::eFrames24) - start.GetFrameCount(FbxTime::eFrames24) + 1;

				for (FbxLongLong i = start.GetFrameCount(FbxTime::eFrames24); i <= end.GetFrameCount(FbxTime::eFrames24); ++i) {
					FbxTime currTime;
					currTime.SetFrame(i, FbxTime::eFrames24);

					Actor::KeyFrame keyFrame;
					keyFrame.numFrame = (int)i;
					//scene->GetAnimationEvaluator()->GetNodeLocalTransform();
					FbxAMatrix currentTransformOffset = node->EvaluateGlobalTransform(currTime) *geometryTransform;
					//keyFrame.transform = toD3DXMATRIX(currentTransformOffset.Inverse() * cluster->GetLink()->EvaluateGlobalTransform(currTime));
					//keyFrame.transform = toD3DXMATRIX(cluster->GetLink()->EvaluateGlobalTransform(currTime) * currentTransformOffset);
					//keyFrame.transform = toD3DXMATRIX(currentTransformOffset);
					
					m_animation.joints[currJointIndex].animation.push_back(keyFrame);
				}
			}
		}
	}

	actor->addAnimation(m_animation);
	actor->addWeights(weights);
}

unsigned int FbxLoader::FindJointIndexUsingName(const std::string& inJointName)
{
	for (unsigned int i = 0; i < m_animation.joints.size(); ++i) {
		if (m_animation.joints[i].name == inJointName) {
			return i;
		}
	}

	throw std::exception("Skeleton information in FBX file is corrupted.");
}

void FbxLoader::ProcessSkeletonHierarchy(FbxNode* inRootNode)
{
	m_animation.joints.clear();
	for (int childIndex = 0; childIndex < inRootNode->GetChildCount(); ++childIndex) {
		FbxNode* currNode = inRootNode->GetChild(childIndex);
		ProcessSkeletonHierarchyRecursively(currNode, 0, 0, -1);
	}
}

void FbxLoader::ProcessSkeletonHierarchyRecursively(FbxNode* inNode, int inDepth, int myIndex, int inParentIndex)
{
	if (inNode->GetNodeAttribute() && inNode->GetNodeAttribute()->GetAttributeType() && inNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton) {
		Actor::Joint currJoint;
		currJoint.parentId = inParentIndex;
		currJoint.name = inNode->GetName();
		m_animation.joints.push_back(currJoint);
	}

	for (int i = 0; i < inNode->GetChildCount(); i++) {
		ProcessSkeletonHierarchyRecursively(inNode->GetChild(i), inDepth + 1, m_animation.joints.size(), myIndex);
	}
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