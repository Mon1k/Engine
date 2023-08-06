#include "FbxLoader.h"
#include "../actor/Actor.h"

bool FbxLoader::load(char* filename, ModelClass* model)
{
	m_model = model;

	FbxManager* manager = FbxManager::Create();
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
	}

	return true;
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
			globalBindposeInverseMatrix = transformLinkMatrix.Inverse() * transformMatrix;// *geometryTransform;

			m_animation.joints[currJointIndex].inverse = toD3DXMATRIX(globalBindposeInverseMatrix);


			int* boneVertexIendicies = cluster->GetControlPointIndices();
			double* boneVertexWeights = cluster->GetControlPointWeights();
			int numBoneVertexIndicies = cluster->GetControlPointIndicesCount();
			for (int j = 0; j < numBoneVertexIndicies; j++) {
				float boneWeight = (float)boneVertexWeights[j];
				if (boneWeight > 0.1f) {
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
					FbxAMatrix currentTransformOffset = node->EvaluateGlobalTransform(currTime) * geometryTransform;
					keyFrame.transform = toD3DXMATRIX(currentTransformOffset.Inverse() * cluster->GetLink()->EvaluateGlobalTransform(currTime));
					
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

D3DXMATRIX FbxLoader::toD3DXMATRIX(FbxAMatrix matrix)
{
	return D3DXMATRIX(
		(float)matrix.Get(0, 0), (float)matrix.Get(0, 1), (float)matrix.Get(0, 2), (float)matrix.Get(0, 3),
		(float)matrix.Get(1, 0), (float)matrix.Get(1, 1), (float)matrix.Get(1, 2), (float)matrix.Get(1, 3),
		(float)matrix.Get(2, 0), (float)matrix.Get(2, 1), (float)matrix.Get(2, 2), (float)matrix.Get(2, 3),
		(float)matrix.Get(3, 0), (float)matrix.Get(3, 1), (float)matrix.Get(3, 2), (float)matrix.Get(3, 3)
	);
}

D3DXMATRIX FbxLoader::toD3DXMATRIX(FbxMatrix matrix)
{
	return D3DXMATRIX(
		(float)matrix.Get(0, 0), (float)matrix.Get(0, 1), (float)matrix.Get(0, 2), (float)matrix.Get(0, 3),
		(float)matrix.Get(1, 0), (float)matrix.Get(1, 1), (float)matrix.Get(1, 2), (float)matrix.Get(1, 3),
		(float)matrix.Get(2, 0), (float)matrix.Get(2, 1), (float)matrix.Get(2, 2), (float)matrix.Get(2, 3),
		(float)matrix.Get(3, 0), (float)matrix.Get(3, 1), (float)matrix.Get(3, 2), (float)matrix.Get(3, 3)
	);
}

FbxAMatrix FbxLoader::convertToLeftHanded(FbxAMatrix fbxMatrix)
{
	FbxAMatrix convertionMatrix;
	FbxVector4 rowX(1.0, 0.0, 0.0, 0.0);
	FbxVector4 rowY(0.0, 1.0, 0.0, 0.0);
	FbxVector4 rowZ(0.0, 0.0, -1.0, 0.0);
	FbxVector4 rowW(0.0, 0.0, 0.0, 1.0);

	convertionMatrix.SetRow(0, rowX);
	convertionMatrix.SetRow(1, rowY);
	convertionMatrix.SetRow(2, rowZ);
	convertionMatrix.SetRow(3, rowW);

	FbxAMatrix convertedMatrix = fbxMatrix * convertionMatrix;
	return convertedMatrix;
}

FbxMatrix FbxLoader::ConvertMatrix(FbxAMatrix Matrix)
{
	FbxMatrix UEMatrix;

	for (int i = 0; i < 4; ++i) {
		FbxVector4 Row = Matrix.GetRow(i);
		if (i == 1) {
			UEMatrix.Set(i, 0, -Row[0]);
			UEMatrix.Set(i, 1, Row[1]);
			UEMatrix.Set(i, 2, -Row[2]);
			UEMatrix.Set(i, 3, -Row[3]);
		} else {
			UEMatrix.Set(i, 0, Row[0]);
			UEMatrix.Set(i, 1, -Row[1]);
			UEMatrix.Set(i, 2, Row[2]);
			UEMatrix.Set(i, 3, Row[3]);
		}
	}

	return UEMatrix;
}