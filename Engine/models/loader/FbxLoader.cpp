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

	int countChilds = rootNode->GetChildCount();


	std::vector<D3DXVECTOR3> vertices, texcoords, normals;
	int indexModel = 0;

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
			loadAnimations(scene, mesh, dynamic_cast<Actor*>(linkModel));
		}

		indexModel++;
	}

	return true;
}

void FbxLoader::loadAnimations(FbxScene* scene, FbxMesh* mesh, Actor* actor)
{
	FbxNode* node = mesh->GetNode();

	FbxCriteria criteria;
	//scene->GetSrcObjectCount(criteria.ObjectType(FbxAnimStack::ClassId));
	int numAnimations = scene->GetSrcObjectCount<FbxAnimStack>();
	for (int i = 0; i < numAnimations; i++) {
		FbxAnimStack* stack = (FbxAnimStack*)scene->GetSrcObject<FbxAnimStack>(i);
		int numLayers = stack->GetMemberCount();

		Actor::Animation animation;
		animation.name = stack->GetName();
		animation.countAnimations = numLayers;
		
		FbxTakeInfo* takeInfo = scene->GetTakeInfo(stack->GetName());
		FbxTime start = takeInfo->mLocalTimeSpan.GetStart();
		FbxTime end = takeInfo->mLocalTimeSpan.GetStop();

		long countS = start.GetFrameCount(FbxTime::eFrames24);
		long countE = end.GetFrameCount(FbxTime::eFrames24);
		animation.currentTime = countS;
		animation.frameTime = countE;
		animation.totalTime = countE - countS;


		for (int j = 0; j < numLayers; j++) {
			FbxAnimLayer* layer = (FbxAnimLayer*)stack->GetMember(j);

			FbxAnimCurve* positionCurve = node->LclTranslation.GetCurve(layer);
			FbxAnimCurve* scalingCurve = node->LclScaling.GetCurve(layer);

			Actor::Joint joint;
			if (positionCurve != 0) {
				int numKeys = positionCurve->KeyGetCount();
				for (int k = 0; k < numKeys; k++) {
					FbxAnimCurveKey key = positionCurve->KeyGet(k);
					FbxDouble3 vector = node->EvaluateLocalTranslation(key.GetTime());

					joint.position = D3DXVECTOR3((float)vector[0], (float)vector[1], (float)vector[2]);
					animation.joints.push_back(joint);
				}
			} else {
				FbxDouble3 vector = node->LclTranslation.Get();
				joint.position = D3DXVECTOR3((float)vector[0], (float)vector[1], (float)vector[2]);
				animation.joints.push_back(joint);
			}
			if (scalingCurve != 0) {
				int numKeys = scalingCurve->KeyGetCount();
				for (int k = 0; k < numKeys; k++) {
					FbxAnimCurveKey key = scalingCurve->KeyGet(k);
					FbxDouble3 vector = node->EvaluateLocalScaling(key.GetTime());

					joint.scaling = D3DXVECTOR4((float)vector[0], (float)vector[1], (float)vector[2], 0.0f);
					animation.joints.push_back(joint);
				}
			} else {
				FbxDouble3 vector = node->LclScaling.Get();
				joint.scaling = D3DXVECTOR4((float)vector[0], (float)vector[1], (float)vector[2], 0.0f);
				animation.joints.push_back(joint);
			}
		}

		

		actor->addAnimation(animation);
	}

	int numDeformers = mesh->GetDeformerCount();
	FbxSkin* skin = (FbxSkin*)mesh->GetDeformer(0, FbxDeformer::eSkin);
	if (skin != 0) {
		int numBones = skin->GetClusterCount();
		for (int i = 0; i < numBones; i++) {
			FbxCluster* cluster = skin->GetCluster(i);
			FbxNode* bone = cluster->GetLink();

			FbxAMatrix positionMatrix;
			cluster->GetTransformLinkMatrix(positionMatrix);

			int* boneVertexIndicies = cluster->GetControlPointIndices();
			double* boneVertexWeights = cluster->GetControlPointWeights();

			int numBoneVertexIndicies = cluster->GetControlPointIndicesCount();
			for (int j = 0; j < numBoneVertexIndicies; j++) {
				int boneVertexIndex = boneVertexIndicies[j];
				float boneWeight = (float)boneVertexWeights[j];
				Actor::Weight weight;
				weight.bias = boneWeight;
				weight.joint = boneVertexIndex;
				actor->addWeight(weight);
			}
		}
	}


	int t = 0;
}