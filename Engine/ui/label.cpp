#include "label.h"
#include "../Options.h"

Label::Label()
{
	m_Text = 0;
	m_align = ALIGN_LEFT;
	m_valign = VALIGN_MIDDLE;
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

void Label::update()
{
	D3DXVECTOR3 color = m_Text->getColor();
	Add(&m_Text->getText()[0], m_x, m_y, color.x, color.y, color.z);
}

bool Label::Add(std::string text, int positionX, int positionY, float red, float green, float blue)
{
	m_x = positionX;
	m_y = positionY;

	int x, y;
	if (m_align == ALIGN_LEFT) {
		x = m_x;
	}
	else if (m_align == ALIGN_RIGHT) {
		x = m_x + m_width - std::string(text).size() * 6.3;
	}
	else {
		x = m_x + m_width / 2 - std::string(text).size() / 2 * 6.3;
	}


	if (m_valign == VALIGN_TOP) {
		y = m_y;
	}
	else if (m_valign == VALIGN_MIDDLE) {
		y = m_y + m_height / 3;
	} else {
		y = m_y + m_height - 20;
	}
	
	return m_Text->AddText(text, x, y, red, green, blue);
}

bool Label::Add(std::string text, int positionX, int positionY)
{
	return Add(text, positionX, positionY, 1.0f, 1.0f, 1.0f);
}

bool Label::Add(std::string text)
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
	if (m_Text->getText().length() == 0) {
		return true;
	}

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