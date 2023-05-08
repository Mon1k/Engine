#pragma once

#include "AbstractGUI.h"

#include "../textureshaderclass.h"
#include "../bitmapclass.h"
#include "../textclass.h"

class Input : public AbstractGui
{
public:
    Input();
    ~Input();

    bool Initialize(int, int, WCHAR*, int, int, int, int);
    void Shutdown();
    bool setText(char*);
    bool Render();

    bool isFocused() {
        return m_bIsFocused;
    }

private:
    TextureShaderClass* m_TextureShader;
    BitmapClass* m_Bitmap;
    TextClass* m_Text;

    bool m_bIsFocused;
};