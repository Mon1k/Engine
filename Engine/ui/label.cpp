#include "label.h"
#include "../Options.h"

Label::Label()
{
	m_Text = 0;
}


Label::Label(const Label& other)
{
}

Label::~Label()
{
}

bool Label::Initialize(int bitmapWidth, int bitmapHeight)
{
	bool result;

	m_width = bitmapWidth;
	m_height = bitmapHeight;

	m_Text = new TextClass;
	result = m_Text->Initialize(m_D3D->GetDevice(), m_D3D->GetDeviceContext(), Options::screen_width, Options::screen_height);
	if (!result) {
		MessageBox(NULL, L"Could not initialize the text object.", L"Error", MB_OK);
		return false;
	}

	return true;
}


bool Label::Add(char* text, int positionX, int positionY, float red, float green, float blue)
{
	m_x = positionX;
	m_y = positionY;

	return m_Text->AddText(text, m_x + m_width / 3, m_y + m_height / 3, red, green, blue);
}

bool Label::Add(char* text, int positionX, int positionY)
{
	return Add(text, positionX, positionY, 1.0f, 1.0f, 1.0f);
}

bool Label::Add(char* text)
{
	return Add(text, m_x, m_y, 1.0f, 1.0f, 1.0f);
}

void Label::Shutdown()
{
	if (m_Text) {
		m_Text->Shutdown();
		delete m_Text;
		m_Text = 0;
	}
}

bool Label::Render()
{
	bool result;
	D3DXMATRIX worldMatrix, orthoMatrix;

	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetOrthoMatrix(orthoMatrix);

	result = m_Text->Render(m_D3D->GetDeviceContext(), worldMatrix, orthoMatrix, m_baseViewMatrix);
	if (!result) {
		return false;
	}

	return true;
}