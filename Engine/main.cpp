#include "systemclass.h"

#include "ui/UIManager.h"
#include "ui/button.h"
#include "ui/checkbox.h"
#include "ui/label.h"
#include "ui/cursor.h"

#include "models/ModelManager.h"
#include "modelclass.h"
#include "models/modelbumpclass.h"
#include "models/bbox.h"

#include "lightclass.h"

#include "lightshaderclass.h"
#include "textures/bumpmapshaderclass.h"
#include "textures/multitextureshaderclass.h"
#include "textures/lightmapshaderclass.h"
#include "alphamapshaderclass.h"
#include "textures/specmapshaderclass.h"
#include "textures/transparentshaderclass.h"
#include "textures/translateshaderclass.h"

class App: public SystemClass
{
public:
	bool init()
	{
		if (!SystemClass::init()) {
			return false;
		}

		int size = sizeof(m_Counters) / sizeof(m_Counters[0]);
		for (int i = 0; i < size; i++) {
			m_Counters[i] = 0;
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


		LightClass* light = new LightClass;
		light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
		light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
		light->SetDirection(0.0f, 0.0f, 1.0f);
		light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
		light->SetSpecularPower(64.0f);

		////
		ModelClass* model = new ModelClass;
		result = model->Initialize(m_Graphics->getD3D(), "data/models/midpoly_town_house_01.obj", { L"data/textures/T_brightwood_basecolor.png" });
		if (!result) {
			MessageBox(NULL, L"Could not initialize the model 1 object.", L"Error", MB_OK);
			return;
		}

		LightShaderClass* shader = new LightShaderClass;
		result = shader->Initialize(m_Graphics->getD3D()->GetDevice());
		if (!result) {
			MessageBox(NULL, L"Could not initialize the light shader object.", L"Error", MB_OK);
			return;
		}

		shader->addLights({ light });
		model->addShader(shader);
		m_modelManager->Add(model);


		////
		ModelBumpClass* model2 = new ModelBumpClass;
		result = model2->Initialize(m_Graphics->getD3D(), "data/models/cube.ds", { L"data/textures/stone01.dds", L"data/textures/bump01.dds" });
		if (!result) {
			MessageBox(NULL, L"Could not initialize the model 2 object.", L"Error", MB_OK);
			return;
		}
		model2->SetScale(D3DXVECTOR3(5.0f, 5.0f, 5.0f));
		model2->SetPosition(D3DXVECTOR3(5.0f, 0.0f, -20.0f));

		BumpMapShaderClass* shader2 = new BumpMapShaderClass;
		result = shader2->Initialize(m_Graphics->getD3D()->GetDevice());
		if (!result) {
			MessageBox(NULL, L"Could not initialize the bump map shader object.", L"Error", MB_OK);
			return;
		}

		shader2->addLights({ light });
		model2->addShader(shader2);
		m_modelManager->Add(model2);


		////
		ModelClass* model3 = new ModelClass;
		result = model3->Initialize(m_Graphics->getD3D(), "data/models/square.ds", { L"data/textures/stone01.dds", L"data/textures/dirt01.dds" });
		if (!result) {
			MessageBox(NULL, L"Could not initialize the model 3 object", L"Error", MB_OK);
			return;
		}
		model3->SetScale(D3DXVECTOR3(5.0f, 5.0f, 1.0f));
		model3->SetPosition(D3DXVECTOR3(35.0f, 0.0f, -20.0f));

		MultiTextureShaderClass* shader3 = new MultiTextureShaderClass;
		result = shader3->Initialize(m_Graphics->getD3D()->GetDevice());
		if (!result) {
			MessageBox(NULL, L"Could not initialize the multitexture shader object", L"Error", MB_OK);
			return;
		}

		model3->addShader(shader3);
		m_modelManager->Add(model3);


		////
		ModelClass* model4 = new ModelClass;
		result = model4->Initialize(m_Graphics->getD3D(), "data/models/square.ds", { L"data/textures/stone01.dds", L"data/textures/light01.dds" });
		if (!result) {
			MessageBox(NULL, L"Could not initialize the model 4 object", L"Error", MB_OK);
			return;
		}
		model4->SetScale(D3DXVECTOR3(5.0f, 5.0f, 1.0f));
		model4->SetPosition(D3DXVECTOR3(25.0f, 0.0f, -20.0f));

		LightMapShaderClass* shader4 = new LightMapShaderClass;
		result = shader4->Initialize(m_Graphics->getD3D()->GetDevice());
		if (!result) {
			MessageBox(NULL, L"Could not initialize the light map shader object.", L"Error", MB_OK);
			return;
		}

		model4->addShader(shader4);
		m_modelManager->Add(model4);


		////
		ModelClass* model5 = new ModelClass;
		result = model5->Initialize(m_Graphics->getD3D(), "data/models/square.ds", { L"data/textures/stone01.dds", L"data/textures/dirt01.dds", L"data/textures/alpha01.dds" });
		if (!result) {
			MessageBox(NULL, L"Could not initialize the model model 5 object.", L"Error", MB_OK);
			return;
		}
		model5->SetScale(D3DXVECTOR3(5.0f, 5.0f, 1.0f));
		model5->SetPosition(D3DXVECTOR3(15.0f, 0.0f, -20.0f));

		AlphaMapShaderClass* shader5 = new AlphaMapShaderClass;
		result = shader5->Initialize(m_Graphics->getD3D()->GetDevice());
		if (!result) {
			MessageBox(NULL, L"Could not initialize the alpha map shader object.", L"Error", MB_OK);
			return;
		}

		model5->addShader(shader5);
		m_modelManager->Add(model5);

		
		////
		ModelBumpClass* model6 = new ModelBumpClass;
		result = model6->Initialize(m_Graphics->getD3D(), "data/models/square.ds", { L"data/textures/stone02.dds", L"data/textures/bump02.dds", L"data/textures/spec02.dds" });
		if (!result) {
			MessageBox(NULL, L"Could not initialize the model model 6 object.", L"Error", MB_OK);
			return;
		}
		model6->SetScale(D3DXVECTOR3(5.0f, 5.0f, 5.0f));
		model6->SetPosition(D3DXVECTOR3(-5.0f, 0.0f, -20.0f));

		SpecMapShaderClass* shader6 = new SpecMapShaderClass;
		result = shader6->Initialize(m_Graphics->getD3D()->GetDevice());
		if (!result) {
			MessageBox(NULL, L"Could not initialize the specular map shader object.", L"Error", MB_OK);
			return;
		}

		shader6->addLights({ light });
		model6->addShader(shader6);
		model6->setId(6);
		m_modelManager->Add(model6);


		////
		ModelClass* model7 = new ModelClass;
		result = model7->Initialize(m_Graphics->getD3D(), "data/models/square.ds", { L"data/textures/explosion.png" });
		if (!result) {
			MessageBox(NULL, L"Could not initialize the model model 7 object.", L"Error", MB_OK);
			return;
		}
		model7->SetScale(D3DXVECTOR3(5.0f, 5.0f, 5.0f));
		model7->SetPosition(D3DXVECTOR3(-15.0f, 0.0f, -20.0f));

		TranslateShaderClass* shader7 = new TranslateShaderClass;
		result = shader7->Initialize(m_Graphics->getD3D()->GetDevice());
		if (!result) {
			MessageBox(NULL, L"Could not initialize the translate shader object.", L"Error", MB_OK);
			return;
		}
		shader7->setMaxFrame(8, 6);

		model7->addShader(shader7);
		model7->setId(7);
		m_modelManager->Add(model7);


		////
		ModelClass* model8 = new ModelClass;
		result = model8->Initialize(m_Graphics->getD3D(), "data/models/square.ds", { L"data/textures/stone01.dds" });
		if (!result) {
			MessageBox(NULL, L"Could not initialize the model model 8 object.", L"Error", MB_OK);
			return;
		}
		model8->SetScale(D3DXVECTOR3(5.0f, 5.0f, 5.0f));
		model8->SetPosition(D3DXVECTOR3(15.0f, 0.0f, -30.0f));

		TransparentShaderClass* shader8 = new TransparentShaderClass;
		result = shader8->Initialize(m_Graphics->getD3D()->GetDevice());
		if (!result) {
			MessageBox(NULL, L"Could not initialize the transparent shader object.", L"Error", MB_OK);
			return;
		}

		model8->addShader(shader8);
		m_modelManager->Add(model8);


		////
		D3DXVECTOR3 position, size;
		model8->GetBoundingBox(position, size);
		BBox* bbox = new BBox;
		bbox->CreateBox(m_Graphics->getD3D(), position, size);
		m_modelManager->Add(bbox);
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
		float time = m_Timer->GetTime();

		m_Counters[0] += time;
		if (m_Counters[0] > 50) {
			AbstractModel* model = m_modelManager->getById(7);
			TranslateShaderClass* shader = dynamic_cast<TranslateShaderClass*>(model->getShader());
			shader->incrementFrame();
			m_Counters[0] = 0;
		}
	}

private:
	UIManager* m_uiManager;
	ModelManager* m_modelManager;
	float m_Counters[2]{};
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