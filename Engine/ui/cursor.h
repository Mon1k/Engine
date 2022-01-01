#ifndef _CURSOR_H_
#define _CURSOR_H_

#include "AbstractGUI.h"

#include "../d3dclass.h"
#include "../textureshaderclass.h"
#include "../bitmapclass.h"

class Cursor: public AbstractGui
{
public:
    Cursor();
    Cursor(const Cursor&);
    ~Cursor();

    bool Initialize(D3DClass*, int, int, HWND, WCHAR*, int, int, D3DXMATRIX);
    void Shutdown();
    bool set(int, int);
    bool Render();

private:
    D3DClass* m_D3D;

    TextureShaderClass* m_TextureShader;
    BitmapClass* m_Bitmap;
    D3DMATRIX m_ViewMatrix;

    int m_width, m_height, m_x, m_y;
};

#endif