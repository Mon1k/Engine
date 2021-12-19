#include "UIManager.h"

UIManager::UIManager()
{
    elements.clear();
    m_D3D = 0;
}

bool UIManager::Initialize(D3DClass* d3d)
{
    m_D3D = d3d;

    return true;
}

bool UIManager::Add(AbstractGui* ui)
{
    elements.push_back(ui);

    return true;
}

void UIManager::Shutdown()
{
    int size = elements.size();
    for (int i = 0; i < size; i++) {
        elements[i]->Shutdown();
    }
}

void UIManager::Render()
{
    // Turn off the Z buffer to begin all 2D rendering.
    m_D3D->TurnZBufferOff();

    // Turn on the alpha blending before rendering the text.
    m_D3D->TurnOnAlphaBlending();

    int size = elements.size();
    for (int i = 0; i < size; i++) {
        if (elements[i]->isVisible()) {
            elements[i]->Render();
        }
    }

    // Turn off alpha blending after rendering the text.
    m_D3D->TurnOffAlphaBlending();

    // Turn the Z buffer back on now that all 2D rendering has completed.
    m_D3D->TurnZBufferOn();
}

void UIManager::onPressUi(int x, int y)
{
    int size = elements.size();
    for (int i = 0; i < size; i++) {
        if (elements[i]->isVisible()) {
            elements[i]->onPress(x, y);
        }
    }
}