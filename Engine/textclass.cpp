#include "textclass.h"
#include "Options.h"

TextClass::TextClass()
{
	m_Font = 0;
	m_FontShader = 0;

	m_sentence = 0;
	m_text = "";
}


TextClass::TextClass(const TextClass& other)
{
}


TextClass::~TextClass()
{
}

bool TextClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int screenWidth, int screenHeight)
{
	bool result;

	m_device = device;
	m_deviceContext = deviceContext;

	// Create the font object.
	m_Font = new FontClass;
	if (!m_Font) {
		return false;
	}

	// Initialize the font object.
	result = m_Font->Initialize(device, "data/fontdata.txt", L"data/font.dds");
	if (!result) {
		MessageBox(NULL, L"Could not initialize the font object.", L"Error", MB_OK);
		return false;
	}

	// Create the font shader object.
	m_FontShader = new FontShaderClass;
	if (!m_FontShader) {
		return false;
	}

	// Initialize the font shader object.
	result = m_FontShader->Initialize(device);
	if (!result) {
		MessageBox(NULL, L"Could not initialize the font shader object.", L"Error", MB_OK);
		return false;
	}

	// Initialize the first sentence.
	result = InitializeSentence(&m_sentence, 128, m_device);
	if (!result) {
		return false;
	}

	return true;
}

bool TextClass::AddText(std::string text, int positionX, int positionY, float red, float green, float blue)
{
	bool result;

	m_text = text;

	// Now update the sentence vertex buffer with the new string information.
	result = UpdateSentence(m_sentence, &m_text[0], positionX, positionY, red, green, blue, m_deviceContext);
	if (!result) {
		return false;
	}

	updateRectangle(positionX, positionY);
}

void TextClass::Shutdown()
{
	// Release the first sentence.
	ReleaseSentence(&m_sentence);

	// Release the font shader object.
	if (m_FontShader) {
		m_FontShader->Shutdown();
		delete m_FontShader;
		m_FontShader = 0;
	}

	// Release the font object.
	if (m_Font) {
		m_Font->Shutdown();
		delete m_Font;
		m_Font = 0;
	}
}

bool TextClass::Render(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX orthoMatrix, D3DXMATRIX viewMatrix)
{
	bool result;

	// Draw the first sentence.
	result = RenderSentence(deviceContext, m_sentence, worldMatrix, orthoMatrix, viewMatrix);
	if (!result) {
		return false;
	}

	return true;
}

bool TextClass::InitializeSentence(SentenceType** sentence, int maxLength, ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;


	// Create a new sentence object.
	*sentence = new SentenceType;
	if (!*sentence) {
		return false;
	}

	// Initialize the sentence buffers to null.
	(*sentence)->vertexBuffer = 0;
	(*sentence)->indexBuffer = 0;

	// Set the maximum length of the sentence.
	(*sentence)->maxLength = maxLength;

	// Set the number of vertices in the vertex array.
	(*sentence)->vertexCount = 6 * maxLength;

	// Set the number of indexes in the index array.
	(*sentence)->indexCount = (*sentence)->vertexCount;

	// Create the vertex array.
	vertices = new VertexType[(*sentence)->vertexCount];
	if (!vertices) {
		return false;
	}

	// Create the index array.
	indices = new unsigned long[(*sentence)->indexCount];
	if (!indices) {
		return false;
	}

	// Initialize vertex array to zeros at first.
	memset(vertices, 0, (sizeof(VertexType) * (*sentence)->vertexCount));

	// Initialize the index array.
	for (i = 0; i < (*sentence)->indexCount; i++) {
		indices[i] = i;
	}

	// Set up the description of the dynamic vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * (*sentence)->vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &(*sentence)->vertexBuffer);
	if (FAILED(result)) {
		return false;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * (*sentence)->indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &(*sentence)->indexBuffer);
	if (FAILED(result)) {
		return false;
	}

	// Release the vertex array as it is no longer needed.
	delete[] vertices;
	vertices = 0;

	// Release the index array as it is no longer needed.
	delete[] indices;
	indices = 0;

	return true;
}

bool TextClass::UpdateSentence(SentenceType* sentence, std::string text, int positionX, int positionY, float red, float green, float blue,
	ID3D11DeviceContext* deviceContext)
{
	int numLetters;
	VertexType* vertices;
	float drawX, drawY;
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* verticesPtr;

	// Store the color of the sentence.
	sentence->red = red;
	sentence->green = green;
	sentence->blue = blue;

	// Get the number of letters in the sentence.
	numLetters = text.size();

	// Check for possible buffer overflow.
	if (numLetters > sentence->maxLength) {
		return false;
	}

	// Create the vertex array.
	vertices = new VertexType[sentence->vertexCount];
	if (!vertices) {
		return false;
	}

	// Initialize vertex array to zeros at first.
	memset(vertices, 0, (sizeof(VertexType) * sentence->vertexCount));

	// Calculate the X and Y pixel position on the screen to start drawing to.
	drawX = (float)(((Options::screen_width / 2) * -1) + positionX);
	drawY = (float)((Options::screen_height / 2) - positionY);

	// Use the font class to build the vertex array from the sentence text and sentence draw location.
	m_Font->BuildVertexArray((void*)vertices, &text[0], drawX, drawY);

	// Lock the vertex buffer so it can be written to.
	result = deviceContext->Map(sentence->vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)) {
		return false;
	}

	// Get a pointer to the data in the vertex buffer.
	verticesPtr = (VertexType*)mappedResource.pData;

	// Copy the data into the vertex buffer.
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * sentence->vertexCount));

	// Unlock the vertex buffer.
	deviceContext->Unmap(sentence->vertexBuffer, 0);

	// Release the vertex array as it is no longer needed.
	delete[] vertices;
	vertices = 0;

	return true;
}

void TextClass::updateRectangle(float positionX, float positionY)
{
	m_rectangle.position = new ds::math::Point(positionX, positionY);
	m_rectangle.width = m_rectangle.position->x + m_text.size() * 6.3;
	m_rectangle.height = 10;
}

void TextClass::ReleaseSentence(SentenceType** sentence)
{
	if (*sentence) {
		// Release the sentence vertex buffer.
		if ((*sentence)->vertexBuffer) {
			(*sentence)->vertexBuffer->Release();
			(*sentence)->vertexBuffer = 0;
		}

		// Release the sentence index buffer.
		if ((*sentence)->indexBuffer) {
			(*sentence)->indexBuffer->Release();
			(*sentence)->indexBuffer = 0;
		}

		// Release the sentence.
		delete* sentence;
		*sentence = 0;
	}
}

bool TextClass::RenderSentence(ID3D11DeviceContext* deviceContext, SentenceType* sentence, D3DXMATRIX worldMatrix, D3DXMATRIX orthoMatrix, D3DXMATRIX viewMatrix)
{
	unsigned int stride, offset;
	D3DXVECTOR4 pixelColor;
	bool result;

	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &sentence->vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(sentence->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Create a pixel color vector with the input sentence color.
	pixelColor = D3DXVECTOR4(sentence->red, sentence->green, sentence->blue, 1.0f);

	// Render the text using the font shader.
	result = m_FontShader->Render(deviceContext, sentence->indexCount, worldMatrix, viewMatrix, orthoMatrix, m_Font->GetTexture(),
		pixelColor);
	if (!result) {
		false;
	}

	return true;
}