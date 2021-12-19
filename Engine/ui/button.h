#ifndef _BUTTON_H_
#define _BUTTON_H_

#include "AbstractGUI.h"

#include "../d3dclass.h"
#include "../textureshaderclass.h"
#include "../bitmapclass.h"
#include "../textclass.h"

class Button: public AbstractGui
{
public:
    Button();
    Button(const Button&);
    ~Button();

    bool Initialize(D3DClass*, int, int, HWND, WCHAR*, int, int, D3DXMATRIX);
    void Shutdown();
    bool Add(char*, int, int, float, float, float);
    bool Render();

private:
    D3DClass* m_D3D;

    TextureShaderClass* m_TextureShader;
    BitmapClass* m_Bitmap;
    TextClass* m_Text;
};

#endif