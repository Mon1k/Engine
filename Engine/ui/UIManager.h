#ifndef _UIMANAGER_H_
#define _UIMANAGER_H_

#include <vector>
using namespace std;

#include "AbstractGUI.h"
#include "../d3dclass.h"

class UIManager
{
public:
    UIManager();
    bool Initialize(D3DClass*);
    bool Add(AbstractGui* ui);
    void Render();
    void Shutdown();

private:
    std::vector<AbstractGui*> elements;

    D3DClass* m_D3D;
};

#endif