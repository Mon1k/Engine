#include "ObjLoader.h"

bool ObjLoader::load(char* filename, AbstractModel* model)
{
	typedef struct
	{
		float x, y, z;
	} VertexType;

	typedef struct
	{
		int vIndex1, vIndex2, vIndex3;
		int tIndex1, tIndex2, tIndex3;
		int nIndex1, nIndex2, nIndex3;
	} FaceType;

	ifstream fin;
	char input;

	///// open for countable
	// Initialize the counts.
	int vertexCount = 0;
	int textureCount = 0;
	int normalCount = 0;
	int faceCount = 0;

	// Open the file.
	fin.open(filename);

	// Check if it was successful in opening the file.
	if (fin.fail() == true) {
		return false;
	}

	// Read from the file and continue to read until the end of the file is reached.
	fin.get(input);
	while (!fin.eof()) {
		// If the line starts with 'v' then count either the vertex, the texture coordinates, or the normal vector.
		if (input == 'v') {
			fin.get(input);
			if (input == ' ') {
				vertexCount++;
			}
			if (input == 't') {
				textureCount++;
			}
			if (input == 'n') {
				normalCount++;
			}
		}

		// If the line starts with 'f' then increment the face count.
		if (input == 'f') {
			fin.get(input);
			if (input == ' ') {
				faceCount++;
			}
		}

		// Otherwise read in the remainder of the line.
		while (input != '\n') {
			fin.get(input);
		}

		// Start reading the beginning of the next line.
		fin.get(input);
	}

	// Close the file.
	fin.close();


	///// loaded data set in model
	VertexType* vertices, * texcoords, * normals;
	FaceType* faces;
	int vertexIndex, texcoordIndex, normalIndex, faceIndex, vIndex, tIndex, nIndex;
	char input2;

	// Initialize the four data structures.
	vertices = new VertexType[vertexCount];
	if (!vertices) {
		return false;
	}

	texcoords = new VertexType[textureCount];
	if (!texcoords) {
		return false;
	}

	normals = new VertexType[normalCount];
	if (!normals) {
		return false;
	}

	faces = new FaceType[faceCount];
	if (!faces) {
		return false;
	}

	// Initialize the indexes.
	vertexIndex = 0;
	texcoordIndex = 0;
	normalIndex = 0;
	faceIndex = 0;

	// Open the file.
	fin.open(filename);

	// Check if it was successful in opening the file.
	if (fin.fail() == true) {
		return false;
	}

	// Read in the vertices, texture coordinates, and normals into the data structures.
	// Important: Also convert to left hand coordinate system since Maya uses right hand coordinate system.
	fin.get(input);
	while (!fin.eof()) {
		if (input == 'v') {
			fin.get(input);

			// Read in the vertices.
			if (input == ' ') {
				fin >> vertices[vertexIndex].x >> vertices[vertexIndex].y >> vertices[vertexIndex].z;

				// Invert the Z vertex to change to left hand system.
				vertices[vertexIndex].z = vertices[vertexIndex].z * -1.0f;
				vertexIndex++;
			}

			// Read in the texture uv coordinates.
			if (input == 't') {
				fin >> texcoords[texcoordIndex].x >> texcoords[texcoordIndex].y;

				// Invert the V texture coordinates to left hand system.
				texcoords[texcoordIndex].y = 1.0f - texcoords[texcoordIndex].y;
				texcoordIndex++;
			}

			// Read in the normals.
			if (input == 'n') {
				fin >> normals[normalIndex].x >> normals[normalIndex].y >> normals[normalIndex].z;

				// Invert the Z normal to change to left hand system.
				normals[normalIndex].z = normals[normalIndex].z * -1.0f;
				normalIndex++;
			}
		}

		// Read in the faces.
		if (input == 'f') {
			fin.get(input);
			if (input == ' ') {
				// Read the face data in backwards to convert it to a left hand system from right hand system.
				fin >> faces[faceIndex].vIndex3 >> input2 >> faces[faceIndex].tIndex3 >> input2 >> faces[faceIndex].nIndex3
					>> faces[faceIndex].vIndex2 >> input2 >> faces[faceIndex].tIndex2 >> input2 >> faces[faceIndex].nIndex2
					>> faces[faceIndex].vIndex1 >> input2 >> faces[faceIndex].tIndex1 >> input2 >> faces[faceIndex].nIndex1;
				faceIndex++;
			}
		}

		// Read in the remainder of the line.
		while (input != '\n') {
			fin.get(input);
		}

		// Start reading the beginning of the next line.
		fin.get(input);
	}

	// Close the file.
	fin.close();

	///// create model with readed data
	model->setVertexCount(faceIndex * 3);
	model->setIndexCount(faceIndex * 3);

	int index = 0;
	for (int i = 0; i < faceIndex; i++) {
		for (int j = 1; j <= 3; j++) {
			switch (j) {
			case 1:
				vIndex = faces[i].vIndex1 - 1;
				tIndex = faces[i].tIndex1 - 1;
				nIndex = faces[i].nIndex1 - 1;
				break;

			case 2:
				vIndex = faces[i].vIndex2 - 1;
				tIndex = faces[i].tIndex2 - 1;
				nIndex = faces[i].nIndex2 - 1;
				break;

			case 3:
				vIndex = faces[i].vIndex3 - 1;
				tIndex = faces[i].tIndex3 - 1;
				nIndex = faces[i].nIndex3 - 1;
				break;
			}
			model->m_model[index].x = vertices[vIndex].x;
			model->m_model[index].y = vertices[vIndex].y;
			model->m_model[index].z = vertices[vIndex].z;
			model->m_model[index].tu = texcoords[tIndex].x;
			model->m_model[index].tv = texcoords[tIndex].y;
			model->m_model[index].nx = normals[nIndex].x;
			model->m_model[index].ny = normals[nIndex].y;
			model->m_model[index].nz = normals[nIndex].z;
			index++;
		}
	}

	// Release the four data structures.
	if (vertices) {
		delete[] vertices;
		vertices = 0;
	}
	if (texcoords) {
		delete[] texcoords;
		texcoords = 0;
	}
	if (normals) {
		delete[] normals;
		normals = 0;
	}
	if (faces) {
		delete[] faces;
		faces = 0;
	}

	return true;
}