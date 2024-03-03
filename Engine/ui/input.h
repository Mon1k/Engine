#pragma once

#include "AbstractGUI.h"

#include "../textureshaderclass.h"
#include "../bitmapclass.h"
#include "../textclass.h"
#include "../inputclass.h"

#include <string>

class Input : public AbstractGui
{
public:
    Input();
    ~Input();

    bool Initialize(int, int, int, int);
    void Shutdown();
    bool Render();

    bool setText(std::string);
    bool updateText(std::string);
    void updateText();
    virtual void onMousePress(int x, int y, int button);
    virtual void onKeyboardPress(InputClass::EventKey);
    virtual bool isIntersect(int x, int y);
    virtual void frame(float counter);

    std::string getValue()
    {
        return m_String;
    }

    float getValueFloat()
    {
        return std::stof(m_String);
    }

    int getValueInt()
    {
        return std::stoi(m_String);
    }

    void setMaxSize(int size)
    {
        m_MaxSize = size;
    }

    void setUsesFlashCursor(bool isFlash)
    {
        m_usesFlashCursor = isFlash;
    }

    char replaceSymbolByEvent(InputClass::EventKey);

    virtual void unfocus();
    void hideCaret();
    void showCaret();

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