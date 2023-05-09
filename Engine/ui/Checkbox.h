#ifndef _CHECKBOX_H_
#define _CHECKBOX_H_

#include "AbstractGUI.h"

#include "../textureshaderclass.h"
#include "../bitmapclass.h"
#include "../textclass.h"
#include "../inputclass.h"

class Checkbox: public AbstractGui
{
public:
    Checkbox();
    ~Checkbox();

    bool Initialize(int, int, WCHAR*, WCHAR*, int, int);
    void Shutdown();
    bool Add(char*, int, int, float, float, float);
    bool Add(char*, int, int);
    bool Render();

    virtual bool isIntersect(int x, int y);

    bool getIsMarked() {
        return b_IsMarked;
    };
    void setMarked(bool marked) {
        b_IsMarked = marked;
    };
    void toogle() {
        b_IsMarked = !b_IsMarked;
    };
    virtual void onMousePress(int x, int y, int button)
    {
        if (button == MOUSE_BUTTON1) {
            toogle();
        }
    }

private:
    TextureShaderClass* m_TextureShader;
    BitmapClass* m_Bitmap;
    BitmapClass* m_Bitmap_marked;
    TextClass* m_Text;

    bool b_IsMarked;
};

#endif