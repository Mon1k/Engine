#include "modelclass.h"


ModelClass::ModelClass()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_Texture = 0;
	m_model = 0;
}


ModelClass::ModelClass(const ModelClass& other)
{
}


ModelClass::~ModelClass()
{
}


bool ModelClass::Initialize(ID3D11Device* device, char* modelFilename, WCHAR* textureFilename)
{
	bool result;

	// Load in the model data,
	result = LoadModel(modelFilename);
	if (!result) {
		return false;
	}

	// Initialize the vertex and index buffers.
	result = InitializeBuffers(device);
	if (!result) {
		return false;
	}

	// Load the texture for this model.
	if (wcslen(textureFilename) > 0) {
		result = LoadTexture(device, textureFilename);
		if (!result) {
			return false;
		}
	}

	return true;
}


void ModelClass::Shutdown()
{
	// Release the model texture.
	ReleaseTexture();

	// Shutdown the vertex and index buffers.
	ShutdownBuffers();

	// Release the model data.
	ReleaseModel();

	return;
}


void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);

	return;
}


int ModelClass::GetIndexCount()
{
	return m_indexCount;
}

ID3D11ShaderResourceView* ModelClass::GetTexture()
{
	return m_Texture->GetTexture();
}


bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;

	vertices = new VertexType[m_vertexCount];
	if (!vertices) {
		return false;
	}

	// Create the index array.
	indices = new unsigned long[m_indexCount];
	if (!indices) {
		return false;
	}

	// Load the vertex array and index array with data.
	for (i = 0; i < m_vertexCount; i++) {
		vertices[i].position = D3DXVECTOR3(m_model[i].x, m_model[i].y, m_model[i].z);
		vertices[i].texture = D3DXVECTOR2(m_model[i].tu, m_model[i].tv);
		vertices[i].normal = D3DXVECTOR3(m_model[i].nx, m_model[i].ny, m_model[i].nz);

		indices[i] = i;
	}

	// Set up the description of the static vertex buffer.
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
    result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result)) {
		return false;
	}

	// Set up the description of the static index buffer.
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
    indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result)) {
		return false;
	}

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;

	delete [] indices;
	indices = 0;

	return true;
}

bool ModelClass::LoadTexture(ID3D11Device* device, WCHAR* filename)
{
	bool result;

	// Create the texture object.
	m_Texture = new TextureClass;
	if (!m_Texture) {
		return false;
	}

	// Initialize the texture object.
	result = m_Texture->Initialize(device, filename);
	if (!result) {
		return false;
	}

	return true;
}

void ModelClass::ReleaseTexture()
{
	// Release the texture object.
	if (m_Texture) {
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}

	return;
}

bool ModelClass::LoadModel(char* filename)
{
	std::string string(filename);
	
	if (string.rfind(".ds") != std::string::npos) {
		return LoadModelDs(filename);
	} else if (string.rfind(".obj") != std::string::npos) {
		return LoadModelObj(filename);
	}

	return false;
}

bool ModelClass::LoadModelDs(char* filename)
{
	ifstream fin;
	char input;
	int i;


	// Open the model file.
	fin.open(filename);

	// If it could not open the file then exit.
	if (fin.fail()) {
		return false;
	}

	// Read up to the value of vertex count.
	fin.get(input);
	while (input != ':') {
		fin.get(input);
	}

	// Read in the vertex count.
	fin >> m_vertexCount;

	// Set the number of indices to be the same as the vertex count.
	m_indexCount = m_vertexCount;

	// Create the model using the vertex count that was read in.
	m_model = new ModelType[m_vertexCount];
	if (!m_model) {
		return false;
	}

	// Read up to the beginning of the data.
	fin.get(input);
	while (input != ':') {
		fin.get(input);
	}
	fin.get(input);
	fin.get(input);

	// Read in the vertex data.
	for (i = 0; i < m_vertexCount; i++) {
		fin >> m_model[i].x >> m_model[i].y >> m_model[i].z;
		fin >> m_model[i].tu >> m_model[i].tv;
		fin >> m_model[i].nx >> m_model[i].ny >> m_model[i].nz;
	}

	// Close the model file.
	fin.close();

	return true;
}

bool ModelClass::LoadModelObj(char* filename)
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
	m_indexCount = m_vertexCount = faceIndex * 3;
	m_model = new ModelType[m_vertexCount];
	if (!m_model) {
		return false;
	}

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
			m_model[index].x = vertices[vIndex].x;
			m_model[index].y = vertices[vIndex].y;
			m_model[index].z = vertices[vIndex].z;
			m_model[index].tu = texcoords[tIndex].x;
			m_model[index].tv = texcoords[tIndex].y;
			m_model[index].nx = normals[nIndex].x;
			m_model[index].ny = normals[nIndex].y;
			m_model[index].nz = normals[nIndex].z;
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

void ModelClass::ReleaseModel()
{
	if (m_model) {
		delete[] m_model;
		m_model = 0;
	}

	return;
}

void ModelClass::ShutdownBuffers()
{
	// Release the index buffer.
	if (m_indexBuffer) {
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// Release the vertex buffer.
	if (m_vertexBuffer) {
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}


void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType);
	offset = 0;
    
	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

    // Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}