#ifndef _CURSOR_H_
#define _CURSOR_H_

#include "AbstractGUI.h"

#include "../textureshaderclass.h"
#include "../bitmapclass.h"

class Cursor: public AbstractGui
{
public:
    Cursor();
    Cursor(const Cursor&);
    ~Cursor();

    bool Initialize(int, int, WCHAR*, int, int);
    void Shutdown();
    bool set(int, int);
    bool Render();

private:
    TextureShaderClass* m_TextureShader;
    BitmapClass* m_Bitmap;
};

#endif