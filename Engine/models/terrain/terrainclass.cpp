#include "terrainclass.h"

TerrainClass::TerrainClass(): Model()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_heightMap = 0;
	m_repeat = 0;
	m_vertices = 0;
	m_shader = 0;
	m_quadTree = 0;

	m_scaleNormal = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
}


TerrainClass::TerrainClass(const TerrainClass& other)
{
}


TerrainClass::~TerrainClass()
{
}

bool TerrainClass::Initialize(D3DClass* d3dClass, FrustumClass* frustum, char* heightMapFilename, std::string textureFilename, std::string normalTextureFilename)
{
	bool result;

	m_D3D = d3dClass;

	// Load in the height map for the terrain.
	result = LoadHeightMap(heightMapFilename);
	if (!result) {
		return false;
	}
	
	// Calculate the normals for the terrain data.
	result = CalculateNormals();
	if (!result) {
		return false;
	}

	// Calculate the texture coordinates.
	CalculateTextureCoordinates();

	// Calculate the normal, tangent, and binormal vectors for the terrain model.
	BuildTerrainModel();
	CalculateTerrainVectors();

	if (!LoadTexturesArray({ textureFilename, normalTextureFilename })) {
		return false;
	}

	// Initialize the vertex array that will hold the geometry for the terrain.
	result = InitializeBuffers(m_D3D->GetDevice());
	if (!result) {
		return false;
	}

	m_shader = new TerrainShaderClass();
	m_shader->Initialize(m_D3D->GetDevice());

	m_quadTree = new QuadTreeClass;
	m_quadTree->Initialize(m_vertices, m_vertexCount, m_D3D, frustum);

	return true;
}

void TerrainClass::Shutdown()
{
	// Release the texture.
	ReleaseTexture();

	// Release the vertex array.
	ShutdownBuffers();

	// Release the height map data.
	ShutdownHeightMap();
}

void TerrainClass::ShutdownBuffers()
{
	// Release the vertex array.
	if (m_vertices) {
		delete[] m_vertices;
		m_vertices = 0;
	}

	if (m_shader) {
		m_shader->Shutdown();
		m_shader = 0;
	}

	if (m_quadTree) {
		m_quadTree->Shutdown();
		m_quadTree = 0;
	}
}

void TerrainClass::ShutdownHeightMap()
{
	if (m_heightMap) {
		delete[] m_heightMap;
		m_heightMap = 0;
	}
}

bool TerrainClass::LoadHeightMap(char* filename)
{
	FILE* filePtr;
	int error;
	unsigned int count;
	BITMAPFILEHEADER bitmapFileHeader;
	BITMAPINFOHEADER bitmapInfoHeader;
	int imageSize, i, j, k, index;
	unsigned char* bitmapImage;
	unsigned char height;

	m_path = std::string(filename);

	// Open the height map file in binary.
	error = fopen_s(&filePtr, filename, "rb");
	if (error != 0) {
		return false;
	}

	// Read in the file header.
	count = fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
	if (count != 1) {
		return false;
	}

	// Read in the bitmap info header.
	count = fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
	if (count != 1) {
		return false;
	}

	// Save the dimensions of the terrain.
	m_terrainWidth = bitmapInfoHeader.biWidth;
	m_terrainHeight = bitmapInfoHeader.biHeight;

	// Calculate the number of vertices in the terrain mesh.
	m_vertexCount = (m_terrainWidth - 1) * (m_terrainHeight - 1) * 6;
	m_indexCount = m_vertexCount;

	// Calculate the size of the bitmap image data.
	imageSize = m_terrainWidth * m_terrainHeight * 3;

	// Allocate memory for the bitmap image data.
	bitmapImage = new unsigned char[imageSize];
	if (!bitmapImage) {
		return false;
	}

	// Move to the beginning of the bitmap data.
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	// Read in the bitmap image data.
	count = fread(bitmapImage, 1, imageSize, filePtr);
	if (count != imageSize) {
		return false;
	}

	// Close the file.
	error = fclose(filePtr);
	if (error != 0) {
		return false;
	}

	// Create the structure to hold the height map data.
	m_heightMap = new HeightMapType[m_terrainWidth * m_terrainHeight];
	if (!m_heightMap) {
		return false;
	}

	// Initialize the position in the image data buffer.
	k = 0;

	m_Min = D3DXVECTOR3(FLT_MAX, FLT_MAX, FLT_MAX);
	m_Max = D3DXVECTOR3(FLT_MIN, FLT_MIN, FLT_MIN);
	m_origScale = scale;
	m_origPosition = position;

	// Read the image data into the height map.
	for (j = 0; j < m_terrainHeight; j++) {
		for (i = 0; i < m_terrainWidth; i++) {
			height = bitmapImage[k];

			index = (m_terrainHeight * j) + i;

			m_heightMap[index].x = (float)i * scale.x + position.x;
			m_heightMap[index].y = (float)height * scale.y + position.y;
			m_heightMap[index].z = (float)j * scale.z + position.z;

			if (m_heightMap[index].x > m_Max.x) {
				m_Max.x = m_heightMap[index].x;
			}
			if (m_heightMap[index].y > m_Max.y) {
				m_Max.y = m_heightMap[index].y;
			}
			if (m_heightMap[index].z > m_Max.z) {
				m_Max.z = m_heightMap[index].z;
			}
			if (m_heightMap[index].x < m_Min.x) {
				m_Min.x = m_heightMap[index].x;
			}
			if (m_heightMap[index].y < m_Min.y) {
				m_Min.y = m_heightMap[index].y;
			}
			if (m_heightMap[index].z < m_Min.z) {
				m_Min.z = m_heightMap[index].z;
			}

			k += 3;
		}
	}

	// Release the bitmap image data.
	delete[] bitmapImage;
	bitmapImage = 0;


	// restore for normal worldmartix
	// for this variable separate var, as uses position in quad without matrix for calc position
	scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	position = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	return true;
}

