#ifndef _BUTTON_H_
#define _BUTTON_H_

#include "AbstractGUI.h"

#include "../textureshaderclass.h"
#include "../bitmapclass.h"
#include "../textclass.h"

class Button: public AbstractGui
{
public:
    Button();
    Button(const Button&);
    ~Button();

    bool Initialize(WCHAR*, int, int);
    void Shutdown();
    bool Add(char*, int, int, float, float, float);
    bool Add(char*, int, int);
    bool setText(char*);
    bool Render();

private:
    TextureShaderClass* m_TextureShader;
    BitmapClass* m_Bitmap;
    TextClass* m_Text;
};

#endif