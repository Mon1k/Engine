#pragma once

#include "AbstractGUI.h"

#include "../textureshaderclass.h"
#include "../bitmapclass.h"
#include "../textclass.h"
#include "../inputclass.h"

#include <string>
using namespace std;

class Input : public AbstractGui
{
public:
    Input();
    ~Input();

    bool Initialize(int, int, WCHAR*, int, int, int, int);
    void Shutdown();
    bool Render();

    bool setText(char*);
    bool updateText(char*);
    void updateText();
    virtual void onMousePress(int x, int y, int button);
    virtual void onKeyboardPress(int key, char symbol);
    virtual bool isIntersect(int x, int y);
    virtual void frame(float counter);

    void setMaxSize(int size)
    {
        m_MaxSize = size;
    }

    void setUsesFlashCursor(bool isFlash)
    {
        m_usesFlashCursor = isFlash;
    }

private:
    TextureShaderClass* m_TextureShader;
    BitmapClass* m_Bitmap;
    TextClass* m_Text;

    bool m_Flash;
    bool m_usesFlashCursor;
    int m_CursorShift;
    float m_FrameCounter;
    int m_MaxSize;
    std::string m_String;
    std::string m_ViewedString;
};