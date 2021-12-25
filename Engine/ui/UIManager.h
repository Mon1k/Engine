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

    void onMouseClick(int x, int y, int button);

private:
    std::vector<AbstractGui*> elements;

    D3DClass* m_D3D;
    D3DXMATRIX m_baseViewMatrix;
};

#endif