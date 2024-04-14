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

    static Input* createString(void* value, D3DClass* d3d, int width, int height, int positionX, int positionY);
    static Input* createFloat(void* value, D3DClass* d3d, int width, int height, int positionX, int positionY);
    static Input* createInt(void* value, D3DClass* d3d, int width, int height, int positionX, int positionY);
    
    
    bool initialize(int width, int height, int positionX, int positionY);
    virtual void Shutdown();
    virtual bool Render();

    bool setText(std::string);
    bool updateText(std::string);
    void updateText();
    virtual bool onMousePress(int x, int y, int button);
    virtual bool onKeyboardPress(InputClass::EventKey event);
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

    virtual void focus();
    virtual void unfocus();
    void hideCaret();
    void showCaret();

    void setValue(const type_info* ti, void* value)
    {
        m_typeInfo = ti;
        m_value = value;
    }
    std::string getValueRef();

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
    
    const type_info* m_typeInfo;
    void* m_value;
};