#pragma once

#include "AbstractGUI.h"
#include "group.h"
#include "label.h"
#include "button.h"

#include "../textureshaderclass.h"
#include "../bitmapclass.h"


#include <string>
using namespace std;

class Window : public Group
{
public:
    enum EventType : int {
        WINDOW_CLOSE = 32769
    };

    static const int HEADER_BUTTON_CLOSE = 1;
    static const int HEADER_BUTTON_MAXIMIZE = 2;
    static const int HEADER_BUTTON_MINIMIZE = 4;

public:
    Window();
    ~Window();

    bool Initialize(int, int, int, int);
    bool addHeader(std::string, int);
    bool addBody();

    void Shutdown();
    bool Render();
    void addChild(AbstractGui*);
    void setTitle(std::string);
    
    Group* getHeader() {
        return m_header;
    }
    
    Group* getBody() {
        return m_body;
    }

protected:
    TextureShaderClass* m_TextureShader;
    BitmapClass* m_BitmapHeader;
    BitmapClass* m_BitmapBody;

    Label* m_title;
    Group* m_header;
    Group* m_body;
};