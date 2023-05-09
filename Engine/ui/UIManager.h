#ifndef _UIMANAGER_H_
#define _UIMANAGER_H_

#include <vector>
using namespace std;

#include "../d3dclass.h"
#include "../inputclass.h"
#include "AbstractGUI.h"

class UIManager
{
public:
    UIManager();
    bool Initialize(D3DClass*, D3DXMATRIX);
    void EventProccesor(InputClass*);
    bool Add(AbstractGui* ui);
    void Render();
    void Shutdown();
    void frame(float);

    std::vector<AbstractGui*> getElements()
    {
        return this->m_elements;
    };
    std::vector<AbstractGui*> getEvents()
    {
        return this->m_events;
    };
    AbstractGui* getById(int id)
    {
        int size = m_elements.size();
        for (int i = 0; i < size; i++) {
            if (m_elements[i]->getId() == id) {
                return m_elements[i];
            }
        }

        return NULL;
    }

    void onMouseClick(int x, int y, int button);
    void onKeyboardClick(int, char);

    bool isFocused() {
        return m_IsFocused;
    }

private:
    std::vector<AbstractGui*> m_elements;
    std::vector<AbstractGui*> m_events;
    bool m_IsFocused;

    D3DClass* m_D3D;
    D3DXMATRIX m_baseViewMatrix;
};

#endif