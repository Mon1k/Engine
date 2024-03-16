#include "UIManager.h"
#include "cursor.h"
#include "group.h"

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

AbstractGui* UIManager::add(AbstractGui* ui)
{
    ui->m_D3D = m_D3D;
    ui->m_baseViewMatrix = m_baseViewMatrix;
    ui->m_manager = this;
    m_elements.push_back(ui);

    return ui;
}

void UIManager::remove(int id)
{
    size_t size = m_elements.size();
    for (size_t i = 0; i < size; i++) {
        if (m_elements[i]->getId() == id) {
            m_elements[i]->Shutdown();
            m_elements.erase(m_elements.begin() + i);
            return;
        }
    }
}

AbstractGui* UIManager::getById(int id)
{
    size_t size = m_elements.size();
    for (size_t i = 0; i < size; i++) {
        AbstractGui* node =  dynamic_cast<AbstractGui*>(m_elements[i]->getById(id));
        if (node) {
            return node;
        }
    }

    return NULL;
}

int UIManager::getNextId()
{
    int id = 0;
    size_t size = m_elements.size();
    for (size_t i = 0; i < size; i++) {
        int elmId = m_elements[i]->getId();
        if (dynamic_cast<Group*>(m_elements[i]) != nullptr) {
            elmId = ((Group*)m_elements[i])->getLastId();
        }
        if (elmId > id) {
            id = elmId;
        }
    }

    return id + 1;
}

void UIManager::Shutdown()
{
    size_t size = m_elements.size();
    for (size_t i = 0; i < size; i++) {
        m_elements[i]->Shutdown();
    }
}

void UIManager::Render()
{
    // Turn off the Z buffer to begin all 2D rendering.
    m_D3D->TurnZBufferOff();

    // Turn on the alpha blending before rendering the text.
    m_D3D->TurnOnAlphaBlending();

    std::vector<AbstractGui*> deffered;

    size_t size = m_elements.size();
    for (size_t i = 0; i < size; i++) {
        if (m_elements[i]->isVisible()) {
            if (dynamic_cast<Cursor*>(m_elements[i]) != nullptr) {
                deffered.push_back(m_elements[i]);
            }
            else {
                m_elements[i]->Render();
            }
        }
    }

    size = deffered.size();
    for (size_t i = 0; i < size; i++) {
        deffered[i]->Render();
    }

    // Turn off alpha blending after rendering the text.
    m_D3D->TurnOffAlphaBlending();

    // Turn the Z buffer back on now that all 2D rendering has completed.
    m_D3D->TurnZBufferOn();
}

void UIManager::onMouseClick(int x, int y, int button)
{
    for (size_t i = 0; i < m_elements.size(); i++) {
        if (m_elements[i]->isVisible()) {
            if (m_elements[i]->isIntersect(x, y)) {
                m_events.push_back(m_elements[i]);
                m_elements[i]->focus();
                if (m_elements[i]->onMousePress(x, y, button)) {
                    return;
                }
            }
            else {
                m_elements[i]->unfocus();
            }
        }
    }
}

void UIManager::onKeyboardClick(InputClass::EventKey event)
{
    for (size_t i = 0; i < m_elements.size(); i++) {
        if (m_elements[i]->isVisible() && m_elements[i]->isFocused()) {
            m_events.push_back(m_elements[i]);
            m_elements[i]->onKeyboardPress(event);
        }
    }
}

void UIManager::EventProccesor(InputClass* input)
{
    int mouseButton = input->getMouseButton();
    InputClass::EventKey eventKey = input->getKeyDown();

    m_events.clear();

    if (mouseButton >= 0) {
        int mouseX, mouseY;
        input->GetMouseLocation(mouseX, mouseY);
        onMouseClick(mouseX, mouseY, mouseButton);
    }

    if (eventKey.key > 0) {
        onKeyboardClick(eventKey);
    }
}

void UIManager::frame(float counter)
{
    m_IsFocused = false;
    if (m_events.size() > 1) {
        m_IsFocused = true;
        return;
    }

    size_t size = m_elements.size();
    for (size_t i = 0; i < size; i++) {
        if (m_elements[i]->isVisible()) {
            m_elements[i]->frame(counter);
            if (m_elements[i]->isFocused()) {
                m_IsFocused = true;
                return;
            }
        }
    }
}