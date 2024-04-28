#pragma once

#include "../main.h"

#include "../../../Engine/ui/UIManager.h"
#include "../../../Engine/ui/button.h"
#include "../../../Engine/ui/label.h"
#include "../../../Engine/ui/input.h"
#include "../../../Engine/ui/window.h"
#include "../../../Engine/ui/checkbox.h"
#include "../../../Engine/ui/FileInput.h"

class TerrainWindow
{
public:
	TerrainWindow::TerrainWindow(App* app)
	{
		m_app = app;
	}

	void resetUI()
	{
		m_path = "data/models/bath.ds";
		m_texture = "data/textures/wall01.dds";

		m_position = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		m_scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
		m_scaleNormal = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	}

	void initialize()
	{
		resetUI();

		int shift = 0;
		Window* menuTop = dynamic_cast<Window*>(m_app->m_uiManager->getById(1));

		m_Window = new Window;
		m_app->m_uiManager->add(m_Window);
		m_Window->Initialize(600, 500, Options::screen_width - 620, menuTop->m_height + 5);
		m_Window->addHeader("Terrain properties", Window::HEADER_BUTTON_CLOSE);
		m_Window->addBody();
		m_Window->hide();
		m_Window->addEventHandler(Window::EventType::WINDOW_CLOSE, [this] {
			m_app->unselectModel();
		});
		shift += m_Window->m_y + m_Window->getHeader()->m_height + 5;
	}

	Window* getWindow()
	{
		return m_Window;
	}

protected:
	App* m_app;
	Window* m_Window;

	std::string m_path;
	std::string m_texture;
	D3DXVECTOR3 m_position;
	D3DXVECTOR3 m_scale;
	D3DXVECTOR3 m_scaleNormal;
};