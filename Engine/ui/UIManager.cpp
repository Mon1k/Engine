#include "UIManager.h"

UIManager::UIManager()
{
    m_elements.clear();
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
    ui->m_D3D = m_D3D;
    ui->m_baseViewMatrix = m_baseViewMatrix;
    m_elements.push_back(ui);

    return true;
}

AbstractGui* UIManager::getById(int id)
{
    int size = m_elements.size();
    for (int i = 0; i < size; i++) {
        AbstractGui* node =  dynamic_cast<AbstractGui*>(m_elements[i]->getById(id));
        if (node) {
            return node;
        }
    }

    return NULL;
}

void UIManager::Shutdown()
{
    int size = m_elements.size();
    for (int i = 0; i < size; i++) {
        m_elements[i]->Shutdown();
    }
}

void UIManager::Render()
{
    // Turn off the Z buffer to begin all 2D rendering.
    m_D3D->TurnZBufferOff();

    // Turn on the alpha blending before rendering the text.
    m_D3D->TurnOnAlphaBlending();

    int size = m_elements.size();
    for (int i = 0; i < size; i++) {
        if (m_elements[i]->isVisible()) {
            m_elements[i]->Render();
        }
    }

    // Turn off alpha blending after rendering the text.
    m_D3D->TurnOffAlphaBlending();

    // Turn the Z buffer back on now that all 2D rendering has completed.
    m_D3D->TurnZBufferOn();
}

void UIManager::onMouseClick(int x, int y, int button)
{
    int size = m_elements.size();
    for (int i = 0; i < size; i++) {
        if (m_elements[i]->isVisible()) {
            if (m_elements[i]->isIntersect(x, y)) {
                m_elements[i]->focus();
                m_elements[i]->onMousePress(x, y, button);
                m_events.push_back(m_elements[i]);
                return;
            }
            else {
                m_elements[i]->unfocus();
            }
        }
    }
}

void UIManager::onKeyboardClick(int key, char symbol)
{
    int size = m_elements.size();
    for (int i = 0; i < size; i++) {
        if (m_elements[i]->isVisible() && m_elements[i]->isFocused()) {
            m_elements[i]->onKeyboardPress(key, symbol);
            m_events.push_back(m_elements[i]);
        }
    }
}

void UIManager::EventProccesor(InputClass* input)
{
    int mouseButton = input->getMouseButton();
    int keyboardKey = input->getKeyDown();

    m_events.clear();

    if (mouseButton >= 0) {
        int mouseX, mouseY;
        input->GetMouseLocation(mouseX, mouseY);
        onMouseClick(mouseX, mouseY, mouseButton);
    }

    if (keyboardKey > 0) {
        char keyboardSymbol = input->getSymbolKey(keyboardKey);
        onKeyboardClick(keyboardKey, keyboardSymbol);
    }
}

void UIManager::frame(float counter)
{
    m_IsFocused = false;

    int size = m_elements.size();
    for (int i = 0; i < size; i++) {
        if (m_elements[i]->isVisible()) {
            m_elements[i]->frame(counter);
            if (m_elements[i]->isFocused()) {
                m_IsFocused = true;
                return;
            }
        }
    }
}