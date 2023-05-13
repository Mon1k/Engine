#include "window.h"

Window::Window()
{
}

Window::~Window()
{
}

bool Window::Initialize(int screenWidth, int screenHeight, WCHAR* textureFilename, char* title, int width, int height, int positionX, int positionY)
{
	bool result;

	m_x = positionX;
	m_y = positionY;
	m_width = width;
	m_height = height;

	m_header = new Group();
	Group::addChild(m_header);
	m_header->m_x = m_x;
	m_header->m_y = m_y;
	m_header->m_width = width;
	m_header->m_height = 30;
	
	m_body = new Group();
	Group::addChild(m_body);
	m_body->m_x = m_x;
	m_body->m_y = m_y + m_header->m_height;
	m_body->m_width = width;
	m_body->m_height = height - m_header->m_height;
	
	Label* titleWindow = new Label;
	m_header->addChild(titleWindow);
	titleWindow->Initialize(screenWidth, screenHeight, m_header->m_width, m_header->m_height - 2);
	titleWindow->Add(title, m_header->m_x + 1, m_header->m_y + 1);
	


	// Create the texture shader object.
	m_TextureShader = new TextureShaderClass;
	if (!m_TextureShader) {
		return false;
	}

	// Initialize the texture shader object.
	result = m_TextureShader->Initialize(m_D3D->GetDevice());
	if (!result) {
		MessageBox(NULL, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the bitmap object.
	m_Bitmap = new BitmapClass;
	if (!m_Bitmap) {
		return false;
	}

	// Initialize the bitmap object.
	result = m_Bitmap->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight, textureFilename, m_width, m_height);
	if (!result) {
		MessageBox(NULL, L"Could not initialize the bitmap object.", L"Error", MB_OK);
		return false;
	}
}

void Window::addChild(AbstractGui* child)
{
	m_body->addChild(child);
}

void Window::Shutdown()
{
	if (m_Bitmap) {
		m_Bitmap->Shutdown();
		delete m_Bitmap;
		m_Bitmap = 0;
	}

	if (m_TextureShader) {
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}

	Group::Shutdown();
}

bool Window::Render()
{
	bool result;
	D3DXMATRIX worldMatrix, orthoMatrix;

	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetOrthoMatrix(orthoMatrix);

	m_Bitmap->Render(m_D3D->GetDeviceContext(), m_x, m_y);
	result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_Bitmap->GetIndexCount(), worldMatrix, m_baseViewMatrix, orthoMatrix, m_Bitmap->GetTexture());
	if (!result) {
		return false;
	}

	return Group::Render();
}