#include "systemclass.h"
#include "ui/UIManager.h"
#include "ui/button.h"


class App: public SystemClass
{
public:
	bool init()
	{
		if (!SystemClass::init()) {
			return false;
		}

		loadUI();
		loadScene();

		return true;
	}

	void loadUI()
	{
		m_uiManager = m_Graphics->getUiManager();

		Button* button = new Button;
		m_uiManager->Add(button);
		button->Initialize(screenWidth, screenHeight, L"data/textures/ui/button.png", 76, 28);
		button->Add("New", 10, 10);

		Button* button2 = new Button;
		m_uiManager->Add(button2);
		button2->Initialize(screenWidth, screenHeight, L"data/textures/ui/button.png", 76, 28);
		button2->Add("Exit", 10, 40, 1.0f, 0.3f, 0.3f);
	}

	void loadScene()
	{
	}

protected:
	bool Frame()
	{
		bool result = SystemClass::Frame();
		if (!result) {
			return result;
		}
	}

private:
	UIManager* m_uiManager;
};


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	App* app = new App;
	
	if (app->init()) {
		app->run();
	}

	app->shutdown();
	delete app;
	app = 0;

	return 0;
}