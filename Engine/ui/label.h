#ifndef _LABEL_H_
#define _LABEL_H_

#include "../d3dclass.h"
#include "../textclass.h"

class Label
{
public:
    Label();
    Label(const Label&);
    ~Label();

    bool Initialize(D3DClass*, int, int, HWND, int, int, D3DXMATRIX);
    void Shutdown();
    bool Add(char*, int, int, float, float, float);
    bool Render();

private:
    D3DClass* m_D3D;

    TextClass* m_Text;

    int m_width, m_height, m_x, m_y;
};

#endif