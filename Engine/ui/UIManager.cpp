#include "UIManager.h"

UIManager::UIManager()
{
    elements.clear();
    m_D3D = 0;
}

bool UIManager::Initialize(D3DClass* d3d, D3DXMATRIX baseViewMatrix)
{
    m_D3D = d3d;
    m_baseViewMatrix = baseViewMatrix;

    return true;
}

bool UIManager::Add(AbstractGui* ui)
{
    elements.push_back(ui);
    ui->m_D3D = m_D3D;
    ui->m_baseViewMatrix = m_baseViewMatrix;

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

void UIManager::onMouseClick(int x, int y, int button)
{
    int size = elements.size();
    for (int i = 0; i < size; i++) {
        if (elements[i]->isVisible() && elements[i]->onPress(x, y)) {

        }
    }
}

void UIManager::EventProccesor(InputClass* input)
{
    int mouseX, mouseY;
    int mouseButton = input->GetMouseButton();

    if (mouseButton >= 0) {
        input->GetMouseLocation(mouseX, mouseY);
        onMouseClick(mouseX, mouseY, mouseButton);
    }
}