#include "FbxLoader.h"

bool FbxLoader::load(char* filename, ModelClass* model)
{
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

		
		model->setVertexCount(vertices.size());
		model->setIndexCount(vertices.size());

		AbstractModel::ModelType modelType;
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

			model->m_model[j] = modelType;
		}
	}

	return true;
}