#include "systemclass.h"

#include "ui/UIManager.h"
#include "ui/button.h"
#include "ui/checkbox.h"
#include "ui/label.h"
#include "ui/cursor.h"

#include "models/ModelManager.h"
#include "modelclass.h"
#include "lightshaderclass.h"
#include "lightclass.h"

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
		button->setId(1);
		
		Button* button2 = new Button;
		m_uiManager->Add(button2);
		button2->Initialize(screenWidth, screenHeight, L"data/textures/ui/button.png", 76, 28);
		button2->Add("Exit", 10, 40, 1.0f, 0.3f, 0.3f);
		button2->setId(2);

		Checkbox* checkbox = new Checkbox;
		m_uiManager->Add(checkbox);
		checkbox->Initialize(screenWidth, screenHeight, L"data/textures/ui/checkbox.png", L"data/textures/ui/checkbox_marked.png", 18, 18);
		checkbox->Add("Fog", 10, 70);
		checkbox->setId(3);

		Label* label = new Label;
		m_uiManager->Add(label);
		label->Initialize(screenWidth, screenHeight, 76, 28);
		label->Add("Hello World", 10, 100);
		label->setId(4);

		Label* label2 = new Label;
		m_uiManager->Add(label2);
		label2->Initialize(screenWidth, screenHeight, 76, 28);
		label2->Add("", 10, 130, 1.0f, 0.3f, 0.3f);
		label2->setId(5);

		Cursor* cursor = new Cursor;
		m_uiManager->Add(cursor);
		cursor->Initialize(screenWidth, screenHeight, L"data/textures/ui/cursor.png", 32, 32);
		cursor->set(screenWidth / 2, screenHeight / 2);
		cursor->setId(6);
	}

	void loadScene()
	{
		bool result;
		m_modelManager = m_Graphics->getModelManager();

		ModelClass* model = new ModelClass;
		std::vector<std::wstring> textures1 = { L"data/textures/T_brightwood_basecolor.png" };
		result = model->Initialize(m_Graphics->getD3D(), "data/models/midpoly_town_house_01.obj", textures1);
		if (!result) {
			MessageBox(NULL, L"Could not initialize the model 1 object.", L"Error", MB_OK);
			return;
		}

		LightShaderClass* lightShader = new LightShaderClass;
		result = lightShader->Initialize(m_Graphics->getD3D()->GetDevice());
		if (!result) {
			MessageBox(NULL, L"Could not initialize the light shader object.", L"Error", MB_OK);
			return;
		}

		LightClass* light = new LightClass;
		light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
		light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
		light->SetDirection(0.0f, 0.0f, 1.0f);
		light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
		light->SetSpecularPower(64.0f);

		std::vector<LightClass*> lights = { light };
		lightShader->addLights(lights);
		model->addShader(lightShader);
		m_modelManager->Add(model);
	}

protected:
	bool frame()
	{
		if (!SystemClass::frame()) {
			return false;
		}

		frameUI();
		frameScene();

		return true;
	}

	void frameUI()
	{
		int mouseX, mouseY;
		m_Input->GetMouseLocation(mouseX, mouseY);
		int mouseButtonPress = m_Input->getMouseButtonPress();

		std::vector<AbstractGui*> events = m_uiManager->getEvents();
		int size = events.size();
		for (int i = 0; i < size; i++) {
			if (events[i]->getId() == 1) {
				m_Sound->Play();
			}
			if (events[i]->getId() == 2) {
				done = true;
			}
		}

		std::vector<AbstractGui*> elements = m_uiManager->getElements();
		size = elements.size();
		for (int i = 0; i < size; i++) {
			if (elements[i]->getId() == 4) {
				Label* label = dynamic_cast<Label*>(elements[i]);
				char mouseString[128];
				sprintf(mouseString, "Fps: %d, Cpu: %3.2f%%, MouseX: %d, MouseY: %d, MouseButton: %d", m_Fps->GetFps(), m_Fps->GetCpuPercentage(), mouseX, mouseY, mouseButtonPress);
				label->Add(mouseString, 10, 100);
			}
			if (elements[i]->getId() == 5) {
				Label* label = dynamic_cast<Label*>(elements[i]);
				char string[128];
				sprintf(string, "Render objects: %d, triangles: %d", m_modelManager->getRenderCount(), m_modelManager->getTriangleCount());
				label->Add(string, 10, 130, 1.0f, 1.0f, 0.5f);
			}
			if (elements[i]->getId() == 6) {
				Cursor* cursor = dynamic_cast<Cursor*>(elements[i]);
				if (mouseButtonPress == MOUSE_BUTTON2) {
					cursor->set(mouseX, mouseY);
					cursor->show();
				} else {
					cursor->hide();
				}
			}
		}
	}

	void frameScene()
	{

	}

private:
	UIManager* m_uiManager;
	ModelManager* m_modelManager;
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