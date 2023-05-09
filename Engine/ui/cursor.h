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
    bool Render();

    virtual bool isIntersect(int x, int y)
    {
        return false;
    }

private:
    TextureShaderClass* m_TextureShader;
    BitmapClass* m_Bitmap;
};

#endif