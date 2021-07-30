#ifndef _CHECKBOX_H_
#define _CHECKBOX_H_

#include "AbstractGUI.h"

#include "../d3dclass.h"
#include "../textureshaderclass.h"
#include "../bitmapclass.h"
#include "../textclass.h"

class Checkbox: public AbstractGui
{
public:
    Checkbox();
    ~Checkbox();

    bool Initialize(D3DClass*, int, int, HWND, WCHAR*, WCHAR*, int, int, D3DXMATRIX);
    void Shutdown();
    bool Add(char*, int, int, float, float, float);
    bool Render();
    bool onButtonPress(int x, int y);
    bool getIsMarked() {
        return b_IsMarked;
    };
    void setMarked(bool marked) {
        b_IsMarked = marked;
    };
    void MarkedToogle() {
        b_IsMarked = !b_IsMarked;
    };

private:
    D3DClass* m_D3D;

    TextureShaderClass* m_TextureShader;
    BitmapClass* m_Bitmap;
    BitmapClass* m_Bitmap_marked;
    TextClass* m_Text;

    int m_width, m_height, m_x, m_y;
    bool b_IsMarked;
};

#endif