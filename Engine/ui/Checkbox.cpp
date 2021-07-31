#include "Checkbox.h"


Checkbox::Checkbox()
{
	m_D3D = 0;
	m_TextureShader = 0;
	m_Bitmap = 0;
	m_Bitmap_marked = 0;
	m_Text = 0;

	b_IsMarked = false;

	m_width = 0;
	m_height = 0;
	m_x = 0;
	m_y = 0;
}


Checkbox::~Checkbox()
{
}

bool Checkbox::Initialize(D3DClass* d3d, int screenWidth, int screenHeight, HWND hwnd, WCHAR* textureFilename, WCHAR* textureFilenameMarked, int bitmapWidth, int bitmapHeight, D3DXMATRIX baseViewMatrix)
{
	bool result;

	m_D3D = d3d;

	// Create the text object.
	m_Text = new TextClass;
	if (!m_Text) {
		return false;
	}

	// Initialize the text object.
	result = m_Text->Initialize(m_D3D->GetDevice(), m_D3D->GetDeviceContext(), hwnd, screenWidth, screenHeight, baseViewMatrix);
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the text object.", L"Error", MB_OK);
		return false;
	}

	// Create the texture shader object.
	m_TextureShader = new TextureShaderClass;
	if (!m_TextureShader) {
		return false;
	}

	// Initialize the texture shader object.
	result = m_TextureShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}


	m_width = bitmapWidth;
	m_height = bitmapHeight;

	// Create the bitmap object.
	m_Bitmap = new BitmapClass;
	m_Bitmap_marked = new BitmapClass;

	// Initialize the bitmap object.
	result = m_Bitmap->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight, textureFilename, m_width, m_height);
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the bitmap object.", L"Error", MB_OK);
		return false;
	}
	result = m_Bitmap_marked->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight, textureFilenameMarked, m_width, m_height);
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the bitmap marked object.", L"Error", MB_OK);
		return false;
	}
}


bool Checkbox::Add(char* text, int positionX, int positionY, float red, float green, float blue)
{
	m_x = positionX;
	m_y = positionY;

	return m_Text->AddText(text, m_x + m_width / 4 + m_width, m_y + m_height / 3, red, green, blue);
}

void Checkbox::Shutdown()
{
	// Release the text object.
	if (m_Text) {
		m_Text->Shutdown();
		delete m_Text;
		m_Text = 0;
	}

	// Release the bitmap object.
	if (m_Bitmap) {
		m_Bitmap->Shutdown();
		delete m_Bitmap;
		m_Bitmap = 0;
	}
	if (m_Bitmap_marked) {
		m_Bitmap_marked->Shutdown();
		delete m_Bitmap_marked;
		m_Bitmap_marked = 0;
	}

	// Release the texture shader object.
	if (m_TextureShader) {
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}
}

bool Checkbox::onButtonPress(int x, int y)
{
	if (x >= m_x && x <= m_x + m_width && y >= m_y && y <= m_y + m_height) {
		return true;
	}

	return false;
}

bool Checkbox::Render()
{
	bool result;
	D3DXMATRIX worldMatrix, orthoMatrix;
	BitmapClass* bitMapTemp;

	if (!visible) {
		return true;
	}

	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetOrthoMatrix(orthoMatrix);

	// Turn off the Z buffer to begin all 2D rendering.
	m_D3D->TurnZBufferOff();

	// Turn on the alpha blending before rendering the text.
	m_D3D->TurnOnAlphaBlending();

	if (b_IsMarked) {
		bitMapTemp = m_Bitmap_marked;
	} else {
		bitMapTemp = m_Bitmap;
	}
	bitMapTemp->Render(m_D3D->GetDeviceContext(), m_x, m_y);
	// Render the bitmap with the texture shader.
	result = m_TextureShader->Render(m_D3D->GetDeviceContext(), bitMapTemp->GetIndexCount(), worldMatrix, m_Text->GetViewMartix(), orthoMatrix, bitMapTemp->GetTexture());
	if (!result) {
		return false;
	}

	// Render the text strings.
	result = m_Text->Render(m_D3D->GetDeviceContext(), worldMatrix, orthoMatrix);
	if (!result) {
		return false;
	}
	// Turn off alpha blending after rendering the text.
	m_D3D->TurnOffAlphaBlending();

	// Turn the Z buffer back on now that all 2D rendering has completed.
	m_D3D->TurnZBufferOn();

	return true;
}