bool TerrainClass::CalculateNormals()
{
	int i, j, index1, index2, index3, index, count;
	float vertex1[3], vertex2[3], vertex3[3], vector1[3], vector2[3], sum[3], length;
	VectorType* normals;

	// Create a temporary array to hold the un-normalized normal vectors.
	normals = new VectorType[(m_terrainHeight - 1) * (m_terrainWidth - 1)];
	if (!normals) {
		return false;
	}

	// Go through all the faces in the mesh and calculate their normals.
	for (j = 0; j < (m_terrainHeight - 1); j++) {
		for (i = 0; i < (m_terrainWidth - 1); i++) {
			index1 = (j * m_terrainHeight) + i;
			index2 = (j * m_terrainHeight) + (i + 1);
			index3 = ((j + 1) * m_terrainHeight) + i;

			// Get three vertices from the face.
			vertex1[0] = m_heightMap[index1].x * m_scaleNormal.x;
			vertex1[1] = m_heightMap[index1].y * m_scaleNormal.y;
			vertex1[2] = m_heightMap[index1].z * m_scaleNormal.z;

			vertex2[0] = m_heightMap[index2].x * m_scaleNormal.x;
			vertex2[1] = m_heightMap[index2].y * m_scaleNormal.y;
			vertex2[2] = m_heightMap[index2].z * m_scaleNormal.z;

			vertex3[0] = m_heightMap[index3].x * m_scaleNormal.x;
			vertex3[1] = m_heightMap[index3].y * m_scaleNormal.y;
			vertex3[2] = m_heightMap[index3].z * m_scaleNormal.z;

			// Calculate the two vectors for this face.
			vector1[0] = vertex1[0] - vertex3[0];
			vector1[1] = vertex1[1] - vertex3[1];
			vector1[2] = vertex1[2] - vertex3[2];
			vector2[0] = vertex3[0] - vertex2[0];
			vector2[1] = vertex3[1] - vertex2[1];
			vector2[2] = vertex3[2] - vertex2[2];

			index = (j * (m_terrainHeight - 1)) + i;

			// Calculate the cross product of those two vectors to get the un-normalized value for this face normal.
			normals[index].x = (vector1[1] * vector2[2]) - (vector1[2] * vector2[1]);
			normals[index].y = (vector1[2] * vector2[0]) - (vector1[0] * vector2[2]);
			normals[index].z = (vector1[0] * vector2[1]) - (vector1[1] * vector2[0]);
		}
	}

	// Now go through all the vertices and take an average of each face normal 	
	// that the vertex touches to get the averaged normal for that vertex.
	for (j = 0; j < m_terrainHeight; j++) {
		for (i = 0; i < m_terrainWidth; i++) {
			// Initialize the sum.
			sum[0] = 0.0f;
			sum[1] = 0.0f;
			sum[2] = 0.0f;

			// Initialize the count.
			count = 0;

			// Bottom left face.
			if (((i - 1) >= 0) && ((j - 1) >= 0)) {
				index = ((j - 1) * (m_terrainHeight - 1)) + (i - 1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Bottom right face.
			if ((i < (m_terrainWidth - 1)) && ((j - 1) >= 0)) {
				index = ((j - 1) * (m_terrainHeight - 1)) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Upper left face.
			if (((i - 1) >= 0) && (j < (m_terrainHeight - 1))) {
				index = (j * (m_terrainHeight - 1)) + (i - 1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Upper right face.
			if ((i < (m_terrainWidth - 1)) && (j < (m_terrainHeight - 1))) {
				index = (j * (m_terrainHeight - 1)) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Take the average of the faces touching this vertex.
			sum[0] = (sum[0] / (float)count);
			sum[1] = (sum[1] / (float)count);
			sum[2] = (sum[2] / (float)count);

			// Calculate the length of this normal.
			length = sqrt((sum[0] * sum[0]) + (sum[1] * sum[1]) + (sum[2] * sum[2]));

			// Get an index to the vertex location in the height map array.
			index = (j * m_terrainHeight) + i;

			// Normalize the final shared normal for this vertex and store it in the height map array.
			m_heightMap[index].nx = (sum[0] / length);
			m_heightMap[index].ny = (sum[1] / length);
			m_heightMap[index].nz = (sum[2] / length);
		}
	}

	// Release the temporary normals.
	delete[] normals;
	normals = 0;

	return true;
}

void TerrainClass::CalculateTextureCoordinates()
{
	int incrementCount, i, j, tuCount, tvCount;
	float incrementValue, tuCoordinate, tvCoordinate;
	float textureRepeat = getTextureRepeat();


	// Calculate how much to increment the texture coordinates by.
	incrementValue = textureRepeat / (float)m_terrainWidth;

	// Calculate how many times to repeat the texture.
	incrementCount = m_terrainWidth / textureRepeat;

	// Initialize the tu and tv coordinate values.
	tuCoordinate = 0.0f;
	tvCoordinate = 1.0f;

	// Initialize the tu and tv coordinate indexes.
	tuCount = 0;
	tvCount = 0;

	// Loop through the entire height map and calculate the tu and tv texture coordinates for each vertex.
	for (j = 0; j < m_terrainHeight; j++) {
		for (i = 0; i < m_terrainWidth; i++) {
			// Store the texture coordinate in the height map.
			m_heightMap[(m_terrainHeight * j) + i].tu = tuCoordinate;
			m_heightMap[(m_terrainHeight * j) + i].tv = tvCoordinate;

			// Increment the tu texture coordinate by the increment value and increment the index by one.
			tuCoordinate += incrementValue;
			tuCount++;

			// Check if at the far right end of the texture and if so then start at the beginning again.
			if (tuCount == incrementCount) {
				tuCoordinate = 0.0f;
				tuCount = 0;
			}
		}

		// Increment the tv texture coordinate by the increment value and increment the index by one.
		tvCoordinate -= incrementValue;
		tvCount++;

		// Check if at the top of the texture and if so then start at the bottom again.
		if (tvCount == incrementCount) {
			tvCoordinate = 1.0f;
			tvCount = 0;
		}
	}
}

bool TerrainClass::BuildTerrainModel()
{
	int i, j, index, index1, index2, index3, index4;
	float incrementSize, tu2Left, tu2Right, tv2Top, tv2Bottom;

	// Create the terrain model array.
	m_TerrainModel = new HeightMapType[m_vertexCount];
	if (!m_TerrainModel) {
		return false;
	}

	incrementSize = 1.0f / (m_terrainWidth - 1.0f);
	tu2Left = 0.0f;
	tu2Right = incrementSize;
	tv2Bottom = 1.0f;
	tv2Top = 1.0f - incrementSize;

	// Load the terrain model with the height map terrain data.
	index = 0;

	for (j = 0; j < (m_terrainHeight - 1); j++) {
		for (i = 0; i < (m_terrainWidth - 1); i++) {
			index1 = (m_terrainWidth * j) + i;          // Bottom left.
			index2 = (m_terrainWidth * j) + (i + 1);      // Bottom right.
			index3 = (m_terrainWidth * (j + 1)) + i;      // Upper left.
			index4 = (m_terrainWidth * (j + 1)) + (i + 1);  // Upper right.

			// Upper left.
			m_TerrainModel[index].x = m_heightMap[index3].x;
			m_TerrainModel[index].y = m_heightMap[index3].y;
			m_TerrainModel[index].z = m_heightMap[index3].z;
			m_TerrainModel[index].nx = m_heightMap[index3].nx;
			m_TerrainModel[index].ny = m_heightMap[index3].ny;
			m_TerrainModel[index].nz = m_heightMap[index3].nz;
			m_TerrainModel[index].tu = 0.0f;
			m_TerrainModel[index].tv = 0.0f;
			m_TerrainModel[index].tu2 = tu2Left;
			m_TerrainModel[index].tv2 = tv2Top;
			index++;

			// Upper right.
			m_TerrainModel[index].x = m_heightMap[index4].x;
			m_TerrainModel[index].y = m_heightMap[index4].y;
			m_TerrainModel[index].z = m_heightMap[index4].z;
			m_TerrainModel[index].nx = m_heightMap[index4].nx;
			m_TerrainModel[index].ny = m_heightMap[index4].ny;
			m_TerrainModel[index].nz = m_heightMap[index4].nz;
			m_TerrainModel[index].tu = 1.0f;
			m_TerrainModel[index].tv = 0.0f;
			m_TerrainModel[index].tu2 = tu2Right;
			m_TerrainModel[index].tv2 = tv2Top;
			index++;

			// Bottom left.
			m_TerrainModel[index].x = m_heightMap[index1].x;
			m_TerrainModel[index].y = m_heightMap[index1].y;
			m_TerrainModel[index].z = m_heightMap[index1].z;
			m_TerrainModel[index].nx = m_heightMap[index1].nx;
			m_TerrainModel[index].ny = m_heightMap[index1].ny;
			m_TerrainModel[index].nz = m_heightMap[index1].nz;
			m_TerrainModel[index].tu = 0.0f;
			m_TerrainModel[index].tv = 1.0f;
			m_TerrainModel[index].tu2 = tu2Left;
			m_TerrainModel[index].tv2 = tv2Bottom;
			index++;

			// Bottom left.
			m_TerrainModel[index].x = m_heightMap[index1].x;
			m_TerrainModel[index].y = m_heightMap[index1].y;
			m_TerrainModel[index].z = m_heightMap[index1].z;
			m_TerrainModel[index].nx = m_heightMap[index1].nx;
			m_TerrainModel[index].ny = m_heightMap[index1].ny;
			m_TerrainModel[index].nz = m_heightMap[index1].nz;
			m_TerrainModel[index].tu = 0.0f;
			m_TerrainModel[index].tv = 1.0f;
			m_TerrainModel[index].tu2 = tu2Left;
			m_TerrainModel[index].tv2 = tv2Bottom;
			index++;

			// Upper right.
			m_TerrainModel[index].x = m_heightMap[index4].x;
			m_TerrainModel[index].y = m_heightMap[index4].y;
			m_TerrainModel[index].z = m_heightMap[index4].z;
			m_TerrainModel[index].nx = m_heightMap[index4].nx;
			m_TerrainModel[index].ny = m_heightMap[index4].ny;
			m_TerrainModel[index].nz = m_heightMap[index4].nz;
			m_TerrainModel[index].tu = 1.0f;
			m_TerrainModel[index].tv = 0.0f;
			m_TerrainModel[index].tu2 = tu2Right;
			m_TerrainModel[index].tv2 = tv2Top;
			index++;

			// Bottom right.
			m_TerrainModel[index].x = m_heightMap[index2].x;
			m_TerrainModel[index].y = m_heightMap[index2].y;
			m_TerrainModel[index].z = m_heightMap[index2].z;
			m_TerrainModel[index].nx = m_heightMap[index2].nx;
			m_TerrainModel[index].ny = m_heightMap[index2].ny;
			m_TerrainModel[index].nz = m_heightMap[index2].nz;
			m_TerrainModel[index].tu = 1.0f;
			m_TerrainModel[index].tv = 1.0f;
			m_TerrainModel[index].tu2 = tu2Right;
			m_TerrainModel[index].tv2 = tv2Bottom;
			index++;

			// Increment the tu texture coords for the alpha map.
			tu2Left += incrementSize;
			tu2Right += incrementSize;
		}

		// Reset the tu texture coordinate increments for the alpha map.
		tu2Left = 0.0f;
		tu2Right = incrementSize;

		// Increment the tv texture coords for the alpha map.
		tv2Top -= incrementSize;
		tv2Bottom -= incrementSize;
	}

	return true;
}

void TerrainClass::CalculateTerrainVectors()
{
	int faceCount, i, index;
	HeightMapType vertex1, vertex2, vertex3;
	VectorType tangent, binormal;


	// Calculate the number of faces in the terrain model.
	faceCount = m_vertexCount / 3;

	// Initialize the index to the model data.
	index = 0;

	// Go through all the faces and calculate the the tangent, binormal, and normal vectors.
	for (i = 0; i < faceCount; i++) {
		// Get the three vertices for this face from the terrain model.
		vertex1.x = m_TerrainModel[index].x;
		vertex1.y = m_TerrainModel[index].y;
		vertex1.z = m_TerrainModel[index].z;
		vertex1.tu = m_TerrainModel[index].tu;
		vertex1.tv = m_TerrainModel[index].tv;
		vertex1.nx = m_TerrainModel[index].nx;
		vertex1.ny = m_TerrainModel[index].ny;
		vertex1.nz = m_TerrainModel[index].nz;
		index++;

		vertex2.x = m_TerrainModel[index].x;
		vertex2.y = m_TerrainModel[index].y;
		vertex2.z = m_TerrainModel[index].z;
		vertex2.tu = m_TerrainModel[index].tu;
		vertex2.tv = m_TerrainModel[index].tv;
		vertex2.nx = m_TerrainModel[index].nx;
		vertex2.ny = m_TerrainModel[index].ny;
		vertex2.nz = m_TerrainModel[index].nz;
		index++;

		vertex3.x = m_TerrainModel[index].x;
		vertex3.y = m_TerrainModel[index].y;
		vertex3.z = m_TerrainModel[index].z;
		vertex3.tu = m_TerrainModel[index].tu;
		vertex3.tv = m_TerrainModel[index].tv;
		vertex3.nx = m_TerrainModel[index].nx;
		vertex3.ny = m_TerrainModel[index].ny;
		vertex3.nz = m_TerrainModel[index].nz;
		index++;

		// Calculate the tangent and binormal of that face.
		CalculateTangentBinormal(vertex1, vertex2, vertex3, tangent, binormal);

		// Store the tangent and binormal for this face back in the model structure.
		m_TerrainModel[index - 1].tx = tangent.x;
		m_TerrainModel[index - 1].ty = tangent.y;
		m_TerrainModel[index - 1].tz = tangent.z;
		m_TerrainModel[index - 1].bx = binormal.x;
		m_TerrainModel[index - 1].by = binormal.y;
		m_TerrainModel[index - 1].bz = binormal.z;

		m_TerrainModel[index - 2].tx = tangent.x;
		m_TerrainModel[index - 2].ty = tangent.y;
		m_TerrainModel[index - 2].tz = tangent.z;
		m_TerrainModel[index - 2].bx = binormal.x;
		m_TerrainModel[index - 2].by = binormal.y;
		m_TerrainModel[index - 2].bz = binormal.z;

		m_TerrainModel[index - 3].tx = tangent.x;
		m_TerrainModel[index - 3].ty = tangent.y;
		m_TerrainModel[index - 3].tz = tangent.z;
		m_TerrainModel[index - 3].bx = binormal.x;
		m_TerrainModel[index - 3].by = binormal.y;
		m_TerrainModel[index - 3].bz = binormal.z;
	}
}


void TerrainClass::CalculateTangentBinormal(HeightMapType vertex1, HeightMapType vertex2, HeightMapType vertex3, VectorType& tangent,
	VectorType& binormal)
{
	float vector1[3], vector2[3];
	float tuVector[2], tvVector[2];
	float den;
	float length;


	// Calculate the two vectors for this face.
	vector1[0] = vertex2.x - vertex1.x;
	vector1[1] = vertex2.y - vertex1.y;
	vector1[2] = vertex2.z - vertex1.z;

	vector2[0] = vertex3.x - vertex1.x;
	vector2[1] = vertex3.y - vertex1.y;
	vector2[2] = vertex3.z - vertex1.z;

	// Calculate the tu and tv texture space vectors.
	tuVector[0] = vertex2.tu - vertex1.tu;
	tvVector[0] = vertex2.tv - vertex1.tv;

	tuVector[1] = vertex3.tu - vertex1.tu;
	tvVector[1] = vertex3.tv - vertex1.tv;

	// Calculate the denominator of the tangent/binormal equation.
	den = 1.0f / (tuVector[0] * tvVector[1] - tuVector[1] * tvVector[0]);

	// Calculate the cross products and multiply by the coefficient to get the tangent and binormal.
	tangent.x = (tvVector[1] * vector1[0] - tvVector[0] * vector2[0]) * den;
	tangent.y = (tvVector[1] * vector1[1] - tvVector[0] * vector2[1]) * den;
	tangent.z = (tvVector[1] * vector1[2] - tvVector[0] * vector2[2]) * den;

	binormal.x = (tuVector[0] * vector2[0] - tuVector[1] * vector1[0]) * den;
	binormal.y = (tuVector[0] * vector2[1] - tuVector[1] * vector1[1]) * den;
	binormal.z = (tuVector[0] * vector2[2] - tuVector[1] * vector1[2]) * den;

	// Calculate the length of this normal.
	length = sqrt((tangent.x * tangent.x) + (tangent.y * tangent.y) + (tangent.z * tangent.z));

	// Normalize the normal and then store it
	tangent.x = tangent.x / length;
	tangent.y = tangent.y / length;
	tangent.z = tangent.z / length;

	// Calculate the length of this normal.
	length = sqrt((binormal.x * binormal.x) + (binormal.y * binormal.y) + (binormal.z * binormal.z));

	// Normalize the normal and then store it
	binormal.x = binormal.x / length;
	binormal.y = binormal.y / length;
	binormal.z = binormal.z / length;
}

bool TerrainClass::InitializeBuffers(ID3D11Device* device)
{
	int index, i, j, index1, index2, index3, index4;
	float tu, tv;

	// Create the vertex array.
	m_vertices = new VertexType[m_vertexCount];
	if (!m_vertices) {
		return false;
	}

	// Initialize the index to the vertex buffer.
	index = 0;

	// Load the vertex and index array with the terrain data.
	for (j = 0; j < (m_terrainHeight - 1); j++) {
		for (i = 0; i < (m_terrainWidth - 1); i++) {
			index1 = (m_terrainHeight * j) + i;          // Bottom left.
			index2 = (m_terrainHeight * j) + (i + 1);      // Bottom right.
			index3 = (m_terrainHeight * (j + 1)) + i;      // Upper left.
			index4 = (m_terrainHeight * (j + 1)) + (i + 1);  // Upper right.

			// Upper left.
			tv = m_heightMap[index3].tv;

			// Modify the texture coordinates to cover the top edge.
			if (tv == 1.0f) { 
				tv = 0.0f; 
			}

			m_vertices[index].position = D3DXVECTOR3(m_heightMap[index3].x, m_heightMap[index3].y, m_heightMap[index3].z);
			m_vertices[index].texture = D3DXVECTOR4(m_heightMap[index3].tu, tv, 0.0f, 0.0f);
			m_vertices[index].normal = D3DXVECTOR3(m_heightMap[index3].nx, m_heightMap[index3].ny, m_heightMap[index3].nz);
			//m_vertices[index].tangent = D3DXVECTOR3(m_heightMap[index3].tx, m_heightMap[index3].ty, m_heightMap[index3].tz);
			//m_vertices[index].binormal = D3DXVECTOR3(m_heightMap[index3].bx, m_heightMap[index3].by, m_heightMap[index3].bz);
			m_vertices[index].tangent = D3DXVECTOR3(m_TerrainModel[index].tx, m_TerrainModel[index].ty, m_TerrainModel[index].tz);
			m_vertices[index].binormal = D3DXVECTOR3(m_TerrainModel[index].bx, m_TerrainModel[index].by, m_TerrainModel[index].bz);
			m_vertices[index].texture2 = D3DXVECTOR2(m_TerrainModel[index].tu2, m_TerrainModel[index].tv2);
			index++;

			// Upper right.
			tu = m_heightMap[index4].tu;
			tv = m_heightMap[index4].tv;

			// Modify the texture coordinates to cover the top and right edge.
			if (tu == 0.0f) { 
				tu = 1.0f; 
			}
			if (tv == 1.0f) { 
				tv = 0.0f;
			}

			m_vertices[index].position = D3DXVECTOR3(m_heightMap[index4].x, m_heightMap[index4].y, m_heightMap[index4].z);
			m_vertices[index].texture = D3DXVECTOR4(tu, tv, 1.0f, 0.0f);
			m_vertices[index].normal = D3DXVECTOR3(m_heightMap[index4].nx, m_heightMap[index4].ny, m_heightMap[index4].nz);
			//m_vertices[index].tangent = D3DXVECTOR3(m_heightMap[index4].tx, m_heightMap[index4].ty, m_heightMap[index4].tz);
			//m_vertices[index].binormal = D3DXVECTOR3(m_heightMap[index4].bx, m_heightMap[index4].by, m_heightMap[index4].bz);
			m_vertices[index].tangent = D3DXVECTOR3(m_TerrainModel[index].tx, m_TerrainModel[index].ty, m_TerrainModel[index].tz);
			m_vertices[index].binormal = D3DXVECTOR3(m_TerrainModel[index].bx, m_TerrainModel[index].by, m_TerrainModel[index].bz);
			m_vertices[index].texture2 = D3DXVECTOR2(m_TerrainModel[index].tu2, m_TerrainModel[index].tv2);
			index++;

			// Bottom left.
			m_vertices[index].position = D3DXVECTOR3(m_heightMap[index1].x, m_heightMap[index1].y, m_heightMap[index1].z);
			m_vertices[index].texture = D3DXVECTOR4(m_heightMap[index1].tu, m_heightMap[index1].tv, 0.0f, 1.0f);
			m_vertices[index].normal = D3DXVECTOR3(m_heightMap[index1].nx, m_heightMap[index1].ny, m_heightMap[index1].nz);
			//m_vertices[index].tangent = D3DXVECTOR3(m_heightMap[index1].tx, m_heightMap[index1].ty, m_heightMap[index1].tz);
			//m_vertices[index].binormal = D3DXVECTOR3(m_heightMap[index1].bx, m_heightMap[index1].by, m_heightMap[index1].bz);
			m_vertices[index].tangent = D3DXVECTOR3(m_TerrainModel[index].tx, m_TerrainModel[index].ty, m_TerrainModel[index].tz);
			m_vertices[index].binormal = D3DXVECTOR3(m_TerrainModel[index].bx, m_TerrainModel[index].by, m_TerrainModel[index].bz);
			m_vertices[index].texture2 = D3DXVECTOR2(m_TerrainModel[index].tu2, m_TerrainModel[index].tv2);
			index++;

			// Bottom left.
			m_vertices[index].position = D3DXVECTOR3(m_heightMap[index1].x, m_heightMap[index1].y, m_heightMap[index1].z);
			m_vertices[index].texture = D3DXVECTOR4(m_heightMap[index1].tu, m_heightMap[index1].tv, 0.0f, 1.0f);
			m_vertices[index].normal = D3DXVECTOR3(m_heightMap[index1].nx, m_heightMap[index1].ny, m_heightMap[index1].nz);
			//m_vertices[index].tangent = D3DXVECTOR3(m_heightMap[index1].tx, m_heightMap[index1].ty, m_heightMap[index1].tz);
			//m_vertices[index].binormal = D3DXVECTOR3(m_heightMap[index1].bx, m_heightMap[index1].by, m_heightMap[index1].bz);
			m_vertices[index].tangent = D3DXVECTOR3(m_TerrainModel[index].tx, m_TerrainModel[index].ty, m_TerrainModel[index].tz);
			m_vertices[index].binormal = D3DXVECTOR3(m_TerrainModel[index].bx, m_TerrainModel[index].by, m_TerrainModel[index].bz);
			m_vertices[index].texture2 = D3DXVECTOR2(m_TerrainModel[index].tu2, m_TerrainModel[index].tv2);
			index++;

			// Upper right.
			tu = m_heightMap[index4].tu;
			tv = m_heightMap[index4].tv;

			// Modify the texture coordinates to cover the top and right edge.
			if (tu == 0.0f) { 
				tu = 1.0f; 
			}
			if (tv == 1.0f) { 
				tv = 0.0f; 
			}

			m_vertices[index].position = D3DXVECTOR3(m_heightMap[index4].x, m_heightMap[index4].y, m_heightMap[index4].z);
			m_vertices[index].texture = D3DXVECTOR4(tu, tv, 1.0f, 0.0f);
			m_vertices[index].normal = D3DXVECTOR3(m_heightMap[index4].nx, m_heightMap[index4].ny, m_heightMap[index4].nz);
			//m_vertices[index].tangent = D3DXVECTOR3(m_heightMap[index4].tx, m_heightMap[index4].ty, m_heightMap[index4].tz);
			//m_vertices[index].binormal = D3DXVECTOR3(m_heightMap[index4].bx, m_heightMap[index4].by, m_heightMap[index4].bz);
			m_vertices[index].tangent = D3DXVECTOR3(m_TerrainModel[index].tx, m_TerrainModel[index].ty, m_TerrainModel[index].tz);
			m_vertices[index].binormal = D3DXVECTOR3(m_TerrainModel[index].bx, m_TerrainModel[index].by, m_TerrainModel[index].bz);
			m_vertices[index].texture2 = D3DXVECTOR2(m_TerrainModel[index].tu2, m_TerrainModel[index].tv2);
			index++;

			// Bottom right.
			tu = m_heightMap[index2].tu;

			// Modify the texture coordinates to cover the right edge.
			if (tu == 0.0f) { 
				tu = 1.0f;
			}

			m_vertices[index].position = D3DXVECTOR3(m_heightMap[index2].x, m_heightMap[index2].y, m_heightMap[index2].z);
			m_vertices[index].texture = D3DXVECTOR4(tu, m_heightMap[index2].tv, 1.0f, 1.0f);
			m_vertices[index].normal = D3DXVECTOR3(m_heightMap[index2].nx, m_heightMap[index2].ny, m_heightMap[index2].nz);
			//m_vertices[index].tangent = D3DXVECTOR3(m_heightMap[index2].tx, m_heightMap[index2].ty, m_heightMap[index2].tz);
			//m_vertices[index].binormal = D3DXVECTOR3(m_heightMap[index2].bx, m_heightMap[index2].by, m_heightMap[index2].bz);
			m_vertices[index].tangent = D3DXVECTOR3(m_TerrainModel[index].tx, m_TerrainModel[index].ty, m_TerrainModel[index].tz);
			m_vertices[index].binormal = D3DXVECTOR3(m_TerrainModel[index].bx, m_TerrainModel[index].by, m_TerrainModel[index].bz);
			m_vertices[index].texture2 = D3DXVECTOR2(m_TerrainModel[index].tu2, m_TerrainModel[index].tv2);
			index++;
		}
	}

	delete[] m_TerrainModel;
	m_TerrainModel = 0;

	return true;
}

void TerrainClass::CopyVertexArray(void* vertexList)
{
	memcpy(vertexList, m_vertices, sizeof(VertexType) * m_vertexCount);
}

void TerrainClass::Render(CameraClass* camera)
{
	Render(camera, 0);
}

void TerrainClass::Render(CameraClass* camera, ID3D11ShaderResourceView* texture)
{
	D3DXMATRIX viewMatrix, projectionMatrix;

	camera->GetViewMatrix(viewMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	if (m_BBox) {
		m_BBox->Render();
	}

	TerrainShaderClass* shader = dynamic_cast<TerrainShaderClass*>(m_shader);
	shader->SetShaderParameters(m_D3D->GetDeviceContext(), GetWorldMatrix(), viewMatrix, projectionMatrix, getLight(0), m_TextureArray, texture);
	m_quadTree->Render(m_shader);
}