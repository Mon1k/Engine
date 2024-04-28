#ifndef _CHECKBOX_H_
#define _CHECKBOX_H_

#include "AbstractGUI.h"
#include "../tool/Ref.h"

#include "../textureshaderclass.h"
#include "../bitmapclass.h"
#include "../textclass.h"
#include "../inputclass.h"

class Checkbox: public AbstractGui, public ValueRef
{
public:
    Checkbox();
    ~Checkbox();

    bool initialize();
    void Shutdown();
    bool Add(char*, int, int, float, float, float);
    bool Add(char*, int, int);
    bool Render();

    virtual bool isIntersect(int x, int y);
    virtual void frame(float counter);

    bool getIsMarked()
    {
        return b_IsMarked;
    }
    void setMarked(bool marked)
    {
        b_IsMarked = marked;
    }
    void toogle()
    {
        b_IsMarked = !b_IsMarked;
        setValueRef(std::to_string((int)b_IsMarked));
    }

    virtual bool onMousePress(int x, int y, int button)
    {
        if (button == MOUSE_BUTTON1) {
            toogle();
            return proccesedEventHandlers(AbstractGui::EventType::MOUSE_DOWN);
        }

        return false;
    }

private:
    TextureShaderClass* m_TextureShader;
    BitmapClass* m_Bitmap;
    BitmapClass* m_Bitmap_marked;
    TextClass* m_Text;

    bool b_IsMarked;
};

#endif