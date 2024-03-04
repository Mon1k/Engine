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

    bool Initialize(int width, int height);
    bool Initialize(WCHAR* texture, int width, int height);
    
    virtual bool Render();
    virtual void Shutdown();

    bool Add(char* text, int positionX, int positionY, float red, float green, float blue);
    bool Add(char* text, int positionX, int positionY);
    bool setText(char* text);
    

private:
    TextureShaderClass* m_TextureShader;
    BitmapClass* m_Bitmap;
    TextClass* m_Text;
};

#endif