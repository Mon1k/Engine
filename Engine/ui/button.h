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

    void setAlign(int align)
    {
        m_align = align;
    }

    bool Add(char* text, int positionX, int positionY, float red, float green, float blue);
    bool Add(char* text, int positionX, int positionY);
    bool setText(char* text);
    
    enum {
        ALIGN_LEFT = 0,
        ALIGN_RIGHT = 1,
        ALIGN_CENTER = 2
    };

private:
    TextureShaderClass* m_TextureShader;
    BitmapClass* m_Bitmap;
    TextClass* m_Text;
    int m_align;
};

#endif