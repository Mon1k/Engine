#include "window.h"
#include "../Options.h"

Window::Window()
{
	m_header = 0;
	m_BitmapHeader = 0;

	m_body = 0;
	m_BitmapBody = 0;
}

Window::~Window()
{
}

bool Window::Initialize(int width, int height, int positionX, int positionY)
{
	bool result;

	m_x = positionX;
	m_y = positionY;
	m_width = width;
	m_height = height;

	m_TextureShader = new TextureShaderClass;
	result = m_TextureShader->Initialize(m_D3D->GetDevice());
	if (!result) {
		MessageBox(NULL, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	return true;
}

bool Window::addHeader(char* title, int flags)
{
	bool result;
	int rightShift = 0;
	int paddingX = 2, paddingY = 2;
	int headerHeight = 30;
	int headerHeightElm = headerHeight - paddingX - paddingY;

	m_header = new Group();
	Group::addChild(m_header);
	m_header->m_x = m_x;
	m_header->m_y = m_y;
	m_header->m_width = m_width;
	m_header->m_height = headerHeight;

	
	if (flags) {
		if (flags & this->HEADER_BUTTON_CLOSE) {
			Button* buttonClose = new Button;
			m_header->addChild(buttonClose);
			buttonClose->Initialize(L"data/textures/ui/button_close.png", headerHeightElm, headerHeightElm);
			buttonClose->m_x = m_header->m_x + m_header->m_width - headerHeightElm;
			buttonClose->m_y = m_header->m_y + paddingY;
			buttonClose->addEventHandler([this] { 
				this->hide();
				return 0;
			});
			rightShift += headerHeightElm + paddingX;
		}
	}

	m_title = new Label;
	m_header->addChild(m_title);
	m_title->Initialize(m_header->m_width - rightShift, headerHeightElm);
	m_title->Add(title, m_header->m_x + paddingX, m_header->m_y + paddingY);

	m_BitmapHeader = new BitmapClass;
	result = m_BitmapHeader->Initialize(m_D3D->GetDevice(), Options::screen_width, Options::screen_height, L"data/textures/ui/window_header.png", m_header->m_width, m_header->m_height);
	if (!result) {
		MessageBox(NULL, L"Could not initialize the bitmap objec headert.", L"Error", MB_OK);
		return false;
	}

	return true;
}

bool Window::addBody()
{
	bool result;

	m_body = new Group();
	Group::addChild(m_body);
	m_body->m_x = m_x;
	m_body->m_y = m_y;
	m_body->m_width = m_width;
	m_body->m_height = m_height;

	m_BitmapBody = new BitmapClass;
	result = m_BitmapBody->Initialize(m_D3D->GetDevice(), Options::screen_width, Options::screen_height, L"data/textures/ui/window_body.png", m_body->m_width, m_body->m_height);
	if (!result) {
		MessageBox(NULL, L"Could not initialize the bitmap object body.", L"Error", MB_OK);
		return false;
	}

	return true;
}

void Window::addChild(AbstractGui* child)
{
	m_body->addChild(child);
}

void Window::setTitle(char* title)
{
	m_title->Add(title, m_title->m_x, m_title->m_y);
}

void Window::Shutdown()
{
	if (m_BitmapHeader) {
		m_BitmapHeader->Shutdown();
		delete m_BitmapHeader;
		m_BitmapHeader = 0;
	}

	if (m_BitmapBody) {
		m_BitmapBody->Shutdown();
		delete m_BitmapBody;
		m_BitmapBody = 0;
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

	if (m_body) {
		m_BitmapBody->Render(m_D3D->GetDeviceContext(), m_body->m_x, m_body->m_y);
		result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_BitmapBody->GetIndexCount(), worldMatrix, m_baseViewMatrix, orthoMatrix, m_BitmapBody->GetTexture());
		if (!result) {
			return false;
		}
	}

	if (m_header) {
		m_BitmapHeader->Render(m_D3D->GetDeviceContext(), m_header->m_x, m_header->m_y);
		result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_BitmapHeader->GetIndexCount(), worldMatrix, m_baseViewMatrix, orthoMatrix, m_BitmapHeader->GetTexture());
		if (!result) {
			return false;
		}
	}

	return Group::Render();
}